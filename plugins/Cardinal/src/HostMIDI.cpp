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
 * This file contains a substantial amount of code from VCVRack's core/CV_MIDI.cpp and core/MIDI_CV.cpp
 * Copyright (C) 2016-2021 VCV.
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 */

#include "plugincontext.hpp"
#include "ModuleWidgets.hpp"

#include <algorithm>

// -----------------------------------------------------------------------------------------------------------

USE_NAMESPACE_DISTRHO;

struct HostMIDI : TerminalModule {
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
        CLK_INPUT, // RETRIGGER_OUTPUT
        VOL_INPUT, // CLOCK_OUTPUT
        PAN_INPUT, // CLOCK_DIV_OUTPUT
        START_INPUT,
        STOP_INPUT,
        CONTINUE_INPUT,
        NUM_INPUTS
    };
    enum OutputIds {
        PITCH_OUTPUT,
        GATE_OUTPUT,
        VELOCITY_OUTPUT,
        AFTERTOUCH_OUTPUT,
        PITCHBEND_OUTPUT,
        MODWHEEL_OUTPUT,
        RETRIGGER_OUTPUT, // CLK_INPUT
        CLOCK_OUTPUT,     // VOL_INPUT
        CLOCK_DIV_OUTPUT, // PAN_INPUT
        START_OUTPUT,
        STOP_OUTPUT,
        CONTINUE_OUTPUT,
        NUM_OUTPUTS
    };
    enum LightIds {
        NUM_LIGHTS
    };

    CardinalPluginContext* const pcontext;

    struct MidiInput {
        // Cardinal specific
        CardinalPluginContext* const pcontext;
        midi::Message converterMsg;
        const MidiEvent* midiEvents;
        uint32_t midiEventsLeft;
        uint32_t midiEventFrame;
        uint32_t lastProcessCounter;
        bool wasPlaying;
        uint8_t channel;

        // stuff from Rack
        /** Number of semitones to bend up/down by pitch wheel */
        float pwRange;
        bool smooth;
        int channels;
        enum PolyMode {
            ROTATE_MODE,
            REUSE_MODE,
            RESET_MODE,
            MPE_MODE,
            NUM_POLY_MODES
        };
        PolyMode polyMode;

        bool pedal;
        // Indexed by channel
        uint8_t notes[16];
        bool gates[16];
        bool gatesForceGap[16];
        bool gateForceGaps;
        uint8_t velocities[16];
        uint8_t aftertouches[16];
        std::vector<uint8_t> heldNotes;

        int rotateIndex;

        /** Pitch wheel.
        When MPE is disabled, only the first channel is used.
        [channel]
        */
        uint16_t pws[16];
        /** [channel] */
        uint8_t mods[16];
        dsp::ExponentialFilter pwFilters[16];
        dsp::ExponentialFilter modFilters[16];

        dsp::PulseGenerator startPulse;
        dsp::PulseGenerator stopPulse;
        dsp::PulseGenerator continuePulse;
        dsp::PulseGenerator retriggerPulses[16];

        MidiInput(CardinalPluginContext* const pc)
            : pcontext(pc)
        {
            converterMsg.bytes.resize(0xff);
            heldNotes.reserve(128);
            for (int c = 0; c < 16; c++) {
                pwFilters[c].setTau(1 / 30.f);
                modFilters[c].setTau(1 / 30.f);
            }
            reset();
        }

        void reset()
        {
            midiEvents = nullptr;
            midiEventsLeft = 0;
            midiEventFrame = 0;
            lastProcessCounter = 0;
            wasPlaying = false;
            channel = 0;
            smooth = false;
            channels = 1;
            polyMode = ROTATE_MODE;
            pwRange = 0;
            gateForceGaps = false;
            panic(true);
        }

        /** Resets performance state */
        void panic(const bool resetPitch)
        {
            for (int c = 0; c < 16; c++) {
                notes[c] = 60;
                gates[c] = false;
                gatesForceGap[c] = false;
                velocities[c] = 0;
                aftertouches[c] = 0;
                pwFilters[c].reset();
                modFilters[c].reset();
            }
            if (resetPitch) {
                for (int c = 0; c < 16; c++) {
                    notes[c] = 60;
                    pws[c] = 8192;
                    mods[c] = 0;
                }
            }
            pedal = false;
            rotateIndex = -1;
            heldNotes.clear();
        }

        bool process(const ProcessArgs& args, std::vector<rack::engine::Output>& outputs, const bool isBypassed)
        {
            // Cardinal specific
            const uint32_t processCounter = pcontext->processCounter;
            const bool processCounterChanged = lastProcessCounter != processCounter;

            if (processCounterChanged)
            {
                lastProcessCounter = processCounter;

                midiEvents = pcontext->midiEvents;
                midiEventsLeft = pcontext->midiEventCount;

                if (isBypassed)
                {
                    midiEventFrame = 1;
                    return true;
                }

                midiEventFrame = 0;

                if (pcontext->playing)
                {
                    if (! wasPlaying)
                    {
                        wasPlaying = true;
                        if (pcontext->frame == 0)
                            startPulse.trigger(1e-3);

                        continuePulse.trigger(1e-3);
                    }
                }
                else if (wasPlaying)
                {
                    wasPlaying = false;
                    stopPulse.trigger(1e-3);
                }
            }
            else if (isBypassed)
            {
                ++midiEventFrame;
                return false;
            }

            while (midiEventsLeft != 0)
            {
                const MidiEvent& midiEvent(*midiEvents);

                if (midiEvent.frame > midiEventFrame)
                    break;

                ++midiEvents;
                --midiEventsLeft;

                const uint8_t* data;

                if (midiEvent.size > MidiEvent::kDataSize)
                {
                    data = midiEvent.dataExt;
                    converterMsg.bytes.resize(midiEvent.size);
                }
                else
                {
                    data = midiEvent.data;
                }

                if (channel != 0 && data[0] < 0xF0)
                {
                    if ((data[0] & 0x0F) != (channel - 1))
                        continue;
                }

                converterMsg.frame = midiEventFrame;
                std::memcpy(converterMsg.bytes.data(), data, midiEvent.size);

                processMessage(converterMsg);
            }

            ++midiEventFrame;

            // Rack stuff
            // Set pitch and mod wheel
            const int wheelChannels = (polyMode == MPE_MODE) ? 16 : 1;
            float pwValues[16] = {};
            outputs[PITCHBEND_OUTPUT].setChannels(wheelChannels);
            outputs[MODWHEEL_OUTPUT].setChannels(wheelChannels);
            for (int c = 0; c < wheelChannels; c++) {
                float pw = (int16_t(pws[c]) - 8192) / 8191.f;
                pw = clamp(pw, -1.f, 1.f);
                if (smooth)
                    pw = pwFilters[c].process(args.sampleTime, pw);
                else
                    pwFilters[c].out = pw;
                pwValues[c] = pw;
                outputs[PITCHBEND_OUTPUT].setVoltage(pw * 5.f, c);

                float mod = mods[c] / 127.f;
                mod = clamp(mod, 0.f, 1.f);
                if (smooth)
                    mod = modFilters[c].process(args.sampleTime, mod);
                else
                    modFilters[c].out = mod;
                outputs[MODWHEEL_OUTPUT].setVoltage(mod * 10.f, c);
            }

            // Set note outputs
            outputs[PITCH_OUTPUT].setChannels(channels);
            outputs[GATE_OUTPUT].setChannels(channels);
            outputs[VELOCITY_OUTPUT].setChannels(channels);
            outputs[AFTERTOUCH_OUTPUT].setChannels(channels);
            outputs[RETRIGGER_OUTPUT].setChannels(channels);

            for (int c = 0; c < channels; c++) {
                float pw = pwValues[(polyMode == MPE_MODE) ? c : 0];
                float pitch = (notes[c] - 60.f + pw * pwRange) / 12.f;
                outputs[PITCH_OUTPUT].setVoltage(pitch, c);
                outputs[GATE_OUTPUT].setVoltage(gates[c] && !gatesForceGap[c] ? 10.f : 0.f, c);
                outputs[VELOCITY_OUTPUT].setVoltage(rescale(velocities[c], 0, 127, 0.f, 10.f), c);
                outputs[AFTERTOUCH_OUTPUT].setVoltage(rescale(aftertouches[c], 0, 127, 0.f, 10.f), c);
                outputs[RETRIGGER_OUTPUT].setVoltage(retriggerPulses[c].process(args.sampleTime) ? 10.f : 0.f, c);
                gatesForceGap[c] = false;
            }

            outputs[START_OUTPUT].setVoltage(startPulse.process(args.sampleTime) ? 10.f : 0.f);
            outputs[STOP_OUTPUT].setVoltage(stopPulse.process(args.sampleTime) ? 10.f : 0.f);
            outputs[CONTINUE_OUTPUT].setVoltage(continuePulse.process(args.sampleTime) ? 10.f : 0.f);

            return processCounterChanged;
        }

        void processMessage(const midi::Message& msg)
        {
            // DEBUG("MIDI: %ld %s", msg.getFrame(), msg.toString().c_str());

            switch (msg.getStatus()) {
                // note off
                case 0x8: {
                    releaseNote(msg.getNote());
                } break;
                // note on
                case 0x9: {
                    if (msg.getValue() > 0) {
                        int c = msg.getChannel();
                        pressNote(msg.getNote(), &c);
                        velocities[c] = msg.getValue();
                    }
                    else {
                        // For some reason, some keyboards send a "note on" event with a velocity of 0 to signal that the key has been released.
                        releaseNote(msg.getNote());
                    }
                } break;
                // key pressure
                case 0xa: {
                    // Set the aftertouches with the same note
                    // TODO Should we handle the MPE case differently?
                    for (int c = 0; c < 16; c++) {
                        if (notes[c] == msg.getNote())
                            aftertouches[c] = msg.getValue();
                    }
                } break;
                // cc
                case 0xb: {
                    processCC(msg);
                } break;
                // channel pressure
                case 0xd: {
                    if (polyMode == MPE_MODE) {
                        // Set the channel aftertouch
                        aftertouches[msg.getChannel()] = msg.getNote();
                    }
                    else {
                        // Set all aftertouches
                        for (int c = 0; c < 16; c++) {
                            aftertouches[c] = msg.getNote();
                        }
                    }
                } break;
                // pitch wheel
                case 0xe: {
                    int c = (polyMode == MPE_MODE) ? msg.getChannel() : 0;
                    pws[c] = ((uint16_t) msg.getValue() << 7) | msg.getNote();
                } break;
                case 0xf: {
                    processSystem(msg);
                } break;
                default: break;
            }
        }

        void processCC(const midi::Message& msg) {
            switch (msg.getNote()) {
                // mod
                case 0x01: {
                    int c = (polyMode == MPE_MODE) ? msg.getChannel() : 0;
                    mods[c] = msg.getValue();
                } break;
                // sustain
                case 0x40: {
                    if (msg.getValue() >= 64)
                        pressPedal();
                    else
                        releasePedal();
                } break;
                // all notes off (panic)
                case 0x7b: {
                    if (msg.getValue() == 0) {
                        panic(false);
                    }
                } break;
                default: break;
            }
        }

        void processSystem(const midi::Message& msg)
        {
            switch (msg.getChannel())
            {
            // Start
            case 0xa:
                startPulse.trigger(1e-3);
                break;
            // Continue
            case 0xb:
                continuePulse.trigger(1e-3);
                break;
            // Stop
            case 0xc:
                stopPulse.trigger(1e-3);
                break;
            }
        }

        int assignChannel(uint8_t note) {
            if (channels == 1)
                return 0;

            switch (polyMode) {
                case REUSE_MODE: {
                    // Find channel with the same note
                    for (int c = 0; c < channels; c++) {
                        if (notes[c] == note)
                            return c;
                    }
                } // fallthrough

                case ROTATE_MODE: {
                    // Find next available channel
                    for (int i = 0; i < channels; i++) {
                        rotateIndex++;
                        if (rotateIndex >= channels)
                            rotateIndex = 0;
                        if (!gates[rotateIndex])
                            return rotateIndex;
                    }
                    // No notes are available. Advance rotateIndex once more.
                    rotateIndex++;
                    if (rotateIndex >= channels)
                        rotateIndex = 0;
                    return rotateIndex;
                } break;

                case RESET_MODE: {
                    for (int c = 0; c < channels; c++) {
                        if (!gates[c])
                            return c;
                    }
                    return channels - 1;
                } break;

                case MPE_MODE: {
                    // This case is handled by querying the MIDI message channel.
                    return 0;
                } break;

                default: return 0;
            }
        }

        void pressNote(uint8_t note, int* channel) {
            // Remove existing similar note
            auto it = std::find(heldNotes.begin(), heldNotes.end(), note);
            if (it != heldNotes.end())
                heldNotes.erase(it);
            // Push note
            heldNotes.push_back(note);
            // Determine actual channel
            if (polyMode == MPE_MODE) {
                // Channel is already decided for us
            }
            else {
                *channel = assignChannel(note);
            }
            // Set note
            notes[*channel] = note;
            gates[*channel] = true;
            retriggerPulses[*channel].trigger(1e-3);
        }

        void releaseNote(uint8_t note) {
            // Remove the note
            auto it = std::find(heldNotes.begin(), heldNotes.end(), note);
            if (it != heldNotes.end())
                heldNotes.erase(it);
            // Hold note if pedal is pressed
            if (pedal)
                return;
            // Turn off gate of all channels with note
            for (int c = 0; c < channels; c++) {
                if (notes[c] == note) {
                    gates[c] = false;
                    // this will stay low even when gates[c] = true
                    // is set by a note on before the gate is sent as low
                    gatesForceGap[c] = gateForceGaps;
                }
            }
            // Set last note if monophonic
            if (channels == 1) {
                if (note == notes[0] && !heldNotes.empty()) {
                    uint8_t lastNote = heldNotes.back();
                    notes[0] = lastNote;
                    gates[0] = true;
                    return;
                }
            }
        }

        void pressPedal() {
            if (pedal)
                return;
            pedal = true;
        }

        void releasePedal() {
            if (!pedal)
                return;
            pedal = false;
            // Set last note if monophonic
            if (channels == 1) {
                if (!heldNotes.empty()) {
                    uint8_t lastNote = heldNotes.back();
                    notes[0] = lastNote;
                }
            }
            // Clear notes that are not held if polyphonic
            else {
                for (int c = 0; c < channels; c++) {
                    if (!gates[c])
                        continue;
                    gates[c] = false;
                    for (uint8_t note : heldNotes) {
                        if (notes[c] == note) {
                            gates[c] = true;
                            break;
                        }
                    }
                }
            }
        }

        void setChannels(const int channels)
        {
            if (channels == this->channels)
                return;
            this->channels = channels;
            panic(true);
        }

        void setPolyMode(const PolyMode polyMode)
        {
            if (polyMode == this->polyMode)
                return;
            this->polyMode = polyMode;
            panic(true);
        }
    } midiInput;

    struct MidiOutput : dsp::MidiGenerator<PORT_MAX_CHANNELS> {
        CardinalPluginContext* const pcontext;
        uint8_t channel = 0;

        // caching
        struct {
            bool gate = false;
            bool velocity = false;
            bool aftertouch = false;
            bool pitchbend = false;
            bool modwheel = false;
            bool start = false;
            bool stop = false;
            bool cont = false;
        } connected;

        MidiOutput(CardinalPluginContext* const pc)
            : pcontext(pc) {}

        void onMessage(const midi::Message& message) override
        {
            pcontext->writeMidiMessage(message, channel);
        }
    } midiOutput;

    HostMIDI()
        : pcontext(static_cast<CardinalPluginContext*>(APP)),
          midiInput(pcontext),
          midiOutput(pcontext)
    {
        if (pcontext == nullptr)
            throw rack::Exception("Plugin context is null");

        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configInput(PITCH_INPUT, "1V/octave pitch");
        configInput(GATE_INPUT, "Gate");
        configInput(VELOCITY_INPUT, "Velocity");
        configInput(AFTERTOUCH_INPUT, "Aftertouch");
        configInput(PITCHBEND_INPUT, "Pitchbend");
        configInput(MODWHEEL_INPUT, "Mod wheel");
        configInput(CLK_INPUT, "Clock");
        configInput(VOL_INPUT, "Volume");
        configInput(PAN_INPUT, "Pan");
        configInput(START_INPUT, "Start trigger");
        configInput(STOP_INPUT, "Stop trigger");
        configInput(CONTINUE_INPUT, "Continue trigger");
        configOutput(PITCH_OUTPUT, "1V/octave pitch");
        configOutput(GATE_OUTPUT, "Gate");
        configOutput(VELOCITY_OUTPUT, "Velocity");
        configOutput(AFTERTOUCH_OUTPUT, "Aftertouch");
        configOutput(PITCHBEND_OUTPUT, "Pitchbend");
        configOutput(MODWHEEL_OUTPUT, "Mod wheel");
        configOutput(RETRIGGER_OUTPUT, "Retrigger");
        configOutput(CLOCK_OUTPUT, "Clock");
        configOutput(CLOCK_DIV_OUTPUT, "Clock divider");
        configOutput(START_OUTPUT, "Start trigger");
        configOutput(STOP_OUTPUT, "Stop trigger");
        configOutput(CONTINUE_OUTPUT, "Continue trigger");
    }

    void onReset() override
    {
        midiInput.reset();
        midiOutput.reset();
        midiOutput.channel = 0;
    }

    void processTerminalInput(const ProcessArgs& args) override
    {
        if (midiInput.process(args, outputs, isBypassed()))
        {
            midiOutput.frame = 0;
            midiOutput.connected.gate = inputs[GATE_INPUT].isConnected();
            midiOutput.connected.velocity = inputs[VELOCITY_INPUT].isConnected();
            midiOutput.connected.aftertouch = inputs[AFTERTOUCH_INPUT].isConnected();
            midiOutput.connected.pitchbend = inputs[PITCHBEND_INPUT].isConnected();
            midiOutput.connected.modwheel = inputs[MODWHEEL_INPUT].isConnected();
            midiOutput.connected.start = inputs[START_INPUT].isConnected();
            midiOutput.connected.stop = inputs[STOP_INPUT].isConnected();
            midiOutput.connected.cont = inputs[CONTINUE_INPUT].isConnected();
        }
        else
        {
            ++midiOutput.frame;
        }
    }

    void processTerminalOutput(const ProcessArgs&) override
    {
        if (isBypassed())
            return;

        auto connected = midiOutput.connected;

        for (int c = 0; c < inputs[PITCH_INPUT].getChannels(); ++c)
        {
            if (connected.velocity)
            {
                const constexpr float n = 10.f * 100.f / 127.f;
                const int vel = clamp(
                    static_cast<int>(inputs[VELOCITY_INPUT].getNormalPolyVoltage(n, c) / 10.f * 127.f + 0.5f), 0, 127);
                midiOutput.setVelocity(vel, c);
            }
            else
            {
                midiOutput.setVelocity(100, c);
            }

            const int note = clamp(static_cast<int>(inputs[PITCH_INPUT].getVoltage(c) * 12.f + 60.5f), 0, 127);
            const bool gate = connected.gate ? inputs[GATE_INPUT].getPolyVoltage(c) >= 1.f : false;
            midiOutput.setNoteGate(note, gate, c);

            if (connected.aftertouch)
            {
                const int aft = clamp(
                    static_cast<int>(inputs[AFTERTOUCH_INPUT].getPolyVoltage(c) / 10.f * 127.f + 0.5f), 0, 127);
                midiOutput.setKeyPressure(aft, c);
            }
            else
            {
                midiOutput.setKeyPressure(0, c);
            }
        }

        if (connected.pitchbend)
        {
            const int pw = clamp(
                static_cast<int>((inputs[PITCHBEND_INPUT].getVoltage() + 5.f) / 10.f * 16383.f + 0.5f), 0, 16383);
            midiOutput.setPitchWheel(pw);
        }
        else
        {
            midiOutput.setPitchWheel(0);
        }

        if (connected.modwheel)
        {
            const int mw = clamp(
                static_cast<int>(inputs[MODWHEEL_INPUT].getVoltage() / 10.f * 127.f + 0.5f), 0, 127);
            midiOutput.setModWheel(mw);
        }
        else
        {
            midiOutput.setModWheel(0);
        }

        const bool start = connected.start ? inputs[START_INPUT].getVoltage() >= 1.f : false;
        midiOutput.setStart(start);

        const bool stop = connected.stop ? inputs[STOP_INPUT].getVoltage() >= 1.f : false;
        midiOutput.setStop(stop);

        const bool cont = connected.cont ? inputs[CONTINUE_INPUT].getVoltage() >= 1.f : false;
        midiOutput.setContinue(cont);
    }

    json_t* dataToJson() override
    {
        json_t* const rootJ = json_object();
        DISTRHO_SAFE_ASSERT_RETURN(rootJ != nullptr, nullptr);

        json_object_set_new(rootJ, "pwRange", json_real(midiInput.pwRange));
        json_object_set_new(rootJ, "smooth", json_boolean(midiInput.smooth));
        json_object_set_new(rootJ, "forceGateGaps", json_boolean(midiInput.gateForceGaps));
        json_object_set_new(rootJ, "channels", json_integer(midiInput.channels));
        json_object_set_new(rootJ, "polyMode", json_integer(midiInput.polyMode));

        // Saving/restoring pitch and mod doesn't make much sense for MPE.
        if (midiInput.polyMode != MidiInput::MPE_MODE)
        {
            json_object_set_new(rootJ, "lastPitch", json_integer(midiInput.pws[0]));
            json_object_set_new(rootJ, "lastMod", json_integer(midiInput.mods[0]));
        }

        json_object_set_new(rootJ, "inputChannel", json_integer(midiInput.channel));
        json_object_set_new(rootJ, "outputChannel", json_integer(midiOutput.channel));

        return rootJ;
    }

    void dataFromJson(json_t* const rootJ) override
    {
        if (json_t* const pwRangeJ = json_object_get(rootJ, "pwRange"))
            midiInput.pwRange = json_number_value(pwRangeJ);
        // For backwards compatibility, set to 0 if undefined in JSON.
        else
            midiInput.pwRange = 0;

        if (json_t* const smoothJ = json_object_get(rootJ, "smooth"))
            midiInput.smooth = json_boolean_value(smoothJ);

        if (json_t* const forceGateGapsJ = json_object_get(rootJ, "forceGateGaps"))
            midiInput.gateForceGaps = json_boolean_value(forceGateGapsJ);

        if (json_t* const channelsJ = json_object_get(rootJ, "channels"))
            midiInput.setChannels(json_integer_value(channelsJ));

        if (json_t* const polyModeJ = json_object_get(rootJ, "polyMode"))
            midiInput.polyMode = (MidiInput::PolyMode) json_integer_value(polyModeJ);

        if (json_t* const lastPitchJ = json_object_get(rootJ, "lastPitch"))
            midiInput.pws[0] = json_integer_value(lastPitchJ);

        if (json_t* const lastModJ = json_object_get(rootJ, "lastMod"))
            midiInput.mods[0] = json_integer_value(lastModJ);

        if (json_t* const inputChannelJ = json_object_get(rootJ, "inputChannel"))
            midiInput.channel = json_integer_value(inputChannelJ);

        if (json_t* const outputChannelJ = json_object_get(rootJ, "outputChannel"))
            midiOutput.channel = json_integer_value(outputChannelJ) & 0x0F;
    }
};

