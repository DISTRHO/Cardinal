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
# include "ModuleWidgets.hpp"
# include "Widgets.hpp"
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
struct glBarsRendererWidget : OpenGlWidgetWithBrowserPreview {
    glBarsModule* const glBars;

    glBarsRendererWidget(glBarsModule* const module)
        : glBars(module)
    {
        if (glBars != nullptr && APP->window->pixelRatio < 2.0f)
            oversample = 2.0f;
    }

    void draw(const DrawArgs&) override
    {
    }

    void drawLayer(const DrawArgs& args, const int layer) override
    {
        if (layer != 1)
            return;

        OpenGlWidgetWithBrowserPreview::draw(args);
    }

    void drawFramebuffer() override
    {
        DISTRHO_SAFE_ASSERT_RETURN(glBars != nullptr,);

        drawFramebuffer(glBars->state, getFramebufferSize());
    }

    void drawFramebufferForBrowserPreview() override
    {
        glBarsState state;
        drawFramebuffer(state, box.size);
    }

    void drawFramebuffer(glBarsState& state, const Vec& fbSize)
    {
        glDisable(GL_BLEND);
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glViewport(0.0, -50 * oversample, fbSize.x * oversample, fbSize.y * oversample);
        glFrustum(-1, 1, -1, 1, 1.5, 10);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        state.Render();

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glEnable(GL_BLEND);
    }

    void step() override
    {
        OpenGlWidget::step();

        if (glBars != nullptr)
            oversample = APP->window->pixelRatio < 2.0f ? 2.0f : 1.0f;
    }
};

struct glBarsWidget : ModuleWidgetWith25HP {
    glBarsWidget(glBarsModule* const module)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/glBars.svg")));

        createAndAddScrews();

        addInput(createInput<PJ301MPort>(Vec(135.0f, 20.0f), module, glBarsModule::IN1_INPUT));

        const float size = mm2px(127.0f);
        glBarsRendererWidget* const glBarsRenderer = new glBarsRendererWidget(module);
        glBarsRenderer->box.pos = Vec((box.size.x - size) * 0.5f, (box.size.y - size) * 0.5f);
        glBarsRenderer->box.size = Vec(size, size);
        addChild(glBarsRenderer);
    }

    void draw(const DrawArgs& args) override
    {
        drawBackground(args.vg);
        ModuleWidgetWith25HP::draw(args);
    }
};
#else
struct glBarsWidget : ModuleWidget {
    glBarsWidget(glBarsModule* const module) {
        setModule(module);

        addInput(createInput<PJ301MPort>({}, module, glBarsModule::IN1_INPUT));
    }
};
#endif

Model* modelGlBars = createModel<glBarsModule, glBarsWidget>("glBars");
