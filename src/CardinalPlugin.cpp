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

#include <osdialog.h>

#ifdef NDEBUG
# undef DEBUG
#endif

#include <atomic>
#include <list>

#include "DistrhoPluginUtils.hpp"
#include "PluginDriver.hpp"
#include "WindowParameters.hpp"
#include "extra/Base64.hpp"
#include "extra/SharedResourcePointer.hpp"

namespace rack {
namespace plugin {
void initStaticPlugins();
void destroyStaticPlugins();
}
}

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------------------------------------

struct Initializer {
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
        settings::threadCount = 1;

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
                }
            }

            if (asset::systemDir.empty())
            {
               #ifdef CARDINAL_PLUGIN_SOURCE_DIR
                // Make system dir point to source code location as fallback
                asset::systemDir = CARDINAL_PLUGIN_SOURCE_DIR DISTRHO_OS_SEP_STR "Rack";

                // And if that fails, use install target prefix
                if (! system::isDirectory(system::join(asset::systemDir, "res")))
               #endif
                {
                    asset::bundlePath = CARDINAL_PLUGIN_PREFIX "/share/Cardinal/PluginManifests";
                    asset::systemDir = CARDINAL_PLUGIN_PREFIX "/share/Cardinal";
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
    }

    ~Initializer()
    {
        using namespace rack;

        INFO("Destroying plugins");
        plugin::destroyStaticPlugins();

        INFO("Destroying MIDI devices");
        midi::destroy();

        INFO("Destroying audio devices");
        audio::destroy();

        INFO("Destroying logger");
        logger::destroy();
    }
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
    std::atomic<bool> fIsProcessing;
    rack::audio::Device* fCurrentDevice;
    Mutex fDeviceMutex;
    std::list<CardinalMidiInputDevice*> fMidiInputs;
    volatile pthread_t fProcessThread;

    float fWindowParameters[kWindowParameterCount];

public:
    CardinalPlugin()
        : CardinalBasePlugin(kModuleParameters + kWindowParameterCount, 0, 1),
          fInitializer(this),
          fAudioBufferIn(nullptr),
          fAudioBufferOut(nullptr),
          fIsActive(false),
          fIsProcessing(false),
          fCurrentDevice(nullptr),
#ifdef PTW32_DLLPORT
          fProcessThread({nullptr, 0})
#else
          fProcessThread(0)
#endif
    {
        fWindowParameters[kWindowParameterCableOpacity] = 50.0f;
        fWindowParameters[kWindowParameterCableTension] = 50.0f;
        fWindowParameters[kWindowParameterRackBrightness] = 100.0f;
        fWindowParameters[kWindowParameterHaloBrightness] = 25.0f;

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
        context->patch->templatePath = rack::system::join(rack::asset::systemDir, "template.vcv");
        // context->patch->templatePath = CARDINAL_PLUGIN_SOURCE_DIR DISTRHO_OS_SEP_STR "template.vcv";

        context->event = new rack::widget::EventState;
        context->scene = new rack::app::Scene;
        context->event->rootWidget = context->scene;

        context->patch->loadTemplate();
        context->engine->startFallbackThread();
    }

    ~CardinalPlugin() override
    {
        {
            const MutexLocker cml(context->mutex);
            rack::contextSet(context);

            /*
            delete context->scene;
            context->scene = nullptr;
            delete context->event;
            context->event = nullptr;
            */
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

    bool isProcessing() const noexcept override
    {
        return fIsProcessing.load() && pthread_equal(fProcessThread, pthread_self() != 0);
    }

    bool canAssignDevice() const noexcept override
    {
        const MutexLocker cml(fDeviceMutex);
        return fCurrentDevice == nullptr;
    }

    void assignDevice(rack::audio::Device* const dev) noexcept override
    {
        DISTRHO_SAFE_ASSERT_RETURN(fCurrentDevice == nullptr,);

        const MutexLocker cml(fDeviceMutex);
        fCurrentDevice = dev;
    }

    bool clearDevice(rack::audio::Device* const dev) noexcept override
    {
        const MutexLocker cml(fDeviceMutex);

        if (fCurrentDevice != dev)
            return false;

        fCurrentDevice = nullptr;
        return true;
    }

    void addMidiInput(CardinalMidiInputDevice* const dev) override
    {
        const MutexLocker cml(fDeviceMutex);

        fMidiInputs.push_back(dev);
    }

    void removeMidiInput(CardinalMidiInputDevice* const dev) override
    {
        const MutexLocker cml(fDeviceMutex);

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
        case kWindowParameterCableOpacity:
            parameter.name = "Cable Opacity";
            parameter.symbol = "cableOpacity";
            parameter.unit = "%";
            parameter.hints = kParameterIsAutomable;
            parameter.ranges.def = 50.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 100.0f;
            break;
        case kWindowParameterCableTension:
            parameter.name = "Cable Tension";
            parameter.symbol = "cableTension";
            parameter.unit = "%";
            parameter.hints = kParameterIsAutomable;
            parameter.ranges.def = 50.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 100.0f;
            break;
        case kWindowParameterRackBrightness:
            parameter.name = "Rack Brightness";
            parameter.symbol = "rackBrightness";
            parameter.unit = "%";
            parameter.hints = kParameterIsAutomable;
            parameter.ranges.def = 100.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 100.0f;
            break;
        case kWindowParameterHaloBrightness:
            parameter.name = "Halo Brightness";
            parameter.symbol = "haloBrightness";
            parameter.unit = "%";
            parameter.hints = kParameterIsAutomable;
            parameter.ranges.def = 25.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 100.0f;
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

            if (fCurrentDevice != nullptr)
                fCurrentDevice->onStartStream();
        }
    }

    void deactivate() override
    {
        {
            const MutexLocker cml(fDeviceMutex);

            if (fCurrentDevice != nullptr)
                fCurrentDevice->onStopStream();
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

        fProcessThread = pthread_self();

        const MutexLocker cml(fDeviceMutex);
        // const MutexTryLocker cmtl(fPatchMutex);

        if (fCurrentDevice == nullptr /*|| cmtl.wasNotLocked()*/)
        {
            std::memset(outputs[0], 0, sizeof(float)*frames);
            std::memset(outputs[1], 0, sizeof(float)*frames);
            return;
        }

        for (uint32_t i=0, j=0; i<frames; ++i)
        {
            fAudioBufferIn[j++] = inputs[0][i];
            fAudioBufferIn[j++] = inputs[1][i];
        }

        std::memset(fAudioBufferOut, 0, sizeof(float)*frames*DISTRHO_PLUGIN_NUM_OUTPUTS);

        fIsProcessing.store(1);

        for (CardinalMidiInputDevice* dev : fMidiInputs)
            dev->handleMessagesFromHost(midiEvents, midiEventCount);

        fCurrentDevice->processBuffer(fAudioBufferIn, DISTRHO_PLUGIN_NUM_INPUTS,
                                      fAudioBufferOut, DISTRHO_PLUGIN_NUM_OUTPUTS, frames);
        fIsProcessing.store(0);

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
