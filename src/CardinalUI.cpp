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
#include <asset.hpp>
#include <context.hpp>
#include <helpers.hpp>
#include <patch.hpp>
#include <settings.hpp>
#include <string.hpp>
#include <system.hpp>
#include <ui/Button.hpp>
#include <ui/MenuItem.hpp>
#include <ui/MenuSeparator.hpp>
#include <window/Window.hpp>

#ifdef NDEBUG
# undef DEBUG
#endif

#include <Application.hpp>
#include "AsyncDialog.hpp"
#include "PluginContext.hpp"
#include "WindowParameters.hpp"

GLFWAPI int glfwGetKeyScancode(int) { return 0; }

GLFWAPI const char* glfwGetClipboardString(GLFWwindow*)
{
    CardinalPluginContext* const context = static_cast<CardinalPluginContext*>(APP);
    DISTRHO_SAFE_ASSERT_RETURN(context != nullptr, nullptr);
    DISTRHO_SAFE_ASSERT_RETURN(context->ui != nullptr, nullptr);

    const char* mimeType = nullptr;
    size_t dataSize = 0;

    if (const void* const clipboard = context->ui->getClipboard(mimeType, dataSize))
    {
        if (mimeType == nullptr || std::strcmp(mimeType, "text/plain") != 0)
            return nullptr;
        return static_cast<const char*>(clipboard);
    }

    return nullptr;
}

GLFWAPI void glfwSetClipboardString(GLFWwindow*, const char* const text)
{
    DISTRHO_SAFE_ASSERT_RETURN(text != nullptr,);

    CardinalPluginContext* const context = static_cast<CardinalPluginContext*>(APP);
    DISTRHO_SAFE_ASSERT_RETURN(context != nullptr,);
    DISTRHO_SAFE_ASSERT_RETURN(context->ui != nullptr,);

    context->ui->setClipboard(nullptr, text, std::strlen(text)+1);
}

GLFWAPI void glfwSetCursor(GLFWwindow*, GLFWcursor* const cursor)
{
    CardinalPluginContext* const context = static_cast<CardinalPluginContext*>(APP);
    DISTRHO_SAFE_ASSERT_RETURN(context != nullptr,);
    DISTRHO_SAFE_ASSERT_RETURN(context->ui != nullptr,);

    context->ui->setCursor(cursor != nullptr ? kMouseCursorDiagonal : kMouseCursorArrow);
}

GLFWAPI double glfwGetTime(void)
{
    CardinalPluginContext* const context = static_cast<CardinalPluginContext*>(APP);
    DISTRHO_SAFE_ASSERT_RETURN(context != nullptr, 0.0);
    DISTRHO_SAFE_ASSERT_RETURN(context->ui != nullptr, 0.0);

    return context->ui->getApp().getTime();
}

GLFWAPI const char* glfwGetKeyName(const int key, int)
{
    switch (key)
    {
    case '\"': return "\"";
    case '\'': return "\'";
    case '\\': return "\\";
    case ' ': return " ";
    case '!': return "!";
    case '#': return "#";
    case '$': return "$";
    case '%': return "%";
    case '&': return "&";
    case '(': return "(";
    case ')': return ")";
    case '*': return "*";
    case '+': return "+";
    case ',': return ",";
    case '-': return "-";
    case '.': return ".";
    case '/': return "/";
    case '0': return "0";
    case '1': return "1";
    case '2': return "2";
    case '3': return "3";
    case '4': return "4";
    case '5': return "5";
    case '6': return "6";
    case '7': return "7";
    case '8': return "8";
    case '9': return "9";
    case ':': return ":";
    case ';': return ";";
    case '<': return "<";
    case '=': return "=";
    case '>': return ">";
    case '?': return "?";
    case '@': return "@";
    case 'A': return "A";
    case 'B': return "B";
    case 'C': return "C";
    case 'D': return "D";
    case 'E': return "E";
    case 'F': return "F";
    case 'G': return "G";
    case 'H': return "H";
    case 'I': return "I";
    case 'J': return "J";
    case 'K': return "K";
    case 'L': return "L";
    case 'M': return "M";
    case 'N': return "N";
    case 'O': return "O";
    case 'P': return "P";
    case 'Q': return "Q";
    case 'R': return "R";
    case 'S': return "S";
    case 'T': return "T";
    case 'U': return "U";
    case 'V': return "V";
    case 'W': return "W";
    case 'X': return "X";
    case 'Y': return "Y";
    case 'Z': return "Z";
    case '[': return "[";
    case ']': return "]";
    case '^': return "^";
    case '_': return "_";
    case '`': return "`";
    case 'a': return "a";
    case 'b': return "b";
    case 'c': return "c";
    case 'd': return "d";
    case 'e': return "e";
    case 'f': return "f";
    case 'g': return "g";
    case 'h': return "h";
    case 'i': return "i";
    case 'j': return "j";
    case 'k': return "k";
    case 'l': return "l";
    case 'm': return "m";
    case 'n': return "n";
    case 'o': return "o";
    case 'p': return "p";
    case 'q': return "q";
    case 'r': return "r";
    case 's': return "s";
    case 't': return "t";
    case 'u': return "u";
    case 'v': return "v";
    case 'w': return "w";
    case 'x': return "x";
    case 'y': return "y";
    case 'z': return "z";
    default: return nullptr;
    }
}

