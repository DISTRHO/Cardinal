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

// -----------------------------------------------------------------------------------------------------------
// from PluginContext.hpp

namespace DISTRHO {

static constexpr const uint kModuleParameters = 24;

struct CardinalPluginContext : rack::Context {
    uint32_t bufferSize;
    double sampleRate;
    float parameters[kModuleParameters];
    // more stuff follows, but we dont care..
};

}

using namespace DISTRHO;

// -----------------------------------------------------------------------------------------------------------

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

        CardinalPluginContext* const pcontext = reinterpret_cast<CardinalPluginContext*>(APP);

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
        if (CardinalPluginContext* const pcontext = reinterpret_cast<CardinalPluginContext*>(APP))
        {
            for (uint i=0; i<kModuleParameters; ++i)
                outputs[i].setVoltage(parameters[i].process(sampleTime, pcontext->parameters[i]));
        }
    }

    void onSampleRateChange(const SampleRateChangeEvent& e) override
    {
        if (CardinalPluginContext* const pcontext = reinterpret_cast<CardinalPluginContext*>(APP))
        {
            const double fall = 1.0 / (double(pcontext->bufferSize) / e.sampleRate);

            for (uint i=0; i<kModuleParameters; ++i)
            {
                parameters[i].reset();
                parameters[i].setRiseFall(fall, fall);
            }

            sampleTime = e.sampleTime;
        }
    }
};

struct HostParametersWidget : ModuleWidget {
    HostParametersWidget(HostParameters* const module) {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/HostParameters.svg")));

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        const float startX = 10.0f;
        const float startY = 170.0f;
        const float padding = 30.0f;

        for (int i=0; i<24; ++i)
        {
            const float x = startX + int(i / 6) * padding;
            const float y = startY + int(i % 6) * padding;
            addOutput(createOutput<PJ301MPort>(Vec(x, y), module, i));
        }
    }
};

Model* modelHostParameters = createModel<HostParameters, HostParametersWidget>("HostParameters");
