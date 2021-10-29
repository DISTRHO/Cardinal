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
        kHostTimeBar,
        kHostTimeBeat,
        kHostTimeClock,
        kHostTimeCount
    };

    rack::dsp::PulseGenerator pulseBar, pulseBeat, pulseClock;
    float sampleTime = 0.0f;

    HostTime()
    {
        config(NUM_PARAMS, NUM_INPUTS, kHostTimeCount, kHostTimeCount);

        CardinalPluginContext* const pcontext = reinterpret_cast<CardinalPluginContext*>(APP);

        if (pcontext == nullptr)
            throw rack::Exception("Plugin context is null.");
    }

    void process(const ProcessArgs& args) override
    {
        if (CardinalPluginContext* const pcontext = reinterpret_cast<CardinalPluginContext*>(APP))
        {
            const bool playing = pcontext->playing;

            if (playing)
            {
                if (pcontext->tick == 0.0)
                {
                    pulseClock.trigger();
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

                if ((pcontext->tickClock += pcontext->ticksPerFrame) >= pcontext->ticksPerClock)
                {
                    pcontext->tickClock -= pcontext->ticksPerClock;
                    pulseClock.trigger();
                }
            }

            const bool hasBar = pulseBar.process(args.sampleTime);
            const bool hasBeat = pulseBeat.process(args.sampleTime);
            const bool hasClock = pulseClock.process(args.sampleTime);

            lights[kHostTimeRolling].setBrightness(playing ? 1.0f : 0.0f);
            lights[kHostTimeBar].setBrightnessSmooth(hasBar ? 1.0f : 0.0f, args.sampleTime * 0.5f);
            lights[kHostTimeBeat].setBrightnessSmooth(hasBeat ? 1.0f : 0.0f, args.sampleTime);
            lights[kHostTimeClock].setBrightnessSmooth(hasClock ? 1.0f : 0.0f, args.sampleTime * 2.0f);

            outputs[kHostTimeRolling].setVoltage(playing ? 10.0f : 0.0f);
            outputs[kHostTimeBar].setVoltage(hasBar ? 10.0f : 0.0f);
            outputs[kHostTimeBeat].setVoltage(hasBeat ? 10.0f : 0.0f);
            outputs[kHostTimeClock].setVoltage(hasClock ? 10.0f : 0.0f);
        }
    }
};

struct HostTimeWidget : ModuleWidget {
    static constexpr const float startX = 10.0f;
    static constexpr const float startY = 73.0f;
    static constexpr const float padding = 32.0f;

    HostTimeWidget(HostTime* const module)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/HostTime.svg")));

        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addOutput(createOutput<PJ301MPort>(Vec(startX, startY + 0 * padding), module, HostTime::kHostTimeRolling));
        addOutput(createOutput<PJ301MPort>(Vec(startX, startY + 1 * padding), module, HostTime::kHostTimeBar));
        addOutput(createOutput<PJ301MPort>(Vec(startX, startY + 2 * padding), module, HostTime::kHostTimeBeat));
        addOutput(createOutput<PJ301MPort>(Vec(startX, startY + 3 * padding), module, HostTime::kHostTimeClock));

        const float x = startX + 28;
        addChild(createLightCentered<SmallLight<GreenLight>> (Vec(x, startY + 0 * padding + 12), module, HostTime::kHostTimeRolling));
        addChild(createLightCentered<SmallLight<RedLight>>   (Vec(x, startY + 1 * padding + 12), module, HostTime::kHostTimeBar));
        addChild(createLightCentered<SmallLight<YellowLight>>(Vec(x, startY + 2 * padding + 12), module, HostTime::kHostTimeBeat));
        addChild(createLightCentered<SmallLight<YellowLight>>(Vec(x, startY + 3 * padding + 12), module, HostTime::kHostTimeClock));
    }

    void drawOutputLine(NVGcontext* const vg, const uint offset, const char* const text)
    {
        const float y = startY + offset * padding;
        nvgBeginPath(vg);
        nvgRoundedRect(vg, startX - 1.0f, y - 2.0f, box.size.x - (startX + 1) * 2, 28.0f, 4);
        nvgFillColor(vg, nvgRGBA(0xda, 0xda, 0xda, 0xf0));
        nvgFill(vg);
        nvgStrokeColor(vg, nvgRGBA(0x4a, 0x4a, 0x4a, 0xc0));
        nvgStroke(vg);
        nvgBeginPath(vg);
        nvgFillColor(vg, color::BLACK);
        nvgText(vg, startX + 36, y + 16, text, nullptr);
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

        drawOutputLine(args.vg, 0, "Playing");
        drawOutputLine(args.vg, 1, "Bar");
        drawOutputLine(args.vg, 2, "Beat");
        drawOutputLine(args.vg, 3, "Clock");

        ModuleWidget::draw(args);
    }
};

Model* modelHostTime = createModel<HostTime, HostTimeWidget>("HostTime");
