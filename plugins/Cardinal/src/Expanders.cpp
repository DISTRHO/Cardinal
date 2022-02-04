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

struct CardinalExpanderForInputMIDI : CardinalExpanderFromCVToCarlaMIDI {
    enum ParamIds {
        NUM_PARAMS
    };
    enum InputIds {
        PITCH_INPUT,
        GATE_INPUT,
        VEL_INPUT,
        AFT_INPUT,
        PW_INPUT,
        MW_INPUT,
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
    int8_t mw;
    int16_t pw;

    Module* lastConnectedModule = nullptr;

    CardinalExpanderForInputMIDI() {
        static_assert(NUM_INPUTS == kNumInputs, "Invalid input configuration");
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configInput(PITCH_INPUT, "Pitch (1V/oct)");
        configInput(GATE_INPUT, "Gate");
        configInput(VEL_INPUT, "Velocity");
        configInput(AFT_INPUT, "Aftertouch");
        configInput(PW_INPUT, "Pitch wheel");
        configInput(MW_INPUT, "Mod wheel");
        onReset();
    }

    /** Must be called before setNoteGate(). */
    void setVelocity(int8_t vel, int c) {
        vels[c] = vel;
    }

    void setNoteGate(int8_t note, bool gate, int c) {
        if (midiEventCount == MAX_MIDI_EVENTS)
            return;
        bool changedNote = gate && gates[c] && (note != notes[c]);
        bool enabledGate = gate && !gates[c];
        bool disabledGate = !gate && gates[c];
        if (changedNote || disabledGate) {
            // Note off
            NativeMidiEvent& m(midiEvents[midiEventCount++]);
            m.time = frame;
            m.port = 0;
            m.size = 3;
            m.data[0] = 0x80;
            m.data[1] = notes[c];
            m.data[2] = vels[c];
        }
        if (changedNote || enabledGate) {
            // Note on
            NativeMidiEvent& m(midiEvents[midiEventCount++]);
            m.time = frame;
            m.port = 0;
            m.size = 3;
            m.data[0] = 0x90;
            m.data[1] = note;
            m.data[2] = vels[c];
        }
        notes[c] = note;
        gates[c] = gate;
    }

    void setKeyPressure(int8_t val, int c) {
        if (keyPressures[c] == val || midiEventCount == MAX_MIDI_EVENTS)
            return;
        keyPressures[c] = val;
        // Polyphonic key pressure
        NativeMidiEvent& m(midiEvents[midiEventCount++]);
        m.time = frame;
        m.port = 0;
        m.size = 3;
        m.data[0] = 0xa0;
        m.data[1] = notes[c];
        m.data[2] = val;
    }

    void setModWheel(int8_t mw) {
        if (this->mw == mw || midiEventCount == MAX_MIDI_EVENTS)
            return;
        this->mw = mw;
        // Modulation Wheel (CC1)
        NativeMidiEvent& m(midiEvents[midiEventCount++]);
        m.time = frame;
        m.port = 0;
        m.size = 3;
        m.data[0] = 0xb0;
        m.data[1] = 1;
        m.data[2] = mw;
    }

    void setPitchWheel(int16_t pw) {
        if (this->pw == pw || midiEventCount == MAX_MIDI_EVENTS)
            return;
        this->pw = pw;
        // Pitch Wheel
        NativeMidiEvent& m(midiEvents[midiEventCount++]);
        m.time = frame;
        m.port = 0;
        m.size = 3;
        m.data[0] = 0xe0;
        m.data[1] = pw & 0x7f;
        m.data[2] = (pw >> 7) & 0x7f;
    }

    void onReset() override
    {
        for (uint c = 0; c < CHANNELS; c++) {
            vels[c] = 100;
            notes[c] = 60;
            gates[c] = false;
            keyPressures[c] = -1;
        }
        mw = -1;
        pw = 0x2000;
        midiEventCount = 0;
        frame = UINT_MAX;
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
            int vel = (int) std::round(inputs[VEL_INPUT].getNormalPolyVoltage(10.f * 100 / 127, c) / 10.f * 127);
            vel = clamp(vel, 0, 127);
            setVelocity(vel, c);

            int note = (int) std::round(inputs[PITCH_INPUT].getVoltage(c) * 12.f + 60.f);
            note = clamp(note, 0, 127);
            bool gate = inputs[GATE_INPUT].getPolyVoltage(c) >= 1.f;
            setNoteGate(note, gate, c);

            int aft = (int) std::round(inputs[AFT_INPUT].getPolyVoltage(c) / 10.f * 127);
            aft = clamp(aft, 0, 127);
            setKeyPressure(aft, c);
        }

        int pw = (int) std::round((inputs[PW_INPUT].getVoltage() + 5.f) / 10.f * 0x4000);
        pw = clamp(pw, 0, 0x3fff);
        setPitchWheel(pw);

        int mw = (int) std::round(inputs[MW_INPUT].getVoltage() / 10.f * 127);
        mw = clamp(mw, 0, 127);
        setModWheel(mw);

        ++frame;
    }
};

// --------------------------------------------------------------------------------------------------------------------

struct CardinalExpanderForInputMIDIWidget : ModuleWidgetWithSideScrews<> {
    static constexpr const float startX = 14.0f;
    static constexpr const float startY = 90.0f;
    static constexpr const float padding = 49.0f;

    CardinalExpanderForInputMIDIWidget(CardinalExpanderForInputMIDI* const module)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ExpanderMIDI.svg")));

        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        for (int i=0; i<CardinalExpanderForInputMIDI::NUM_INPUTS; ++i)
            addInput(createInput<PJ301MPort>(Vec(startX + 4.0f, startY + padding * i), module, i));
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

        nvgBeginPath(args.vg);
        nvgRect(args.vg, box.size.x * 0.5f, 0, box.size.x, box.size.y);
        nvgFillColor(args.vg, color::BLACK);
        nvgFontFaceId(args.vg, 0);
        nvgFontSize(args.vg, 11);
        nvgTextAlign(args.vg, NVG_ALIGN_CENTER);

        nvgText(args.vg, box.size.x * 0.666f, startY + padding * 0 - 4.0f, "V/Oct", nullptr);
        nvgText(args.vg, box.size.x * 0.666f, startY + padding * 1 - 4.0f, "Gate", nullptr);
        nvgText(args.vg, box.size.x * 0.666f, startY + padding * 2 - 4.0f, "Vel", nullptr);
        nvgText(args.vg, box.size.x * 0.666f, startY + padding * 3 - 4.0f, "Aft", nullptr);
        nvgText(args.vg, box.size.x * 0.666f, startY + padding * 4 - 4.0f, "PW", nullptr);
        nvgText(args.vg, box.size.x * 0.666f, startY + padding * 5 - 4.0f, "MW", nullptr);

        ModuleWidgetWithSideScrews::draw(args);
    }
};

// --------------------------------------------------------------------------------------------------------------------

Model* modelExpanderInputMIDI = createModel<CardinalExpanderForInputMIDI, CardinalExpanderForInputMIDIWidget>("ExpanderInputMIDI");

// --------------------------------------------------------------------------------------------------------------------
