/*
 * DISTRHO Cardinal Plugin
 * Copyright (C) 2021 Filipe Coelho <falktx@falktx.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * For a full copy of the GNU General Public License see the LICENSE file.
 */

#include <asset.hpp>
#include <library.hpp>
#include <midi.hpp>
#include <patch.hpp>
#include <plugin.hpp>
#include <random.hpp>
#include <settings.hpp>
#include <system.hpp>

#include <app/Scene.hpp>
#include <engine/Engine.hpp>
#include <ui/common.hpp>
#include <window/Window.hpp>

#ifdef NDEBUG
# undef DEBUG
#endif

#ifdef HAVE_LIBLO
# include <lo/lo.h>
# include "extra/Thread.hpp"
#endif

#include <list>

#include "DistrhoPluginUtils.hpp"
#include "PluginDriver.hpp"
#include "WindowParameters.hpp"
#include "extra/Base64.hpp"
#include "extra/SharedResourcePointer.hpp"

#define REMOTE_HOST_PORT "2228"

namespace rack {
namespace plugin {
    void initStaticPlugins();
    void destroyStaticPlugins();
}
#if defined(__MOD_DEVICES__) && !defined(HEADLESS)
namespace window {
    void WindowInit(Window* window, DISTRHO_NAMESPACE::Plugin* plugin);
}
#endif
}

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------------------------------------

