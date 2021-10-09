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

#include <app/common.hpp>
#include <app/Scene.hpp>
#include <context.hpp>
#include <engine/Engine.hpp>
#include <network.hpp>
#include <patch.hpp>
#include <ui/common.hpp>
#include <window/Window.hpp>

#ifdef NDEBUG
# undef DEBUG
#endif
#include "DistrhoUI.hpp"
#include "ResizeHandle.hpp"

namespace rack {
namespace network {
    std::string encodeUrl(const std::string&) { return {}; }
    json_t* requestJson(Method, const std::string&, json_t*, const CookieMap&) { return nullptr; }
    bool requestDownload(const std::string&, const std::string&, float*, const CookieMap&) { return false; }
}
}

#ifdef DPF_AS_GLFW
GLFWAPI const char* glfwGetClipboardString(GLFWwindow* window) { return nullptr; }
GLFWAPI void glfwSetClipboardString(GLFWwindow* window, const char*) {}
GLFWAPI const char* glfwGetKeyName(int key, int scancode) { return nullptr; }
GLFWAPI int glfwGetKeyScancode(int key) { return 0; }

namespace rack {
namespace window {
    DISTRHO_NAMESPACE::UI* lastUI = nullptr;

    void mouseButtonCallback(Window* win, int button, int action, int mods);
    void cursorPosCallback(Window* win, double xpos, double ypos);
    void scrollCallback(Window* win, double x, double y);
}
}
#endif

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------------------------------------

struct Initializer2 {
    Initializer2()
    {
        using namespace rack;

    }

    ~Initializer2()
    {
        using namespace rack;

    }
};

static const Initializer2& getInitializer2Instance()
{
    static const Initializer2 init;
    return init;
}

class CardinalUI : public UI
{
    ResizeHandle fResizeHandle;
public:
    CardinalUI()
        : UI(1280, 720),
          fResizeHandle(this)
    {
        using namespace rack;

        /*
           The following code was based from VCVRack adapters/standalone.cpp

           Copyright (C) 2016-2021 VCV

           This program is free software: you can redistribute it and/or modify it under the terms of the
           GNU General Public License as published by the Free Software Foundation, either version 3 of the
           License, or (at your option) any later version.
        */
        // Initialize context
        d_stdout("UI context ptr %p", NanoVG::getContext());
#ifdef DPF_AS_GLFW
        window::lastUI = this;
#endif
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
#ifdef DPF_AS_GLFW
        window::lastUI = nullptr;
#endif

    	APP->engine->startFallbackThread();
    }

    ~CardinalUI() override
    {
        using namespace rack;

	    delete APP;
	    contextSet(NULL);
    }

    void onNanoDisplay() override
    {
		APP->window->step();
    }

    void uiIdle() override
    {
        repaint();
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

#ifdef DPF_AS_GLFW
    bool onMouse(const MouseEvent& ev) override
    {
        int button;
        int mods = 0;
        int action = ev.press;

        if (ev.mod & kModifierControl)
            mods |= GLFW_MOD_CONTROL;
        if (ev.mod & kModifierShift)
            mods |= GLFW_MOD_SHIFT;
        if (ev.mod & kModifierAlt)
            mods |= GLFW_MOD_ALT;

#ifdef DISTRHO_OS_MAC
        switch (ev.button)
        {
        case 1:
            button = GLFW_MOUSE_BUTTON_LEFT;
            break;
        case 2:
            button = GLFW_MOUSE_BUTTON_RIGHT;
            break;
        case 3:
            button = GLFW_MOUSE_BUTTON_MIDDLE;
            break;
        default:
            button = 0;
            break;
        }
#else
        switch (ev.button)
        {
        case 1:
            button = GLFW_MOUSE_BUTTON_LEFT;
            break;
        case 2:
            button = GLFW_MOUSE_BUTTON_MIDDLE;
            break;
        case 3:
            button = GLFW_MOUSE_BUTTON_RIGHT;
            break;
//         case 4:
//             button = GLFW_MOUSE_WHEELUP;
//             break;
//         case 5:
//             button = GLFW_MOUSE_WHEELDOWN;
//             break;
        default:
            button = 0;
            break;
        }
#endif

        mouseButtonCallback(APP->window, button, action, mods);
        return true;
    }

    bool onMotion(const MotionEvent& ev) override
    {
        cursorPosCallback(APP->window, ev.pos.getX(), ev.pos.getY());
        return true;
    }

    bool onScroll(const ScrollEvent& ev) override
    {
        scrollCallback(APP->window, ev.delta.getX(), ev.delta.getY());
        return true;
    }
#endif

    #if 0
    void onResize(const ResizeEvent& ev) override
    {
        UI::onResize(ev);
        // APP->window->setSize(rack::math::Vec(ev.size.getWidth(), ev.size.getHeight()));
    }
    #endif

    // TODO uiFocus

private:
   /**
      Set our UI class as non-copyable and add a leak detector just in case.
    */
    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CardinalUI)
};

/* ------------------------------------------------------------------------------------------------------------
 * UI entry point, called by DPF to create a new UI instance. */

UI* createUI()
{
    getInitializer2Instance();
    return new CardinalUI();
}

// -----------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
