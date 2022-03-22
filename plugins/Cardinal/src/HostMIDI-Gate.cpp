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

/**
 * This file contains a substantial amount of code from VCVRack's core/Gate_MIDI.cpp and core/MIDI_Gate.cpp
 * Copyright (C) 2016-2021 VCV.
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 */

#include "plugincontext.hpp"
#include "Widgets.hpp"

#include <algorithm>

// -----------------------------------------------------------------------------------------------------------

USE_NAMESPACE_DISTRHO;

struct HostMIDIGate : TerminalModule {
    enum ParamIds {
        NUM_PARAMS
    };
    enum InputIds {
        ENUMS(GATE_INPUTS, 18),
        NUM_INPUTS
    };
    enum OutputIds {
        ENUMS(GATE_OUTPUTS, 18),
        NUM_OUTPUTS
    };
    enum LightIds {
        NUM_LIGHTS
    };

    CardinalPluginContext* const pcontext;

    struct MidiInput {
        // Cardinal specific
        CardinalPluginContext* const pcontext;
        const MidiEvent* midiEvents;
        uint32_t midiEventsLeft;
        uint32_t midiEventFrame;
        uint32_t lastProcessCounter;
        uint8_t channel;

        // stuff from Rack
        /** [cell][channel] */
        bool gates[18][16];
        /** [cell][channel] */
        float gateTimes[18][16];
        /** [cell][channel] */
        uint8_t velocities[18][16];
        /** Cell ID in learn mode, or -1 if none. */
        int learningId;

        bool mpeMode;

        MidiInput(CardinalPluginContext* const pc)
            : pcontext(pc)
        {
            reset();
        }

        void reset()
        {
            midiEvents = nullptr;
            midiEventsLeft = 0;
            midiEventFrame = 0;
            lastProcessCounter = 0;
            channel = 0;
            learningId = -1;
            mpeMode = false;
            panic();
        }

        void panic()
        {
            for (int i = 0; i < 18; ++i)
            {
                for (int c = 0; c < 16; ++c)
                {
                    gates[i][c] = false;
                    gateTimes[i][c] = 0.f;
                }
            }
        }

        bool process(const ProcessArgs& args, std::vector<rack::engine::Output>& outputs,
                     const bool velocityMode, int8_t learnedNotes[18], const bool isBypassed)
        {
            // Cardinal specific
            const uint32_t processCounter = pcontext->processCounter;
            const bool processCounterChanged = lastProcessCounter != processCounter;

            if (processCounterChanged)
            {
                lastProcessCounter = processCounter;
                midiEvents = pcontext->midiEvents;
                midiEventsLeft = pcontext->midiEventCount;
                midiEventFrame = 0;
            }

            if (isBypassed)
            {
                ++midiEventFrame;
                return processCounterChanged;
            }

            while (midiEventsLeft != 0)
            {
                const MidiEvent& midiEvent(*midiEvents);

                if (midiEvent.frame > midiEventFrame)
                    break;

                ++midiEvents;
                --midiEventsLeft;

                const uint8_t* const data = midiEvent.size > MidiEvent::kDataSize
                                          ? midiEvent.dataExt
                                          : midiEvent.data;

                if (channel != 0 && data[0] < 0xF0)
                {
                    if ((data[0] & 0x0F) != (channel - 1))
                        continue;
                }

                // adapted from Rack
                switch (data[0] & 0xF0)
                {
                // note on
                case 0x90:
                    if (data[2] > 0)
                    {
                        const int c = mpeMode ? (data[0] & 0x0F) : 0;
                        const int8_t note = data[1];
                        // Learn
                        if (learningId >= 0)
                        {
                            // NOTE: does the same as `setLearnedNote`
                            if (note >= 0)
                            {
                                for (int id = 0; id < 18; ++id)
                                {
                                    if (learnedNotes[id] == note)
                                        learnedNotes[id] = -1;
                                }
                            }
                            learnedNotes[learningId] = note;
                            learningId = -1;
                        }
                        // Find id
                        for (int id = 0; id < 18; ++id)
                        {
                            if (learnedNotes[id] == note)
                            {
                                gates[id][c] = true;
                                gateTimes[id][c] = 1e-3f;
                                velocities[id][c] = data[2];
                            }
                        }
                        break;
                    }
                    // fall-through
                // note off
                case 0x80:
                    const int c = mpeMode ? (data[0] & 0x0F) : 0;
                    const int8_t note = data[1];
                    // Find id
                    for (int id = 0; id < 18; ++id)
                    {
                        if (learnedNotes[id] == note)
                            gates[id][c] = false;
                    }
                    break;
                }
            }

            ++midiEventFrame;

            // Rack stuff
            const int channels = mpeMode ? 16 : 1;

            for (int i = 0; i < 18; i++)
            {
                outputs[GATE_OUTPUTS + i].setChannels(channels);
                for (int c = 0; c < channels; c++)
                {
                    // Make sure all pulses last longer than 1ms
                    if (gates[i][c] || gateTimes[i][c] > 0.f)
                    {
                        float velocity = velocityMode ? (velocities[i][c] / 127.f) : 1.f;
                        outputs[GATE_OUTPUTS + i].setVoltage(velocity * 10.f, c);
                        gateTimes[i][c] -= args.sampleTime;
                    }
                    else
                    {
                        outputs[GATE_OUTPUTS + i].setVoltage(0.f, c);
                    }
                }
            }

            return processCounterChanged;
        }

    } midiInput;

