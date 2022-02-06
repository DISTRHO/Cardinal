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

#include "Expander.hpp"
#include "ModuleWidgets.hpp"

// --------------------------------------------------------------------------------------------------------------------

/**
 * This class contains a substantial amount of code from VCVRack's dsp/midi.hpp
 * Copyright (C) 2016-2021 VCV.
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 */

struct CardinalExpanderForInputMIDI : CardinalExpanderFromCVToCarlaMIDI {
    enum ParamIds {
        NUM_PARAMS
    };
    enum InputIds {
        PITCH_INPUT,
        GATE_INPUT,
        VELOCITY_INPUT,
        AFTERTOUCH_INPUT,
        PITCHBEND_INPUT,
        MODWHEEL_INPUT,
        NUM_INPUTS
    };
    enum OutputIds {
        NUM_OUTPUTS
    };
    enum LightIds {
        NUM_LIGHTS
    };

    static const constexpr uint CHANNELS = 16;
    int8_t vels[CHANNELS];
    int8_t notes[CHANNELS];
    bool gates[CHANNELS];
    int8_t keyPressures[CHANNELS];
    int8_t modwheel;
    int16_t pitchbend;

    uint8_t channel = 0;
    Module* lastConnectedModule = nullptr;

    CardinalExpanderForInputMIDI()
    {
        static_assert(NUM_INPUTS == kNumInputs, "Invalid input configuration");
        static_assert(NUM_OUTPUTS == kNumOutputs, "Invalid output configuration");
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configInput(PITCH_INPUT, "1V/octave pitch");
        configInput(GATE_INPUT, "Gate");
        configInput(VELOCITY_INPUT, "Velocity");
        configInput(AFTERTOUCH_INPUT, "Aftertouch");
        configInput(PITCHBEND_INPUT, "Pitchbend");
        configInput(MODWHEEL_INPUT, "Mod wheel");
        onReset();
    }

    /** Must be called before setNoteGate(). */
    void setVelocity(int8_t vel, int c)
    {
        vels[c] = vel;
    }

    void setNoteGate(int8_t note, bool gate, int c)
    {
        if (midiEventCount == MAX_MIDI_EVENTS || frame == UINT_MAX)
            return;

        const bool changedNote = gate && gates[c] && (note != notes[c]);
        const bool enabledGate = gate && !gates[c];
        const bool disabledGate = !gate && gates[c];

        if (changedNote || disabledGate)
        {
            // Note off
            NativeMidiEvent& m(midiEvents[midiEventCount++]);
            m.time = frame;
            m.port = 0;
            m.size = 3;
            m.data[0] = 0x80 | channel;
            m.data[1] = notes[c];
            m.data[2] = vels[c];
        }

        if (changedNote || enabledGate)
        {
            // Note on
            NativeMidiEvent& m(midiEvents[midiEventCount++]);
            m.time = frame;
            m.port = 0;
            m.size = 3;
            m.data[0] = 0x90 | channel;
            m.data[1] = note;
            m.data[2] = vels[c];
        }

        notes[c] = note;
        gates[c] = gate;
    }

    void setKeyPressure(int8_t val, int c)
    {
        if (keyPressures[c] == val)
            return;

        keyPressures[c] = val;

        if (midiEventCount == MAX_MIDI_EVENTS || frame == UINT_MAX)
            return;

        // Polyphonic key pressure
        NativeMidiEvent& m(midiEvents[midiEventCount++]);
        m.time = frame;
        m.port = 0;
        m.size = 3;
        m.data[0] = 0xa0 | channel;
        m.data[1] = notes[c];
        m.data[2] = val;
    }

    void setModWheel(int8_t modwheel)
    {
        if (this->modwheel == modwheel)
            return;

        this->modwheel = modwheel;

        if (midiEventCount == MAX_MIDI_EVENTS || frame == UINT_MAX)
            return;

        // Modulation Wheel (CC1)
        NativeMidiEvent& m(midiEvents[midiEventCount++]);
        m.time = frame;
        m.port = 0;
        m.size = 3;
        m.data[0] = 0xb0 | channel;
        m.data[1] = 1;
        m.data[2] = modwheel;
    }

