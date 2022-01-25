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
        : glBars(module)
    {
        if (APP->window->pixelRatio < 2.0f)
            oversample = 2.0f;
    }

    void draw(const DrawArgs&) override
    {
    }

    void drawLayer(const DrawArgs& args, int layer) override
    {
        if (layer != 1)
            return;

        OpenGlWidget::draw(args);
    }

    void drawFramebuffer() override {
        math::Vec fbSize = getFramebufferSize();

        glDisable(GL_BLEND);
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glViewport(0.0, -100, fbSize.x * oversample, fbSize.y * oversample);
        glFrustum(-1, 1, -1, 1, 1.5, 10);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBars->state.Render();

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glEnable(GL_BLEND);
    }

    void step() override {
        OpenGlWidget::step();

        oversample = APP->window->pixelRatio < 2.0f ? 2.0f : 1.0f;
    }
};

struct glBarsWidget : ModuleWidget {
    glBarsRendererWidget* const glBarsRenderer;

    glBarsWidget(glBarsModule* const module)
        : glBarsRenderer(new glBarsRendererWidget(module))
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/glBars.svg")));

        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addInput(createInput<PJ301MPort>(Vec(135.0f, 20.0f), module, glBarsModule::IN1_INPUT));

        const float size = mm2px(127.0f);
        glBarsRenderer->box.pos = Vec((box.size.x - size) * 0.5f, (box.size.y - size) * 0.5f);
        glBarsRenderer->box.size = Vec(size, size);
        addChild(glBarsRenderer);
    }

    void draw(const DrawArgs& args) override
    {
        nvgBeginPath(args.vg);
        nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
        nvgFillPaint(args.vg, nvgLinearGradient(args.vg, 0, 0, 0, box.size.y,
                                                nvgRGB(0x18, 0x19, 0x19), nvgRGB(0x21, 0x22, 0x22)));
        nvgFill(args.vg);

        ModuleWidget::draw(args);
    }
};
#else
typedef ModuleWidget glBarsWidget;
#endif

Model* modelGlBars = createModel<glBarsModule, glBarsWidget>("glBars");
