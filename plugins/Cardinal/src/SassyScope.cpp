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

    void drawImGui() override
    {
        const float scaleFactor = getScaleFactor();

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(box.size.x * scaleFactor, box.size.y * scaleFactor));

        do_show_scope_window(module != nullptr ? &module->scope : getFakeScopeInstance(), scaleFactor);
    }

    void onButton(const ButtonEvent& e)
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
        }

        ImGuiWidget::onButton(e);
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

