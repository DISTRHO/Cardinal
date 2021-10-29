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

struct HostTime : Module {
    enum ParamIds {
        NUM_PARAMS
    };
    enum InputIds {
        NUM_INPUTS
    };
    enum HostTimeIds {
        kHostTimeRolling,
        kHostTimeBeat,
        kHostTimeBar,
        kHostTimeCount
    };

    rack::dsp::PulseGenerator pulseBeat, pulseBar;
    float sampleTime = 0.0f;

    HostTime() {
        config(NUM_PARAMS, NUM_INPUTS, kHostTimeCount, kHostTimeCount);

        CardinalPluginContext* const pcontext = reinterpret_cast<CardinalPluginContext*>(APP);

        if (pcontext == nullptr)
            throw rack::Exception("Plugin context is null.");

        sampleTime = 1.0f / static_cast<float>(pcontext->sampleRate);
    }

    void process(const ProcessArgs&) override
    {
        if (CardinalPluginContext* const pcontext = reinterpret_cast<CardinalPluginContext*>(APP))
        {
            const bool playing = pcontext->playing;

            if (playing)
            {
                if (pcontext->tick == 0.0)
                {
                    pulseBeat.trigger();
                    if (pcontext->beat == 1)
                        pulseBar.trigger();
                }

                if ((pcontext->tick += pcontext->ticksPerFrame) >= pcontext->ticksPerBeat)
                {
                    pcontext->tick -= pcontext->ticksPerBeat;
                    pulseBeat.trigger();

                    if (++pcontext->beat > pcontext->beatsPerBar)
                    {
                        pcontext->beat = 1;
                        ++pcontext->bar;
                        pulseBar.trigger();
                    }
                }
            }

            const bool hasBeat = pulseBeat.process(sampleTime);
            const bool hasBar = pulseBar.process(sampleTime);

            lights[kHostTimeRolling].setBrightness(playing ? 1.0 : 0.0f);
            lights[kHostTimeBeat].setBrightness(hasBeat ? 1.0 : 0.0f);
            lights[kHostTimeBar].setBrightness(hasBar ? 1.0 : 0.0f);

            outputs[kHostTimeRolling].setVoltage(playing ? 10.0f : 0.0f);
            outputs[kHostTimeBeat].setVoltage(hasBeat ? 10.0f : 0.0f);
            outputs[kHostTimeBar].setVoltage(hasBar ? 10.0f : 0.0f);
        }
    }

    void onSampleRateChange(const SampleRateChangeEvent& e) override
    {
        sampleTime = e.sampleTime;
    }
};

struct HostTimeWidget : ModuleWidget {
    HostTimeWidget(HostTime* const module) {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/HostTime.svg")));

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        const float startX = 10.0f;
        const float startY = 170.0f;
        const float padding = 30.0f;

        addOutput(createOutput<PJ301MPort>(Vec(startX, startY + 0 * padding), module, HostTime::kHostTimeRolling));
        addOutput(createOutput<PJ301MPort>(Vec(startX, startY + 1 * padding), module, HostTime::kHostTimeBeat));
        addOutput(createOutput<PJ301MPort>(Vec(startX, startY + 2 * padding), module, HostTime::kHostTimeBar));

		addChild(createLightCentered<SmallLight<GreenLight>>(Vec(startX + padding, startY + 0 * padding), module, HostTime::kHostTimeRolling));
		addChild(createLightCentered<SmallLight<YellowLight>>(Vec(startX + padding, startY + 1 * padding), module, HostTime::kHostTimeBeat));
		addChild(createLightCentered<SmallLight<RedLight>>(Vec(startX + padding, startY + 2 * padding), module, HostTime::kHostTimeBar));
    }
};

Model* modelHostTime = createModel<HostTime, HostTimeWidget>("HostTime");
