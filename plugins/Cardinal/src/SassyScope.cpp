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

#include "plugincontext.hpp"
#include "ImGuiWidget.hpp"
#include "sassy/sassy.hpp"
#include "sassy/sassy_scope.cpp"

namespace ffft {
template<class DT, int size>
class FFTRealWithSize : public FFTReal<DT> {
public:
    explicit FFTRealWithSize() : FFTReal<DT>(size) {}
};
}

struct SassyScopeModule : Module {
    enum ParamIds {
        NUM_PARAMS
    };
    enum InputIds {
        INPUT1,
        INPUT2,
        INPUT3,
        INPUT4,
        NUM_INPUTS
    };
    enum OutputIds {
        NUM_OUTPUTS
    };
    enum LightIds {
        NUM_LIGHTS
    };

    ScopeData scope;

    ffft::FFTRealWithSize<float, 16*2> fftObj16;
    ffft::FFTRealWithSize<float, 32*2> fftObj32;
    ffft::FFTRealWithSize<float, 64*2> fftObj64;
    ffft::FFTRealWithSize<float, 128*2> fftObj128;
    ffft::FFTRealWithSize<float, 256*2> fftObj256;
    ffft::FFTRealWithSize<float, 512*2> fftObj512;
    ffft::FFTRealWithSize<float, 1024*2> fftObj1024;
    ffft::FFTRealWithSize<float, 2048*2> fftObj2048;
    ffft::FFTRealWithSize<float, 4096*2> fftObj4096;
    ffft::FFTRealWithSize<float, 8192*2> fftObj8192;
    ffft::FFTRealWithSize<float, 16384*2> fftObj16384;
    ffft::FFTRealWithSize<float, 32768*2> fftObj32768;
    ffft::FFTRealWithSize<float, 65536*2> fftObj65536;

