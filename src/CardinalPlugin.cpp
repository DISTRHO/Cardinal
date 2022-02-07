/*
 * DISTRHO Cardinal Plugin
 * Copyright (C) 2021-2022 Filipe Coelho <falktx@falktx.com>
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

#include <app/Browser.hpp>
#include <app/Scene.hpp>
#include <engine/Engine.hpp>
#include <ui/common.hpp>
#include <window/Window.hpp>

#ifdef NDEBUG
# undef DEBUG
#endif

#if defined(HAVE_LIBLO) && defined(HEADLESS)
# include <lo/lo.h>
# include "extra/Thread.hpp"
# include "CardinalCommon.hpp"
#endif

#include <list>

#include "DistrhoPluginUtils.hpp"
#include "PluginContext.hpp"
#include "extra/Base64.hpp"
#include "extra/SharedResourcePointer.hpp"

#ifndef HEADLESS
# include "WindowParameters.hpp"
static const constexpr uint kCardinalStateCount = 3; // patch, text, windowSize
#else
# define kWindowParameterCount 0
static const constexpr uint kCardinalStateCount = 2; // patch, text
#endif

namespace rack {
namespace plugin {
    void initStaticPlugins();
    void destroyStaticPlugins();
}
#ifndef HEADLESS
namespace window {
    void WindowInit(Window* window, DISTRHO_NAMESPACE::Plugin* plugin);
}
#endif
}

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------------------------------------

struct Initializer
#if defined(HAVE_LIBLO) && defined(HEADLESS)
: public Thread
#endif
{
#if defined(HAVE_LIBLO) && defined(HEADLESS)
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
        settings::windowPos = math::Vec(0, 0);
#ifdef HEADLESS
        settings::headless = true;
#endif

        // copied from https://community.vcvrack.com/t/16-colour-cable-palette/15951
        settings::cableColors = {
            color::fromHexString("#ff5252"),
            color::fromHexString("#ff9352"),
            color::fromHexString("#ffd452"),
            color::fromHexString("#e8ff52"),
            color::fromHexString("#a8ff52"),
            color::fromHexString("#67ff52"),
            color::fromHexString("#52ff7d"),
            color::fromHexString("#52ffbe"),
            color::fromHexString("#52ffff"),
            color::fromHexString("#52beff"),
            color::fromHexString("#527dff"),
            color::fromHexString("#6752ff"),
            color::fromHexString("#a852ff"),
            color::fromHexString("#e952ff"),
            color::fromHexString("#ff52d4"),
            color::fromHexString("#ff5293"),
        };

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

                if (system::exists(system::join(asset::systemDir, "res")))
                {
                    templatePath = CARDINAL_PLUGIN_SOURCE_DIR DISTRHO_OS_SEP_STR "template.vcv";
                }
                // If source code dir does not exist use install target prefix as system dir
                else
               #endif
                if (system::exists(CARDINAL_PLUGIN_PREFIX "/share/Cardinal"))
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

        // Report to user if something is wrong with the installation
        if (asset::systemDir.empty())
        {
            d_stderr2("Failed to locate Cardinal plugin bundle.\n"
                      "Install Cardinal with its bundle folder intact and try again.");
        }
        else if (! system::exists(asset::systemDir))
        {
            d_stderr2("System directory \"%s\" does not exist.\n"
                      "Make sure Cardinal was downloaded and installed correctly.", asset::systemDir.c_str());
        }

        INFO("Initializing plugins");
        plugin::initStaticPlugins();

        INFO("Initializing plugin browser DB");
        app::browserInit();

#if defined(HAVE_LIBLO) && defined(HEADLESS)
        INFO("Initializing OSC Remote control");
        oscServer = lo_server_new_with_proto(REMOTE_HOST_PORT, LO_UDP, osc_error_handler);
        DISTRHO_SAFE_ASSERT_RETURN(oscServer != nullptr,);

        lo_server_add_method(oscServer, "/hello", "", osc_hello_handler, this);
        lo_server_add_method(oscServer, "/load", "b", osc_load_handler, this);
        lo_server_add_method(oscServer, nullptr, nullptr, osc_fallback_handler, nullptr);

        startThread();
#elif defined(HEADLESS)
        INFO("OSC Remote control is not enabled in this build");
#endif
    }

    ~Initializer()
    {
        using namespace rack;

#if defined(HAVE_LIBLO) && defined(HEADLESS)
        if (oscServer != nullptr)
        {
            stopThread(5000);
            lo_server_del_method(oscServer, nullptr, nullptr);
            lo_server_free(oscServer);
            oscServer = nullptr;
        }
#endif

        INFO("Clearing asset paths");
        asset::bundlePath.clear();
        asset::systemDir.clear();
        asset::userDir.clear();

        INFO("Destroying plugins");
        plugin::destroyStaticPlugins();

        INFO("Destroying logger");
        logger::destroy();
    }

#if defined(HAVE_LIBLO) && defined(HEADLESS)
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

void CardinalPluginContext::writeMidiMessage(const rack::midi::Message& message, const uint8_t channel)
{
    const size_t size = message.bytes.size();
    DISTRHO_SAFE_ASSERT_RETURN(size > 0,);
    DISTRHO_SAFE_ASSERT_RETURN(message.frame >= 0,);

    MidiEvent event;
    event.frame = message.frame;

    switch (message.bytes[0] & 0xF0)
    {
    case 0x80:
    case 0x90:
    case 0xA0:
    case 0xB0:
    case 0xE0:
        event.size = 3;
        break;
    case 0xC0:
    case 0xD0:
        event.size = 2;
        break;
    case 0xF0:
        switch (message.bytes[0] & 0x0F)
        {
        case 0x0:
        case 0x4:
        case 0x5:
        case 0x7:
        case 0x9:
        case 0xD:
            // unsupported
            return;
        case 0x1:
        case 0x2:
        case 0x3:
        case 0xE:
            event.size = 3;
            break;
        case 0x6:
        case 0x8:
        case 0xA:
        case 0xB:
        case 0xC:
        case 0xF:
            event.size = 1;
            break;
        }
        break;
    }

    DISTRHO_SAFE_ASSERT_RETURN(size >= event.size,);

    std::memcpy(event.data, message.bytes.data(), event.size);

    if (channel != 0 && event.data[0] < 0xF0)
        event.data[0] |= channel & 0x0F;

    plugin->writeMidiEvent(event);
}

// -----------------------------------------------------------------------------------------------------------

struct ScopedContext {
    ScopedContext(const CardinalBasePlugin* const plugin)
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

   #if DISTRHO_PLUGIN_NUM_INPUTS != 0
    /* If host audio ins == outs we can get issues for inplace processing.
     * So allocate a float array that will serve as safe copy for those cases.
     */
    float** fAudioBufferCopy;
   #endif

    std::string fAutosavePath;
    uint64_t fPreviousFrame;
    String fStateText;
    String fWindowSize;

   #ifndef HEADLESS
    // real values, not VCV interpreted ones
    float fWindowParameters[kWindowParameterCount];
   #endif