// --------------------------------------------------------------------------------------------------------------------

#ifndef HEADLESS
struct HostMIDIWidget : ModuleWidgetWith9HP {
    HostMIDI* const module;

    HostMIDIWidget(HostMIDI* const m)
        : module(m)
    {
        setModule(m);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/HostMIDI.svg")));

        createAndAddScrews();

        createAndAddInput(0, HostMIDI::PITCH_INPUT);
        createAndAddInput(1, HostMIDI::GATE_INPUT);
        createAndAddInput(2, HostMIDI::VELOCITY_INPUT);
        createAndAddInput(3, HostMIDI::AFTERTOUCH_INPUT);
        createAndAddInput(4, HostMIDI::PITCHBEND_INPUT);
        createAndAddInput(5, HostMIDI::MODWHEEL_INPUT);
        createAndAddInput(6, HostMIDI::START_INPUT);
        createAndAddInput(7, HostMIDI::STOP_INPUT);
        createAndAddInput(8, HostMIDI::CONTINUE_INPUT);

        createAndAddOutput(0, HostMIDI::PITCH_OUTPUT);
        createAndAddOutput(1, HostMIDI::GATE_OUTPUT);
        createAndAddOutput(2, HostMIDI::VELOCITY_OUTPUT);
        createAndAddOutput(3, HostMIDI::AFTERTOUCH_OUTPUT);
        createAndAddOutput(4, HostMIDI::PITCHBEND_OUTPUT);
        createAndAddOutput(5, HostMIDI::MODWHEEL_OUTPUT);
        createAndAddOutput(6, HostMIDI::START_OUTPUT);
        createAndAddOutput(7, HostMIDI::STOP_OUTPUT);
        createAndAddOutput(8, HostMIDI::CONTINUE_OUTPUT);
        createAndAddOutput(9, HostMIDI::RETRIGGER_OUTPUT);
    }

