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

#include <app/MenuBar.hpp>
#include <app/Scene.hpp>
#include <asset.hpp>
#include <context.hpp>
#include <engine/Engine.hpp>
#include <helpers.hpp>
#include <patch.hpp>
#include <settings.hpp>
#include <string.hpp>
#include <system.hpp>
#include <ui/Button.hpp>
#include <ui/MenuItem.hpp>
#include <ui/MenuSeparator.hpp>
#include <window/Window.hpp>

#ifdef DISTRHO_OS_WASM
# include <ui/Button.hpp>
# include <ui/Label.hpp>
# include <ui/MenuOverlay.hpp>
# include <ui/SequentialLayout.hpp>
# include "CardinalCommon.hpp"
# include <emscripten/emscripten.h>
#endif

#ifdef NDEBUG
# undef DEBUG
#endif

#include <Application.hpp>
#include "AsyncDialog.hpp"
#include "PluginContext.hpp"
#include "WindowParameters.hpp"

namespace rack {
namespace app {
    widget::Widget* createMenuBar(bool isStandalone);
}
namespace window {
    void WindowSetPluginUI(Window* window, DISTRHO_NAMESPACE::UI* ui);
    void WindowSetMods(Window* window, int mods);
    void WindowSetInternalSize(rack::window::Window* window, math::Vec size);
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

#ifdef DISTRHO_OS_WASM
struct WasmWelcomeDialog : rack::widget::OpaqueWidget
{
    static const constexpr float margin = 10;
    static const constexpr float buttonWidth = 110;

    WasmWelcomeDialog()
    {
        using rack::ui::Button;
        using rack::ui::Label;
        using rack::ui::MenuOverlay;
        using rack::ui::SequentialLayout;

        box.size = rack::math::Vec(550, 310);

        SequentialLayout* const layout = new SequentialLayout;
        layout->box.pos = rack::math::Vec(0, 0);
        layout->box.size = box.size;
        layout->orientation = SequentialLayout::VERTICAL_ORIENTATION;
        layout->margin = rack::math::Vec(margin, margin);
        layout->spacing = rack::math::Vec(margin, margin);
        layout->wrap = false;
        addChild(layout);

        SequentialLayout* const contentLayout = new SequentialLayout;
        contentLayout->spacing = rack::math::Vec(margin, margin);
        layout->addChild(contentLayout);

        SequentialLayout* const buttonLayout = new SequentialLayout;
        buttonLayout->alignment = SequentialLayout::CENTER_ALIGNMENT;
        buttonLayout->box.size = box.size;
        buttonLayout->spacing = rack::math::Vec(margin, margin);
        layout->addChild(buttonLayout);

        Label* const label = new Label;
        label->box.size.x = box.size.x - 2*margin;
        label->box.size.y = box.size.y - 2*margin - 40;
        label->fontSize = 20;
        label->text = ""
            "Welcome!\n"
            "\n"
            "This is a special web-assembly version of Cardinal, "
            "allowing you to enjoy eurorack-style modules directly in your browser.\n"
            "\n"
            "This is still very much a work in progress, "
            "minor issues and occasional crashes are expected.\n"
            "\n"
            "Proceed with caution and have fun!";
        contentLayout->addChild(label);

        struct JoinDiscussionButton : Button {
            WasmWelcomeDialog* dialog;
            void onAction(const ActionEvent& e) override {
                patchUtils::openBrowser("https://github.com/DISTRHO/Cardinal/issues/287");
                dialog->getParent()->requestDelete();
            }
        };
        JoinDiscussionButton* const discussionButton = new JoinDiscussionButton;
        discussionButton->box.size.x = buttonWidth;
        discussionButton->text = "Join discussion";
        discussionButton->dialog = this;
        buttonLayout->addChild(discussionButton);

        struct DismissButton : Button {
            WasmWelcomeDialog* dialog;
            void onAction(const ActionEvent& e) override {
                dialog->getParent()->requestDelete();
            }
        };
        DismissButton* const dismissButton = new DismissButton;
        dismissButton->box.size.x = buttonWidth;
        dismissButton->text = "Dismiss";
        dismissButton->dialog = this;
        buttonLayout->addChild(dismissButton);

        MenuOverlay* const overlay = new MenuOverlay;
        overlay->bgColor = nvgRGBAf(0, 0, 0, 0.33);
        overlay->addChild(this);
        APP->scene->addChild(overlay);
    }

    void step() override
    {
        OpaqueWidget::step();
        box.pos = parent->box.size.minus(box.size).div(2).round();
    }

