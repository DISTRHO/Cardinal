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

#include "plugincontext.hpp"

// -----------------------------------------------------------------------------------------------------------

USE_NAMESPACE_DISTRHO;

struct HostParameters : Module {
    enum ParamIds {
        NUM_PARAMS
    };
    enum InputIds {
        NUM_INPUTS
    };
    enum OutputIds {
        NUM_OUTPUTS = 24
    };
    enum LightIds {
        NUM_LIGHTS
    };

    rack::dsp::SlewLimiter parameters[kModuleParameters];
    float sampleTime = 0.0f;

    HostParameters()
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        const CardinalPluginContext* const pcontext = static_cast<CardinalPluginContext*>(APP);

        if (pcontext == nullptr)
            throw rack::Exception("Plugin context is null.");

        const float fsampleRate = static_cast<float>(pcontext->sampleRate);
        SampleRateChangeEvent e = {
            fsampleRate,
            1.0f / fsampleRate
        };
        onSampleRateChange(e);
    }

    void process(const ProcessArgs&) override
    {
        if (const CardinalPluginContext* const pcontext = static_cast<CardinalPluginContext*>(APP))
        {
            for (uint32_t i=0; i<kModuleParameters; ++i)
                outputs[i].setVoltage(parameters[i].process(sampleTime, pcontext->parameters[i]));
        }
    }

    void onSampleRateChange(const SampleRateChangeEvent& e) override
    {
        if (const CardinalPluginContext* const pcontext = static_cast<CardinalPluginContext*>(APP))
        {
            const double fall = 1.0 / (double(pcontext->bufferSize) / e.sampleRate);

            for (uint32_t i=0; i<kModuleParameters; ++i)
            {
                parameters[i].reset();
                parameters[i].setRiseFall(fall, fall);
            }

            sampleTime = e.sampleTime;
        }
    }
};

#ifndef HEADLESS
struct CardinalParameterPJ301MPort : PJ301MPort {
	void onDragStart(const DragStartEvent& e) override {
        if (const CardinalPluginContext* const pcontext = static_cast<CardinalPluginContext*>(APP))
            handleHostParameterDrag(pcontext, portId, true);
        PJ301MPort::onDragStart(e);
    }
	void onDragEnd(const DragEndEvent& e) override {
        if (const CardinalPluginContext* const pcontext = static_cast<CardinalPluginContext*>(APP))
            handleHostParameterDrag(pcontext, portId, false);
        PJ301MPort::onDragEnd(e);
    }
};

struct HostParametersWidget : ModuleWidget {
    static constexpr const float startX = 10.0f;
    static constexpr const float startY = 90.0f;
    static constexpr const float paddingH = 30.0f;
    static constexpr const float paddingV = 49.0f;

    HostParametersWidget(HostParameters* const module)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/HostParameters.svg")));

        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        for (int i=0; i<24; ++i)
        {
            const float x = startX + int(i / 6) * paddingH;
            const float y = startY + int(i % 6) * paddingV;
            addOutput(createOutput<CardinalParameterPJ301MPort>(Vec(x, y), module, i));
        }
    }

    void draw(const DrawArgs& args) override
    {
        nvgBeginPath(args.vg);
        nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
        nvgFillPaint(args.vg, nvgLinearGradient(args.vg, 0, 0, 0, box.size.y,
                                                nvgRGB(0x18, 0x19, 0x19), nvgRGB(0x21, 0x22, 0x22)));
        nvgFill(args.vg);

        nvgFontFaceId(args.vg, 0);
        nvgFontSize(args.vg, 14);

        char text[] = { '0', '0', '\0' };
        for (int i=0; i<24; ++i)
        {
            const float x = startX + int(i / 6) * paddingH;
            const float y = startY + int(i % 6) * paddingV;
            nvgBeginPath(args.vg);
            nvgRoundedRect(args.vg, x - 1.0f, y - 19.0f, paddingH - 4.0f, paddingV - 4.0f, 4);
            nvgFillColor(args.vg, nvgRGB(0xd0, 0xd0, 0xd0));
            nvgFill(args.vg);

            if (text[1]++ == '9')
            {
                text[1] = '0';
                ++text[0];
            }
            nvgBeginPath(args.vg);
            nvgFillColor(args.vg, color::BLACK);
            nvgText(args.vg, x + 4.0f, y - 4.0f, text, nullptr);
        }

        ModuleWidget::draw(args);
    }
};
#else
typedef ModuleWidget HostParametersWidget;
#endif


Model* modelHostParameters = createModel<HostParameters, HostParametersWidget>("HostParameters");
