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

#include "RemoteUI.hpp"

#include <asset.hpp>
// #include <random.hpp>
#include <patch.hpp>
#include <settings.hpp>
#include <system.hpp>

#include <app/Scene.hpp>
#include <engine/Engine.hpp>

#include "AsyncDialog.hpp"
#include "WindowParameters.hpp"

// --------------------------------------------------------------------------------------------------------------------

namespace rack {
namespace app {
    widget::Widget* createMenuBar(bool isStandalone);
}
namespace window {
    void WindowSetPluginRemote(Window* window, NanoTopLevelWidget* tlw);
    void WindowSetMods(Window* window, int mods);
    void WindowSetInternalSize(rack::window::Window* window, math::Vec size);
}
}

// --------------------------------------------------------------------------------------------------------------------

CardinalRemoteUI::CardinalRemoteUI(Window& window, const std::string& templatePath)
    : NanoTopLevelWidget(window)
{
    CardinalPluginContext& context(*static_cast<CardinalPluginContext*>(rack::contextGet()));
    context.nativeWindowId = getWindow().getNativeWindowHandle();
    context.tlw = this;

    window.setIgnoringKeyRepeat(true);
    context.nativeWindowId = window.getNativeWindowHandle();

    const double scaleFactor = getScaleFactor();

    setGeometryConstraints(648 * scaleFactor, 538 * scaleFactor);

    if (scaleFactor != 1.0)
        setSize(DISTRHO_UI_DEFAULT_WIDTH * scaleFactor, DISTRHO_UI_DEFAULT_HEIGHT * scaleFactor);

    rack::window::WindowSetPluginRemote(context.window, this);

    if (rack::widget::Widget* const menuBar = context.scene->menuBar)
    {
        context.scene->removeChild(menuBar);
        delete menuBar;
    }

    context.scene->menuBar = rack::app::createMenuBar(true);
    context.scene->addChildBelow(context.scene->menuBar, context.scene->rackScroll);

    // hide "Browse VCV Library" button
    rack::widget::Widget* const browser = context.scene->browser->children.back();
    rack::widget::Widget* const headerLayout = browser->children.front();
    rack::widget::Widget* const libraryButton = headerLayout->children.back();
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
    {
        static bool shown = false;

        if (! shown)
        {
            shown = true;
            asyncDialog::create(errorMessage.c_str());
        }
    }

    context.window->step();

    // WindowParametersSetCallback(context.window, this);
}

CardinalRemoteUI::~CardinalRemoteUI()
{
    CardinalPluginContext& context(*static_cast<CardinalPluginContext*>(rack::contextGet()));
    context.nativeWindowId = 0;
}

void CardinalRemoteUI::onNanoDisplay()
{
    CardinalPluginContext& context(*static_cast<CardinalPluginContext*>(rack::contextGet()));
    context.window->step();

    // TODO
    repaint();
}

// --------------------------------------------------------------------------------------------------------------------

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

bool CardinalRemoteUI::onMouse(const MouseEvent& ev)
{
    if (ev.press)
        getWindow().focus();

    const int action = ev.press ? GLFW_PRESS : GLFW_RELEASE;
    int mods = glfwMods(ev.mod);

    int button;

    switch (ev.button)
    {
    case 1: button = GLFW_MOUSE_BUTTON_LEFT;   break;
    case 2: button = GLFW_MOUSE_BUTTON_RIGHT;  break;
    case 3: button = GLFW_MOUSE_BUTTON_MIDDLE; break;
    default:
        button = ev.button;
        break;
    }

   #ifdef DISTRHO_OS_MAC
    // Remap Ctrl-left click to right click on macOS
    if (button == GLFW_MOUSE_BUTTON_LEFT && (mods & RACK_MOD_MASK) == GLFW_MOD_CONTROL) {
        button = GLFW_MOUSE_BUTTON_RIGHT;
        mods &= ~GLFW_MOD_CONTROL;
    }
    // Remap Ctrl-shift-left click to middle click on macOS
    if (button == GLFW_MOUSE_BUTTON_LEFT && (mods & RACK_MOD_MASK) == (GLFW_MOD_CONTROL | GLFW_MOD_SHIFT)) {
        button = GLFW_MOUSE_BUTTON_MIDDLE;
        mods &= ~(GLFW_MOD_CONTROL | GLFW_MOD_SHIFT);
    }
   #endif

    CardinalPluginContext* context = static_cast<CardinalPluginContext*>(rack::contextGet());
    return context->event->handleButton(lastMousePos, button, action, mods);
}

bool CardinalRemoteUI::onMotion(const MotionEvent& ev)
{
    const rack::math::Vec mousePos = rack::math::Vec(ev.pos.getX(), ev.pos.getY()).div(getScaleFactor()).round();
    const rack::math::Vec mouseDelta = mousePos.minus(lastMousePos);

    lastMousePos = mousePos;

    CardinalPluginContext* context = static_cast<CardinalPluginContext*>(rack::contextGet());
    return context->event->handleHover(mousePos, mouseDelta);
}

bool CardinalRemoteUI::onScroll(const ScrollEvent& ev)
{
    rack::math::Vec scrollDelta = rack::math::Vec(ev.delta.getX(), ev.delta.getY());
   #ifndef DISTRHO_OS_MAC
    scrollDelta = scrollDelta.mult(50.0);
   #endif

    const int mods = glfwMods(ev.mod);

    CardinalPluginContext* context = static_cast<CardinalPluginContext*>(rack::contextGet());
    return context->event->handleScroll(lastMousePos, scrollDelta);
}

bool CardinalRemoteUI::onCharacterInput(const CharacterInputEvent& ev)
{
    if (ev.character < ' ' || ev.character >= kKeyDelete)
        return false;

    const int mods = glfwMods(ev.mod);

    CardinalPluginContext* context = static_cast<CardinalPluginContext*>(rack::contextGet());
    return context->event->handleText(lastMousePos, ev.character);
}

bool CardinalRemoteUI::onKeyboard(const KeyboardEvent& ev)
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
    default:
        // glfw expects uppercase
        if (ev.key >= 'a' && ev.key <= 'z')
            key = ev.key - ('a' - 'A');
        else
            key = ev.key;
        break;
    }

    CardinalPluginContext* context = static_cast<CardinalPluginContext*>(rack::contextGet());
    return context->event->handleKey(lastMousePos, key, ev.keycode, action, mods);
}

void CardinalRemoteUI::onResize(const ResizeEvent& ev)
{
    NanoTopLevelWidget::onResize(ev);

    CardinalPluginContext* context = static_cast<CardinalPluginContext*>(rack::contextGet());
    if (context->window != nullptr)
        WindowSetInternalSize(context->window, rack::math::Vec(ev.size.getWidth(), ev.size.getHeight()));
}

// --------------------------------------------------------------------------------------------------------------------
