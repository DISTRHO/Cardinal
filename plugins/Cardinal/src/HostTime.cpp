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
        kHostTimeReset,
        kHostTimeBar,
        kHostTimeBeat,
        kHostTimeClock,
        kHostTimeBarPhase,
        kHostTimeBeatPhase,
        kHostTimeCount
    };

    rack::dsp::PulseGenerator pulseReset, pulseBar, pulseBeat, pulseClock;
    float sampleTime = 0.0f;
    int64_t lastBlockFrame = -1;
    // cached time values
    struct {
        bool reset = true;
        int32_t bar = 0;
        int32_t beat = 0;
        double tick = 0.0;
        double tickClock = 0.0;
    } timeInfo;

    HostTime()
    {
        config(NUM_PARAMS, NUM_INPUTS, kHostTimeCount, kHostTimeCount);

        const CardinalPluginContext* const pcontext = static_cast<CardinalPluginContext*>(APP);

        if (pcontext == nullptr)
            throw rack::Exception("Plugin context is null.");
    }

    void process(const ProcessArgs& args) override
    {
        if (const CardinalPluginContext* const pcontext = static_cast<CardinalPluginContext*>(APP))
        {
            const int64_t blockFrame = pcontext->engine->getBlockFrame();

            // Update time position if running a new audio block
            if (lastBlockFrame != blockFrame)
            {
                lastBlockFrame = blockFrame;
                timeInfo.reset = pcontext->reset;
                timeInfo.bar = pcontext->bar;
                timeInfo.beat = pcontext->beat;
                timeInfo.tick = pcontext->tick;
                timeInfo.tickClock = pcontext->tickClock;
            }

            const bool playing = pcontext->playing;
            const bool playingWithBBT = playing && pcontext->bbtValid;

            if (playingWithBBT)
            {
                if (timeInfo.tick == 0.0)
                {
                    pulseReset.trigger();
                    pulseClock.trigger();
                    pulseBeat.trigger();
                    if (timeInfo.beat == 1)
                        pulseBar.trigger();
                }

                if (timeInfo.reset)
                {
                    timeInfo.reset = false;
                    pulseReset.trigger();
                }

                if ((timeInfo.tick += pcontext->ticksPerFrame) >= pcontext->ticksPerBeat)
                {
                    timeInfo.tick -= pcontext->ticksPerBeat;
                    pulseBeat.trigger();

                    if (++timeInfo.beat > pcontext->beatsPerBar)
                    {
                        timeInfo.beat = 1;
                        ++timeInfo.bar;
                        pulseBar.trigger();
                    }
                }

                if ((timeInfo.tickClock += pcontext->ticksPerFrame) >= pcontext->ticksPerClock)
                {
                    timeInfo.tickClock -= pcontext->ticksPerClock;
                    pulseClock.trigger();
                }
            }

            const bool hasReset = pulseReset.process(args.sampleTime);
            const bool hasBar = pulseBar.process(args.sampleTime);
            const bool hasBeat = pulseBeat.process(args.sampleTime);
            const bool hasClock = pulseClock.process(args.sampleTime);
            const float beatPhase = playingWithBBT && pcontext->ticksPerBeat > 0.0
                                  ? timeInfo.tick / pcontext->ticksPerBeat
                                  : 0.0f;
            const float barPhase = playingWithBBT && pcontext->beatsPerBar > 0
                                 ? ((float) (timeInfo.beat - 1) + beatPhase) / pcontext->beatsPerBar
                                 : 0.0f;

            lights[kHostTimeRolling].setBrightness(playing ? 1.0f : 0.0f);
            lights[kHostTimeReset].setBrightnessSmooth(hasReset ? 1.0f : 0.0f, args.sampleTime * 0.5f);
            lights[kHostTimeBar].setBrightnessSmooth(hasBar ? 1.0f : 0.0f, args.sampleTime * 0.5f);
            lights[kHostTimeBeat].setBrightnessSmooth(hasBeat ? 1.0f : 0.0f, args.sampleTime);
            lights[kHostTimeClock].setBrightnessSmooth(hasClock ? 1.0f : 0.0f, args.sampleTime * 2.0f);
            lights[kHostTimeBarPhase].setBrightness(barPhase);
            lights[kHostTimeBeatPhase].setBrightness(beatPhase);

            outputs[kHostTimeRolling].setVoltage(playing ? 10.0f : 0.0f);
            outputs[kHostTimeReset].setVoltage(hasReset ? 10.0f : 0.0f);
            outputs[kHostTimeBar].setVoltage(hasBar ? 10.0f : 0.0f);
            outputs[kHostTimeBeat].setVoltage(hasBeat ? 10.0f : 0.0f);
            outputs[kHostTimeClock].setVoltage(hasClock ? 10.0f : 0.0f);
            outputs[kHostTimeBarPhase].setVoltage(barPhase * 10.0f);
            outputs[kHostTimeBeatPhase].setVoltage(beatPhase * 10.0f);
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
        addOutput(createOutput<PJ301MPort>(Vec(startX, startY + 1 * padding), module, HostTime::kHostTimeReset));
        addOutput(createOutput<PJ301MPort>(Vec(startX, startY + 2 * padding), module, HostTime::kHostTimeBar));
        addOutput(createOutput<PJ301MPort>(Vec(startX, startY + 3 * padding), module, HostTime::kHostTimeBeat));
        addOutput(createOutput<PJ301MPort>(Vec(startX, startY + 4 * padding), module, HostTime::kHostTimeClock));
        addOutput(createOutput<PJ301MPort>(Vec(startX, startY + 5 * padding), module, HostTime::kHostTimeBarPhase));
        addOutput(createOutput<PJ301MPort>(Vec(startX, startY + 6 * padding), module, HostTime::kHostTimeBeatPhase));

        const float x = startX + 28;
        addChild(createLightCentered<SmallLight<GreenLight>> (Vec(x, startY + 0 * padding + 12), module, HostTime::kHostTimeRolling));
        addChild(createLightCentered<SmallLight<WhiteLight>> (Vec(x, startY + 1 * padding + 12), module, HostTime::kHostTimeReset));
        addChild(createLightCentered<SmallLight<RedLight>>   (Vec(x, startY + 2 * padding + 12), module, HostTime::kHostTimeBar));
        addChild(createLightCentered<SmallLight<YellowLight>>(Vec(x, startY + 3 * padding + 12), module, HostTime::kHostTimeBeat));
        addChild(createLightCentered<SmallLight<YellowLight>>(Vec(x, startY + 4 * padding + 12), module, HostTime::kHostTimeClock));
        addChild(createLightCentered<SmallLight<YellowLight>>(Vec(x, startY + 5 * padding + 12), module, HostTime::kHostTimeBarPhase));
        addChild(createLightCentered<SmallLight<YellowLight>>(Vec(x, startY + 6 * padding + 12), module, HostTime::kHostTimeBeatPhase));
    }

    void drawOutputLine(NVGcontext* const vg, const uint offset, const char* const text)
    {
        const float y = startY + offset * padding;
        nvgBeginPath(vg);
        nvgRoundedRect(vg, startX - 1.0f, y - 2.0f, box.size.x - (startX + 1) * 2, 28.0f, 4);
        nvgFillColor(vg, nvgRGB(0xd0, 0xd0, 0xd0));
        nvgFill(vg);
//         nvgStrokeColor(vg, nvgRGBA(0x4a, 0x4a, 0x4a, 0xc0));
//         nvgStroke(vg);
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
        drawOutputLine(args.vg, 1, "Reset");
        drawOutputLine(args.vg, 2, "Bar");
        drawOutputLine(args.vg, 3, "Beat");
        drawOutputLine(args.vg, 4, "Clock");

        nvgFontSize(args.vg, 11);
        drawOutputLine(args.vg, 5, "Bar Phase");
        drawOutputLine(args.vg, 6, "Beat Phase");

        ModuleWidget::draw(args);
    }
};

Model* modelHostTime = createModel<HostTime, HostTimeWidget>("HostTime");
