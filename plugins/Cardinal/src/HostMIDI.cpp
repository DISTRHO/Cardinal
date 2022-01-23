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

#include "plugincontext.hpp"

// -----------------------------------------------------------------------------------------------------------

USE_NAMESPACE_DISTRHO;

struct HostMIDI : Module {
    CardinalPluginContext* const pcontext;

    HostMIDI()
        : pcontext(static_cast<CardinalPluginContext*>(APP))
    {
        if (pcontext == nullptr)
            throw rack::Exception("Plugin context is null");

        config(0, 9, 9, 0);
    }

    void process(const ProcessArgs&) override
    {}
};

// --------------------------------------------------------------------------------------------------------------------

struct HostMIDIWidget : ModuleWidget {
    static constexpr const float startX_In = 14.0f;
    static constexpr const float startX_Out = 96.0f;
    static constexpr const float startY = 74.0f;
    static constexpr const float padding = 29.0f;
    static constexpr const float middleX = startX_In + (startX_Out - startX_In) * 0.5f + padding * 0.25f;

    HostMIDI* const module;

    HostMIDIWidget(HostMIDI* const m)
        : module(m)
    {
        setModule(m);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/HostMIDI.svg")));

        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        for (uint i=0; i<9; ++i)
            addInput(createInput<PJ301MPort>(Vec(startX_In, startY + padding * i), m, i));

        for (uint i=0; i<9; ++i)
            addOutput(createOutput<PJ301MPort>(Vec(startX_Out, startY + padding * i), m, i));
    }

    void drawTextLine(NVGcontext* const vg, const float offsetX, const uint posY, const char* const text)
    {
        const float y = startY + posY * padding;
        nvgBeginPath(vg);
        nvgFillColor(vg, color::WHITE);
        nvgText(vg, middleX + offsetX, y + 16, text, nullptr);
    }

    void draw(const DrawArgs& args) override
    {
        nvgBeginPath(args.vg);
        nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
        nvgFillPaint(args.vg, nvgLinearGradient(args.vg, 0, 0, 0, box.size.y,
                                                nvgRGB(0x18, 0x19, 0x19), nvgRGB(0x21, 0x22, 0x22)));
        nvgFill(args.vg);

        nvgFontFaceId(args.vg, 0);
        nvgFontSize(args.vg, 11);
        nvgTextAlign(args.vg, NVG_ALIGN_CENTER);

        nvgBeginPath(args.vg);
        nvgRoundedRect(args.vg, startX_Out - 4.0f, startY - 2.0f, padding, padding * 9, 4);
        nvgFillColor(args.vg, nvgRGB(0xd0, 0xd0, 0xd0));
        nvgFill(args.vg);

        drawTextLine(args.vg, 0.0f, 0, "V/Oct");
        drawTextLine(args.vg, 0.0f, 1, "Gate");
        drawTextLine(args.vg, 0.0f, 2, "Vel");
        drawTextLine(args.vg, 0.0f, 3, "Aft");
        drawTextLine(args.vg, 0.0f, 4, "PW");
        drawTextLine(args.vg, 0.0f, 5, "MW");
        drawTextLine(args.vg, 0.0f, 6, "Start");
        drawTextLine(args.vg, 0.0f, 7, "Stop");
        drawTextLine(args.vg, 0.0f, 8, "Cont");

        ModuleWidget::draw(args);
    }
};

// --------------------------------------------------------------------------------------------------------------------

Model* modelHostMIDI = createModel<HostMIDI, HostMIDIWidget>("HostMIDI");

// --------------------------------------------------------------------------------------------------------------------