    void draw(const DrawArgs& args) override
    {
        drawBackground(args.vg);
        drawOutputJacksArea(args.vg, 10);
        setupTextLines(args.vg);

        drawTextLine(args.vg, 0, "V/Oct");
        drawTextLine(args.vg, 1, "Gate");
        drawTextLine(args.vg, 2, "Velocity");
        drawTextLine(args.vg, 3, "Aftertouch");
        drawTextLine(args.vg, 4, "Pitchbend");
        drawTextLine(args.vg, 5, "Mod Wheel");
        drawTextLine(args.vg, 6, "Start");
        drawTextLine(args.vg, 7, "Stop");
        drawTextLine(args.vg, 8, "Cont");
        drawTextLine(args.vg, 9, "Retrigger");

        ModuleWidgetWith9HP::draw(args);
    }

    void appendContextMenu(Menu* const menu) override
    {
        menu->addChild(new MenuSeparator);
        menu->addChild(createMenuLabel("MIDI Input"));

        menu->addChild(createBoolPtrMenuItem("Force gate gaps between notes", "", &module->midiInput.gateForceGaps));

        menu->addChild(createBoolPtrMenuItem("Smooth pitch/mod wheel", "", &module->midiInput.smooth));

        static const std::vector<float> pwRanges = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 24, 36, 48};
        menu->addChild(createSubmenuItem("Pitch bend range", string::f("%g", module->midiInput.pwRange), [=](Menu* menu) {
            for (size_t i = 0; i < pwRanges.size(); i++) {
                menu->addChild(createCheckMenuItem(string::f("%g", pwRanges[i]), "",
                    [=]() {return module->midiInput.pwRange == pwRanges[i];},
                    [=]() {module->midiInput.pwRange = pwRanges[i];}
                ));
            }
        }));

