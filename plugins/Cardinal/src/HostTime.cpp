/*
 * DISTRHO Cardinal Plugin
 * Copyright (C) 2021-2026 Filipe Coelho <falktx@falktx.com>
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
#include "plugincontext.hpp"
#include "ModuleWidgets.hpp"
#include "engine/TerminalModule.hpp"

// --------------------------------------------------------------------------------------------------------------------

struct HostTime : TerminalModule {
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
        kHostTimeBPM,
        kHostTimeCount
    };

    enum BarDivisions {
        Bars1 = 1,
        Bars4 = 4,
        Bars8 = 8
    };

    const CardinalPluginContext* const pcontext;

    rack::dsp::PulseGenerator pulseReset, pulseBar, pulseBeat, pulseClock;
    float sampleTime = 0.0f;
    uint32_t lastProcessCounter = 0;
    BarDivisions barDivision = Bars1;
    // cached time values
    struct {
        bool reset = true;
        int32_t bar = 0;
        int32_t beat = 0;
        double tick = 0.0;
        double tickClock = 0.0;
        uint32_t seconds = 0;
    } timeInfo;

    HostTime()
        : pcontext(static_cast<CardinalPluginContext*>(APP))
    {
        if (pcontext == nullptr)
            throw rack::Exception("Plugin context is null.");

        config(NUM_PARAMS, NUM_INPUTS, kHostTimeCount, kHostTimeCount);
    }

    void processTerminalInput(const ProcessArgs& args) override
    {
        const uint32_t processCounter = pcontext->processCounter;

        // local variables for faster access
        double tick, tickClock;

        // Update time position if running a new audio block
        if (lastProcessCounter != processCounter)
        {
            lastProcessCounter = processCounter;
            timeInfo.reset = pcontext->reset;
            timeInfo.bar = pcontext->bar;
            timeInfo.beat = pcontext->beat;
            timeInfo.seconds = pcontext->frame / pcontext->sampleRate;
            tick = pcontext->tick;
            tickClock = pcontext->tickClock;
        }
        else
        {
            tick = timeInfo.tick;
            tickClock = timeInfo.tickClock;
        }

        const bool playing = pcontext->playing;
        const bool playingWithBBT = playing && pcontext->bbtValid;

        if (playingWithBBT)
        {
            if (d_isZero(tick))
            {
                pulseBeat.trigger();
                if (timeInfo.beat == 1)
                    pulseBar.trigger();
            }

            if (d_isZero(tickClock))
                pulseClock.trigger();

            if (timeInfo.reset)
            {
                timeInfo.reset = false;
                pulseReset.trigger();
            }

            tick += pcontext->ticksPerFrame;

            // give a little help to keep tick active,
            // as otherwise we might miss it if located at the very end of the audio block
            if (tick + 0.0001 >= pcontext->ticksPerBeat)
            {
                tick -= pcontext->ticksPerBeat;
                pulseBeat.trigger();

                if (++timeInfo.beat > pcontext->beatsPerBar)
                {
                    timeInfo.beat = 1;
                    ++timeInfo.bar;

                    if (timeInfo.bar % barDivision == 1)
                        pulseBar.trigger();
                }
            }

            if ((tickClock += pcontext->ticksPerFrame) >= pcontext->ticksPerClock)
            {
                tickClock -= pcontext->ticksPerClock;
                pulseClock.trigger();
            }
        }

        // store back the local values
        timeInfo.tick = tick;
        timeInfo.tickClock = tickClock;

        if (isBypassed())
            return;

        const bool hasReset = pulseReset.process(args.sampleTime);
        const bool hasBar = pulseBar.process(args.sampleTime);
        const bool hasBeat = pulseBeat.process(args.sampleTime);
        const bool hasClock = pulseClock.process(args.sampleTime);
        const float beatPhase = playingWithBBT && pcontext->ticksPerBeat > 0.0
                              ? tick / pcontext->ticksPerBeat
                              : 0.0f;
        const float barPhase = playingWithBBT && pcontext->beatsPerBar > 0
                              ? ((float)((timeInfo.bar - 1) % barDivision) + (timeInfo.beat - 1) + beatPhase)
                              / (pcontext->beatsPerBar * barDivision)
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
        outputs[kHostTimeBPM].setVoltage(playingWithBBT ? std::log2(pcontext->beatsPerMinute / 120.0) : 0.0);
    }

    void processTerminalOutput(const ProcessArgs&) override
    {}

    json_t* dataToJson() override {
        json_t* rootJ = json_object();
        json_object_set_new(rootJ, "barDivision", json_integer(barDivision));
        return rootJ;
    }

    void dataFromJson(json_t* rootJ) override {
        if (json_t* bdJ = json_object_get(rootJ, "barDivision")) {
            int value = json_integer_value(bdJ);
            if (value == Bars1 || value == Bars4 || value == Bars8)
                barDivision = static_cast<BarDivisions>(value);
        }
    }
};

// --------------------------------------------------------------------------------------------------------------------

#ifndef HEADLESS
struct HostTimeWidget : ModuleWidgetWith8HP {
    static constexpr const float startX = 10.0f;
    static constexpr const float startY_top = 71.0f;
    static constexpr const float startY_cv = 115.0f;
    static constexpr const float padding = 32.0f;

    HostTime* const module;
    std::string monoFontPath;

    HostTimeWidget(HostTime* const m)
        : module(m)
    {
        setModule(m);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/HostTime.svg")));
        monoFontPath = asset::system("res/fonts/ShareTechMono-Regular.ttf");

        createAndAddScrews();

        addOutput(createOutput<PJ301MPort>(Vec(startX, startY_cv + 0 * padding), m, HostTime::kHostTimeRolling));
        addOutput(createOutput<PJ301MPort>(Vec(startX, startY_cv + 1 * padding), m, HostTime::kHostTimeReset));
        addOutput(createOutput<PJ301MPort>(Vec(startX, startY_cv + 2 * padding), m, HostTime::kHostTimeBar));
        addOutput(createOutput<PJ301MPort>(Vec(startX, startY_cv + 3 * padding), m, HostTime::kHostTimeBeat));
        addOutput(createOutput<PJ301MPort>(Vec(startX, startY_cv + 4 * padding), m, HostTime::kHostTimeClock));
        addOutput(createOutput<PJ301MPort>(Vec(startX, startY_cv + 5 * padding), m, HostTime::kHostTimeBarPhase));
        addOutput(createOutput<PJ301MPort>(Vec(startX, startY_cv + 6 * padding), m, HostTime::kHostTimeBeatPhase));
        addOutput(createOutput<PJ301MPort>(Vec(startX, startY_cv + 7 * padding), m, HostTime::kHostTimeBPM));

        const float x = startX + 28;
        addChild(createLightCentered<SmallLight<GreenLight>> (Vec(x, startY_cv + 0 * padding + 12), m, HostTime::kHostTimeRolling));
        addChild(createLightCentered<SmallLight<WhiteLight>> (Vec(x, startY_cv + 1 * padding + 12), m, HostTime::kHostTimeReset));
        addChild(createLightCentered<SmallLight<RedLight>>   (Vec(x, startY_cv + 2 * padding + 12), m, HostTime::kHostTimeBar));
        addChild(createLightCentered<SmallLight<YellowLight>>(Vec(x, startY_cv + 3 * padding + 12), m, HostTime::kHostTimeBeat));
        addChild(createLightCentered<SmallLight<YellowLight>>(Vec(x, startY_cv + 4 * padding + 12), m, HostTime::kHostTimeClock));
        addChild(createLightCentered<SmallLight<YellowLight>>(Vec(x, startY_cv + 5 * padding + 12), m, HostTime::kHostTimeBarPhase));
        addChild(createLightCentered<SmallLight<YellowLight>>(Vec(x, startY_cv + 6 * padding + 12), m, HostTime::kHostTimeBeatPhase));
    }

    void drawOutputLine(NVGcontext* const vg, const uint offset, const char* const text)
    {
        const float y = startY_cv + offset * padding;
        nvgBeginPath(vg);
        nvgRoundedRect(vg, startX - 1.0f, y - 2.f, box.size.x - startX * 2 + 2.f, 28.f, 4);
        nvgFillColor(vg, rack::settings::preferDarkPanels ? nvgRGB(0xd0, 0xd0, 0xd0) : nvgRGB(0x2f, 0x2f, 0x2f));
        nvgFill(vg);
        nvgBeginPath(vg);
        nvgFillColor(vg, rack::settings::preferDarkPanels ? color::BLACK : color::WHITE);
        nvgText(vg, startX + 36, y + 16, text, nullptr);
    }

    void draw(const DrawArgs& args) override
    {
        drawBackground(args.vg);

        nvgFontFaceId(args.vg, 0);
        nvgFontSize(args.vg, 14);

        drawOutputLine(args.vg, 0, "Playing");
        drawOutputLine(args.vg, 1, "Reset");
        drawOutputLine(args.vg, 2, "Bar");
        drawOutputLine(args.vg, 3, "Beat");
        drawOutputLine(args.vg, 4, "Step");

        nvgFontSize(args.vg, 11);
        drawOutputLine(args.vg, 5, "Bar Phase");
        drawOutputLine(args.vg, 6, "Beat Phase");
        drawOutputLine(args.vg, 7, "BPM");

        nvgBeginPath(args.vg);
        nvgRoundedRect(args.vg, startX - 1.0f, startY_top, 98.0f, 38.0f, 4); // 98
        nvgFillColor(args.vg, color::BLACK);
        nvgFill(args.vg);

        ModuleWidget::draw(args);
    }

    void drawLayer(const DrawArgs& args, int layer) override
    {
        if (layer == 1)
        {
            nvgFontSize(args.vg, 17);
            nvgFillColor(args.vg, nvgRGBf(0.76f, 0.11f, 0.22f));

            char timeString1[24];
            char timeString2[24];

            std::shared_ptr<Font> monoFont = APP->window->loadFont(monoFontPath);

            if (module != nullptr && monoFont != nullptr)
            {
                nvgFontFaceId(args.vg, monoFont->handle);

                const uint32_t seconds = module->timeInfo.seconds;
                std::snprintf(timeString1, sizeof(timeString1), "  %02d:%02d:%02d",
                              (seconds / 3600) % 100,
                              (seconds / 60) % 60,
                              seconds % 60);
                std::snprintf(timeString2, sizeof(timeString2), "%03d:%02d:%04d",
                              module->timeInfo.bar % 1000,
                              module->timeInfo.beat % 100,
                              static_cast<int>(module->timeInfo.tick + 0.5));
            }
            else
            {
                std::strcpy(timeString1, "  00:00:00");
                std::strcpy(timeString2, "001:01:0000");
            }

            nvgText(args.vg, startX + 3.5f, startY_top + 15.0f, timeString1, nullptr);
            nvgText(args.vg, startX + 3.5f, startY_top + 33.0f, timeString2, nullptr);
        }

        ModuleWidget::drawLayer(args, layer);
    }

    void appendContextMenu(Menu* menu) override {
        struct BarDivisionItem : MenuItem {
            HostTime* module;
            HostTime::BarDivisions value;
            void onAction(const event::Action& e) override {
                module->barDivision = value;
            }
        };

        menu->addChild(new MenuSeparator);
        menu->addChild(construct<MenuLabel>(&MenuLabel::text, "Bar Division"));
        menu->addChild(construct<BarDivisionItem>(&BarDivisionItem::text, "Bars/1", &BarDivisionItem::module, module, &BarDivisionItem::value, HostTime::Bars1));
        menu->addChild(construct<BarDivisionItem>(&BarDivisionItem::text, "Bars/4", &BarDivisionItem::module, module, &BarDivisionItem::value, HostTime::Bars4));
        menu->addChild(construct<BarDivisionItem>(&BarDivisionItem::text, "Bars/8", &BarDivisionItem::module, module, &BarDivisionItem::value, HostTime::Bars8));
    }
};
#else
struct HostTimeWidget : ModuleWidget {
    HostTimeWidget(HostTime* const module) {
        setModule(module);
        for (uint i=0; i<HostTime::kHostTimeCount; ++i)
            addOutput(createOutput<PJ301MPort>({}, module, i));
    }
};
#endif

// --------------------------------------------------------------------------------------------------------------------

Model* modelHostTime = createModel<HostTime, HostTimeWidget>("HostTime");

// --------------------------------------------------------------------------------------------------------------------
