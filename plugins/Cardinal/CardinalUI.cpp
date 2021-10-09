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
#include <ui/common.hpp>
#include <window/Window.hpp>

#ifdef NDEBUG
# undef DEBUG
#endif
#include "DistrhoUI.hpp"
#include "ResizeHandle.hpp"

GLFWAPI const char* glfwGetClipboardString(GLFWwindow* window) { return nullptr; }
GLFWAPI void glfwSetClipboardString(GLFWwindow* window, const char*) {}
GLFWAPI const char* glfwGetKeyName(int key, int scancode) { return nullptr; }
GLFWAPI int glfwGetKeyScancode(int key) { return 0; }

namespace rack {
namespace window {
    DISTRHO_NAMESPACE::UI* lastUI = nullptr;

    void mouseButtonCallback(Context* ctx, int button, int action, int mods);
    void cursorPosCallback(Context* ctx, double xpos, double ypos);
    void cursorEnterCallback(Context* ctx, int entered);
    void scrollCallback(Context* ctx, double x, double y);
    void charCallback(Context* ctx, unsigned int codepoint);
    void keyCallback(Context* ctx, int key, int scancode, int action, int mods);
}
}

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------------------------------------

rack::Context* getRackContextFromPlugin(void* ptr);

class CardinalUI : public UI
{
    rack::Context* const fContext;
    ResizeHandle fResizeHandle;

    struct ScopedContext {
        ScopedContext(CardinalUI* const ui)
        {
            rack::contextSet(ui->fContext);
        }

        ~ScopedContext()
        {
	        rack::contextSet(nullptr);
        }
    };

public:
    CardinalUI()
        : UI(1280, 720),
          fContext(getRackContextFromPlugin(getPluginInstancePointer())),
          fResizeHandle(this)
    {
        const ScopedContext sc(this);

        // Initialize context
        d_stdout("UI context ptr %p", NanoVG::getContext());
        rack::window::lastUI = this;
        fContext->window = new rack::window::Window;
        rack::window::lastUI = nullptr;
    }

    ~CardinalUI() override
    {
        const ScopedContext sc(this);

        delete fContext->window;
        fContext->window = nullptr;
    }

    void onNanoDisplay() override
    {
        const ScopedContext sc(this);

		fContext->window->step();
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

    bool onMouse(const MouseEvent& ev) override
    {
        const ScopedContext sc(this);

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

        rack::window::mouseButtonCallback(fContext, button, action, mods);
        return true;
    }

    bool onMotion(const MotionEvent& ev) override
    {
        const ScopedContext sc(this);

        rack::window::cursorPosCallback(fContext, ev.pos.getX(), ev.pos.getY());
        return true;
    }

    bool onScroll(const ScrollEvent& ev) override
    {
        const ScopedContext sc(this);

        rack::window::scrollCallback(fContext, ev.delta.getX(), ev.delta.getY());
        return true;
    }

    #if 0
    void onResize(const ResizeEvent& ev) override
    {
        UI::onResize(ev);
        // APP->window->setSize(rack::math::Vec(ev.size.getWidth(), ev.size.getHeight()));
    }
    #endif

    // TODO uiFocus

    bool onCharacterInput(const CharacterInputEvent& ev) override
    {
        if (ev.character == 0)
            return false;

        const ScopedContext sc(this);

        rack::window::charCallback(fContext, ev.character);
        return true;
    }

    bool onKeyboard(const KeyboardEvent& ev) override
    {
        const ScopedContext sc(this);

        int mods = 0;
        int action = ev.press;

        if (ev.mod & kModifierControl)
            mods |= GLFW_MOD_CONTROL;
        if (ev.mod & kModifierShift)
            mods |= GLFW_MOD_SHIFT;
        if (ev.mod & kModifierAlt)
            mods |= GLFW_MOD_ALT;

        // TODO special key conversion
        rack::window::keyCallback(fContext, ev.key, ev.keycode, action, mods);
        return true;
    }

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
    return new CardinalUI();
}

// -----------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