    SassyScopeModule()
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        scope.fft.average = 1;
        scope.fft.obj16 = &fftObj16;
        scope.fft.obj32 = &fftObj32;
        scope.fft.obj64 = &fftObj64;
        scope.fft.obj128 = &fftObj128;
        scope.fft.obj256 = &fftObj256;
        scope.fft.obj512 = &fftObj512;
        scope.fft.obj1024 = &fftObj1024;
        scope.fft.obj2048 = &fftObj2048;
        scope.fft.obj4096 = &fftObj4096;
        scope.fft.obj8192 = &fftObj8192;
        scope.fft.obj16384 = &fftObj16384;
        scope.fft.obj32768 = &fftObj32768;
        scope.fft.obj65536 = &fftObj65536;
    }

    void process(const ProcessArgs&) override
    {
        scope.probe(inputs[INPUT1].getVoltage(),
                    inputs[INPUT2].getVoltage(),
                    inputs[INPUT3].getVoltage(),
                    inputs[INPUT4].getVoltage());
    }

    void onSampleRateChange(const SampleRateChangeEvent& e) override
    {
        scope.realloc(e.sampleRate);
    }

    json_t* dataToJson() override
    {
        json_t* const rootJ = json_object();
        DISTRHO_SAFE_ASSERT_RETURN(rootJ != nullptr, nullptr);

        json_object_set_new(rootJ, "mTimeScale", json_real(scope.mTimeScale));
        json_object_set_new(rootJ, "mTimeScaleSlider", json_integer(scope.mTimeScaleSlider));
        json_object_set_new(rootJ, "mSyncMode", json_integer(scope.mSyncMode));
        json_object_set_new(rootJ, "mSyncChannel", json_integer(scope.mSyncChannel));
        json_object_set_new(rootJ, "mMode", json_integer(scope.mMode));
        json_object_set_new(rootJ, "mDisplay", json_integer(scope.mDisplay));
        json_object_set_new(rootJ, "mFFTZoom", json_integer(scope.mFFTZoom));
        json_object_set_new(rootJ, "mPot", json_integer(scope.mPot));
        json_object_set_new(rootJ, "fft.average", json_integer(scope.fft.average));

        json_object_set_new(rootJ, "mCh0.mEnabled", json_boolean(scope.mCh[0].mEnabled));
        json_object_set_new(rootJ, "mCh0.mScale", json_real(scope.mCh[0].mScale));
        json_object_set_new(rootJ, "mCh0.mScaleSlider", json_integer(scope.mCh[0].mScaleSlider));
        json_object_set_new(rootJ, "mCh0.mOffset", json_integer(scope.mCh[0].mOffset));

        json_object_set_new(rootJ, "mCh1.mEnabled", json_boolean(scope.mCh[1].mEnabled));
        json_object_set_new(rootJ, "mCh1.mScale", json_real(scope.mCh[1].mScale));
        json_object_set_new(rootJ, "mCh1.mScaleSlider", json_integer(scope.mCh[1].mScaleSlider));
        json_object_set_new(rootJ, "mCh1.mOffset", json_integer(scope.mCh[1].mOffset));

        json_object_set_new(rootJ, "mCh2.mEnabled", json_boolean(scope.mCh[2].mEnabled));
        json_object_set_new(rootJ, "mCh2.mScale", json_real(scope.mCh[2].mScale));
        json_object_set_new(rootJ, "mCh2.mScaleSlider", json_integer(scope.mCh[2].mScaleSlider));
        json_object_set_new(rootJ, "mCh2.mOffset", json_integer(scope.mCh[2].mOffset));

        json_object_set_new(rootJ, "mCh3.mEnabled", json_boolean(scope.mCh[3].mEnabled));
        json_object_set_new(rootJ, "mCh3.mScale", json_real(scope.mCh[3].mScale));
        json_object_set_new(rootJ, "mCh3.mScaleSlider", json_integer(scope.mCh[3].mScaleSlider));
        json_object_set_new(rootJ, "mCh3.mOffset", json_integer(scope.mCh[3].mOffset));

        return rootJ;
    }

    void dataFromJson(json_t* const rootJ) override
    {
        if (json_t* const mTimeScaleJ = json_object_get(rootJ, "mTimeScale"))
            scope.mTimeScale = json_real_value(mTimeScaleJ);

        if (json_t* const mTimeScaleSliderJ = json_object_get(rootJ, "mTimeScaleSlider"))
            scope.mTimeScaleSlider = json_integer_value(mTimeScaleSliderJ);

        if (json_t* const mSyncModeJ = json_object_get(rootJ, "mSyncMode"))
            scope.mSyncMode = json_integer_value(mSyncModeJ);

        if (json_t* const mSyncChannelJ = json_object_get(rootJ, "mSyncChannel"))
            scope.mSyncChannel = json_integer_value(mSyncChannelJ);

        if (json_t* const mModeJ = json_object_get(rootJ, "mMode"))
            scope.mMode = json_integer_value(mModeJ);

        if (json_t* const mDisplayJ = json_object_get(rootJ, "mDisplay"))
            scope.mDisplay = json_integer_value(mDisplayJ);

        if (json_t* const mFFTZoomJ = json_object_get(rootJ, "mFFTZoom"))
            scope.mFFTZoom = json_integer_value(mFFTZoomJ);

        if (json_t* const mPotJ = json_object_get(rootJ, "mPot"))
            scope.mPot = json_integer_value(mPotJ);

        if (json_t* const fftAverageJ = json_object_get(rootJ, "fft.average"))
            scope.fft.average = json_integer_value(fftAverageJ);

        {
            if (json_t* const mCh0mEnabledJ = json_object_get(rootJ, "mCh0.mEnabled"))
                scope.mCh[0].mEnabled = json_boolean_value(mCh0mEnabledJ);

            if (json_t* const mCh0mScaleJ = json_object_get(rootJ, "mCh0.mScale"))
                scope.mCh[0].mScale = json_real_value(mCh0mScaleJ);

            if (json_t* const mCh0mScaleSliderJ = json_object_get(rootJ, "mCh0.mScaleSlider"))
                scope.mCh[0].mScaleSlider = json_integer_value(mCh0mScaleSliderJ);

            if (json_t* const mCh0mOffsetJ = json_object_get(rootJ, "mCh0.mOffset"))
                scope.mCh[0].mOffset = json_integer_value(mCh0mOffsetJ);
        }

        {
            if (json_t* const mCh1mEnabledJ = json_object_get(rootJ, "mCh1.mEnabled"))
                scope.mCh[1].mEnabled = json_boolean_value(mCh1mEnabledJ);

            if (json_t* const mCh1mScaleJ = json_object_get(rootJ, "mCh1.mScale"))
                scope.mCh[1].mScale = json_real_value(mCh1mScaleJ);

            if (json_t* const mCh1mScaleSliderJ = json_object_get(rootJ, "mCh1.mScaleSlider"))
                scope.mCh[1].mScaleSlider = json_integer_value(mCh1mScaleSliderJ);

            if (json_t* const mCh1mOffsetJ = json_object_get(rootJ, "mCh1.mOffset"))
                scope.mCh[1].mOffset = json_integer_value(mCh1mOffsetJ);
        }

        {
            if (json_t* const mCh2mEnabledJ = json_object_get(rootJ, "mCh2.mEnabled"))
                scope.mCh[2].mEnabled = json_boolean_value(mCh2mEnabledJ);

            if (json_t* const mCh2mScaleJ = json_object_get(rootJ, "mCh2.mScale"))
                scope.mCh[2].mScale = json_real_value(mCh2mScaleJ);

            if (json_t* const mCh2mScaleSliderJ = json_object_get(rootJ, "mCh2.mScaleSlider"))
                scope.mCh[2].mScaleSlider = json_integer_value(mCh2mScaleSliderJ);

            if (json_t* const mCh2mOffsetJ = json_object_get(rootJ, "mCh2.mOffset"))
                scope.mCh[2].mOffset = json_integer_value(mCh2mOffsetJ);
        }

        {
            if (json_t* const mCh3mEnabledJ = json_object_get(rootJ, "mCh3.mEnabled"))
                scope.mCh[3].mEnabled = json_boolean_value(mCh3mEnabledJ);

            if (json_t* const mCh3mScaleJ = json_object_get(rootJ, "mCh3.mScale"))
                scope.mCh[3].mScale = json_real_value(mCh3mScaleJ);

            if (json_t* const mCh3mScaleSliderJ = json_object_get(rootJ, "mCh3.mScaleSlider"))
                scope.mCh[3].mScaleSlider = json_integer_value(mCh3mScaleSliderJ);

            if (json_t* const mCh3mOffsetJ = json_object_get(rootJ, "mCh3.mOffset"))
                scope.mCh[3].mOffset = json_integer_value(mCh3mOffsetJ);
        }
    }
};