struct Initializer
#ifdef HAVE_LIBLO
: public Thread
#endif
{
#ifdef HAVE_LIBLO
    lo_server oscServer = nullptr;
    CardinalBasePlugin* oscPlugin = nullptr;
#endif
    std::string templatePath;

    Initializer(const CardinalBasePlugin* const plugin)
    {
        using namespace rack;

        settings::allowCursorLock = false;
        settings::autoCheckUpdates = false;
        settings::autosaveInterval = 0;
        settings::devMode = true;
        settings::discordUpdateActivity = false;
        settings::isPlugin = true;
        settings::skipLoadOnLaunch = true;
        settings::showTipsOnLaunch = false;
#ifdef HEADLESS
        settings::headless = true;
#endif
#ifdef __MOD_DEVICES__
        settings::threadCount = 3;
#else
        settings::threadCount = 1;
#endif

        system::init();
        logger::init();
        random::init();
        ui::init();

        if (asset::systemDir.empty())
        {
            if (const char* const bundlePath = plugin->getBundlePath())
            {
                if (const char* const resourcePath = getResourcePath(bundlePath))
                {
                    asset::bundlePath = system::join(resourcePath, "PluginManifests");
                    asset::systemDir = resourcePath;
                    templatePath = system::join(asset::systemDir, "template.vcv");
                }
            }

            if (asset::systemDir.empty())
            {
               #ifdef CARDINAL_PLUGIN_SOURCE_DIR
                // Make system dir point to source code location as fallback
                asset::systemDir = CARDINAL_PLUGIN_SOURCE_DIR DISTRHO_OS_SEP_STR "Rack";

                if (system::isDirectory(system::join(asset::systemDir, "res")))
                {
                    templatePath = CARDINAL_PLUGIN_SOURCE_DIR DISTRHO_OS_SEP_STR "template.vcv";
                }
                else
                // If source code dir does not exist use install target prefix as system dir
               #endif
                {
                    asset::bundlePath = CARDINAL_PLUGIN_PREFIX "/share/Cardinal/PluginManifests";
                    asset::systemDir = CARDINAL_PLUGIN_PREFIX "/share/Cardinal";
                    templatePath = system::join(asset::systemDir, "template.vcv");
                }
            }

            asset::userDir = asset::systemDir;
        }

        // Log environment
        INFO("%s %s v%s", APP_NAME.c_str(), APP_EDITION.c_str(), APP_VERSION.c_str());
        INFO("%s", system::getOperatingSystemInfo().c_str());
        INFO("Binary filename: %s", getBinaryFilename());
        INFO("Bundle path: %s", plugin->getBundlePath());
        INFO("System directory: %s", asset::systemDir.c_str());
        INFO("User directory: %s", asset::userDir.c_str());
        INFO("Template patch: %s", templatePath.c_str());

        // Check existence of the system res/ directory
        if (! system::isDirectory(asset::systemDir))
        {
            d_stderr2("System directory \"%s\" does not exist.\n"
                      "Make sure Cardinal was downloaded and installed correctly.", asset::systemDir.c_str());
        }

        INFO("Initializing audio driver");
        rack::audio::addDriver(0, new CardinalAudioDriver);

        INFO("Initializing midi driver");
        rack::midi::addDriver(0, new CardinalMidiDriver);

        INFO("Initializing plugins");
        plugin::initStaticPlugins();

#ifdef HAVE_LIBLO
        INFO("Initializing OSC Remote control");
        oscServer = lo_server_new_with_proto(REMOTE_HOST_PORT, LO_UDP, osc_error_handler);
        DISTRHO_SAFE_ASSERT_RETURN(oscServer != nullptr,);

        lo_server_add_method(oscServer, "/hello", "", osc_hello_handler, this);
        lo_server_add_method(oscServer, "/load", "b", osc_load_handler, this);
        lo_server_add_method(oscServer, nullptr, nullptr, osc_fallback_handler, nullptr);

        startThread();
#else
        INFO("OSC Remote control is not enabled in this build");
#endif
    }

    ~Initializer()
    {
        using namespace rack;

#ifdef HAVE_LIBLO
        if (oscServer != nullptr)
        {
            stopThread(5000);
            lo_server_del_method(oscServer, nullptr, nullptr);
            lo_server_free(oscServer);
            oscServer = nullptr;
        }
#endif

        INFO("Destroying plugins");
        plugin::destroyStaticPlugins();

        INFO("Destroying MIDI devices");
        midi::destroy();

        INFO("Destroying audio devices");
        audio::destroy();

        INFO("Destroying logger");
        logger::destroy();
    }

#ifdef HAVE_LIBLO
    void run() override
    {
        INFO("OSC Thread Listening for remote commands");

        while (! shouldThreadExit())
        {
            d_msleep(200);
            while (lo_server_recv_noblock(oscServer, 0) != 0) {}
        }

        INFO("OSC Thread Closed");
    }

    static void osc_error_handler(int num, const char* msg, const char* path)
    {
        d_stderr("Cardinal OSC Error: code: %i, msg: \"%s\", path: \"%s\")", num, msg, path);
    }

    static int osc_fallback_handler(const char* const path, const char* const types, lo_arg**, int, lo_message, void*)
    {
        d_stderr("Cardinal OSC unhandled message \"%s\" with types \"%s\"", path, types);
        return 0;
    }

    static int osc_hello_handler(const char*, const char*, lo_arg**, int, const lo_message m, void* const self)
    {
        d_stdout("osc_hello_handler()");
        const lo_address source = lo_message_get_source(m);
        lo_send_from(source, static_cast<Initializer*>(self)->oscServer, LO_TT_IMMEDIATE, "/resp", "ss", "hello", "ok");
        return 0;
    }

    static int osc_load_handler(const char*, const char* types, lo_arg** argv, int argc, const lo_message m, void* const self)
    {
        d_stdout("osc_load_handler()");
        DISTRHO_SAFE_ASSERT_RETURN(argc == 1, 0);
        DISTRHO_SAFE_ASSERT_RETURN(types != nullptr && types[0] == 'b', 0);

        const int32_t size = argv[0]->blob.size;
        DISTRHO_SAFE_ASSERT_RETURN(size > 4, 0);

        const uint8_t* const blob = (uint8_t*)(&argv[0]->blob.data);
        DISTRHO_SAFE_ASSERT_RETURN(blob != nullptr, 0);

        bool ok = false;

        if (CardinalBasePlugin* const plugin = static_cast<Initializer*>(self)->oscPlugin)
        {
            CardinalPluginContext* const context = plugin->context;
            std::vector<uint8_t> data(size);
            std::memcpy(data.data(), blob, size);

            const MutexLocker cml(context->mutex);
            rack::contextSet(context);
            rack::system::removeRecursively(context->patch->autosavePath);
            rack::system::createDirectories(context->patch->autosavePath);
            try {
                rack::system::unarchiveToDirectory(data, context->patch->autosavePath);
                context->patch->loadAutosave();
                ok = true;
            }
            catch (rack::Exception& e) {
                WARN("%s", e.what());
            }
            rack::contextSet(nullptr);
        }

        const lo_address source = lo_message_get_source(m);
        lo_send_from(source, static_cast<Initializer*>(self)->oscServer,
                     LO_TT_IMMEDIATE, "/resp", "ss", "load", ok ? "ok" : "fail");
        return 0;
    }
#endif
};