    struct MidiOutput {
        // cardinal specific
        CardinalPluginContext* const pcontext;
        uint8_t channel = 0;

        // base class vars
        uint8_t vels[128];
        bool lastGates[128];
        int64_t frame = 0;

        MidiOutput(CardinalPluginContext* const pc)
            : pcontext(pc)
        {
            reset();
        }

        void reset()
        {
            // base class vars
            for (uint8_t note = 0; note < 128; ++note)
            {
                vels[note] = 100;
                lastGates[note] = false;
            }

            // cardinal specific
            channel = 0;
        }

        void panic()
        {
            // TODO send all notes off CC

            // Send all note off commands
            for (uint8_t note = 0; note < 128; note++)
            {
                // Note off
                midi::Message m;
                m.setStatus(0x8);
                m.setNote(note);
                m.setValue(0);
                m.setFrame(frame);
                sendMessage(m);
                lastGates[note] = false;
            }
        }

        void setVelocity(uint8_t note, uint8_t vel)
        {
            vels[note] = vel;
        }

        void setGate(uint8_t note, bool gate)
        {
            if (gate && !lastGates[note])
            {
                // Note on
                midi::Message m;
                m.setStatus(0x9);
                m.setNote(note);
                m.setValue(vels[note]);
                m.setFrame(frame);
                sendMessage(m);
            }
            else if (!gate && lastGates[note])
            {
                // Note off
                midi::Message m;
                m.setStatus(0x8);
                m.setNote(note);
                m.setValue(vels[note]);
                m.setFrame(frame);
                sendMessage(m);
            }
            lastGates[note] = gate;
        }

        void sendMessage(const midi::Message& message)
        {
            pcontext->writeMidiMessage(message, channel);
        }

    } midiOutput;

    bool velocityMode = false;
    int8_t learnedNotes[18] = {};
    dsp::SchmittTrigger cellTriggers[18];

    HostMIDIGate()
        : pcontext(static_cast<CardinalPluginContext*>(APP)),
          midiInput(pcontext),
          midiOutput(pcontext)
    {
        if (pcontext == nullptr)
            throw rack::Exception("Plugin context is null");

        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        for (int id = 0; id < 18; ++id)
            configInput(GATE_INPUTS + id, string::f("Gate %d", id + 1));

        for (int id = 0; id < 18; ++id)
            configOutput(GATE_OUTPUTS + id, string::f("Gate %d", id + 1));

        onReset();
    }

    void onReset() override
    {
        for (int id = 0; id < 18; ++id)
            learnedNotes[id] = 36 + id;

        velocityMode = false;

        midiInput.reset();
        midiOutput.reset();
    }

    void processTerminalInput(const ProcessArgs& args) override
    {
        if (midiInput.process(args, outputs, velocityMode, learnedNotes, isBypassed()))
            midiOutput.frame = 0;
        else
            ++midiOutput.frame;
    }

