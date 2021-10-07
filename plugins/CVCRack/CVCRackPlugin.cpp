/*
 * DISTRHO CVCRack Plugin
 * Copyright (C) 2021 Filipe Coelho <falktx@falktx.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any purpose with
 * or without fee is hereby granted, provided that the above copyright notice and this
 * permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
 * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <asset.hpp>
#include <audio.hpp>
#include <context.hpp>
#include <library.hpp>
#include <keyboard.hpp>
#include <midi.hpp>
#include <plugin.hpp>
#include <random.hpp>
#include <settings.hpp>
#include <system.hpp>

#include <osdialog.h>

#include "DistrhoPlugin.hpp"

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------------------------------------

struct Initializer {
    Initializer()
    {
        using namespace rack;

        settings::devMode = true;
        system::init();
        asset::init();
        logger::init();
        random::init();

        // Log environment
        INFO("%s %s v%s", APP_NAME.c_str(), APP_EDITION.c_str(), APP_VERSION.c_str());
        INFO("%s", system::getOperatingSystemInfo().c_str());
        INFO("System directory: %s", asset::systemDir.c_str());
        INFO("User directory: %s", asset::userDir.c_str());
        INFO("System time: %s", string::formatTimeISO(system::getUnixTime()).c_str());

        // Load settings
        settings::init();
        try {
            settings::load();
        }
        catch (Exception& e) {
            std::string message = e.what();
            message += "\n\nResetting settings to default";
            d_stdout(message.c_str());
            /*
            if (!osdialog_message(OSDIALOG_WARNING, OSDIALOG_OK_CANCEL, msg.c_str())) {
                exit(1);
            }
            */
        }

        // Check existence of the system res/ directory
        std::string resDir = asset::system("res");
        if (!system::isDirectory(resDir)) {
            std::string message = string::f("Rack's resource directory \"%s\" does not exist. Make sure Rack is correctly installed and launched.", resDir.c_str());
            d_stderr2(message.c_str());
            /*
            osdialog_message(OSDIALOG_ERROR, OSDIALOG_OK, message.c_str());
            */
            exit(1);
        }

        INFO("Initializing environment");
        // network::init();
        audio::init();
        // rtaudioInit();
        midi::init();
        // rtmidiInit();
        keyboard::init();
        plugin::init();
        library::init();
        // discord::init();
    }

    ~Initializer()
    {
        using namespace rack;

        // discord::destroy();
        library::destroy();
        midi::destroy();
        audio::destroy();
        plugin::destroy();
	    INFO("Destroying logger");
	    logger::destroy();
    }
};

static Initializer& getInitializerInstance()
{
    static Initializer init;
    return init;
}

/**
  Plugin to demonstrate parameter outputs using meters.
 */
class CVCRackPlugin : public Plugin
{
public:
    CVCRackPlugin()
        : Plugin(0, 0, 0)
    {
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
        return "CVCRack";
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
        return "https://github.com/DISTRHO/CVCRack";
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
        return d_cconst('d', 'C', 'V', 'C');
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
        // copy inputs over outputs if needed
        if (outputs[0] != inputs[0])
            std::memcpy(outputs[0], inputs[0], sizeof(float)*frames);

        if (outputs[1] != inputs[1])
            std::memcpy(outputs[1], inputs[1], sizeof(float)*frames);
    }

    // -------------------------------------------------------------------------------------------------------

private:
   /**
      Set our plugin class as non-copyable and add a leak detector just in case.
    */
    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CVCRackPlugin)
};

/* ------------------------------------------------------------------------------------------------------------
 * Plugin entry point, called by DPF to create a new plugin instance. */

Plugin* createPlugin()
{
    getInitializerInstance();
    return new CVCRackPlugin();
}

// -----------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