// -----------------------------------------------------------------------------------------------------------

struct ScopedContext {
    const MutexLocker cml;

    ScopedContext(const CardinalBasePlugin* const plugin)
        : cml(plugin->context->mutex)
    {
        rack::contextSet(plugin->context);
    }

    ~ScopedContext()
    {
        rack::contextSet(nullptr);
    }
};

// -----------------------------------------------------------------------------------------------------------

class CardinalPlugin : public CardinalBasePlugin
{
    SharedResourcePointer<Initializer> fInitializer;

    float* fAudioBufferIn;
    float* fAudioBufferOut;
    std::string fAutosavePath;

    // for base/context handling
    bool fIsActive;
    CardinalAudioDevice* fCurrentAudioDevice;
    CardinalMidiOutputDevice* fCurrentMidiOutput;
    std::list<CardinalMidiInputDevice*> fMidiInputs;
    Mutex fDeviceMutex;

    // real values, not VCV interpreted ones
    float fWindowParameters[kWindowParameterCount];

public:
    CardinalPlugin()
        : CardinalBasePlugin(kModuleParameters + kWindowParameterCount, 0, 1),
          fInitializer(this),
          fAudioBufferIn(nullptr),
          fAudioBufferOut(nullptr),
          fIsActive(false),
          fCurrentAudioDevice(nullptr),
          fCurrentMidiOutput(nullptr)
    {
        fWindowParameters[kWindowParameterShowTooltips] = 1.0f;
        fWindowParameters[kWindowParameterCableOpacity] = 50.0f;
        fWindowParameters[kWindowParameterCableTension] = 50.0f;
        fWindowParameters[kWindowParameterRackBrightness] = 100.0f;
        fWindowParameters[kWindowParameterHaloBrightness] = 25.0f;
        fWindowParameters[kWindowParameterKnobMode] = 0.0f;
        fWindowParameters[kWindowParameterWheelKnobControl] = 0.0f;
        fWindowParameters[kWindowParameterWheelSensitivity] = 1.0f;
        fWindowParameters[kWindowParameterLockModulePositions] = 0.0f;

        // create unique temporary path for this instance
        try {
            char uidBuf[24];
            const std::string tmp = rack::system::getTempDirectory();

            for (int i=1;; ++i)
            {
                std::snprintf(uidBuf, sizeof(uidBuf), "Cardinal.%04d", i);
                const std::string trypath = rack::system::join(tmp, uidBuf);

                if (! rack::system::exists(trypath))
                {
                    if (rack::system::createDirectories(trypath))
                        fAutosavePath = trypath;
                    break;
                }
            }
        } DISTRHO_SAFE_EXCEPTION("create unique temporary path");

        const ScopedContext sc(this);

        context->engine = new rack::engine::Engine;
        context->history = new rack::history::State;
        context->patch = new rack::patch::Manager;
        context->patch->autosavePath = fAutosavePath;
        context->patch->templatePath = fInitializer->templatePath;

        context->event = new rack::widget::EventState;
        context->scene = new rack::app::Scene;
        context->event->rootWidget = context->scene;
        context->patch->loadTemplate();
        context->engine->startFallbackThread();

#ifdef HAVE_LIBLO
        fInitializer->oscPlugin = this;
#endif

#if defined(__MOD_DEVICES__) && !defined(HEADLESS)
        context->window = new rack::window::Window;
        rack::window::WindowInit(context->window, this);
        /*
        context->scene->removeChild(context->scene->menuBar);
        context->scene->menuBar = rack::app::createMenuBar(getWindow(), getApp().isStandalone());
        context->scene->addChildBelow(context->scene->menuBar, context->scene->rackScroll);
        */
#endif
    }