    void processTerminalOutput(const ProcessArgs&) override
    {
        if (isBypassed())
            return;

        for (int id = 0; id < 18; ++id)
        {
            const int8_t note = learnedNotes[id];

            if (note < 0)
                continue;

            if (velocityMode)
            {
                uint8_t vel = (uint8_t) clamp(std::round(inputs[GATE_INPUTS + id].getVoltage() / 10.f * 127), 0.f, 127.f);
                midiOutput.setVelocity(note, vel);
                midiOutput.setGate(note, vel > 0);
            }
            else
            {
                const bool gate = inputs[GATE_INPUTS + id].getVoltage() >= 1.f;
                midiOutput.setVelocity(note, 100);
                midiOutput.setGate(note, gate);
            }
        }
    }

    void setLearnedNote(const int id, const int8_t note) {
        // Unset IDs of similar note
        if (note >= 0)
        {
            for (int idx = 0; idx < 18; ++idx)
            {
                if (learnedNotes[idx] == note)
                    learnedNotes[idx] = -1;
            }
        }
        learnedNotes[id] = note;
    }

    json_t* dataToJson() override
    {
        json_t* const rootJ = json_object();
        DISTRHO_SAFE_ASSERT_RETURN(rootJ != nullptr, nullptr);

        // input and output
        if (json_t* const notesJ = json_array())
        {
            for (int id = 0; id < 18; ++id)
                json_array_append_new(notesJ, json_integer(learnedNotes[id]));
            json_object_set_new(rootJ, "notes", notesJ);
        }
        json_object_set_new(rootJ, "velocity", json_boolean(velocityMode));

        // input only
        json_object_set_new(rootJ, "mpeMode", json_boolean(midiInput.mpeMode));

        // separate
        json_object_set_new(rootJ, "inputChannel", json_integer(midiInput.channel));
        json_object_set_new(rootJ, "outputChannel", json_integer(midiOutput.channel));

        return rootJ;
    }

    void dataFromJson(json_t* const rootJ) override
    {
        // input and output
        if (json_t* const notesJ = json_object_get(rootJ, "notes"))
        {
            for (int id = 0; id < 18; ++id)
            {
                if (json_t* const noteJ = json_array_get(notesJ, id))
                    setLearnedNote(id, json_integer_value(noteJ));
                else
                    learnedNotes[id] = -1;
            }
        }

        if (json_t* const velocityJ = json_object_get(rootJ, "velocity"))
            velocityMode = json_boolean_value(velocityJ);

        // input only
        if (json_t* const mpeModeJ = json_object_get(rootJ, "mpeMode"))
            midiInput.mpeMode = json_boolean_value(mpeModeJ);

        // separate
        if (json_t* const inputChannelJ = json_object_get(rootJ, "inputChannel"))
            midiInput.channel = json_integer_value(inputChannelJ);

        if (json_t* const outputChannelJ = json_object_get(rootJ, "outputChannel"))
            midiOutput.channel = json_integer_value(outputChannelJ) & 0x0F;
    }
};

// --------------------------------------------------------------------------------------------------------------------

#ifndef HEADLESS
/**
 * Based on VCVRack's NoteChoice as defined in src/core/plugin.hpp
 * Copyright (C) 2016-2021 VCV.
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 */
struct CardinalNoteChoice : CardinalLedDisplayChoice {
    HostMIDIGate* const module;
    const int id;
    int8_t focusNote = -1;

    CardinalNoteChoice(HostMIDIGate* const m, const int i)
      : CardinalLedDisplayChoice(),
        module(m),
        id(i) {}

    void step() override
    {
        int8_t note;

        if (module == nullptr)
        {
            note = id + 36;
        }
        else if (module->midiInput.learningId == id)
        {
            note = focusNote;
            color.a = 0.5;
        }
        else
        {
            note = module->learnedNotes[id];
            color.a = 1.0f;

            // Cancel focus if no longer learning
            if (APP->event->getSelectedWidget() == this)
                APP->event->setSelectedWidget(NULL);
        }

        // Set text
        if (note < 0)
        {
            text = "--";
        }
        else
        {
            static const char* noteNames[12] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
            const int oct = note / 12 - 1;
            const int semi = note % 12;
            text = string::f("%s%d", noteNames[semi], oct);
        }
    }

