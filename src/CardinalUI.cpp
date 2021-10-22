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

#include <app/Scene.hpp>
#include <context.hpp>
#include <helpers.hpp>
#include <patch.hpp>
#include <settings.hpp>
#include <ui/Button.hpp>
#include <ui/MenuItem.hpp>
#include <ui/MenuSeparator.hpp>
#include <window/Window.hpp>

#ifdef NDEBUG
# undef DEBUG
#endif

#include "DistrhoUI.hpp"
#include "PluginContext.hpp"
#include "WindowParameters.hpp"
#include "ResizeHandle.hpp"

GLFWAPI const char* glfwGetClipboardString(GLFWwindow* window) { return nullptr; }
GLFWAPI void glfwSetClipboardString(GLFWwindow* window, const char*) {}
GLFWAPI const char* glfwGetKeyName(int key, int scancode) { return nullptr; }
GLFWAPI int glfwGetKeyScancode(int key) { return 0; }

namespace rack {
namespace window {
    void WindowInit(Window* window, DISTRHO_NAMESPACE::UI* ui);
    void WindowMods(Window* window, int mods);
}
}

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------------------------------------

struct CardinalMenuButton : rack::ui::Button {
    void step() override {
        box.size.x = bndLabelWidth(APP->window->vg, -1, text.c_str()) + 1.0;
        Widget::step();
    }
    void draw(const DrawArgs& args) override {
        BNDwidgetState state = BND_DEFAULT;
        if (APP->event->hoveredWidget == this)
            state = BND_HOVER;
        if (APP->event->draggedWidget == this)
            state = BND_ACTIVE;
        bndMenuItem(args.vg, 0.0, 0.0, box.size.x, box.size.y, state, -1, text.c_str());
        Widget::draw(args);
    }
};

struct CardinalFileButton : CardinalMenuButton {
    CardinalFileButton()
        : CardinalMenuButton()
    {
        text = "File";
    }

    void onAction(const ActionEvent& e) override {
        rack::ui::Menu* menu = rack::createMenu();
        menu->cornerFlags = BND_CORNER_TOP;
        menu->box.pos = getAbsoluteOffset(rack::math::Vec(0, box.size.y));

        menu->addChild(rack::createMenuItem("New", RACK_MOD_CTRL_NAME "+N", []() {
            APP->patch->loadTemplateDialog();
        }));

        menu->addChild(rack::createMenuItem("Open", RACK_MOD_CTRL_NAME "+O", []() {
            APP->patch->loadDialog();
        }));

        /*
        menu->addChild(rack::createMenuItem("Save", RACK_MOD_CTRL_NAME "+S", []() {
            APP->patch->saveDialog();
        }));

        menu->addChild(rack::createMenuItem("Save as", RACK_MOD_CTRL_NAME "+Shift+S", []() {
            APP->patch->saveAsDialog();
        }));
        */

        menu->addChild(rack::createMenuItem("Revert", RACK_MOD_CTRL_NAME "+" RACK_MOD_SHIFT_NAME "+O", []() {
            APP->patch->revertDialog();
        }, APP->patch->path == ""));

        menu->addChild(new rack::ui::MenuSeparator);

        menu->addChild(rack::createMenuItem("Quit", RACK_MOD_CTRL_NAME "+Q", []() {
            APP->window->close();
        }));
    }
};

// -----------------------------------------------------------------------------------------------------------

CardinalPluginContext* getRackContextFromPlugin(void* ptr);

// -----------------------------------------------------------------------------------------------------------

