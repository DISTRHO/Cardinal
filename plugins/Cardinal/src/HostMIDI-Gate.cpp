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

#include <algorithm>

// -----------------------------------------------------------------------------------------------------------

USE_NAMESPACE_DISTRHO;

struct HostMIDIGate : Module {
    enum ParamIds {
        NUM_PARAMS
    };
    enum InputIds {
        ENUMS(GATE_INPUTS, 16),
        NUM_INPUTS
    };
    enum OutputIds {
        ENUMS(GATE_OUTPUTS, 16),
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
        int64_t lastBlockFrame;
        uint8_t channel;

        // stuff from Rack
        /** [cell][channel] */
        bool gates[16][16];
        /** [cell][channel] */
        float gateTimes[16][16];
        /** [cell][channel] */
        uint8_t velocities[16][16];
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
            lastBlockFrame = -1;
            channel = 0;
            learningId = -1;
            mpeMode = false;
            panic();
        }

        void panic()
        {
            for (int i = 0; i < 16; ++i)
            {
                for (int c = 0; c < 16; ++c)
                {
                    gates[i][c] = false;
                    gateTimes[i][c] = 0.f;
                }
            }
        }

        bool process(const ProcessArgs& args, std::vector<rack::engine::Output>& outputs,
                     const bool velocityMode, uint8_t learnedNotes[16])
        {
            // Cardinal specific
            const int64_t blockFrame = pcontext->engine->getBlockFrame();
            const bool blockFrameChanged = lastBlockFrame != blockFrame;

            if (blockFrameChanged)
            {
                lastBlockFrame = blockFrame;

                midiEvents = pcontext->midiEvents;
                midiEventsLeft = pcontext->midiEventCount;
                midiEventFrame = 0;
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
                        // Learn
                        if (learningId >= 0) {
                            learnedNotes[learningId] = data[1];
                            learningId = -1;
                        }
                        // Find id
                        for (int i = 0; i < 16; i++) {
                            if (learnedNotes[i] == data[1]) {
                                gates[i][c] = true;
                                gateTimes[i][c] = 1e-3f;
                                velocities[i][c] = data[2];
                            }
                        }
                        break;
                    }
                    // fall-through
                // note off
                case 0x80:
                    const int c = mpeMode ? (data[0] & 0x0F) : 0;
                    // Find id
                    for (int i = 0; i < 16; i++) {
                        if (learnedNotes[i] == data[1]) {
                            gates[i][c] = false;
                        }
                    }
                    break;
                }
            }

            ++midiEventFrame;

            // Rack stuff
            const int channels = mpeMode ? 16 : 1;

            for (int i = 0; i < 16; i++) {
                outputs[GATE_OUTPUTS + i].setChannels(channels);
                for (int c = 0; c < channels; c++) {
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

            return blockFrameChanged;
        }

    } midiInput;

    struct MidiOutput {
        // cardinal specific
        CardinalPluginContext* const pcontext;
        uint8_t channel = 0;

        // base class vars
        int vels[128];
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
            for (int note = 0; note < 128; ++note)
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
            for (int note = 0; note < 128; note++)
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

        void setVelocity(int vel, int note)
        {
            vels[note] = vel;
        }

        void setGate(bool gate, int note)
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
    uint8_t learnedNotes[16] = {};

    HostMIDIGate()
        : pcontext(static_cast<CardinalPluginContext*>(APP)),
          midiInput(pcontext),
          midiOutput(pcontext)
    {
        if (pcontext == nullptr)
            throw rack::Exception("Plugin context is null");

        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        for (int i = 0; i < 16; i++)
            configInput(GATE_INPUTS + i, string::f("Cell %d", i + 1));

        for (int i = 0; i < 16; i++)
            configOutput(GATE_OUTPUTS + i, string::f("Gate %d", i + 1));

        onReset();
    }

    void onReset() override
    {
        for (int y = 0; y < 4; ++y)
            for (int x = 0; x < 4; ++x)
                learnedNotes[4 * y + x] = 36 + 4 * (3 - y) + x;

        velocityMode = false;

        midiInput.reset();
        midiOutput.reset();
    }

    void process(const ProcessArgs& args) override
    {
        if (midiInput.process(args, outputs, velocityMode, learnedNotes))
            midiOutput.frame = 0;
        else
            ++midiOutput.frame;

        for (int i = 0; i < 16; i++)
        {
            const int note = learnedNotes[i];

            if (velocityMode)
            {
                int vel = (int) std::round(inputs[GATE_INPUTS + i].getVoltage() / 10.f * 127);
                vel = clamp(vel, 0, 127);
                midiOutput.setVelocity(vel, note);
                midiOutput.setGate(vel > 0, note);
            }
            else
            {
                const bool gate = inputs[GATE_INPUTS + i].getVoltage() >= 1.f;
                midiOutput.setVelocity(100, note);
                midiOutput.setGate(gate, note);
            }
        }
    }

    json_t* dataToJson() override
    {
        json_t* const rootJ = json_object();
        DISTRHO_SAFE_ASSERT_RETURN(rootJ != nullptr, nullptr);

        // input and output
        if (json_t* const notesJ = json_array())
        {
            for (int i = 0; i < 16; i++)
                json_array_append_new(notesJ, json_integer(learnedNotes[i]));
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
            for (int i = 0; i < 16; i++)
            {
                if (json_t* const noteJ = json_array_get(notesJ, i))
                    learnedNotes[i] = json_integer_value(noteJ);
                else
                    learnedNotes[i] = -1;
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

struct HostMIDIGateWidget : ModuleWidget {
    static constexpr const float startX_In = 14.0f;
    static constexpr const float startX_Out = 96.0f;
    static constexpr const float startY = 74.0f;
    static constexpr const float padding = 29.0f;
    static constexpr const float middleX = startX_In + (startX_Out - startX_In) * 0.5f + padding * 0.35f;

    HostMIDIGate* const module;

    HostMIDIGateWidget(HostMIDIGate* const m)
        : module(m)
    {
        setModule(m);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/HostMIDI.svg")));

        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
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

// --------------------------------------------------------------------------------------------------------------------

Model* modelHostMIDIGate = createModel<HostMIDIGate, HostMIDIGateWidget>("HostMIDIGate");

// --------------------------------------------------------------------------------------------------------------------