    ~CardinalPlugin() override
    {
#ifdef HAVE_LIBLO
        fInitializer->oscPlugin = nullptr;
#endif

        {
            const MutexLocker cml(context->mutex);
            rack::contextSet(context);
#if defined(__MOD_DEVICES__) && !defined(HEADLESS)
            delete context->window;
            context->window = nullptr;
#endif
        }

        delete context;
        rack::contextSet(nullptr);

        if (! fAutosavePath.empty())
            rack::system::removeRecursively(fAutosavePath);
    }

    CardinalPluginContext* getRackContext() const noexcept
    {
        return context;
    }

protected:
   /* --------------------------------------------------------------------------------------------------------
    * Cardinal Base things */

    bool isActive() const noexcept override
    {
        return fIsActive;
    }

    bool canAssignAudioDevice() const noexcept override
    {
        const MutexLocker cml(fDeviceMutex);
        return fCurrentAudioDevice == nullptr;
    }

    bool canAssignMidiOutputDevice() const noexcept override
    {
        const MutexLocker cml(fDeviceMutex);
        return fCurrentMidiOutput == nullptr;
    }

    void assignAudioDevice(CardinalAudioDevice* const dev) noexcept override
    {
        DISTRHO_SAFE_ASSERT_RETURN(fCurrentAudioDevice == nullptr,);

        const MutexLocker cml(fDeviceMutex);
        fCurrentAudioDevice = dev;
    }

    void assignMidiOutputDevice(CardinalMidiOutputDevice* const dev) noexcept override
    {
        DISTRHO_SAFE_ASSERT_RETURN(fCurrentMidiOutput == nullptr,);

        const MutexLocker cml(fDeviceMutex);
        fCurrentMidiOutput = dev;
    }

    bool clearAudioDevice(CardinalAudioDevice* const dev) noexcept override
    {
        const MutexLocker cml(fDeviceMutex);

        if (fCurrentAudioDevice != dev)
            return false;

        fCurrentAudioDevice = nullptr;
        return true;
    }

    bool clearMidiOutputDevice(CardinalMidiOutputDevice* const dev) noexcept override
    {
        const MutexLocker cml(fDeviceMutex);

        if (fCurrentMidiOutput != dev)
            return false;

        fCurrentMidiOutput = nullptr;
        return true;
    }

    void addMidiInput(CardinalMidiInputDevice* const dev) override
    {
        // NOTE this will xrun
        const MutexLocker cml(fDeviceMutex);

        fMidiInputs.push_back(dev);
    }

    void removeMidiInput(CardinalMidiInputDevice* const dev) override
    {
        // NOTE this will xrun
        const MutexLocker cml(fDeviceMutex);

        fMidiInputs.remove(dev);
    }

   /* --------------------------------------------------------------------------------------------------------
    * Information */

    const char* getLabel() const override
    {
        return "Cardinal";
    }

    const char* getDescription() const override
    {
        return ""
        "Cardinal is an open-source self-contained special plugin version of VCVRack, using DPF.\n"
        "It is NOT an official VCV project, and it is not affiliated with it in any way.\n";
    }

    const char* getMaker() const override
    {
        return "DISTRHO";
    }

    const char* getHomePage() const override
    {
        return "https://github.com/DISTRHO/Cardinal";
    }

    const char* getLicense() const override
    {
        return "GPLv3+";
    }

    uint32_t getVersion() const override
    {
        return d_version(2, 0, 0);
    }

    int64_t getUniqueId() const override
    {
        return d_cconst('d', 'C', 'd', 'n');
    }

   /* --------------------------------------------------------------------------------------------------------
    * Init */

