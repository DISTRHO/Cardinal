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
#include <patch.hpp>
#include <ui/common.hpp>
#include <window/Window.hpp>

#include "PluginContext.hpp"

#include "DistrhoUI.hpp"
#include "ResizeHandle.hpp"

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

CardinalPluginContext* getRackContextFromPlugin(void* ptr);

class CardinalUI : public UI
{
    CardinalPluginContext* const fContext;
    rack::math::Vec fLastMousePos;
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

        fContext->event = new rack::widget::EventState;
        fContext->scene = new rack::app::Scene;
        fContext->event->rootWidget = fContext->scene;

        // Initialize context
        d_stdout("UI context ptr %p", NanoVG::getContext());
        rack::window::lastUI = this;
        fContext->window = new rack::window::Window;
        rack::window::lastUI = nullptr;

        // we need to reload current patch for things to show on screen :(
        // FIXME always save
        if (! fContext->patch->hasAutosave())
            fContext->patch->saveAutosave();
        fContext->patch->loadAutosave();
    }

    ~CardinalUI() override
    {
        const ScopedContext sc(this);

        delete fContext->window;
        fContext->window = nullptr;

        delete fContext->scene;
        fContext->scene = nullptr;

        delete fContext->event;
        fContext->event = nullptr;
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
        int action = ev.press ? GLFW_PRESS : GLFW_RELEASE;

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

        /*
        #if defined ARCH_MAC
        // Remap Ctrl-left click to right click on Mac
        if (button == GLFW_MOUSE_BUTTON_LEFT && (mods & RACK_MOD_MASK) == GLFW_MOD_CONTROL) {
            button = GLFW_MOUSE_BUTTON_RIGHT;
            mods &= ~GLFW_MOD_CONTROL;
        }
        // Remap Ctrl-shift-left click to middle click on Mac
        if (button == GLFW_MOUSE_BUTTON_LEFT && (mods & RACK_MOD_MASK) == (GLFW_MOD_CONTROL | GLFW_MOD_SHIFT)) {
            button = GLFW_MOUSE_BUTTON_MIDDLE;
            mods &= ~(GLFW_MOD_CONTROL | GLFW_MOD_SHIFT);
        }
        #endif
        */

        return fContext->event->handleButton(fLastMousePos, button, action, mods);
    }

    bool onMotion(const MotionEvent& ev) override
    {
        const ScopedContext sc(this);

        rack::math::Vec mousePos = rack::math::Vec(ev.pos.getX(), ev.pos.getY()).div(getScaleFactor()).round();
        // .div(ctx->window->pixelRatio / ctx->window->windowRatio).round();
        rack::math::Vec mouseDelta = mousePos.minus(fLastMousePos);

        /*
        // Workaround for GLFW warping mouse to a different position when the cursor is locked or unlocked.
        if (ctx->window->internal->ignoreNextMouseDelta)
        {
            ctx->window->internal->ignoreNextMouseDelta = false;
            mouseDelta = rack::math::Vec();
        }
        */

        fLastMousePos = mousePos;
        return fContext->event->handleHover(mousePos, mouseDelta);
    }

    bool onScroll(const ScrollEvent& ev) override
    {
        const ScopedContext sc(this);

        rack::math::Vec scrollDelta = rack::math::Vec(ev.delta.getX(), ev.delta.getY());
#ifdef DISTRHO_OS_MAC
        scrollDelta = scrollDelta.mult(10.0);
#else
        scrollDelta = scrollDelta.mult(50.0);
#endif
        return fContext->event->handleScroll(fLastMousePos, scrollDelta);
    }

    bool onCharacterInput(const CharacterInputEvent& ev) override
    {
        if (ev.character <= ' ' || ev.character >= kKeyDelete)
            return false;

        const ScopedContext sc(this);

        return fContext->event->handleText(fLastMousePos, ev.character);
    }

    bool onKeyboard(const KeyboardEvent& ev) override
    {
        const ScopedContext sc(this);

        int key;
        int mods = 0;
        int action = ev.press ? GLFW_PRESS : GLFW_RELEASE;

        /* These are unsupported in pugl right now
        #define GLFW_KEY_KP_0               320
        #define GLFW_KEY_KP_1               321
        #define GLFW_KEY_KP_2               322
        #define GLFW_KEY_KP_3               323
        #define GLFW_KEY_KP_4               324
        #define GLFW_KEY_KP_5               325
        #define GLFW_KEY_KP_6               326
        #define GLFW_KEY_KP_7               327
        #define GLFW_KEY_KP_8               328
        #define GLFW_KEY_KP_9               329
        #define GLFW_KEY_KP_DECIMAL         330
        #define GLFW_KEY_KP_DIVIDE          331
        #define GLFW_KEY_KP_MULTIPLY        332
        #define GLFW_KEY_KP_SUBTRACT        333
        #define GLFW_KEY_KP_ADD             334
        #define GLFW_KEY_KP_ENTER           335
        #define GLFW_KEY_KP_EQUAL           336
        */

        switch (ev.key)
        {
        case '\r': key = GLFW_KEY_ENTER; break;
        case '\t': key = GLFW_KEY_TAB; break;
        case kKeyBackspace: key = GLFW_KEY_BACKSPACE; break;
        case kKeyEscape: key = GLFW_KEY_ESCAPE; break;
        case kKeyDelete: key = GLFW_KEY_DELETE; break;
        case kKeyF1: key = GLFW_KEY_F1; break;
        case kKeyF2: key = GLFW_KEY_F2; break;
        case kKeyF3: key = GLFW_KEY_F3; break;
        case kKeyF4: key = GLFW_KEY_F4; break;
        case kKeyF5: key = GLFW_KEY_F5; break;
        case kKeyF6: key = GLFW_KEY_F6; break;
        case kKeyF7: key = GLFW_KEY_F7; break;
        case kKeyF8: key = GLFW_KEY_F8; break;
        case kKeyF9: key = GLFW_KEY_F9; break;
        case kKeyF10: key = GLFW_KEY_F10; break;
        case kKeyF11: key = GLFW_KEY_F11; break;
        case kKeyF12: key = GLFW_KEY_F12; break;
        case kKeyLeft: key = GLFW_KEY_LEFT; break;
        case kKeyUp: key = GLFW_KEY_UP; break;
        case kKeyRight: key = GLFW_KEY_RIGHT; break;
        case kKeyDown: key = GLFW_KEY_DOWN; break;
        case kKeyPageUp: key = GLFW_KEY_PAGE_UP; break;
        case kKeyPageDown: key = GLFW_KEY_PAGE_DOWN; break;
        case kKeyHome: key = GLFW_KEY_HOME; break;
        case kKeyEnd: key = GLFW_KEY_END; break;
        case kKeyInsert: key = GLFW_KEY_INSERT; break;
        case kKeyShiftL: key = GLFW_KEY_LEFT_SHIFT; break;
        case kKeyShiftR: key = GLFW_KEY_RIGHT_SHIFT; break;
        case kKeyControlL: key = GLFW_KEY_LEFT_CONTROL; break;
        case kKeyControlR: key = GLFW_KEY_RIGHT_CONTROL; break;
        case kKeyAltL: key = GLFW_KEY_LEFT_ALT; break;
        case kKeyAltR: key = GLFW_KEY_RIGHT_ALT; break;
        case kKeySuperL: key = GLFW_KEY_LEFT_SUPER; break;
        case kKeySuperR: key = GLFW_KEY_RIGHT_SUPER; break;
        case kKeyMenu: key = GLFW_KEY_MENU; break;
        case kKeyCapsLock: key = GLFW_KEY_CAPS_LOCK; break;
        case kKeyScrollLock: key = GLFW_KEY_SCROLL_LOCK; break;
        case kKeyNumLock: key = GLFW_KEY_NUM_LOCK; break;
        case kKeyPrintScreen: key = GLFW_KEY_PRINT_SCREEN; break;
        case kKeyPause: key = GLFW_KEY_PAUSE; break;
        default: key = ev.key; break;
        }

        if (ev.mod & kModifierControl)
            mods |= GLFW_MOD_CONTROL;
        if (ev.mod & kModifierShift)
            mods |= GLFW_MOD_SHIFT;
        if (ev.mod & kModifierAlt)
            mods |= GLFW_MOD_ALT;

        return fContext->event->handleKey(fLastMousePos, key, ev.keycode, action, mods);
    }

    void uiFocus(const bool focus, CrossingMode) override
    {
        const ScopedContext sc(this);

        if (! focus)
            fContext->event->handleLeave();
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
