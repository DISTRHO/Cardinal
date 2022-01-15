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

#ifndef HEADLESS
# include "glBars.hpp"
#else
# include "plugin.hpp"
#endif

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

#ifndef HEADLESS
    glBarsState state;
    float audioData[SAMPLES_PER_DRAW];
    unsigned audioDataFill = 0;
#endif

    glBarsModule() {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    }

    void process(const ProcessArgs&) override {
#ifndef HEADLESS
        audioData[audioDataFill++] = inputs[IN1_INPUT].getVoltage();

        if (audioDataFill == SAMPLES_PER_DRAW) {
            audioDataFill = 0;
            state.AudioData(audioData, SAMPLES_PER_DRAW);
        }
#endif
    }
};

#ifndef HEADLESS
struct glBarsRendererWidget : OpenGlWidget {
    glBarsModule* const glBars;

    glBarsRendererWidget(glBarsModule* const module)
        : glBars(module) {}

    void drawFramebuffer() override {
        math::Vec fbSize = getFramebufferSize();

        glDisable(GL_BLEND);
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glViewport(0.0, 0.0, fbSize.x, fbSize.y);
        glFrustum(-1, 1, -1, 1, 1.5, 10);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glClear(GL_COLOR_BUFFER_BIT);

        glBars->state.Render();

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glEnable(GL_BLEND);
    }
};

struct glBarsWidget : ModuleWidget {
    glBarsRendererWidget* const glBarsRenderer;

    glBarsWidget(glBarsModule* const module)
        : glBarsRenderer(new glBarsRendererWidget(module))
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/glBars.svg")));

        addChild(createWidget<ScrewBlack>(Vec(0, 0)));
        addChild(createWidget<ScrewBlack>(Vec(0, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        glBarsRenderer->box.pos = Vec(2 * RACK_GRID_WIDTH, 0);
        glBarsRenderer->box.size = Vec(box.size.x - 2 * RACK_GRID_WIDTH, box.size.y);
        addChild(glBarsRenderer);

        addInput(createInput<PJ301MPort>(Vec(3, 54), module, glBarsModule::IN1_INPUT));
    }
};
#else
typedef ModuleWidget glBarsWidget;
#endif

Model* modelGlBars = createModel<glBarsModule, glBarsWidget>("glBars");
