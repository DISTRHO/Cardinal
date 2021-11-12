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

#include "plugin.hpp"

struct CardinalBlankModule : Module {
    enum ParamIds {
        NUM_PARAMS
    };
    enum InputIds {
        NUM_INPUTS
    };
    enum OutputIds {
        NUM_OUTPUTS
    };
    enum LightIds {
        NUM_LIGHTS
    };

    CardinalBlankModule() {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    }
};

struct CardinalBlankImage : OpaqueWidget {
    int imageId = -2;
    int imageWidth = 0;
    int imageHeight = 0;

    CardinalBlankImage(const math::Vec& size) {
        box.size = size;
    }

    /*
    ~CardinalBlankWidget()
    {
        if (imageId >= 0)
            nvgDeleteImage(args.vg, imageId);
    }
    */

    void draw(const DrawArgs& args) override
    {
        if (imageId == -2)
        {
            imageId = nvgCreateImage(args.vg, asset::plugin(pluginInstance, "res/Miku/Miku.png").c_str(), 0);

            if (imageId != -1)
                nvgImageSize(args.vg, imageId, &imageWidth, &imageHeight);
        }

        if (imageId != -1 && imageWidth != 0 && imageHeight != 0)
        {
            const float boxscale = std::min(box.size.x / imageWidth, box.size.y / imageHeight);
            const float imgHeight = imageHeight * boxscale;
            nvgBeginPath(args.vg);
            nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
            nvgFillPaint(args.vg, nvgImagePattern(args.vg,
                                                  0,
                                                  box.size.y * 0.5 - imgHeight * 0.5f,
                                                  box.size.x,
                                                  imageHeight * boxscale, 0, imageId, 1.0f));
            nvgFill(args.vg);
        }

        OpaqueWidget::draw(args);
    }
};

struct CardinalBlankWidget : ModuleWidget {
    CardinalBlankWidget(CardinalBlankModule* const module) {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Blank.svg")));

        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        FramebufferWidget* const fbWidget = new FramebufferWidget;
        fbWidget->oversample = 2.0;
        fbWidget->addChild(new CardinalBlankImage(box.size));
        addChild(fbWidget);
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

Model* modelCardinalBlank = createModel<CardinalBlankModule, CardinalBlankWidget>("Blank");