    void onSelect(const SelectEvent& e) override
    {
        DISTRHO_SAFE_ASSERT_RETURN(module != nullptr,);

        module->midiInput.learningId = id;
        focusNote = -1;
        e.consume(this);
    }

    void onDeselect(const DeselectEvent&) override
    {
        DISTRHO_SAFE_ASSERT_RETURN(module != nullptr,);

        if (module->midiInput.learningId == id)
        {
            if (focusNote >= 0)
                module->setLearnedNote(id, focusNote);
            module->midiInput.learningId = -1;
        }
    }

    void onSelectText(const SelectTextEvent& e) override
    {
        const int c = e.codepoint;

        if ('a' <= c && c <= 'g')
        {
            static const int majorNotes[7] = {9, 11, 0, 2, 4, 5, 7};
            focusNote = majorNotes[c - 'a'];
        }
        else if (c == '#')
        {
            if (focusNote >= 0)
                focusNote += 1;
        }
        else if ('0' <= c && c <= '9')
        {
            if (focusNote >= 0)
            {
                focusNote = focusNote % 12;
                focusNote += 12 * (c - '0' + 1);
            }
        }

        if (focusNote < 0)
            focusNote = -1;

        e.consume(this);
    }

    void onSelectKey(const SelectKeyEvent& e) override
    {
        if (e.key != GLFW_KEY_ENTER && e.key != GLFW_KEY_KP_ENTER)
            return;
        if (e.action != GLFW_PRESS)
            return;
        if (e.mods & RACK_MOD_MASK)
            return;

        DeselectEvent eDeselect;
        onDeselect(eDeselect);
        APP->event->selectedWidget = NULL;
        e.consume(this);
    }
};

struct NoteGridDisplay : Widget {
    void draw(const DrawArgs& args) override
    {
        nvgBeginPath(args.vg);
        nvgRoundedRect(args.vg, 0, 0, box.size.x, box.size.y, 4);
        nvgFillColor(args.vg, nvgRGB(0, 0, 0));
        nvgFill(args.vg);

        Widget::draw(args);
    }

    void setModule(HostMIDIGate* const module)
    {
        LedDisplaySeparator* hSeparators[6];
        LedDisplaySeparator* vSeparators[3];
        LedDisplayChoice* choices[3][6];

        // Add vSeparators
        for (int x = 0; x < 3; ++x)
        {
            vSeparators[x] = new LedDisplaySeparator;
            vSeparators[x]->box.pos = Vec(box.size.x / 3 * (x+1), 0.0f);
            vSeparators[x]->box.size = Vec(1.0f, box.size.y);
            addChild(vSeparators[x]);
        }

        // Add hSeparators and choice widgets
        for (int y = 0; y < 6; ++y)
        {
            hSeparators[y] = new LedDisplaySeparator;
            hSeparators[y]->box.pos = Vec(0.0f, box.size.y / 6 * (y+1));
            hSeparators[y]->box.size = Vec(box.size.x, 1.0f);
            addChild(hSeparators[y]);

            for (int x = 0; x < 3; ++x)
            {
                const int id = 6 * x + y;

                choices[x][y] = new CardinalNoteChoice(module, id);
                choices[x][y]->box.pos = Vec(box.size.x / 3 * x, box.size.y / 6 * y);
                choices[x][y]->box.size = Vec(box.size.x / 3, box.size.y / 6);
                addChild(choices[x][y]);
            }
        }
	}
};

struct HostMIDIGateWidget : ModuleWidget {
    static constexpr const float startX_In = 14.0f;
    static constexpr const float startX_Out = 115.0f;
    static constexpr const float startY = 190.0f;
    static constexpr const float padding = 29.0f;

    HostMIDIGate* const module;