    void draw(const DrawArgs& args) override
    {
        bndMenuBackground(args.vg, 0.0, 0.0, box.size.x, box.size.y, 0);
        Widget::draw(args);
    }
};

struct WasmPatchStorageLoadingDialog : rack::widget::OpaqueWidget
{
    static const constexpr float margin = 10;

    rack::ui::MenuOverlay* overlay;

    WasmPatchStorageLoadingDialog()
    {
        using rack::ui::Label;
        using rack::ui::MenuOverlay;
        using rack::ui::SequentialLayout;

        box.size = rack::math::Vec(300, 50);

        SequentialLayout* const layout = new SequentialLayout;
        layout->box.pos = rack::math::Vec(0, 0);
        layout->box.size = box.size;
        layout->orientation = SequentialLayout::VERTICAL_ORIENTATION;
        layout->margin = rack::math::Vec(margin, margin);
        layout->spacing = rack::math::Vec(margin, margin);
        layout->wrap = false;
        addChild(layout);

        Label* const label = new Label;
        label->box.size.x = box.size.x - 2*margin;
        label->box.size.y = box.size.y - 2*margin - 40;
        label->fontSize = 16;
        label->text = "Load patch from PatchStorage...\n";
        layout->addChild(label);

        overlay = new MenuOverlay;
        overlay->bgColor = nvgRGBAf(0, 0, 0, 0.33);
        overlay->addChild(this);
        APP->scene->addChild(overlay);
    }

    void step() override
    {
        OpaqueWidget::step();
        box.pos = parent->box.size.minus(box.size).div(2).round();
    }

    void draw(const DrawArgs& args) override
    {
        bndMenuBackground(args.vg, 0.0, 0.0, box.size.x, box.size.y, 0);
        Widget::draw(args);
    }
};

static void downloadPatchStorageFailed(const char* const filename)
{
    d_stdout("downloadPatchStorageFailed %s", filename);
    CardinalPluginContext* const context = static_cast<CardinalPluginContext*>(APP);
    CardinalBaseUI* const ui = static_cast<CardinalBaseUI*>(context->ui);

    if (ui->psDialog != nullptr)
    {
        ui->psDialog->overlay->requestDelete();
        asyncDialog::create("Failed to fetch patch from PatchStorage");
    }

    using namespace rack;
    context->patch->templatePath = system::join(asset::systemDir, "template-synth.vcv"); // FIXME
    context->patch->loadTemplate();
    context->scene->rackScroll->reset();
}

static void downloadPatchStorageSucceeded(const char* const filename)
{
    d_stdout("downloadPatchStorageSucceeded %s | %s", filename, APP->patch->templatePath.c_str());
    CardinalPluginContext* const context = static_cast<CardinalPluginContext*>(APP);
    CardinalBaseUI* const ui = static_cast<CardinalBaseUI*>(context->ui);

    ui->psDialog->overlay->requestDelete();
    ui->psDialog = nullptr;

    if (FILE* f = fopen(filename, "r"))
    {
        uint8_t buf[8] = {};
        fread(buf, 8, 1, f);
        d_stdout("read patch %x %x %x %x %x %x %x %x",
                buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7]);
        fclose(f);
    }

    try {
        context->patch->load(CARDINAL_IMPORTED_TEMPLATE_FILENAME);
    } catch (rack::Exception& e) {
        const std::string message = rack::string::f("Could not load patch: %s", e.what());
        asyncDialog::create(message.c_str());
        return;
    }

    context->scene->rackScroll->reset();
    context->patch->path = "";
    context->history->setSaved();
}
#endif

// -----------------------------------------------------------------------------------------------------------

class CardinalUI : public CardinalBaseUI,
                   public WindowParametersCallback
{
    rack::math::Vec lastMousePos;
    WindowParameters windowParameters;
    int rateLimitStep = 0;
    int8_t counterForFirstIdlePoint = 0;

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

        if (rack::widget::Widget* const menuBar = context->scene->menuBar)
        {
            context->scene->removeChild(menuBar);
            delete menuBar;
        }

        context->scene->menuBar = rack::app::createMenuBar(getApp().isStandalone());
        context->scene->addChildBelow(context->scene->menuBar, context->scene->rackScroll);

        // hide "Browse VCV Library" button
        rack::widget::Widget* const browser = context->scene->browser->children.back();
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

       #ifdef DISTRHO_OS_WASM
        if (rack::patchStorageSlug != nullptr)
            psDialog = new WasmPatchStorageLoadingDialog();
        else
            new WasmWelcomeDialog();
       #endif

        context->window->step();

        rack::contextSet(nullptr);

        WindowParametersSetCallback(context->window, this);
    }

