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

#include "ImGuiWidget.hpp"

#define BUFFER_SIZE 128

struct IldaeilModule : Module {
    enum ParamIds {
        NUM_PARAMS
    };
    enum InputIds {
        INPUT1,
        INPUT2,
        NUM_INPUTS
    };
    enum OutputIds {
        OUTPUT1,
        OUTPUT2,
        NUM_OUTPUTS
    };
    enum LightIds {
        NUM_LIGHTS
    };

    IldaeilModule() {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    }

    void process(const ProcessArgs&) override {
    }
};

struct IldaeilWidget : ImGuiWidget {
    IldaeilModule* const module;

    IldaeilWidget(IldaeilModule* const m, const float w, const float h)
        : ImGuiWidget(w, h),
          module(m) {}
};

struct IldaeilModuleWidget : ModuleWidget {
    IldaeilWidget* ildaeilWidget = nullptr;

    IldaeilModuleWidget(IldaeilModule* const module)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/glBars.svg")));

        ildaeilWidget = new IldaeilWidget(module, box.size.x - 2 * RACK_GRID_WIDTH, box.size.y);
        ildaeilWidget->box.pos = Vec(2 * RACK_GRID_WIDTH, 0);
        ildaeilWidget->box.size = Vec(box.size.x - 2 * RACK_GRID_WIDTH, box.size.y);
        addChild(ildaeilWidget);

        addChild(createWidget<ScrewSilver>(Vec(0, 0)));
        addChild(createWidget<ScrewSilver>(Vec(0, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addInput(createInput<PJ301MPort>(Vec(3, 54), module, IldaeilModule::INPUT1));
        addInput(createInput<PJ301MPort>(Vec(3, 54 + 30), module, IldaeilModule::INPUT2));
        addInput(createOutput<PJ301MPort>(Vec(3, 54 + 60), module, IldaeilModule::OUTPUT1));
        addInput(createOutput<PJ301MPort>(Vec(3, 54 + 90), module, IldaeilModule::OUTPUT2));
    }
};

Model* modelIldaeil = createModel<IldaeilModule, IldaeilModuleWidget>("Ildaeil");