    void initParameter(const uint32_t index, Parameter& parameter) override
    {
        if (index < kModuleParameters)
        {
            parameter.name = "Parameter ";
            parameter.name += String(index + 1);
            parameter.symbol = "param_";
            parameter.symbol += String(index + 1);
            parameter.unit = "v";
            parameter.hints = kParameterIsAutomable;
            parameter.ranges.def = 0.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 10.0f;
            return;
        }

        switch (index - kModuleParameters)
        {
        case kWindowParameterShowTooltips:
            parameter.name = "Show tooltips";
            parameter.symbol = "tooltips";
            parameter.hints = kParameterIsAutomable|kParameterIsInteger|kParameterIsBoolean;
            parameter.ranges.def = 1.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 1.0f;
            break;
        case kWindowParameterCableOpacity:
            parameter.name = "Cable opacity";
            parameter.symbol = "cableOpacity";
            parameter.unit = "%";
            parameter.hints = kParameterIsAutomable;
            parameter.ranges.def = 50.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 100.0f;
            break;
        case kWindowParameterCableTension:
            parameter.name = "Cable tension";
            parameter.symbol = "cableTension";
            parameter.unit = "%";
            parameter.hints = kParameterIsAutomable;
            parameter.ranges.def = 50.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 100.0f;
            break;
        case kWindowParameterRackBrightness:
            parameter.name = "Room brightness";
            parameter.symbol = "rackBrightness";
            parameter.unit = "%";
            parameter.hints = kParameterIsAutomable;
            parameter.ranges.def = 100.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 100.0f;
            break;
        case kWindowParameterHaloBrightness:
            parameter.name = "Light Bloom";
            parameter.symbol = "haloBrightness";
            parameter.unit = "%";
            parameter.hints = kParameterIsAutomable;
            parameter.ranges.def = 25.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 100.0f;
            break;
        case kWindowParameterKnobMode:
            parameter.name = "Knob mode";
            parameter.symbol = "knobMode";
            parameter.hints = kParameterIsAutomable|kParameterIsInteger;
            parameter.ranges.def = 0.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 2.0f;
            parameter.enumValues.count = 3;
            parameter.enumValues.restrictedMode = true;
            parameter.enumValues.values = new ParameterEnumerationValue[3];
            parameter.enumValues.values[0].label = "Linear";
            parameter.enumValues.values[0].value = 0.0f;
            parameter.enumValues.values[1].label = "Absolute rotary";
            parameter.enumValues.values[1].value = 1.0f;
            parameter.enumValues.values[2].label = "Relative rotary";
            parameter.enumValues.values[2].value = 2.0f;
            break;
        case kWindowParameterWheelKnobControl:
            parameter.name = "Scroll wheel knob control";
            parameter.symbol = "knobScroll";
            parameter.hints = kParameterIsAutomable|kParameterIsInteger|kParameterIsBoolean;
            parameter.ranges.def = 0.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 1.0f;
            break;
        case kWindowParameterWheelSensitivity:
            parameter.name = "Scroll wheel knob sensitivity";
            parameter.symbol = "knobScrollSensitivity";
            parameter.hints = kParameterIsAutomable|kParameterIsLogarithmic;
            parameter.ranges.def = 1.0f;
            parameter.ranges.min = 0.1f;
            parameter.ranges.max = 10.0f;
            break;
        case kWindowParameterLockModulePositions:
            parameter.name = "Lock module positions";
            parameter.symbol = "lockModules";
            parameter.hints = kParameterIsAutomable|kParameterIsInteger|kParameterIsBoolean;
            parameter.ranges.def = 0.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 1.0f;
            break;
        }
    }

    void initState(const uint32_t index, String& stateKey, String& defaultStateValue) override
    {
        DISTRHO_SAFE_ASSERT_RETURN(index == 0,);

        stateKey = "patch";
        defaultStateValue = "";
    }

   /* --------------------------------------------------------------------------------------------------------
    * Internal data */

    float getParameterValue(uint32_t index) const override
    {
        if (index < kModuleParameters)
            return context->parameters[index];

        index -= kModuleParameters;

        if (index < kWindowParameterCount)
            return fWindowParameters[index];

        return 0.0f;
    }

    void setParameterValue(uint32_t index, float value) override
    {
        if (index < kModuleParameters)
        {
            context->parameters[index] = value;
            return;
        }

        index -= kModuleParameters;

        if (index < kWindowParameterCount)
        {
            fWindowParameters[index] = value;
            return;
        }
    }