    void setPitchbend(int16_t pitchbend)
    {
        if (this->pitchbend == pitchbend)
            return;

        this->pitchbend = pitchbend;

        if (midiEventCount == MAX_MIDI_EVENTS || frame == UINT_MAX)
            return;

        // Pitch Wheel
        NativeMidiEvent& m(midiEvents[midiEventCount++]);
        m.time = frame;
        m.port = 0;
        m.size = 3;
        m.data[0] = 0xe0 | channel;
        m.data[1] = pitchbend & 0x7f;
        m.data[2] = (pitchbend >> 7) & 0x7f;
    }

    void panic()
    {
        if (frame != UINT_MAX)
        {
            // Send all note off commands
            for (int note = 0; note <= 127; ++note)
            {
                if (midiEventCount == MAX_MIDI_EVENTS)
                    break;
                // Note off
                NativeMidiEvent& m(midiEvents[midiEventCount++]);
                m.time = frame;
                m.port = 0;
                m.size = 3;
                m.data[0] = 0x80 | channel;
                m.data[1] = note;
                m.data[2] = 0;
            }
        }

        reset();
    }

    void reset()
    {
        for (uint c = 0; c < CHANNELS; ++c)
        {
            vels[c] = 100;
            notes[c] = 60;
            gates[c] = false;
            keyPressures[c] = -1;
        }
        modwheel = -1;
        pitchbend = 0x2000;
        midiEventCount = 0;
        frame = UINT_MAX;
    }

    void onReset() override
    {
        reset();
        channel = 0;
        lastConnectedModule = nullptr;
    }

    void process(const ProcessArgs& args) override
    {
        // only do stuff if there is something close to us
        if (rightExpander.module == nullptr)
        {
            // something was connected before, but not anymore, reset
            if (frame != UINT_MAX)
                onReset();
            return;
        }
        else if (lastConnectedModule != nullptr && lastConnectedModule != rightExpander.module)
        {
            // whatever we were connected to has changed, reset
            lastConnectedModule = rightExpander.module;
            if (frame != UINT_MAX)
                onReset();
            return;
        }

        // wait until expanding side is ready
        if (frame == UINT_MAX)
            return;

        for (int c = 0; c < inputs[PITCH_INPUT].getChannels(); c++) {
            int vel = (int) std::round(inputs[VELOCITY_INPUT].getNormalPolyVoltage(10.f * 100 / 127, c) / 10.f * 127);
            vel = clamp(vel, 0, 127);
            setVelocity(vel, c);

            int note = (int) std::round(inputs[PITCH_INPUT].getVoltage(c) * 12.f + 60.f);
            note = clamp(note, 0, 127);
            bool gate = inputs[GATE_INPUT].getPolyVoltage(c) >= 1.f;
            setNoteGate(note, gate, c);

            int aft = (int) std::round(inputs[AFTERTOUCH_INPUT].getPolyVoltage(c) / 10.f * 127);
            aft = clamp(aft, 0, 127);
            setKeyPressure(aft, c);
        }

        int pitchbend = (int) std::round((inputs[PITCHBEND_INPUT].getVoltage() + 5.f) / 10.f * 16383);
        pitchbend = clamp(pitchbend, 0, 16383);
        setPitchbend(pitchbend);

        int modwheel = (int) std::round(inputs[MODWHEEL_INPUT].getVoltage() / 10.f * 127);
        modwheel = clamp(modwheel, 0, 127);
        setModWheel(modwheel);

        ++frame;
    }

    json_t* dataToJson() override
    {
        json_t* const rootJ = json_object();
        DISTRHO_SAFE_ASSERT_RETURN(rootJ != nullptr, nullptr);

        json_object_set_new(rootJ, "channel", json_integer(channel));
        return rootJ;
    }

    void dataFromJson(json_t* const rootJ) override
    {
        if (json_t* const channelJ = json_object_get(rootJ, "channel"))
            channel = json_integer_value(channelJ) & 0x0F;
    }
};