public:
    CardinalPlugin()
        : CardinalBasePlugin(kModuleParameters + kWindowParameterCount, 0, kCardinalStateCount),
          fInitializer(this),
         #if DISTRHO_PLUGIN_NUM_INPUTS != 0
          fAudioBufferCopy(nullptr),
         #endif
          fPreviousFrame(0)
    {
       #ifndef HEADLESS
        fWindowParameters[kWindowParameterShowTooltips] = 1.0f;
        fWindowParameters[kWindowParameterCableOpacity] = 50.0f;
        fWindowParameters[kWindowParameterCableTension] = 75.0f;
        fWindowParameters[kWindowParameterRackBrightness] = 100.0f;
        fWindowParameters[kWindowParameterHaloBrightness] = 25.0f;
        fWindowParameters[kWindowParameterKnobMode] = 0.0f;
        fWindowParameters[kWindowParameterWheelKnobControl] = 0.0f;
        fWindowParameters[kWindowParameterWheelSensitivity] = 1.0f;
        fWindowParameters[kWindowParameterLockModulePositions] = 0.0f;
        fWindowParameters[kWindowParameterUpdateRateLimit] = 0.0f;
       #endif

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

        const float sampleRate = getSampleRate();
        rack::settings::sampleRate = sampleRate;

        context->bufferSize = getBufferSize();
        context->sampleRate = sampleRate;

        const ScopedContext sc(this);

        context->engine = new rack::engine::Engine;
        context->engine->setSampleRate(sampleRate);

        context->history = new rack::history::State;
        context->patch = new rack::patch::Manager;
        context->patch->autosavePath = fAutosavePath;
        context->patch->templatePath = fInitializer->templatePath;

        context->event = new rack::widget::EventState;
        context->scene = new rack::app::Scene;
        context->event->rootWidget = context->scene;

        if (! isDummyInstance())
            context->window = new rack::window::Window;

        context->patch->loadTemplate();
        context->scene->rackScroll->reset();

#if defined(HAVE_LIBLO) && defined(HEADLESS)
        fInitializer->oscPlugin = this;
#endif
    }

    ~CardinalPlugin() override
    {
#if defined(HAVE_LIBLO) && defined(HEADLESS)
        fInitializer->oscPlugin = nullptr;
#endif

        {
            const ScopedContext sc(this);
            context->patch->clear();
            delete context;
        }

        if (! fAutosavePath.empty())
            rack::system::removeRecursively(fAutosavePath);
    }

    CardinalPluginContext* getRackContext() const noexcept
    {
        return context;
    }