    HostMIDIGateWidget(HostMIDIGate* const m)
        : module(m)
    {
        setModule(m);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/HostMIDIGate.svg")));

        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        for (int i=0; i<18; ++i)
        {
            const float x = startX_In + int(i / 6) * padding;
            const float y = startY + int(i % 6) * padding;
            addInput(createInput<PJ301MPort>(Vec(x, y), module, i));
        }

        for (int i=0; i<18; ++i)
        {
            const float x = startX_Out + int(i / 6) * padding;
            const float y = startY + int(i % 6) * padding;
            addOutput(createOutput<PJ301MPort>(Vec(x, y), module, i));
        }

        NoteGridDisplay* const display = createWidget<NoteGridDisplay>(Vec(startX_In - 3.0f, 70.0f));
        display->box.size = Vec(box.size.x - startX_In * 2.0f + 6.0f, startY - 74.0f - 9.0f);
        display->setModule(m);
        addChild(display);
    }

    void draw(const DrawArgs& args) override
    {
        nvgBeginPath(args.vg);
        nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
        nvgFillPaint(args.vg, nvgLinearGradient(args.vg, 0, 0, 0, box.size.y,
                                                nvgRGB(0x18, 0x19, 0x19), nvgRGB(0x21, 0x22, 0x22)));
        nvgFill(args.vg);

        nvgBeginPath(args.vg);
        nvgRoundedRect(args.vg, startX_Out - 2.5f, startY - 2.0f, padding * 3, padding * 6, 4);
        nvgFillColor(args.vg, nvgRGB(0xd0, 0xd0, 0xd0));
        nvgFill(args.vg);

        ModuleWidget::draw(args);
    }

    void appendContextMenu(Menu* const menu) override
    {
        menu->addChild(new MenuSeparator);
        menu->addChild(createMenuLabel("MIDI Input"));

        menu->addChild(createBoolPtrMenuItem("MPE mode", "", &module->midiInput.mpeMode));

        struct InputChannelItem : MenuItem {
            HostMIDIGate* module;
            Menu* createChildMenu() override {
                Menu* menu = new Menu;
                for (int c = 0; c <= 16; c++) {
                    menu->addChild(createCheckMenuItem((c == 0) ? "All" : string::f("%d", c), "",
                        [=]() {return module->midiInput.channel == c;},
                        [=]() {module->midiInput.channel = c;}
                    ));
                }
                return menu;
            }
        };
        InputChannelItem* const inputChannelItem = new InputChannelItem;
        inputChannelItem->text = "MIDI channel";
        inputChannelItem->rightText = (module->midiInput.channel ? string::f("%d", module->midiInput.channel) : "All")
                                    + "  " + RIGHT_ARROW;
        inputChannelItem->module = module;
        menu->addChild(inputChannelItem);

        menu->addChild(new MenuSeparator);
        menu->addChild(createMenuLabel("MIDI Output"));

        struct OutputChannelItem : MenuItem {
            HostMIDIGate* module;
            Menu* createChildMenu() override {
                Menu* menu = new Menu;
                for (uint8_t c = 0; c < 16; c++) {
                    menu->addChild(createCheckMenuItem(string::f("%d", c+1), "",
                        [=]() {return module->midiOutput.channel == c;},
                        [=]() {module->midiOutput.channel = c;}
                    ));
                }
                return menu;
            }
        };
        OutputChannelItem* const outputChannelItem = new OutputChannelItem;
        outputChannelItem->text = "MIDI channel";
        outputChannelItem->rightText = string::f("%d", module->midiOutput.channel+1) + "  " + RIGHT_ARROW;
        outputChannelItem->module = module;
        menu->addChild(outputChannelItem);

        menu->addChild(new MenuSeparator);
        menu->addChild(createMenuLabel("MIDI Input & Output"));

        menu->addChild(createBoolPtrMenuItem("Velocity mode", "", &module->velocityMode));

        menu->addChild(createMenuItem("Panic", "",
            [=]() { module->midiInput.panic(); module->midiOutput.panic(); }
        ));
    }
};
#else
struct HostMIDIGateWidget : ModuleWidget {
    HostMIDIGateWidget(HostMIDIGate* const module) {
        setModule(module);

        for (int i=0; i<18; ++i)
        {
            addInput(createInput<PJ301MPort>({}, module, i));
            addOutput(createOutput<PJ301MPort>({}, module, i));
        }
    }
};
#endif

// --------------------------------------------------------------------------------------------------------------------

Model* modelHostMIDIGate = createModel<HostMIDIGate, HostMIDIGateWidget>("HostMIDIGate");

// --------------------------------------------------------------------------------------------------------------------