// --------------------------------------------------------------------------------------------------------------------

struct CardinalExpanderForInputMIDIWidget : ModuleWidgetWith3HP {
    static constexpr const float startX = 14.0f;
    static constexpr const float startY = 90.0f;
    static constexpr const float padding = 49.0f;

    CardinalExpanderForInputMIDI* const module;

    CardinalExpanderForInputMIDIWidget(CardinalExpanderForInputMIDI* const m)
        : module(m)
    {
        setModule(m);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ExpanderMIDI.svg")));

        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        for (int i=0; i<CardinalExpanderForInputMIDI::NUM_INPUTS; ++i)
            addInput(createInput<PJ301MPort>(Vec(startX + 4.0f, startY + padding * i), m, i));
    }

    void draw(const DrawArgs& args) override
    {
        drawBackground(args.vg);

        nvgFillColor(args.vg, nvgRGB(0xd0, 0xd0, 0xd0));

        nvgSave(args.vg);
        nvgIntersectScissor(args.vg, startX, 0.0f, box.size.x - startX - 1.0f, box.size.y);

        for (int i=0; i<CardinalExpanderForInputMIDI::NUM_INPUTS; ++i)
        {
            const float y = startY + i * padding;

            nvgBeginPath(args.vg);
            nvgRoundedRect(args.vg, startX, y - 19.0f, box.size.x, padding - 4.0f, 4);
            nvgFill(args.vg);
        }

        nvgRestore(args.vg);

        nvgBeginPath(args.vg);
        nvgRoundedRect(args.vg, 6.5f, startY - 19.0f, 3.0f, padding * 6.0f - 4.0f, 1);
        nvgFill(args.vg);

        nvgFillColor(args.vg, color::BLACK);
        nvgFontFaceId(args.vg, 0);
        nvgFontSize(args.vg, 11);
        nvgTextAlign(args.vg, NVG_ALIGN_CENTER);

        nvgText(args.vg, box.size.x * 0.666f, startY + padding * 0 - 4.0f, "V/Oct", nullptr);
        nvgText(args.vg, box.size.x * 0.666f, startY + padding * 1 - 4.0f, "Gate", nullptr);
        nvgText(args.vg, box.size.x * 0.666f, startY + padding * 2 - 4.0f, "Vel", nullptr);
        nvgText(args.vg, box.size.x * 0.666f, startY + padding * 3 - 4.0f, "Aft", nullptr);
        nvgText(args.vg, box.size.x * 0.666f, startY + padding * 4 - 4.0f, "Pb", nullptr);
        nvgText(args.vg, box.size.x * 0.666f, startY + padding * 5 - 4.0f, "MW", nullptr);

        ModuleWidgetWith3HP::draw(args);
    }

    void appendContextMenu(Menu* const menu) override
    {
        menu->addChild(new MenuSeparator);

        struct ChannelItem : MenuItem {
            CardinalExpanderForInputMIDI* module;
            Menu* createChildMenu() override {
                Menu* menu = new Menu;
                for (uint8_t c = 0; c < 16; c++) {
                    menu->addChild(createCheckMenuItem(string::f("%d", c+1), "",
                        [=]() {return module->channel == c;},
                        [=]() {module->channel = c;}
                    ));
                }
                return menu;
            }
        };
        ChannelItem* const channelItem = new ChannelItem;
        channelItem->text = "MIDI channel";
        channelItem->rightText = string::f("%d", module->channel+1) + "  " + RIGHT_ARROW;
        channelItem->module = module;
        menu->addChild(channelItem);

        menu->addChild(createMenuItem("Panic", "",
            [=]() { module->panic(); }
        ));
    }
};

// --------------------------------------------------------------------------------------------------------------------

Model* modelExpanderInputMIDI = createModel<CardinalExpanderForInputMIDI, CardinalExpanderForInputMIDIWidget>("ExpanderInputMIDI");

// --------------------------------------------------------------------------------------------------------------------