        struct InputChannelItem : MenuItem {
            HostMIDI* module;
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

        menu->addChild(createSubmenuItem("Polyphony channels", string::f("%d", module->midiInput.channels), [=](Menu* menu) {
            for (int c = 1; c <= 16; c++) {
                menu->addChild(createCheckMenuItem((c == 1) ? "Monophonic" : string::f("%d", c), "",
                    [=]() {return module->midiInput.channels == c;},
                    [=]() {module->midiInput.setChannels(c);}
                ));
            }
        }));

        menu->addChild(createIndexPtrSubmenuItem("Polyphony mode", {
            "Rotate",
            "Reuse",
            "Reset",
            "MPE",
        }, &module->midiInput.polyMode));

        menu->addChild(new MenuSeparator);
        menu->addChild(createMenuLabel("MIDI Output"));

        struct OutputChannelItem : MenuItem {
            HostMIDI* module;
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

        menu->addChild(createMenuItem("Panic", "",
            [=]() { module->midiInput.panic(true); module->midiOutput.panic(); }
        ));
    }
};
#else
struct HostMIDIWidget : ModuleWidget {
    HostMIDIWidget(HostMIDI* const module) {
        setModule(module);
        for (uint i=0; i<HostMIDI::NUM_INPUTS; ++i)
            addInput(createInput<PJ301MPort>({}, module, i));
        for (uint i=0; i<HostMIDI::NUM_OUTPUTS; ++i)
            addOutput(createOutput<PJ301MPort>({}, module, i));
    }
};
#endif

// --------------------------------------------------------------------------------------------------------------------

Model* modelHostMIDI = createModel<HostMIDI, HostMIDIWidget>("HostMIDI");

// --------------------------------------------------------------------------------------------------------------------