// used for module browser
static ScopeData* getFakeScopeInstance()
{
    static ScopeData scope;
    static ffft::FFTReal<float> fftObj16(16*2);
    static ffft::FFTReal<float> fftObj32(32*2);
    static ffft::FFTReal<float> fftObj64(64*2);
    static ffft::FFTReal<float> fftObj128(128*2);
    static ffft::FFTReal<float> fftObj256(256*2);
    static ffft::FFTReal<float> fftObj512(512*2);
    static ffft::FFTReal<float> fftObj1024(1024*2);
    static ffft::FFTReal<float> fftObj2048(2048*2);
    static ffft::FFTReal<float> fftObj4096(4096*2);
    static ffft::FFTReal<float> fftObj8192(8192*2);
    static ffft::FFTReal<float> fftObj16384(16384*2);
    static ffft::FFTReal<float> fftObj32768(32768*2);
    static ffft::FFTReal<float> fftObj65536(65536*2);

    static bool needsInit = true;

    if (needsInit)
    {
        needsInit = false;
        scope.fft.average = 1;
        scope.fft.obj16 = &fftObj16;
        scope.fft.obj32 = &fftObj32;
        scope.fft.obj64 = &fftObj64;
        scope.fft.obj128 = &fftObj128;
        scope.fft.obj256 = &fftObj256;
        scope.fft.obj512 = &fftObj512;
        scope.fft.obj1024 = &fftObj1024;
        scope.fft.obj2048 = &fftObj2048;
        scope.fft.obj4096 = &fftObj4096;
        scope.fft.obj8192 = &fftObj8192;
        scope.fft.obj16384 = &fftObj16384;
        scope.fft.obj32768 = &fftObj32768;
        scope.fft.obj65536 = &fftObj65536;
        scope.realloc(48000);
    }

    return &scope;
}

struct SassyScopeWidget : ImGuiWidget {
    SassyScopeModule* module = nullptr;
    int lastClickedSliderBox = -1;
    Rect sliderBoxes[8];

    SassyScopeWidget()
    {
        for (int i=0; i<8; ++i)
        {
            sliderBoxes[i].pos = Vec(8 + (i % 4) * 27, 32 + (i / 4) * 153);
            sliderBoxes[i].size = Vec(19, 150);
        }
    }

    void drawImGui() override
    {
        const float scaleFactor = getScaleFactor();

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(box.size.x * scaleFactor, box.size.y * scaleFactor));

