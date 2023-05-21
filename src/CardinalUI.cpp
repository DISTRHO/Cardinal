/*
 * DISTRHO Cardinal Plugin
 * Copyright (C) 2021-2023 Filipe Coelho <falktx@falktx.com>
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

#ifdef DPF_RUNTIME_TESTING
# include <plugin.hpp>
#endif

#ifdef DISTRHO_OS_WASM
# include <ui/Button.hpp>
# include <ui/Label.hpp>
# include <ui/MenuOverlay.hpp>
# include <ui/SequentialLayout.hpp>
# include <emscripten/emscripten.h>
#endif

#ifdef NDEBUG
# undef DEBUG
#endif

#include "Application.hpp"
#include "AsyncDialog.hpp"
#include "CardinalCommon.hpp"
#include "PluginContext.hpp"
#include "WindowParameters.hpp"
#include "extra/Base64.hpp"

#ifndef DISTRHO_OS_WASM
# include "extra/SharedResourcePointer.hpp"
#endif

#ifndef HEADLESS
# include "extra/ScopedValueSetter.hpp"
#endif

namespace rack {
#ifdef DISTRHO_OS_WASM
namespace asset {
std::string patchesPath();
}
#endif
namespace engine {
void Engine_setAboutToClose(Engine*);
void Engine_setRemoteDetails(Engine*, remoteUtils::RemoteDetails*);
}
namespace window {
    void WindowSetPluginUI(Window* window, DISTRHO_NAMESPACE::UI* ui);
    void WindowSetMods(Window* window, int mods);
    void WindowSetInternalSize(rack::window::Window* window, math::Vec size);
}
}

START_NAMESPACE_DISTRHO

// --------------------------------------------------------------------------------------------------------------------

#if ! DISTRHO_PLUGIN_WANT_DIRECT_ACCESS
uint32_t Plugin::getBufferSize() const noexcept { return 0; }
double Plugin::getSampleRate() const noexcept { return 0.0; }
const char* Plugin::getBundlePath() const noexcept { return nullptr; }
bool Plugin::isSelfTestInstance() const noexcept { return false; }
bool Plugin::writeMidiEvent(const MidiEvent&) noexcept { return false; }
#endif

// --------------------------------------------------------------------------------------------------------------------

#if defined(DISTRHO_OS_WASM) && ! CARDINAL_VARIANT_MINI
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
            "Welcome to Cardinal on the Web!\n"
            "\n"
            "If using mobile/touch devices, please note:\n"
            " - Single quick press does simple mouse click\n"
            " - Press & move does click & drag action\n"
            " - Press & hold does right-click (and opens module browser)\n"
            "\n"
            "Still a bit experimental, so proceed with caution.\n"
            "Have fun!";
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

struct WasmRemotePatchLoadingDialog : rack::widget::OpaqueWidget
{
    static const constexpr float margin = 10;

    rack::ui::MenuOverlay* overlay;

    WasmRemotePatchLoadingDialog(const bool isFromPatchStorage)
    {
        using rack::ui::Label;
        using rack::ui::MenuOverlay;
        using rack::ui::SequentialLayout;

        box.size = rack::math::Vec(300, 40);

        SequentialLayout* const layout = new SequentialLayout;
        layout->box.pos = rack::math::Vec(0, 0);
        layout->box.size = box.size;
        layout->alignment = SequentialLayout::CENTER_ALIGNMENT;
        layout->margin = rack::math::Vec(margin, margin);
        layout->spacing = rack::math::Vec(margin, margin);
        layout->wrap = false;
        addChild(layout);

        Label* const label = new Label;
        label->box.size.x = box.size.x - 2*margin;
        label->box.size.y = box.size.y - 2*margin;
        label->fontSize = 16;
        label->text = isFromPatchStorage
                    ? "Loading patch from PatchStorage...\n"
                    : "Loading remote patch...\n";
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

static void downloadRemotePatchFailed(const char* const filename)
{
    d_stdout("downloadRemotePatchFailed %s", filename);
    CardinalPluginContext* const context = static_cast<CardinalPluginContext*>(APP);
    CardinalBaseUI* const ui = static_cast<CardinalBaseUI*>(context->ui);

    if (ui->psDialog != nullptr)
    {
        ui->psDialog->overlay->requestDelete();
        ui->psDialog = nullptr;
        asyncDialog::create("Failed to fetch remote patch");
    }

    using namespace rack;
    context->patch->templatePath = rack::system::join(asset::patchesPath(), "templates/main.vcv");
    context->patch->loadTemplate();
    context->scene->rackScroll->reset();
}

static void downloadRemotePatchSucceeded(const char* const filename)
{
    d_stdout("downloadRemotePatchSucceeded %s | %s", filename, APP->patch->templatePath.c_str());
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
        context->patch->load(filename);
    } catch (rack::Exception& e) {
        const std::string message = rack::string::f("Could not load patch: %s", e.what());
        asyncDialog::create(message.c_str());
        return;
    }

    context->patch->path.clear();
    context->scene->rackScroll->reset();
    context->history->setSaved();
}
#endif

// -----------------------------------------------------------------------------------------------------------

class CardinalUI : public CardinalBaseUI,
                   public WindowParametersCallback
{
  #if ! DISTRHO_PLUGIN_WANT_DIRECT_ACCESS
   #ifdef DISTRHO_OS_WASM
    ScopedPointer<Initializer> fInitializer;
   #else
    SharedResourcePointer<Initializer> fInitializer;
   #endif
    std::string fAutosavePath;
  #endif

    rack::math::Vec lastMousePos;
    WindowParameters windowParameters;
    int rateLimitStep = 0;
   #if defined(DISTRHO_OS_WASM) && ! CARDINAL_VARIANT_MINI
    int8_t counterForFirstIdlePoint = 0;
   #endif
   #ifdef DPF_RUNTIME_TESTING
    bool inSelfTest = false;
   #endif

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
        : CardinalBaseUI(DISTRHO_UI_DEFAULT_WIDTH, DISTRHO_UI_DEFAULT_HEIGHT),
        #if ! DISTRHO_PLUGIN_WANT_DIRECT_ACCESS
         #ifdef DISTRHO_OS_WASM
          fInitializer(new Initializer(static_cast<const CardinalBasePlugin*>(nullptr), this)),
         #else
          fInitializer(static_cast<const CardinalBasePlugin*>(nullptr), this),
         #endif
        #endif
          lastMousePos()
    {
        rack::contextSet(context);

       #if CARDINAL_VARIANT_MINI && ! DISTRHO_PLUGIN_WANT_DIRECT_ACCESS
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

        const float sampleRate = 60; // fake audio running at 60 fps
        rack::settings::sampleRate = sampleRate;

        context->dataIns = new const float*[DISTRHO_PLUGIN_NUM_INPUTS];
        context->dataOuts = new float*[DISTRHO_PLUGIN_NUM_OUTPUTS];

        for (uint32_t i=0; i<DISTRHO_PLUGIN_NUM_INPUTS;++i)
        {
            float** const bufferptr = const_cast<float**>(&context->dataIns[i]);
            *bufferptr = new float[1];
            (*bufferptr)[0] = 0.f;
        }
        for (uint32_t i=0; i<DISTRHO_PLUGIN_NUM_OUTPUTS;++i)
            context->dataOuts[i] = new float[1];

        context->bufferSize = 1;
        context->sampleRate = sampleRate;

        context->engine = new rack::engine::Engine;
        context->engine->setSampleRate(sampleRate);

        context->history = new rack::history::State;
        context->patch = new rack::patch::Manager;
        context->patch->autosavePath = fAutosavePath;
        context->patch->templatePath = context->patch->factoryTemplatePath = fInitializer->factoryTemplatePath;

        context->event = new rack::widget::EventState;
        context->scene = new rack::app::Scene;
        context->event->rootWidget = context->scene;

        context->window = new rack::window::Window;

        context->patch->loadTemplate();
        context->scene->rackScroll->reset();

        DISTRHO_SAFE_ASSERT(remoteUtils::connectToRemote());

        Engine_setRemoteDetails(context->engine, remoteDetails);
       #endif

        Window& window(getWindow());

        window.setIgnoringKeyRepeat(true);
        context->nativeWindowId = window.getNativeWindowHandle();

        const double scaleFactor = getScaleFactor();

        setGeometryConstraints(648 * scaleFactor, 538 * scaleFactor);

        if (rack::isStandalone() && rack::system::exists(rack::settings::settingsPath))
        {
            const double width = std::max(648.f, rack::settings::windowSize.x) * scaleFactor;
            const double height = std::max(538.f, rack::settings::windowSize.y) * scaleFactor;
            setSize(width, height);
        }
        else if (scaleFactor != 1.0)
        {
            setSize(DISTRHO_UI_DEFAULT_WIDTH * scaleFactor, DISTRHO_UI_DEFAULT_HEIGHT * scaleFactor);
        }

        rack::window::WindowSetPluginUI(context->window, this);

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

       #if defined(DISTRHO_OS_WASM) && ! CARDINAL_VARIANT_MINI
        if (rack::patchStorageSlug != nullptr)
        {
            psDialog = new WasmRemotePatchLoadingDialog(true);
        }
        else if (rack::patchRemoteURL != nullptr)
        {
            psDialog = new WasmRemotePatchLoadingDialog(false);
        }
        else if (rack::patchFromURL != nullptr)
        {
            static_cast<CardinalBasePlugin*>(context->plugin)->setState("patch", rack::patchFromURL);
            rack::contextSet(context);
        }
        else
        {
            new WasmWelcomeDialog();
        }
       #endif

        context->window->step();

        rack::contextSet(nullptr);

        WindowParametersSetCallback(context->window, this);
    }

    ~CardinalUI() override
    {
        rack::contextSet(context);

        context->nativeWindowId = 0;

        rack::window::WindowSetPluginUI(context->window, nullptr);

        context->tlw = nullptr;
        context->ui = nullptr;

       #if CARDINAL_VARIANT_MINI && ! DISTRHO_PLUGIN_WANT_DIRECT_ACCESS
        {
            const ScopedContext sc(this);
            context->patch->clear();

            // do a little dance to prevent context scene deletion from saving to temp dir
            const ScopedValueSetter<bool> svs(rack::settings::headless, true);
            Engine_setAboutToClose(context->engine);
            delete context;
        }

        if (! fAutosavePath.empty())
            rack::system::removeRecursively(fAutosavePath);
       #endif

        rack::contextSet(nullptr);
    }

    void onNanoDisplay() override
    {
        const ScopedContext sc(this);
        context->window->step();
    }

    void uiIdle() override
    {
       #ifdef DPF_RUNTIME_TESTING
        if (inSelfTest)
        {
            context->window->step();
            return;
        }

        if (context->plugin->isSelfTestInstance())
        {
            inSelfTest = true;

            Application& app(getApp());

            const ScopedContext sc(this);

            context->patch->clear();
            app.idle();

            const rack::math::Vec mousePos(getWidth()/2,getHeight()/2);
            context->event->handleButton(mousePos, GLFW_MOUSE_BUTTON_LEFT, GLFW_RELEASE, 0x0);
            context->event->handleHover(mousePos, rack::math::Vec(0,0));
            app.idle();

            for (rack::plugin::Plugin* p : rack::plugin::plugins)
            {
                for (rack::plugin::Model* m : p->models)
                {
                    d_stdout(">>>>>>>>>>>>>>>>> LOADING module %s : %s", p->slug.c_str(), m->slug.c_str());
                    rack::engine::Module* const module = m->createModule();
                    DISTRHO_SAFE_ASSERT_CONTINUE(module != nullptr);

                    rack::CardinalPluginModelHelper* const helper = dynamic_cast<rack::CardinalPluginModelHelper*>(m);
                    DISTRHO_SAFE_ASSERT_CONTINUE(helper != nullptr);

                    d_stdout(">>>>>>>>>>>>>>>>> LOADING moduleWidget %s : %s", p->slug.c_str(), m->slug.c_str());
                    rack::app::ModuleWidget* const moduleWidget = helper->createModuleWidget(module);
                    DISTRHO_SAFE_ASSERT_CONTINUE(moduleWidget != nullptr);

                    d_stdout(">>>>>>>>>>>>>>>>> ADDING TO ENGINE %s : %s", p->slug.c_str(), m->slug.c_str());
                    context->engine->addModule(module);

                    d_stdout(">>>>>>>>>>>>>>>>> ADDING TO RACK VIEW %s : %s", p->slug.c_str(), m->slug.c_str());
                    context->scene->rack->addModuleAtMouse(moduleWidget);

                    for (int i=5; --i>=0;)
                        app.idle();

                    d_stdout(">>>>>>>>>>>>>>>>> REMOVING FROM RACK VIEW %s : %s", p->slug.c_str(), m->slug.c_str());
                    context->scene->rack->removeModule(moduleWidget);
                    app.idle();

                    d_stdout(">>>>>>>>>>>>>>>>> DELETING module + moduleWidget %s : %s", p->slug.c_str(), m->slug.c_str());
                    delete moduleWidget;

                    app.idle();
                }
            }

            inSelfTest = false;
        }
       #endif

       #if defined(DISTRHO_OS_WASM) && ! CARDINAL_VARIANT_MINI
        if (counterForFirstIdlePoint >= 0 && ++counterForFirstIdlePoint == 30)
        {
            counterForFirstIdlePoint = -1;

            if (rack::patchStorageSlug != nullptr)
            {
                std::string url("/patchstorage.php?slug=");
                url += rack::patchStorageSlug;
                std::free(rack::patchStorageSlug);
                rack::patchStorageSlug = nullptr;

                emscripten_async_wget(url.c_str(), context->patch->templatePath.c_str(),
                                      downloadRemotePatchSucceeded, downloadRemotePatchFailed);
            }
            else if (rack::patchRemoteURL != nullptr)
            {
                std::string url("/patchurl.php?url=");
                url += rack::patchRemoteURL;
                std::free(rack::patchRemoteURL);
                rack::patchRemoteURL = nullptr;

                emscripten_async_wget(url.c_str(), context->patch->templatePath.c_str(),
                                      downloadRemotePatchSucceeded, downloadRemotePatchFailed);
            }
        }
       #endif

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

       #if CARDINAL_VARIANT_MINI && ! DISTRHO_PLUGIN_WANT_DIRECT_ACCESS
        {
            const ScopedContext sc(this);
            for (uint32_t i=0; i<DISTRHO_PLUGIN_NUM_OUTPUTS;++i)
                context->dataOuts[i][0] = 0.f;
            ++context->processCounter;
            context->engine->stepBlock(1);
        }
       #endif

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

        setParameterValue(kCardinalParameterStartWindow + param, value * mult);
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
        // host mapped parameters
        if (index < kCardinalParameterCountAtModules)
        {
           #if CARDINAL_VARIANT_MINI && ! DISTRHO_PLUGIN_WANT_DIRECT_ACCESS
            context->parameters[index] = value;
           #endif
            return;
        }

        // bypass
        if (index == kCardinalParameterBypass)
        {
           #if CARDINAL_VARIANT_MINI && ! DISTRHO_PLUGIN_WANT_DIRECT_ACCESS
            context->bypassed = value > 0.5f;
           #endif
            return;
        }

        if (index < kCardinalParameterCountAtWindow)
        {
            switch (index - kCardinalParameterStartWindow)
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
            return;
        }

       #if CARDINAL_VARIANT_MINI && ! DISTRHO_PLUGIN_WANT_DIRECT_ACCESS
        if (index < kCardinalParameterCountAtMiniBuffers)
        {
            float* const buffer = *const_cast<float**>(&context->dataIns[index - kCardinalParameterStartMiniBuffers]);
            buffer[0] = value;
            return;
        }

        switch (index)
        {
        case kCardinalParameterMiniTimeFlags: {
            const int32_t flags = static_cast<int32_t>(value + 0.5f);
            context->playing = flags & 0x1;
            context->bbtValid = flags & 0x2;
            context->reset = flags & 0x4;
            return;
        }
        case kCardinalParameterMiniTimeBar:
            context->bar = static_cast<int32_t>(value + 0.5f);
            return;
        case kCardinalParameterMiniTimeBeat:
            context->beat = static_cast<int32_t>(value + 0.5f);
            return;
        case kCardinalParameterMiniTimeBeatsPerBar:
            context->beatsPerBar = static_cast<int32_t>(value + 0.5f);
            return;
        case kCardinalParameterMiniTimeBeatType:
            context->beatType = static_cast<int32_t>(value + 0.5f);
            context->ticksPerClock = context->ticksPerBeat / context->beatType;
            context->tickClock = std::fmod(context->tick, context->ticksPerClock);
            return;
        case kCardinalParameterMiniTimeFrame:
            context->frame = static_cast<uint64_t>(value * context->sampleRate + 0.5f);
            return;
        case kCardinalParameterMiniTimeBarStartTick:
            context->barStartTick = value;
            return;
        case kCardinalParameterMiniTimeBeatsPerMinute:
            context->beatsPerMinute = value;
            context->ticksPerFrame = 1.0 / (60.0 * context->sampleRate / context->beatsPerMinute / context->ticksPerBeat);
            return;
        case kCardinalParameterMiniTimeTick:
            context->tick = value;
            context->tickClock = std::fmod(context->tick, context->ticksPerClock);
            return;
        case kCardinalParameterMiniTimeTicksPerBeat:
            context->ticksPerBeat = value;
            context->ticksPerClock = context->ticksPerBeat / context->beatType;
            context->ticksPerFrame = 1.0 / (60.0 * context->sampleRate / context->beatsPerMinute / context->ticksPerBeat);
            context->tickClock = std::fmod(context->tick, context->ticksPerClock);
            return;
        }
       #endif
    }

    void stateChanged(const char* const key, const char* const value) override
    {
       #if CARDINAL_VARIANT_MINI && ! DISTRHO_PLUGIN_WANT_DIRECT_ACCESS
        if (std::strcmp(key, "patch") == 0)
        {
            if (fAutosavePath.empty())
                return;

            rack::system::removeRecursively(fAutosavePath);
            rack::system::createDirectories(fAutosavePath);

            FILE* const f = std::fopen(rack::system::join(fAutosavePath, "patch.json").c_str(), "w");
            DISTRHO_SAFE_ASSERT_RETURN(f != nullptr,);

            std::fwrite(value, std::strlen(value), 1, f);
            std::fclose(f);

            const ScopedContext sc(this);

            try {
                context->patch->loadAutosave();
            } catch(const rack::Exception& e) {
                d_stderr(e.what());
            } DISTRHO_SAFE_EXCEPTION_RETURN("setState loadAutosave",);

            return;
        }
       #endif

        if (std::strcmp(key, "windowSize") == 0)
        {
            int width = 0;
            int height = 0;
            std::sscanf(value, "%d:%d", &width, &height);

            if (width > 0 && height > 0)
            {
                const double scaleFactor = getScaleFactor();
                setSize(width * scaleFactor, height * scaleFactor);
            }

            return;
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
       #ifdef DPF_RUNTIME_TESTING
        if (inSelfTest) return false;
       #endif

        if (ev.press)
            getWindow().focus();

        const int action = ev.press ? GLFW_PRESS : GLFW_RELEASE;
        int mods = glfwMods(ev.mod);

        int button;

        switch (ev.button)
        {
        case kMouseButtonLeft: button = GLFW_MOUSE_BUTTON_LEFT;   break;
        case kMouseButtonRight: button = GLFW_MOUSE_BUTTON_RIGHT;  break;
        case kMouseButtonMiddle: button = GLFW_MOUSE_BUTTON_MIDDLE; break;
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
       #ifdef DPF_RUNTIME_TESTING
        if (inSelfTest) return false;
       #endif

        const rack::math::Vec mousePos = rack::math::Vec(ev.pos.getX(), ev.pos.getY()).div(getScaleFactor()).round();
        const rack::math::Vec mouseDelta = mousePos.minus(lastMousePos);

        lastMousePos = mousePos;

        const ScopedContext sc(this, glfwMods(ev.mod));
        return context->event->handleHover(mousePos, mouseDelta);
    }

    bool onScroll(const ScrollEvent& ev) override
    {
       #ifdef DPF_RUNTIME_TESTING
        if (inSelfTest) return false;
       #endif

        rack::math::Vec scrollDelta = rack::math::Vec(-ev.delta.getX(), ev.delta.getY());
       #ifndef DISTRHO_OS_MAC
        scrollDelta = scrollDelta.mult(50.0);
       #endif

        const int mods = glfwMods(ev.mod);
        const ScopedContext sc(this, mods);
        return context->event->handleScroll(lastMousePos, scrollDelta);
    }

    bool onCharacterInput(const CharacterInputEvent& ev) override
    {
       #ifdef DPF_RUNTIME_TESTING
        if (inSelfTest) return false;
       #endif

        if (ev.character < ' ' || ev.character >= kKeyDelete)
            return false;

        const int mods = glfwMods(ev.mod);
        const ScopedContext sc(this, mods);
        return context->event->handleText(lastMousePos, ev.character);
    }

    bool onKeyboard(const KeyboardEvent& ev) override
    {
       #ifdef DPF_RUNTIME_TESTING
        if (inSelfTest) return false;
       #endif

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
        const int width = static_cast<int>(ev.size.getWidth() / scaleFactor + 0.5);
        const int height = static_cast<int>(ev.size.getHeight() / scaleFactor + 0.5);

        char sizeString[64] = {};
        std::snprintf(sizeString, sizeof(sizeString), "%d:%d", width, height);
        setState("windowSize", sizeString);

        if (rack::isStandalone())
            rack::settings::windowSize = rack::math::Vec(width, height);
    }

    void uiFocus(const bool focus, CrossingMode) override
    {
       #ifdef DPF_RUNTIME_TESTING
        if (inSelfTest) return;
       #endif

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
        context->patch->pushRecentPath(sfilename);
        context->history->setSaved();

       #ifdef DISTRHO_OS_WASM
        rack::syncfs();
       #else
        rack::settings::save();
       #endif
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