protected:
   /* --------------------------------------------------------------------------------------------------------
    * Information */

    const char* getLabel() const override
    {
        return DISTRHO_PLUGIN_LABEL;
    }

    const char* getDescription() const override
    {
        return ""
        "Cardinal is an open-source plugin wrapper around VCV Rack.\n"
        "It is NOT an official VCV project, and it is not affiliated with it in any way.\n"
        "\n"
        "Cardinal contains Rack, some 3rd-party modules and a few internal utilities all in a single binary.\n"
        "It does not load external modules and does not connect to the official Rack library/store.\n";
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
        return d_version(0, 22, 2);
    }

    int64_t getUniqueId() const override
    {
       #if CARDINAL_VARIANT_MAIN
        return d_cconst('d', 'C', 'd', 'n');
       #elif CARDINAL_VARIANT_FX
        return d_cconst('d', 'C', 'n', 'F');
       #elif CARDINAL_VARIANT_SYNTH
        return d_cconst('d', 'C', 'n', 'S');
       #else
        #error cardinal variant not set
       #endif
    }

   /* --------------------------------------------------------------------------------------------------------
    * Init */

    void initAudioPort(const bool input, uint32_t index, AudioPort& port) override
    {
        if (index >= 8)
        {
            port.hints = kAudioPortIsCV | kCVPortHasPositiveUnipolarRange | kCVPortHasScaledRange;
            index -= 8;
        }

        CardinalBasePlugin::initAudioPort(input, index, port);
    }

    void initParameter(const uint32_t index, Parameter& parameter) override
    {
        if (index < kModuleParameters)
        {
            parameter.name = "Parameter ";
            parameter.name += String(index + 1);
            parameter.symbol = "param_";
            parameter.symbol += String(index + 1);
            parameter.unit = "v";
            parameter.hints = kParameterIsAutomatable;
            parameter.ranges.def = 0.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 10.0f;
            return;
        }

       #ifndef HEADLESS
        switch (index - kModuleParameters)
        {
        case kWindowParameterShowTooltips:
            parameter.name = "Show tooltips";
            parameter.symbol = "tooltips";
            parameter.hints = kParameterIsAutomatable|kParameterIsInteger|kParameterIsBoolean;
            parameter.ranges.def = 1.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 1.0f;
            break;
        case kWindowParameterCableOpacity:
            parameter.name = "Cable opacity";
            parameter.symbol = "cableOpacity";
            parameter.unit = "%";
            parameter.hints = kParameterIsAutomatable;
            parameter.ranges.def = 50.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 100.0f;
            break;
        case kWindowParameterCableTension:
            parameter.name = "Cable tension";
            parameter.symbol = "cableTension";
            parameter.unit = "%";
            parameter.hints = kParameterIsAutomatable;
            parameter.ranges.def = 75.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 100.0f;
            break;
        case kWindowParameterRackBrightness:
            parameter.name = "Room brightness";
            parameter.symbol = "rackBrightness";
            parameter.unit = "%";
            parameter.hints = kParameterIsAutomatable;
            parameter.ranges.def = 100.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 100.0f;
            break;
        case kWindowParameterHaloBrightness:
            parameter.name = "Light Bloom";
            parameter.symbol = "haloBrightness";
            parameter.unit = "%";
            parameter.hints = kParameterIsAutomatable;
            parameter.ranges.def = 25.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 100.0f;
            break;
        case kWindowParameterKnobMode:
            parameter.name = "Knob mode";
            parameter.symbol = "knobMode";
            parameter.hints = kParameterIsAutomatable|kParameterIsInteger;
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
            parameter.hints = kParameterIsAutomatable|kParameterIsInteger|kParameterIsBoolean;
            parameter.ranges.def = 0.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 1.0f;
            break;
        case kWindowParameterWheelSensitivity:
            parameter.name = "Scroll wheel knob sensitivity";
            parameter.symbol = "knobScrollSensitivity";
            parameter.hints = kParameterIsAutomatable|kParameterIsLogarithmic;
            parameter.ranges.def = 1.0f;
            parameter.ranges.min = 0.1f;
            parameter.ranges.max = 10.0f;
            break;
        case kWindowParameterLockModulePositions:
            parameter.name = "Lock module positions";
            parameter.symbol = "lockModules";
            parameter.hints = kParameterIsAutomatable|kParameterIsInteger|kParameterIsBoolean;
            parameter.ranges.def = 0.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 1.0f;
            break;
        case kWindowParameterUpdateRateLimit:
            parameter.name = "Update rate limit";
            parameter.symbol = "rateLimit";
            parameter.hints = kParameterIsAutomatable|kParameterIsInteger;
            parameter.ranges.def = 0.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 2.0f;
            parameter.enumValues.count = 3;
            parameter.enumValues.restrictedMode = true;
            parameter.enumValues.values = new ParameterEnumerationValue[3];
            parameter.enumValues.values[0].label = "None";
            parameter.enumValues.values[0].value = 0.0f;
            parameter.enumValues.values[1].label = "2x";
            parameter.enumValues.values[1].value = 1.0f;
            parameter.enumValues.values[2].label = "4x";
            parameter.enumValues.values[2].value = 2.0f;
            break;
        }
       #endif
    }

    void initState(const uint32_t index, String& stateKey, String& defaultStateValue) override
    {
        defaultStateValue = "";

        switch (index)
        {
        case 0:
            stateKey = "patch";
            break;
        case 1:
            stateKey = "text";
            break;
       #ifndef HEADLESS
        case 2:
            stateKey = "windowSize";
            break;
       #endif
        }
    }

    uint32_t getStateHints(const uint32_t index) override
    {
        switch (index)
        {
        case 1:
            return kStateIsHostVisible;
        default:
            return 0x0;
        }
    }

   /* --------------------------------------------------------------------------------------------------------
    * Internal data */

    float getParameterValue(uint32_t index) const override
    {
        if (index < kModuleParameters)
            return context->parameters[index];

       #ifndef HEADLESS
        index -= kModuleParameters;

        if (index < kWindowParameterCount)
            return fWindowParameters[index];
       #endif

        return 0.0f;
    }

    void setParameterValue(uint32_t index, float value) override
    {
        if (index < kModuleParameters)
        {
            context->parameters[index] = value;
            return;
        }

       #ifndef HEADLESS
        index -= kModuleParameters;

        if (index < kWindowParameterCount)
        {
            fWindowParameters[index] = value;
            return;
        }
       #endif
    }

    String getState(const char* const key) const override
    {
       #ifndef HEADLESS
        if (std::strcmp(key, "windowSize") == 0)
            return fWindowSize;
       #endif

        if (std::strcmp(key, "text") == 0)
            return fStateText;

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
            // context->history->setSaved();

            try {
                data = rack::system::archiveDirectory(fAutosavePath, 1);
            } DISTRHO_SAFE_EXCEPTION_RETURN("getState archiveDirectory", String());
        }

        return String::asBase64(data.data(), data.size());
    }

    void setState(const char* const key, const char* const value) override
    {
       #ifndef HEADLESS
        if (std::strcmp(key, "windowSize") == 0)
        {
            fWindowSize = value;
            return;
        }
       #endif

        if (std::strcmp(key, "text") == 0)
        {
            fStateText = value;
            return;
        }

        if (std::strcmp(key, "patch") != 0)
            return;
        if (fAutosavePath.empty())
            return;

        const std::vector<uint8_t> data(d_getChunkFromBase64String(value));

        const ScopedContext sc(this);

        rack::system::removeRecursively(fAutosavePath);
        rack::system::createDirectories(fAutosavePath);
        rack::system::unarchiveToDirectory(data, fAutosavePath);

        try {
            context->patch->loadAutosave();
        } DISTRHO_SAFE_EXCEPTION_RETURN("setState loadAutosave",);

        // context->history->setSaved();
    }

   /* --------------------------------------------------------------------------------------------------------
    * Process */

    void activate() override
    {
       #if DISTRHO_PLUGIN_NUM_INPUTS != 0
        const uint32_t bufferSize = getBufferSize();
        fAudioBufferCopy = new float*[DISTRHO_PLUGIN_NUM_INPUTS];
        for (int i=0; i<DISTRHO_PLUGIN_NUM_INPUTS; ++i)
            fAudioBufferCopy[i] = new float[bufferSize];
       #endif

        fPreviousFrame = 0;
    }

    void deactivate() override
    {
       #if DISTRHO_PLUGIN_NUM_INPUTS != 0
        if (fAudioBufferCopy != nullptr)
        {
            for (int i=0; i<DISTRHO_PLUGIN_NUM_INPUTS; ++i)
                delete[] fAudioBufferCopy[i];
            delete[] fAudioBufferCopy;
            fAudioBufferCopy = nullptr;
        }
       #endif
    }

    void run(const float** const inputs, float** const outputs, const uint32_t frames,
             const MidiEvent* const midiEvents, const uint32_t midiEventCount) override
    {
        rack::contextSet(context);

        {
            const TimePosition& timePos(getTimePosition());

            const bool reset = timePos.playing && (timePos.frame == 0 || fPreviousFrame + frames != timePos.frame);

            context->playing = timePos.playing;
            context->bbtValid = timePos.bbt.valid;
            context->frame = timePos.frame;

            if (timePos.bbt.valid)
            {
                const double samplesPerTick = 60.0 * getSampleRate()
                                            / timePos.bbt.beatsPerMinute
                                            / timePos.bbt.ticksPerBeat;
                context->bar = timePos.bbt.bar;
                context->beat = timePos.bbt.beat;
                context->beatsPerBar = timePos.bbt.beatsPerBar;
                context->beatType = timePos.bbt.beatType;
                context->barStartTick = timePos.bbt.barStartTick;
                context->beatsPerMinute = timePos.bbt.beatsPerMinute;
                context->tick = timePos.bbt.tick;
                context->ticksPerBeat = timePos.bbt.ticksPerBeat;
                context->ticksPerClock = timePos.bbt.ticksPerBeat / timePos.bbt.beatType;
                context->ticksPerFrame = 1.0 / samplesPerTick;
                context->tickClock = std::fmod(timePos.bbt.tick, context->ticksPerClock);
            }

            context->reset = reset;
            fPreviousFrame = timePos.frame;
        }

        // separate buffers, use them
        if (inputs != outputs && (inputs == nullptr || inputs[0] != outputs[0]))
        {
            context->dataIns = inputs;
            context->dataOuts = outputs;
        }
        // inline processing, use a safe copy
        else
        {
           #if DISTRHO_PLUGIN_NUM_INPUTS != 0
            for (int i=0; i<DISTRHO_PLUGIN_NUM_INPUTS; ++i)
                std::memcpy(fAudioBufferCopy[i], inputs[i], sizeof(float)*frames);
            context->dataIns = fAudioBufferCopy;
           #else
            context->dataIns = nullptr;
           #endif
            context->dataOuts = outputs;
        }

        for (int i=0; i<DISTRHO_PLUGIN_NUM_OUTPUTS; ++i)
            std::memset(outputs[i], 0, sizeof(float)*frames);

        context->midiEvents = midiEvents;
        context->midiEventCount = midiEventCount;

        context->engine->stepBlock(frames);
    }

    void bufferSizeChanged(const uint32_t newBufferSize) override
    {
        rack::contextSet(context);
        context->bufferSize = newBufferSize;
    }

    void sampleRateChanged(const double newSampleRate) override
    {
        rack::contextSet(context);
        rack::settings::sampleRate = newSampleRate;
        context->sampleRate = newSampleRate;
        context->engine->setSampleRate(newSampleRate);
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