namespace rack {
namespace app {
    widget::Widget* createMenuBar(bool isStandalone);
}
namespace window {
    void WindowSetPluginUI(Window* window, DISTRHO_NAMESPACE::UI* ui);
    void WindowSetMods(Window* window, int mods);
}
}

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------------------------------------

bool CardinalPluginContext::addIdleCallback(IdleCallback* const cb) const
{
    if (ui == nullptr)
        return false;

    ui->addIdleCallback(cb);
    return true;
}

void CardinalPluginContext::removeIdleCallback(IdleCallback* const cb) const
{
    if (ui == nullptr)
        return;

    ui->removeIdleCallback(cb);
}

void handleHostParameterDrag(const CardinalPluginContext* pcontext, uint index, bool started)
{
    DISTRHO_SAFE_ASSERT_RETURN(pcontext->ui != nullptr,);

    if (started)
    {
        pcontext->ui->editParameter(index, true);
        pcontext->ui->setParameterValue(index, pcontext->parameters[index]);
    }
    else
    {
        pcontext->ui->editParameter(index, false);
    }
}

// -----------------------------------------------------------------------------------------------------------

class CardinalUI : public CardinalBaseUI,
                   public WindowParametersCallback
{
    rack::math::Vec lastMousePos;
    WindowParameters windowParameters;
    int rateLimitStep = 0;
    bool firstIdle = true;

    struct ScopedContext {
        CardinalPluginContext* const context;

        ScopedContext(CardinalUI* const ui)
            : context(ui->context)
        {
            rack::contextSet(context);
            WindowParametersRestore(context->window);
        }

        ScopedContext(CardinalUI* const ui, const int mods)
            : context(ui->context)
        {
            rack::contextSet(context);
            rack::window::WindowSetMods(context->window, mods);
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
        : CardinalBaseUI(1228, 666)
    {
        Window& window(getWindow());

        window.setIgnoringKeyRepeat(true);
        context->nativeWindowId = window.getNativeWindowHandle();

        const double scaleFactor = getScaleFactor();

        setGeometryConstraints(648 * scaleFactor, 538 * scaleFactor);

        if (scaleFactor != 1.0)
            setSize(1228 * scaleFactor, 666 * scaleFactor);

        rack::contextSet(context);

        rack::window::WindowSetPluginUI(context->window, this);

        if (context->scene->menuBar != nullptr)
            context->scene->removeChild(context->scene->menuBar);

        context->scene->menuBar = rack::app::createMenuBar(getApp().isStandalone());
        context->scene->addChildBelow(context->scene->menuBar, context->scene->rackScroll);

        // hide "Browse VCV Library" button
        rack::widget::Widget* const browser = context->scene->browser->children.back();
        rack::widget::Widget* const headerLayout = browser->children.front();
        rack::widget::Widget* const favoriteButton = *std::next(headerLayout->children.begin(), 3);
        rack::widget::Widget* const libraryButton = headerLayout->children.back();
        favoriteButton->hide();
        libraryButton->hide();

        // Report to user if something is wrong with the installation
        std::string errorMessage;

        if (rack::asset::systemDir.empty())
        {
            errorMessage = "Failed to locate Cardinal plugin bundle.\n"
                           "Install Cardinal with its plugin bundle folder intact and try again.";
        }
        else if (! rack::system::exists(rack::asset::systemDir))
        {
            errorMessage = rack::string::f("System directory \"%s\" does not exist. "
                                           "Make sure Cardinal was downloaded and installed correctly.",
                                           rack::asset::systemDir.c_str());
        }

        if (! errorMessage.empty())
            asyncDialog::create(errorMessage.c_str());

        context->window->step();

        rack::contextSet(nullptr);

        WindowParametersSetCallback(context->window, this);
    }

    ~CardinalUI() override
    {
        rack::contextSet(context);

        context->nativeWindowId = 0;

        rack::widget::Widget* const menuBar = context->scene->menuBar;
        context->scene->menuBar = nullptr;
        context->scene->removeChild(menuBar);

        rack::window::WindowSetPluginUI(context->window, nullptr);

        rack::contextSet(nullptr);
    }

    void onNanoDisplay() override
    {
        const ScopedContext sc(this);
        context->window->step();
    }

    void uiIdle() override
    {
        if (firstIdle)
        {
            firstIdle = false;
            getWindow().focus();
        }

        if (filebrowserhandle != nullptr && fileBrowserIdle(filebrowserhandle))
        {
            {
                const char* const path = fileBrowserGetPath(filebrowserhandle);

                const ScopedContext sc(this);
                filebrowseraction(path != nullptr ? strdup(path) : nullptr);
            }

            fileBrowserClose(filebrowserhandle);
            filebrowseraction = nullptr;
            filebrowserhandle = nullptr;
        }

        if (windowParameters.rateLimit != 0 && ++rateLimitStep % (windowParameters.rateLimit * 2))
            return;

        rateLimitStep = 0;
        repaint();
    }

    void WindowParametersChanged(const WindowParameterList param, float value) override
    {
        float mult = 1.0f;

        switch (param)
        {
        case kWindowParameterShowTooltips:
            windowParameters.tooltips = value > 0.5f;
            break;
        case kWindowParameterCableOpacity:
            mult = 100.0f;
            windowParameters.cableOpacity = value;
            break;
        case kWindowParameterCableTension:
            mult = 100.0f;
            windowParameters.cableTension = value;
            break;
        case kWindowParameterRackBrightness:
            mult = 100.0f;
            windowParameters.rackBrightness = value;
            break;
        case kWindowParameterHaloBrightness:
            mult = 100.0f;
            windowParameters.haloBrightness = value;
            break;
        case kWindowParameterKnobMode:
            switch (static_cast<int>(value + 0.5f))
            {
            case rack::settings::KNOB_MODE_LINEAR:
                value = 0;
                windowParameters.knobMode = rack::settings::KNOB_MODE_LINEAR;
                break;
            case rack::settings::KNOB_MODE_ROTARY_ABSOLUTE:
                value = 1;
                windowParameters.knobMode = rack::settings::KNOB_MODE_ROTARY_ABSOLUTE;
                break;
            case rack::settings::KNOB_MODE_ROTARY_RELATIVE:
                value = 2;
                windowParameters.knobMode = rack::settings::KNOB_MODE_ROTARY_RELATIVE;
                break;
            }
            break;
        case kWindowParameterWheelKnobControl:
            windowParameters.knobScroll = value > 0.5f;
            break;
        case kWindowParameterWheelSensitivity:
            mult = 1000.0f;
            windowParameters.knobScrollSensitivity = value;
            break;
        case kWindowParameterLockModulePositions:
            windowParameters.lockModules = value > 0.5f;
            break;
        case kWindowParameterUpdateRateLimit:
            windowParameters.rateLimit = static_cast<int>(value + 0.5f);
            rateLimitStep = 0;
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
            windowParameters.tooltips = value > 0.5f;
            break;
        case kWindowParameterCableOpacity:
            windowParameters.cableOpacity = value / 100.0f;
            break;
        case kWindowParameterCableTension:
            windowParameters.cableTension = value / 100.0f;
            break;
        case kWindowParameterRackBrightness:
            windowParameters.rackBrightness = value / 100.0f;
            break;
        case kWindowParameterHaloBrightness:
            windowParameters.haloBrightness = value / 100.0f;
            break;
        case kWindowParameterKnobMode:
            switch (static_cast<int>(value + 0.5f))
            {
            case 0:
                windowParameters.knobMode = rack::settings::KNOB_MODE_LINEAR;
                break;
            case 1:
                windowParameters.knobMode = rack::settings::KNOB_MODE_ROTARY_ABSOLUTE;
                break;
            case 2:
                windowParameters.knobMode = rack::settings::KNOB_MODE_ROTARY_RELATIVE;
                break;
            }
            break;
        case kWindowParameterWheelKnobControl:
            windowParameters.knobScroll = value > 0.5f;
            break;
        case kWindowParameterWheelSensitivity:
            windowParameters.knobScrollSensitivity = value / 1000.0f;
            break;
        case kWindowParameterLockModulePositions:
            windowParameters.lockModules = value > 0.5f;
            break;
        case kWindowParameterUpdateRateLimit:
            windowParameters.rateLimit = static_cast<int>(value + 0.5f);
            rateLimitStep = 0;
            break;
        default:
            return;
        }

        WindowParametersSetValues(context->window, windowParameters);
    }

    void stateChanged(const char* key, const char* value) override
    {
        if (std::strcmp(key, "windowSize") != 0)
            return;

        int width = 0;
        int height = 0;
        std::sscanf(value, "%i:%i", &width, &height);

        if (width > 0 && height > 0)
        {
            const double scaleFactor = getScaleFactor();
            setSize(width * scaleFactor, height * scaleFactor);
        }
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
        if (mod & kModifierSuper)
            mods |= GLFW_MOD_SUPER;

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

        switch (ev.button)
        {
        case 1: button = GLFW_MOUSE_BUTTON_LEFT;   break;
#ifdef DISTRHO_OS_MAC
        case 2: button = GLFW_MOUSE_BUTTON_RIGHT;  break;
        case 3: button = GLFW_MOUSE_BUTTON_MIDDLE; break;
#else
        case 2: button = GLFW_MOUSE_BUTTON_MIDDLE; break;
        case 3: button = GLFW_MOUSE_BUTTON_RIGHT;  break;
#endif
        default:
            button = ev.button;
            break;
        }

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
        return context->event->handleButton(lastMousePos, button, action, mods);
    }

    bool onMotion(const MotionEvent& ev) override
    {
        const rack::math::Vec mousePos = rack::math::Vec(ev.pos.getX(), ev.pos.getY()).div(getScaleFactor()).round();
        const rack::math::Vec mouseDelta = mousePos.minus(lastMousePos);

        lastMousePos = mousePos;

        const ScopedContext sc(this, glfwMods(ev.mod));
        return context->event->handleHover(mousePos, mouseDelta);
    }

    bool onScroll(const ScrollEvent& ev) override
    {
        rack::math::Vec scrollDelta = rack::math::Vec(-ev.delta.getX(), ev.delta.getY());
#ifdef DISTRHO_OS_MAC
        scrollDelta = scrollDelta.mult(10.0);
#else
        scrollDelta = scrollDelta.mult(50.0);
#endif

        const int mods = glfwMods(ev.mod);
        const ScopedContext sc(this, mods);
        return context->event->handleScroll(lastMousePos, scrollDelta);
    }

    bool onCharacterInput(const CharacterInputEvent& ev) override
    {
        if (ev.character < ' ' || ev.character >= kKeyDelete)
            return false;

        const int mods = glfwMods(ev.mod);
        const ScopedContext sc(this, mods);
        return context->event->handleText(lastMousePos, ev.character);
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

        const ScopedContext sc(this, mods);
        return context->event->handleKey(lastMousePos, key, ev.keycode, action, mods);
    }

    void onResize(const ResizeEvent& ev) override
    {
        UI::onResize(ev);

        if (context->window != nullptr)
            context->window->setSize(rack::math::Vec(ev.size.getWidth(), ev.size.getHeight()));

        const double scaleFactor = getScaleFactor();
        char sizeString[64];
        std::snprintf(sizeString, sizeof(sizeString), "%d:%d",
                      (int)(ev.size.getWidth() / scaleFactor), (int)(ev.size.getHeight() / scaleFactor));
        setState("windowSize", sizeString);
    }

    void uiFocus(const bool focus, const CrossingMode mode) override
    {
        if (focus)
        {
            if (mode == kCrossingNormal)
                getWindow().focus();
        }
        else
        {
            const ScopedContext sc(this, 0);
            context->event->handleLeave();
        }
    }

    void uiFileBrowserSelected(const char* const filename) override
    {
        if (filename == nullptr)
            return;

        rack::contextSet(context);
        WindowParametersRestore(context->window);

        std::string sfilename = filename;

        if (saving)
        {
            if (rack::system::getExtension(sfilename) != ".vcv")
                sfilename += ".vcv";

            try {
                context->patch->save(sfilename);
            }
            catch (rack::Exception& e) {
                std::string message = rack::string::f("Could not save patch: %s", e.what());
                asyncDialog::create(message.c_str());
                return;
            }
        }
        else
        {
            try {
                context->patch->load(sfilename);
            } catch (rack::Exception& e) {
                std::string message = rack::string::f("Could not load patch: %s", e.what());
                asyncDialog::create(message.c_str());
                return;
            }
        }

        context->patch->path = sfilename;
        context->history->setSaved();
    }

#if 0
    void uiReshape(const uint width, const uint height) override
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0, width, 0.0, height, -1.0, 1.0);
        glViewport(0, 0, width, height);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }
#endif

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