class CardinalUI : public UI,
                   public WindowParametersCallback
{
    CardinalPluginContext* const fContext;
    rack::math::Vec fLastMousePos;
    ResizeHandle fResizeHandle;
    WindowParameters fWindowParameters;

    struct ScopedContext {
        CardinalPluginContext* const context;
        const MutexLocker cml;

        ScopedContext(CardinalUI* const ui)
            : context(ui->fContext),
              cml(context->mutex)
        {
            rack::contextSet(context);
            WindowParametersRestore(context->window);
        }

        ScopedContext(CardinalUI* const ui, const int mods)
            : context(ui->fContext),
              cml(context->mutex)
        {
            rack::contextSet(context);
            rack::window::WindowMods(context->window, mods);
            WindowParametersRestore(context->window);
        }

        ~ScopedContext()
        {
            if (context->window != nullptr)
                WindowParametersSave(context->window);
            rack::contextSet(nullptr);
        }
    };

public:
    CardinalUI()
        : UI(1228, 666),
          fContext(getRackContextFromPlugin(getPluginInstancePointer())),
          fResizeHandle(this)
    {
        if (isResizable())
            fResizeHandle.hide();

        const double scaleFactor = getScaleFactor();

        if (scaleFactor != 1)
            setSize(1228 * scaleFactor, 666 * scaleFactor);

        fContext->window = new rack::window::Window;

        {
            const ScopedContext sc(this);

            rack::window::WindowInit(fContext->window, this);

            // Hide non-wanted menu entries
            typedef rack::ui::Button rButton;
            // typedef rack::ui::MenuItem rMenuItem;
            typedef rack::widget::Widget rWidget;
            typedef std::list<rWidget*>::iterator rWidgetIterator;

            rWidget* const layout = fContext->scene->menuBar->children.front();

            const auto removeMenu = [layout](const char* const name) -> void
            {
                for (rWidgetIterator it = layout->children.begin(); it != layout->children.end(); ++it)
                {
                    if (rButton* const button = reinterpret_cast<rButton*>(*it))
                    {
                        if (button->text == name)
                        {
                            layout->removeChild(button);
                            // button->parent = nullptr;
                            delete button;
                            break;
                        }
                    }
                }
            };

            removeMenu("File");
            removeMenu("Library");

            layout->addChildBottom(new CardinalFileButton());

            /* FIXME this doesnt work
            if (button->text == "Engine")
            {
                for (rWidgetIterator it2 = button->children.begin(); it2 != button->children.end(); ++it2)
                {
                    if (rMenuItem* const item = reinterpret_cast<rMenuItem*>(*it2))
                    {
                        if (item->text == "Sample rate")
                        {
                            button->children.erase(it2);
                            delete button;
                            break;
                        }
                    }
                }
            }
            */
        }

        WindowParametersSetCallback(fContext->window, this);
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

    void WindowParametersChanged(const WindowParameterList param, float value) override
    {
        float mult = 1.0f;

        switch (param)
        {
        case kWindowParameterShowTooltips:
            fWindowParameters.tooltips = value > 0.5f;
            break;
        case kWindowParameterCableOpacity:
            mult = 100.0f;
            fWindowParameters.cableOpacity = value;
            break;
        case kWindowParameterCableTension:
            mult = 100.0f;
            fWindowParameters.cableTension = value;
            break;
        case kWindowParameterRackBrightness:
            mult = 100.0f;
            fWindowParameters.rackBrightness = value;
            break;
        case kWindowParameterHaloBrightness:
            mult = 100.0f;
            fWindowParameters.haloBrightness = value;
            break;
        case kWindowParameterKnobMode:
            switch (static_cast<int>(value + 0.5f))
            {
            case rack::settings::KNOB_MODE_LINEAR:
                value = 0;
                fWindowParameters.knobMode = rack::settings::KNOB_MODE_LINEAR;
                break;
            case rack::settings::KNOB_MODE_ROTARY_ABSOLUTE:
                value = 1;
                fWindowParameters.knobMode = rack::settings::KNOB_MODE_ROTARY_ABSOLUTE;
                break;
            case rack::settings::KNOB_MODE_ROTARY_RELATIVE:
                value = 2;
                fWindowParameters.knobMode = rack::settings::KNOB_MODE_ROTARY_RELATIVE;
                break;
            }
            break;
        case kWindowParameterWheelKnobControl:
            fWindowParameters.knobScroll = value > 0.5f;
            break;
        case kWindowParameterWheelSensitivity:
            mult = 1000.0f;
            fWindowParameters.knobScrollSensitivity = value;
            break;
        case kWindowParameterLockModulePositions:
            fWindowParameters.lockModules = value > 0.5f;
            break;
        default:
            return;
        }

        setParameterValue(kModuleParameters + param, value * mult);
    }

protected:
   /* --------------------------------------------------------------------------------------------------------
    * DSP/Plugin Callbacks */

   /**
      A parameter has changed on the plugin side.
      This is called by the host to inform the UI about parameter changes.
    */
    void parameterChanged(const uint32_t index, const float value) override
    {
        if (index < kModuleParameters)
            return;

        switch (index - kModuleParameters)
        {
        case kWindowParameterShowTooltips:
            fWindowParameters.tooltips = value > 0.5f;
            break;
        case kWindowParameterCableOpacity:
            fWindowParameters.cableOpacity = value / 100.0f;
            break;
        case kWindowParameterCableTension:
            fWindowParameters.cableTension = value / 100.0f;
            break;
        case kWindowParameterRackBrightness:
            fWindowParameters.rackBrightness = value / 100.0f;
            break;
        case kWindowParameterHaloBrightness:
            fWindowParameters.haloBrightness = value / 100.0f;
            break;
        case kWindowParameterKnobMode:
            switch (static_cast<int>(value + 0.5f))
            {
            case 0:
                fWindowParameters.knobMode = rack::settings::KNOB_MODE_LINEAR;
                break;
            case 1:
                fWindowParameters.knobMode = rack::settings::KNOB_MODE_ROTARY_ABSOLUTE;
                break;
            case 2:
                fWindowParameters.knobMode = rack::settings::KNOB_MODE_ROTARY_RELATIVE;
                break;
            }
            break;
        case kWindowParameterWheelKnobControl:
            fWindowParameters.knobScroll = value > 0.5f;
            break;
        case kWindowParameterWheelSensitivity:
            fWindowParameters.knobScrollSensitivity = value / 1000.0f;
            break;
        case kWindowParameterLockModulePositions:
            fWindowParameters.lockModules = value > 0.5f;
            break;
        default:
            return;
        }

        WindowParametersSetValues(fContext->window, fWindowParameters);
    }

    void stateChanged(const char* key, const char* value) override
    {
    }

    // -------------------------------------------------------------------------------------------------------

    static int glfwMods(const uint mod) noexcept
    {
        int mods = 0;

        if (mod & kModifierControl)
            mods |= GLFW_MOD_CONTROL;
        if (mod & kModifierShift)
            mods |= GLFW_MOD_SHIFT;
        if (mod & kModifierAlt)
            mods |= GLFW_MOD_ALT;

        /*
        if (glfwGetKey(win, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(win, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
            mods |= GLFW_MOD_SHIFT;
        if (glfwGetKey(win, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(win, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)
            mods |= GLFW_MOD_CONTROL;
        if (glfwGetKey(win, GLFW_KEY_LEFT_ALT) == GLFW_PRESS || glfwGetKey(win, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS)
            mods |= GLFW_MOD_ALT;
        if (glfwGetKey(win, GLFW_KEY_LEFT_SUPER) == GLFW_PRESS || glfwGetKey(win, GLFW_KEY_RIGHT_SUPER) == GLFW_PRESS)
            mods |= GLFW_MOD_SUPER;
        */

        return mods;
    }

    bool onMouse(const MouseEvent& ev) override
    {
        const int action = ev.press ? GLFW_PRESS : GLFW_RELEASE;
        const int mods = glfwMods(ev.mod);

        int button;
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

        const ScopedContext sc(this, mods);
        return fContext->event->handleButton(fLastMousePos, button, action, mods);
    }

    bool onMotion(const MotionEvent& ev) override
    {
        const rack::math::Vec mousePos = rack::math::Vec(ev.pos.getX(), ev.pos.getY()).div(getScaleFactor()).round();
        const rack::math::Vec mouseDelta = mousePos.minus(fLastMousePos);

        fLastMousePos = mousePos;

        const ScopedContext sc(this, glfwMods(ev.mod));
        return fContext->event->handleHover(mousePos, mouseDelta);
    }

    bool onScroll(const ScrollEvent& ev) override
    {
        rack::math::Vec scrollDelta = rack::math::Vec(ev.delta.getX(), ev.delta.getY());
#ifdef DISTRHO_OS_MAC
        scrollDelta = scrollDelta.mult(10.0);
#else
        scrollDelta = scrollDelta.mult(50.0);
#endif

        const ScopedContext sc(this, glfwMods(ev.mod));
        return fContext->event->handleScroll(fLastMousePos, scrollDelta);
    }

    bool onCharacterInput(const CharacterInputEvent& ev) override
    {
        if (ev.character <= ' ' || ev.character >= kKeyDelete)
            return false;

        const ScopedContext sc(this, glfwMods(ev.mod));
        return fContext->event->handleText(fLastMousePos, ev.character);
    }

    bool onKeyboard(const KeyboardEvent& ev) override
    {
        const int action = ev.press ? GLFW_PRESS : GLFW_RELEASE;
        const int mods = glfwMods(ev.mod);

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

        int key;
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

        rack::window::WindowMods(fContext->window, mods);

        const ScopedContext sc(this, mods);
        return fContext->event->handleKey(fLastMousePos, key, ev.keycode, action, mods);
    }

    void uiFocus(const bool focus, CrossingMode) override
    {
        if (focus)
            return;

        const ScopedContext sc(this, 0);
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
