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

/**
 * This file is an edited version of VCVRack's MIDI_CV.cpp
 * Copyright (C) 2016-2021 VCV.
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 */

#include "../Rack/src/core/MIDI_CV.cpp"

#include <nanosvg.h>

namespace rack {
namespace core {

static inline void nsvg__deletePaths(NSVGpath* path)
{
}

struct MIDI_CVWidget_Cardinal : ModuleWidget
{
    MIDI_CV* const module;

    MIDI_CVWidget_Cardinal(MIDI_CV* const m)
        : module(m)
    {
        setModule(module);

        std::shared_ptr<Svg> svg(Svg::load(asset::system("res/Core/MIDI_CV.svg")));

        // this section removes the CLK and CLK/N part of the svg
        for (NSVGshape *shape = svg->handle->shapes, *old = nullptr; shape != nullptr;)
        {
            if (std::strcmp(shape->id, "rect22") != 0 &&
                std::strcmp(shape->id, "rect20") != 0 &&
                std::strcmp(shape->id, "path97") != 0 &&
                std::strcmp(shape->id, "path99") != 0 &&
                std::strcmp(shape->id, "path101") != 0 &&
                std::strcmp(shape->id, "path85") != 0 &&
                std::strcmp(shape->id, "path87") != 0 &&
                std::strcmp(shape->id, "path89") != 0 &&
                std::strcmp(shape->id, "path91") != 0 &&
                std::strcmp(shape->id, "path93") != 0)
            {
                old = shape;
                shape = shape->next;
                continue;
            }

            NSVGshape* const next = shape->next;

            for (NSVGpath* path = shape->paths; path != nullptr;)
            {
                NSVGpath* next = path->next;
                std::free(path->pts);
                std::free(path);
                path = next;
            }

            std::free(shape);

            shape = next;

            if (old != nullptr)
                old->next = next;
            else
                svg->handle->shapes = next;
        }

        setPanel(svg);

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.905, 64.347)), module, MIDI_CV::PITCH_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(20.248, 64.347)), module, MIDI_CV::GATE_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(32.591, 64.347)), module, MIDI_CV::VELOCITY_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.905, 80.603)), module, MIDI_CV::AFTERTOUCH_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(20.248, 80.603)), module, MIDI_CV::PW_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(32.591, 80.603)), module, MIDI_CV::MOD_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(32.591, 96.859)), module, MIDI_CV::RETRIGGER_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.905, 113.115)), module, MIDI_CV::START_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(20.248, 113.115)), module, MIDI_CV::STOP_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(32.591, 112.975)), module, MIDI_CV::CONTINUE_OUTPUT));

        MidiDisplay* display = createWidget<MidiDisplay>(mm2px(Vec(0.0, 13.048)));
        display->box.size = mm2px(Vec(40.64, 29.012));
        display->setMidiPort(m != nullptr ? &m->midiInput : nullptr);
        addChild(display);
    }

    void appendContextMenu(Menu* const menu) override
    {
        menu->addChild(new MenuSeparator);

        menu->addChild(createBoolPtrMenuItem("Smooth pitch/mod wheel", "", &module->smooth));

        struct ChannelItem : MenuItem {
            MIDI_CV* module;
            Menu* createChildMenu() override {
                Menu* menu = new Menu;
                for (int c = 1; c <= 16; c++) {
                    menu->addChild(createCheckMenuItem((c == 1) ? "Monophonic" : string::f("%d", c), "",
                        [=]() {return module->channels == c;},
                        [=]() {module->setChannels(c);}
                    ));
                }
                return menu;
            }
        };
        ChannelItem* channelItem = new ChannelItem;
        channelItem->text = "Polyphony channels";
        channelItem->rightText = string::f("%d", module->channels) + "  " + RIGHT_ARROW;
        channelItem->module = module;
        menu->addChild(channelItem);

        menu->addChild(createIndexPtrSubmenuItem("Polyphony mode", {
            "Rotate",
            "Reuse",
            "Reset",
            "MPE",
        }, &module->polyMode));

        menu->addChild(createMenuItem("Panic", "",
            [=]() {module->panic();}
        ));
    }
};

Model* modelMIDI_CV_Cardinal = createModel<MIDI_CV, MIDI_CVWidget_Cardinal>("MIDIToCVInterface");

}
}