    ~CardinalUI() override
    {
        rack::contextSet(context);

        context->nativeWindowId = 0;

        if (rack::widget::Widget* const menuBar = context->scene->menuBar)
        {
            context->scene->removeChild(menuBar);
            delete menuBar;
        }

        context->scene->menuBar = rack::app::createMenuBar();
        context->scene->addChildBelow(context->scene->menuBar, context->scene->rackScroll);

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
        if (counterForFirstIdlePoint >= 0 && ++counterForFirstIdlePoint == 5)
        {
            counterForFirstIdlePoint = -1;

           #ifdef DISTRHO_OS_WASM
            if (rack::patchStorageSlug != nullptr)
            {
                std::string url("/patchstorage.php?slug=");
                url += rack::patchStorageSlug;
                std::free(rack::patchStorageSlug);
                rack::patchStorageSlug = nullptr;

                emscripten_async_wget(url.c_str(), context->patch->templatePath.c_str(),
                                    downloadPatchStorageSucceeded, downloadPatchStorageFailed);
            }
           #endif
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
        case kWindowParameterBrowserSort:
            windowParameters.browserSort = static_cast<int>(value + 0.5f);
            break;
        case kWindowParameterBrowserZoom:
            windowParameters.browserZoom = value;
            value = std::pow(2.f, value) * 100.0f;
            break;
        case kWindowParameterInvertZoom:
            windowParameters.invertZoom = value > 0.5f;
            break;
        case kWindowParameterSqueezeModulePositions:
            windowParameters.squeezeModules = value > 0.5f;
            break;
        default:
            return;
        }

        setParameterValue(kModuleParameters + param + 1, value * mult);
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
        // host mapped parameters + bypass
        if (index <= kModuleParameters)
            return;

        switch (index - kModuleParameters - 1)
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
        case kWindowParameterBrowserSort:
            windowParameters.browserSort = static_cast<int>(value + 0.5f);
            break;
        case kWindowParameterBrowserZoom:
            // round up to nearest valid value
            {
                float rvalue = value - 1.0f;

                if (rvalue <= 25.0f)
                    rvalue = -2.0f;
                else if (rvalue <= 35.0f)
                    rvalue = -1.5f;
                else if (rvalue <= 50.0f)
                    rvalue = -1.0f;
                else if (rvalue <= 71.0f)
                    rvalue = -0.5f;
                else if (rvalue <= 100.0f)
                    rvalue = 0.0f;
                else if (rvalue <= 141.0f)
                    rvalue = 0.5f;
                else if (rvalue <= 200.0f)
                    rvalue = 1.0f;
                else
                    rvalue = 0.0f;

                windowParameters.browserZoom = rvalue;
            }
            break;
        case kWindowParameterInvertZoom:
            windowParameters.invertZoom = value > 0.5f;
            break;
        case kWindowParameterSqueezeModulePositions:
            windowParameters.squeezeModules = value > 0.5f;
            break;
        default:
            return;
        }

        WindowParametersSetValues(context->window, windowParameters);
    }

    void stateChanged(const char* const key, const char* const value) override
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
        rack::math::Vec scrollDelta = rack::math::Vec(ev.delta.getX(), ev.delta.getY());
#ifndef DISTRHO_OS_MAC
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
        default:
            // glfw expects uppercase
            if (ev.key >= 'a' && ev.key <= 'z')
                key = ev.key - ('a' - 'A');
            else
                key = ev.key;
            break;
        }

        const ScopedContext sc(this, mods);
        return context->event->handleKey(lastMousePos, key, ev.keycode, action, mods);
    }

    void onResize(const ResizeEvent& ev) override
    {
        UI::onResize(ev);

        if (context->window != nullptr)
            WindowSetInternalSize(context->window, rack::math::Vec(ev.size.getWidth(), ev.size.getHeight()));

        const double scaleFactor = getScaleFactor();
        char sizeString[64];
        std::snprintf(sizeString, sizeof(sizeString), "%d:%d",
                      (int)(ev.size.getWidth() / scaleFactor), (int)(ev.size.getHeight() / scaleFactor));
        setState("windowSize", sizeString);
    }

    void uiFocus(const bool focus, CrossingMode) override
    {
        if (!focus)
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
            const bool uncompressed = savingUncompressed;
            savingUncompressed = false;

            if (rack::system::getExtension(sfilename) != ".vcv")
                sfilename += ".vcv";

            try {
                if (uncompressed)
                {
                    context->engine->prepareSave();

                    if (json_t* const rootJ = context->patch->toJson())
                    {
                        if (FILE* const file = std::fopen(sfilename.c_str(), "w"))
                        {
                            json_dumpf(rootJ, file, JSON_INDENT(2));
                            std::fclose(file);
                        }
                        json_decref(rootJ);
                    }
                }
                else
                {
                    context->patch->save(sfilename);
                }
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
