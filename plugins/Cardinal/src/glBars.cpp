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

#include "glBars.hpp"

#define SAMPLES_PER_DRAW 256

struct glBarsModule : Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		IN1_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

    glBarsState state;
    float audioData[SAMPLES_PER_DRAW];
    uint audioDataFill = 0;

    glBarsModule() {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    }

    void process(const ProcessArgs&) override {
        audioData[audioDataFill++] = inputs[IN1_INPUT].getVoltage();

        if (audioDataFill == SAMPLES_PER_DRAW) {
            audioDataFill = 0;
            state.AudioData(audioData, SAMPLES_PER_DRAW);
        }
    }
};

struct glBarsRendererWidget : OpenGlWidget {
    glBarsModule* const glBars;

    glBarsRendererWidget(glBarsModule* const module)
        : glBars(module) {}

    void drawFramebuffer() override {
        math::Vec fbSize = getFramebufferSize();
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0, fbSize.x, fbSize.y, 0.0, 0.0, 1.0);
        glViewport(0.0, 0.0, fbSize.x, fbSize.y);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // glDisable(GL_CULL_FACE);
        // glDisable(GL_STENCIL_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glBars->state.Render();
    }
};

struct glBarsWidget : ModuleWidget {
    glBarsRendererWidget* const glBarsRenderer;

    glBarsWidget(glBarsModule* const module)
        : glBarsRenderer(new glBarsRendererWidget(module))
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/glBars.svg")));

        addChild(createWidget<ScrewSilver>(Vec(0, 0)));
        addChild(createWidget<ScrewSilver>(Vec(0, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        glBarsRenderer->box.pos = Vec(2 * RACK_GRID_WIDTH, 0);
        glBarsRenderer->box.size = Vec(box.size.x - 2 * RACK_GRID_WIDTH, box.size.y);
        addChild(glBarsRenderer);

        addInput(createInput<PJ301MPort>(Vec(3, 54), module, glBarsModule::IN1_INPUT));
    }
};

Model* modelGlBars = createModel<glBarsModule, glBarsWidget>("glBars");