        do_show_scope_window(module != nullptr ? &module->scope : getFakeScopeInstance(), scaleFactor);
    }

    void onButton(const ButtonEvent& e) override
    {
        // if mouse press is over draggable areas, do nothing so event can go to Rack
        if (e.action == GLFW_PRESS)
        {
            // bottom left
            if (e.pos.x < 116 && e.pos.y >= 335)
                return;
            // bottom right
            if (e.pos.x >= 456 && e.pos.y >= 348)
                return;
            // fft label
            if (e.pos.x >= 491 && e.pos.y >= 54 && e.pos.y <= 74)
                return;
            // nudge label
            if (e.pos.x >= 463 && e.pos.y >= 236 && e.pos.y <= 255)
                return;
            // center scope
            if (e.pos.x >= 110 && e.pos.x <= 452 && e.pos.y >= 0 && e.pos.y <= 350)
                return;

            // consume for double-click if event belongs to a slider
            lastClickedSliderBox = -1;
            for (int i=0; i<8; ++i)
            {
                if (sliderBoxes[i].contains(e.pos))
                {
                    lastClickedSliderBox = i;
                    e.consume(this);
                    break;
                }
            }
        }

        ImGuiWidget::onButton(e);
    }

    void onDoubleClick(const DoubleClickEvent& e) override
    {
        // handle double-click for slider param reset
        if (lastClickedSliderBox != -1)
        {
            const int i = lastClickedSliderBox;
            lastClickedSliderBox = -1;

            // fake a mouse release
            ButtonEvent e2 = {};
            e2.button = GLFW_MOUSE_BUTTON_LEFT;
            e2.action = GLFW_RELEASE;
            ImGuiWidget::onButton(e2);

            // do the reset
            if (i < 4)
            {
                module->scope.mCh[i].mScaleSlider = 0;
                module->scope.mCh[i].mScale = 1.0f / 5.0f;
            }
            else
            {
                module->scope.mCh[i-4].mOffset = 0;
            }

            e.consume(this);
            return;
        }

        ImGuiWidget::onDoubleClick(e);
    }
};

struct SassyScopeModuleWidget : ModuleWidget {
    SassyScopeModule* scopeModule = nullptr;
    SassyScopeWidget* scopeWidget = nullptr;

    SassyScopeModuleWidget(SassyScopeModule* const module) {
        setModule(module);
        box.size = Vec(RACK_GRID_WIDTH * 37, RACK_GRID_HEIGHT);

        scopeModule = module;
        scopeWidget = new SassyScopeWidget();
        scopeWidget->box.pos = Vec(0, 0);
        scopeWidget->box.size = Vec(box.size.x, box.size.y);
        scopeWidget->module = module;
        addChild(scopeWidget);

        for (int i=0; i<SassyScopeModule::NUM_INPUTS; ++i)
            addInput(createInput<PJ301MPort>(Vec(5 + 26.5f * i, RACK_GRID_HEIGHT - 40), module, i));
    }

    void draw(const DrawArgs& args) override
    {
        nvgBeginPath(args.vg);
        nvgRect(args.vg, 0.0, 0.0, box.size.x, box.size.y);
        nvgFillColor(args.vg, nvgRGB(0x20, 0x20, 0x20));
        nvgFill(args.vg);
        ModuleWidget::draw(args);
    }

    void step() override
    {
        ModuleWidget::step();

        if (scopeModule == nullptr)
            return;

        // Update colors
        for (int i=0; i<SassyScopeModule::NUM_INPUTS; ++i)
        {
            if (CableWidget* const cableWidget = APP->scene->rack->getTopCable(getInput(i)))
            {
                NVGcolor c = cableWidget->color;
                scopeModule->scope.colors[i] = (clamp(int(c.a * 0xff), 0, 0xff) << 24)
                                             | (clamp(int(c.b * 0xff), 0, 0xff) << 16)
                                             | (clamp(int(c.g * 0xff), 0, 0xff) << 8)
                                             | clamp(int(c.r * 0xff), 0, 0xff);
            }
        }
    }

    void appendContextMenu(Menu* const menu) override
    {
        menu->addChild(new MenuSeparator);

        struct AveragingItem : MenuItem {
            ScopeData* scope;
            Menu* createChildMenu() override {
                Menu* menu = new Menu;
                menu->addChild(createCheckMenuItem("1x", "",
                    [=]() {return scope->fft.average == 1;},
                    [=]() {scope->fft.average = 1;}
                ));
                menu->addChild(createCheckMenuItem("4x", "",
                    [=]() {return scope->fft.average == 4;},
                    [=]() {scope->fft.average = 4;}
                ));
                menu->addChild(createCheckMenuItem("16x", "",
                    [=]() {return scope->fft.average == 16;},
                    [=]() {scope->fft.average = 16;}
                ));
                menu->addChild(createCheckMenuItem("64x", "",
                    [=]() {return scope->fft.average == 64;},
                    [=]() {scope->fft.average = 64;}
                ));
                menu->addChild(createCheckMenuItem("256x", "",
                    [=]() {return scope->fft.average == 256;},
                    [=]() {scope->fft.average = 256;}
                ));
                return menu;
            }
        };
        AveragingItem* const averagingItem = new AveragingItem;
        averagingItem->text = "Averaging (FFT mode)";
        averagingItem->rightText = string::f("%d", scopeModule->scope.fft.average) + "  " + RIGHT_ARROW;
        averagingItem->scope = &scopeModule->scope;
        menu->addChild(averagingItem);
    }
};

// --------------------------------------------------------------------------------------------------------------------

Model* modelSassyScope = createModel<SassyScopeModule, SassyScopeModuleWidget>("SassyScope");

// --------------------------------------------------------------------------------------------------------------------