    String getState(const char* const key) const override
    {
        if (std::strcmp(key, "patch") != 0)
            return String();
        if (fAutosavePath.empty())
            return String();

        std::vector<uint8_t> data;

        {
            const ScopedContext sc(this);

            context->engine->prepareSave();
            context->patch->saveAutosave();
            context->patch->cleanAutosave();

            data = rack::system::archiveDirectory(fAutosavePath, 1);
        }

        return String::asBase64(data.data(), data.size());
    }

    void setState(const char* const key, const char* const value) override
    {
        if (std::strcmp(key, "patch") != 0)
            return;
        if (fAutosavePath.empty())
            return;

        const std::vector<uint8_t> data(d_getChunkFromBase64String(value));

        const ScopedContext sc(this);

        rack::system::removeRecursively(fAutosavePath);
        rack::system::createDirectories(fAutosavePath);
        rack::system::unarchiveToDirectory(data, fAutosavePath);

        context->patch->loadAutosave();
    }

   /* --------------------------------------------------------------------------------------------------------
    * Process */

    void activate() override
    {
        const uint32_t bufferSize = getBufferSize() * DISTRHO_PLUGIN_NUM_OUTPUTS;
        fAudioBufferIn = new float[bufferSize];
        fAudioBufferOut = new float[bufferSize];
        std::memset(fAudioBufferIn, 0, sizeof(float)*bufferSize);

        {
            const MutexLocker cml(fDeviceMutex);

            if (fCurrentAudioDevice != nullptr)
                fCurrentAudioDevice->onStartStream();
        }
    }

    void deactivate() override
    {
        {
            const MutexLocker cml(fDeviceMutex);

            if (fCurrentAudioDevice != nullptr)
                fCurrentAudioDevice->onStopStream();
        }

        delete[] fAudioBufferIn;
        delete[] fAudioBufferOut;
        fAudioBufferIn = fAudioBufferOut = nullptr;
    }

    void run(const float** const inputs, float** const outputs, const uint32_t frames,
             const MidiEvent* const midiEvents, const uint32_t midiEventCount) override
    {
        /*
        context->engine->setFrame(getTimePosition().frame);
        context->engine->stepBlock(frames);
        */

        const MutexLocker cml(fDeviceMutex);

        if (fCurrentAudioDevice != nullptr)
        {
            for (uint32_t i=0, j=0; i<frames; ++i)
            {
                fAudioBufferIn[j++] = inputs[0][i];
                fAudioBufferIn[j++] = inputs[1][i];
            }
        }
        else
        {
            std::memset(outputs[0], 0, sizeof(float)*frames);
            std::memset(outputs[1], 0, sizeof(float)*frames);
        }

        std::memset(fAudioBufferOut, 0, sizeof(float)*frames*DISTRHO_PLUGIN_NUM_OUTPUTS);

        for (CardinalMidiInputDevice* dev : fMidiInputs)
            dev->handleMessagesFromHost(midiEvents, midiEventCount);

        if (fCurrentAudioDevice != nullptr)
            fCurrentAudioDevice->processBuffer(fAudioBufferIn, DISTRHO_PLUGIN_NUM_INPUTS,
                                               fAudioBufferOut, DISTRHO_PLUGIN_NUM_OUTPUTS, frames);

        if (fCurrentMidiOutput != nullptr)
            fCurrentMidiOutput->processMessages();

        for (uint32_t i=0, j=0; i<frames; ++i)
        {
            outputs[0][i] = fAudioBufferOut[j++];
            outputs[1][i] = fAudioBufferOut[j++];
        }
    }

    // -------------------------------------------------------------------------------------------------------

private:
   /**
      Set our plugin class as non-copyable and add a leak detector just in case.
    */
    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CardinalPlugin)
};

CardinalPluginContext* getRackContextFromPlugin(void* const ptr)
{
    return static_cast<CardinalPlugin*>(ptr)->getRackContext();
}

/* ------------------------------------------------------------------------------------------------------------
 * Plugin entry point, called by DPF to create a new plugin instance. */

Plugin* createPlugin()
{
    return new CardinalPlugin();
}

// -----------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
