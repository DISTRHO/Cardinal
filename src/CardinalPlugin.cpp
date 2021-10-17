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
#include <audio.hpp>
#include <context.hpp>
#include <library.hpp>
#include <midi.hpp>
#include <patch.hpp>
#include <plugin.hpp>
#include <random.hpp>
// #include <rtaudio.hpp>
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

#include "DistrhoPlugin.hpp"

namespace rack {
namespace plugin {
void initStaticPlugins();
void destroyStaticPlugins();
}
}

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------------------------------------
// The following code was based from VCVRack adapters/standalone.cpp

/*
   Copyright (C) 2016-2021 VCV

   This program is free software: you can redistribute it and/or modify it under the terms of the
   GNU General Public License as published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.
*/

struct Initializer {
    Initializer()
    {
        using namespace rack;

        settings::autoCheckUpdates = false;
        settings::autosaveInterval = 0;
        settings::discordUpdateActivity = false;
        settings::isPlugin = true;
        settings::skipLoadOnLaunch = true;
        settings::showTipsOnLaunch = false;
        settings::threadCount = 1;
        system::init();
        asset::init();
        logger::init();
        random::init();

        // Make system dir point to source code location. It is good enough for now
        asset::systemDir = CARDINAL_PLUGIN_SOURCE_DIR DISTRHO_OS_SEP_STR "Rack";

        // Log environment
        INFO("%s %s v%s", APP_NAME.c_str(), APP_EDITION.c_str(), APP_VERSION.c_str());
        INFO("%s", system::getOperatingSystemInfo().c_str());
        INFO("System directory: %s", asset::systemDir.c_str());
        INFO("User directory: %s", asset::userDir.c_str());
        INFO("System time: %s", string::formatTimeISO(system::getUnixTime()).c_str());

        // Check existence of the system res/ directory
        const std::string resDir = asset::system("res");
        if (! system::isDirectory(resDir))
        {
            d_stderr2("Resource directory \"%s\" does not exist.\n"
                      "Make sure Cardinal was downloaded and installed correctly.", resDir.c_str());
        }

        INFO("Initializing environment");
        audio::init(); // does nothing
        midi::init(); // does nothing
        // rtaudioInit();
        plugin::initStaticPlugins();
        ui::init();
    }

    ~Initializer()
    {
        using namespace rack;

        ui::destroy(); // does nothing

        INFO("Destroying plugins");
        plugin::destroyStaticPlugins();

        midi::destroy();
        audio::destroy();
        INFO("Destroying logger");
        logger::destroy();
    }
};

static const Initializer& getInitializerInstance()
{
    static const Initializer init;
    return init;
}

// -----------------------------------------------------------------------------------------------------------

class CardinalPlugin : public Plugin
{
    rack::Context* const fContext;
    std::string fAutosavePath;

    struct ScopedContext {
        ScopedContext(CardinalPlugin* const plugin)
        {
            rack::contextSet(plugin->fContext);
        }

        ~ScopedContext()
        {
            rack::contextSet(nullptr);
        }
    };

public:
    CardinalPlugin()
        : Plugin(0, 0, 0),
          fContext(new rack::Context)
    {
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

        // create temporary path
        const ScopedContext sc(this);

        fContext->engine = new rack::engine::Engine;
        fContext->history = new rack::history::State;
        fContext->patch = new rack::patch::Manager;
        fContext->patch->autosavePath = fAutosavePath;
        fContext->engine->startFallbackThread();
    }

    ~CardinalPlugin() override
    {
        {
            const ScopedContext sc(this);
            delete fContext;
        }

        if (! fAutosavePath.empty())
            rack::system::removeRecursively(fAutosavePath);
    }

    rack::Context* getRackContext() const noexcept
    {
        return fContext;
    }

protected:
   /* --------------------------------------------------------------------------------------------------------
    * Information */

   /**
      Get the plugin label.
      A plugin label follows the same rules as Parameter::symbol, with the exception that it can start with numbers.
    */
    const char* getLabel() const override
    {
        return "Cardinal";
    }

   /**
      Get an extensive comment/description about the plugin.
    */
    const char* getDescription() const override
    {
        return "...";
    }

   /**
      Get the plugin author/maker.
    */
    const char* getMaker() const override
    {
        return "DISTRHO";
    }

   /**
      Get the plugin homepage.
    */
    const char* getHomePage() const override
    {
        return "https://github.com/DISTRHO/Cardinal";
    }

   /**
      Get the plugin license name (a single line of text).
      For commercial plugins this should return some short copyright information.
    */
    const char* getLicense() const override
    {
        return "ISC";
    }

   /**
      Get the plugin version, in hexadecimal.
    */
    uint32_t getVersion() const override
    {
        return d_version(1, 0, 0);
    }

   /**
      Get the plugin unique Id.
      This value is used by LADSPA, DSSI and VST plugin formats.
    */
    int64_t getUniqueId() const override
    {
        return d_cconst('d', 'C', 'd', 'n');
    }

   /* --------------------------------------------------------------------------------------------------------
    * Init */

   /* --------------------------------------------------------------------------------------------------------
    * Internal data */

   /* --------------------------------------------------------------------------------------------------------
    * Process */

   /**
      Run/process function for plugins without MIDI input.
    */
    void run(const float** inputs, float** outputs, uint32_t frames) override
    {
        /*
        fContext->engine->setFrame(getTimePosition().frame);
        fContext->engine->stepBlock(frames);
        */

        // copy inputs over outputs if needed
        if (outputs[0] != inputs[0])
            std::memcpy(outputs[0], inputs[0], sizeof(float)*frames);

        if (outputs[1] != inputs[1])
            std::memcpy(outputs[1], inputs[1], sizeof(float)*frames);
    }

    /*
    void sampleRateChanged(const double newSampleRate) override
    {
        fContext->engine->setSampleRate(newSampleRate);
    }
    */

    // -------------------------------------------------------------------------------------------------------

private:
   /**
      Set our plugin class as non-copyable and add a leak detector just in case.
    */
    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CardinalPlugin)
};

rack::Context* getRackContextFromPlugin(void* const ptr)
{
    return static_cast<CardinalPlugin*>(ptr)->getRackContext();
}

/* ------------------------------------------------------------------------------------------------------------
 * Plugin entry point, called by DPF to create a new plugin instance. */

Plugin* createPlugin()
{
    getInitializerInstance();
    return new CardinalPlugin();
}

// -----------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
