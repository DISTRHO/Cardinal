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

#include <app/common.hpp>
#include <app/Scene.hpp>
#include <context.hpp>
#include <engine/Engine.hpp>
#include <patch.hpp>
#include <ui/common.hpp>
#include <window/Window.hpp>

#include "DistrhoUI.hpp"

GLFWAPI const char* glfwGetClipboardString(GLFWwindow* window) { return nullptr; }
GLFWAPI void glfwSetClipboardString(GLFWwindow* window, const char*) {}
GLFWAPI const char* glfwGetKeyName(int key, int scancode) { return nullptr; }
GLFWAPI int glfwGetKeyScancode(int key) { return 0; }

namespace rack {
namespace window {
    DISTRHO_NAMESPACE::UI* lastUI = nullptr;
}
}

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------------------------------------

struct Initializer {
    Initializer()
    {
        using namespace rack;

		ui::init();
		window::init();
    }

    ~Initializer()
    {
        using namespace rack;

		window::destroy();
		ui::destroy();
    }
};

static Initializer& getInitializerInstance()
{
    static Initializer init;
    return init;
}

class CVCRackUI : public UI
{
public:
    CVCRackUI()
        : UI(1280, 720)
    {
        using namespace rack;

        // Initialize context
        INFO("Initializing context");
        window::lastUI = this;
        contextSet(new Context);
        APP->engine = new engine::Engine;
        APP->history = new history::State;
        APP->event = new widget::EventState;
        APP->scene = new app::Scene;
        APP->event->rootWidget = APP->scene;
        APP->patch = new patch::Manager;
        /*if (!settings::headless)*/ {
            APP->window = new window::Window;
        }
        window::lastUI = nullptr;

    	APP->engine->startFallbackThread();
    }

    ~CVCRackUI() override
    {
        using namespace rack;

	    delete APP;
	    contextSet(NULL);
    }

    void onNanoDisplay() override
    {
		APP->window->step();
    }

protected:
   /* --------------------------------------------------------------------------------------------------------
    * DSP/Plugin Callbacks */

   /**
      A parameter has changed on the plugin side.
      This is called by the host to inform the UI about parameter changes.
    */
    void parameterChanged(uint32_t index, float value) override
    {
    }

    // -------------------------------------------------------------------------------------------------------

private:
   /**
      Set our UI class as non-copyable and add a leak detector just in case.
    */
    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CVCRackUI)
};

/* ------------------------------------------------------------------------------------------------------------
 * UI entry point, called by DPF to create a new UI instance. */

UI* createUI()
{
    getInitializerInstance();
    return new CVCRackUI();
}

// -----------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
