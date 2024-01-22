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
 * This class contains a substantial amount of code from VCVRack's core/MIDI_CV.cpp
 * Copyright (C) 2016-2021 VCV.
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 */

struct CardinalExpanderForOutputMIDI : CardinalExpanderFromCarlaMIDIToCV {
    enum ParamIds {
        NUM_PARAMS
    };
    enum InputIds {
        NUM_INPUTS
    };
    enum OutputIds {
        PITCH_OUTPUT,
        GATE_OUTPUT,
        VELOCITY_OUTPUT,
        AFTERTOUCH_OUTPUT,
        PITCHBEND_OUTPUT,
        MODWHEEL_OUTPUT,
        NUM_OUTPUTS
    };
    enum LightIds {
        NUM_LIGHTS
    };

    NativeMidiEvent midiEventsCopy[MAX_MIDI_EVENTS];
    const NativeMidiEvent* midiEventsPtr;
    uint32_t midiEventsLeft;
    uint32_t midiEventFrame;
    uint8_t channel;
    Module* lastConnectedModule;
    midi::Message converterMsg;

    // stuff from Rack
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

    CardinalExpanderForOutputMIDI() {
        static_assert(NUM_INPUTS == kNumInputs, "Invalid input configuration");
        static_assert(NUM_OUTPUTS == kNumOutputs, "Invalid output configuration");
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configOutput(PITCH_OUTPUT, "1V/octave pitch");
        configOutput(GATE_OUTPUT, "Gate");
        configOutput(VELOCITY_OUTPUT, "Velocity");
        configOutput(AFTERTOUCH_OUTPUT, "Aftertouch");
        configOutput(PITCHBEND_OUTPUT, "Pitchbend");
        configOutput(MODWHEEL_OUTPUT, "Mod wheel");
        onReset();
    }

    void reset()
    {
        midiEventsPtr = nullptr;
        midiEventCount = 0;
        midiEventsLeft = 0;
        midiEventFrame = 0;
        channel = 0;
        smooth = true;
        channels = 1;
        polyMode = ROTATE_MODE;
        panic();
    }

    void panic()
    {
        for (int c = 0; c < 16; c++) {
            notes[c] = 60;
            gates[c] = false;
            velocities[c] = 0;
            aftertouches[c] = 0;
            pws[c] = 8192;
            mods[c] = 0;
            pwFilters[c].reset();
            modFilters[c].reset();
        }
        pedal = false;
        rotateIndex = -1;
        heldNotes.clear();
    }

    void setChannels(const int channels)
    {
        if (channels == this->channels)
            return;
        this->channels = channels;
        panic();
    }

    void setPolyMode(const PolyMode polyMode)
    {
        if (polyMode == this->polyMode)
            return;
        this->polyMode = polyMode;
        panic();
    }

    void onReset() override
    {
        reset();
        channel = 0;
        lastConnectedModule = nullptr;
    }

    void process(const ProcessArgs& args) override
    {
        // only handle messages if there is something close to us
        if (leftExpander.module == nullptr)
        {
            // something was connected before, but not anymore, reset
            if (midiEventCount != 0)
                onReset();
            return;
        }
        else if (lastConnectedModule != nullptr && lastConnectedModule != leftExpander.module)
        {
            // whatever we were connected to has changed, reset
            lastConnectedModule = leftExpander.module;
            if (midiEventCount != 0)
                onReset();
            return;
        }

        // check if expanding side has messages for us
        if (midiEventCount != 0)
        {
            midiEventFrame = 0;
            midiEventsLeft = midiEventCount;
            midiEventsPtr = midiEventsCopy;
            std::memcpy(midiEventsCopy, midiEvents, sizeof(NativeMidiEvent)*midiEventCount);
            // reset as required
            midiEventCount = 0;
        }

        while (midiEventsLeft != 0)
        {
            const NativeMidiEvent& midiEvent(*midiEventsPtr);

            if (midiEvent.time > midiEventFrame)
                break;

            ++midiEventsPtr;
            --midiEventsLeft;

            const uint8_t* const data = midiEvent.data;

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
        outputs[PITCH_OUTPUT].setChannels(channels);
        outputs[GATE_OUTPUT].setChannels(channels);
        outputs[VELOCITY_OUTPUT].setChannels(channels);
        outputs[AFTERTOUCH_OUTPUT].setChannels(channels);
        for (int c = 0; c < channels; c++) {
            outputs[PITCH_OUTPUT].setVoltage((notes[c] - 60.f) / 12.f, c);
            outputs[GATE_OUTPUT].setVoltage(gates[c] ? 10.f : 0.f, c);
            outputs[VELOCITY_OUTPUT].setVoltage(rescale(velocities[c], 0, 127, 0.f, 10.f), c);
            outputs[AFTERTOUCH_OUTPUT].setVoltage(rescale(aftertouches[c], 0, 127, 0.f, 10.f), c);
        }

        // Set pitch and mod wheel
        const int wheelChannels = (polyMode == MPE_MODE) ? 16 : 1;
        outputs[PITCHBEND_OUTPUT].setChannels(wheelChannels);
        outputs[MODWHEEL_OUTPUT].setChannels(wheelChannels);
        for (int c = 0; c < wheelChannels; c++) {
            float pw = ((int) pws[c] - 8192) / 8191.f;
            pw = clamp(pw, -1.f, 1.f);
            if (smooth)
                pw = pwFilters[c].process(args.sampleTime, pw);
            else
                pwFilters[c].out = pw;
            outputs[PITCHBEND_OUTPUT].setVoltage(pw * 5.f);

            float mod = mods[c] / 127.f;
            mod = clamp(mod, 0.f, 1.f);
            if (smooth)
                mod = modFilters[c].process(args.sampleTime, mod);
            else
                modFilters[c].out = mod;
            outputs[MODWHEEL_OUTPUT].setVoltage(mod * 10.f);
        }
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
                    panic();
                }
            } break;
            default: break;
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

    json_t* dataToJson() override
    {
        json_t* const rootJ = json_object();
        DISTRHO_SAFE_ASSERT_RETURN(rootJ != nullptr, nullptr);

        json_object_set_new(rootJ, "smooth", json_boolean(smooth));
        json_object_set_new(rootJ, "channels", json_integer(channels));
        json_object_set_new(rootJ, "polyMode", json_integer(polyMode));

        // Saving/restoring pitch and mod doesn't make much sense for MPE.
        if (polyMode != MPE_MODE)
        {
            json_object_set_new(rootJ, "lastPitch", json_integer(pws[0]));
            json_object_set_new(rootJ, "lastMod", json_integer(mods[0]));
        }

        json_object_set_new(rootJ, "channel", json_integer(channel));
        return rootJ;
    }

    void dataFromJson(json_t* const rootJ) override
    {
        if (json_t* const smoothJ = json_object_get(rootJ, "smooth"))
            smooth = json_boolean_value(smoothJ);

        if (json_t* const channelsJ = json_object_get(rootJ, "channels"))
            setChannels(json_integer_value(channelsJ));

        if (json_t* const polyModeJ = json_object_get(rootJ, "polyMode"))
            polyMode = (PolyMode) json_integer_value(polyModeJ);

        if (json_t* const lastPitchJ = json_object_get(rootJ, "lastPitch"))
            pws[0] = json_integer_value(lastPitchJ);

        if (json_t* const lastModJ = json_object_get(rootJ, "lastMod"))
            mods[0] = json_integer_value(lastModJ);

        if (json_t* const channelJ = json_object_get(rootJ, "channel"))
            channel = json_integer_value(channelJ);
    }
};

// --------------------------------------------------------------------------------------------------------------------

struct CardinalExpanderForOutputMIDIWidget : ModuleWidgetWith3HP {
    static constexpr const float startX = 1.0f;
    static constexpr const float startY = 90.0f;
    static constexpr const float padding = 49.0f;

    CardinalExpanderForOutputMIDI* const module;

    CardinalExpanderForOutputMIDIWidget(CardinalExpanderForOutputMIDI* const m)
        : module(m)
    {
        setModule(m);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ExpanderMIDI.svg")));

        addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        for (int i=0; i<CardinalExpanderForOutputMIDI::NUM_OUTPUTS; ++i)
            addOutput(createOutput<PJ301MPort>(Vec(startX + 4.0f, startY + padding * i), m, i));
    }

    void draw(const DrawArgs& args) override
    {
        drawBackground(args.vg);

        nvgFillColor(args.vg, nvgRGB(0xd0, 0xd0, 0xd0));

        nvgSave(args.vg);
        nvgIntersectScissor(args.vg, startX, 0.0f, box.size.x - startX - 1.0f, box.size.y);

        for (int i=0; i<CardinalExpanderForOutputMIDI::NUM_OUTPUTS; ++i)
        {
            const float y = startY + i * padding;

            nvgBeginPath(args.vg);
            nvgRoundedRect(args.vg, -4.0f, y - 19.0f, 35.0f, padding - 4.0f, 4);
            nvgFill(args.vg);
        }

        nvgRestore(args.vg);

        nvgBeginPath(args.vg);
        nvgRoundedRect(args.vg, box.size.x - 9.5f, startY - 19.0f, 3.0f, padding * 6.0f - 4.0f, 1);
        nvgFill(args.vg);

        nvgFillColor(args.vg, color::BLACK);
        nvgFontFaceId(args.vg, 0);
        nvgFontSize(args.vg, 11);
        nvgTextAlign(args.vg, NVG_ALIGN_CENTER);

        nvgText(args.vg, box.size.x * 0.333f + 1.0f, startY + padding * 0 - 4.0f, "V/Oct", nullptr);
        nvgText(args.vg, box.size.x * 0.333f + 1.0f, startY + padding * 1 - 4.0f, "Gate", nullptr);
        nvgText(args.vg, box.size.x * 0.333f + 1.0f, startY + padding * 2 - 4.0f, "Vel", nullptr);
        nvgText(args.vg, box.size.x * 0.333f + 1.0f, startY + padding * 3 - 4.0f, "Aft", nullptr);
        nvgText(args.vg, box.size.x * 0.333f + 1.0f, startY + padding * 4 - 4.0f, "Pb", nullptr);
        nvgText(args.vg, box.size.x * 0.333f + 1.0f, startY + padding * 5 - 4.0f, "MW", nullptr);

        ModuleWidgetWithSideScrews::draw(args);
    }

    void appendContextMenu(Menu* const menu) override
    {
        menu->addChild(new MenuSeparator);

        menu->addChild(createBoolPtrMenuItem("Smooth pitch/mod wheel", "", &module->smooth));

        struct ChannelItem : MenuItem {
            CardinalExpanderForOutputMIDI* module;
            Menu* createChildMenu() override {
                Menu* menu = new Menu;
                for (int c = 0; c <= 16; c++) {
                    menu->addChild(createCheckMenuItem((c == 0) ? "All" : string::f("%d", c), "",
                        [=]() {return module->channel == c;},
                        [=]() {module->channel = c;}
                    ));
                }
                return menu;
            }
        };
        ChannelItem* const channelItem = new ChannelItem;
        channelItem->text = "MIDI channel";
        channelItem->rightText = (module->channel ? string::f("%d", module->channel) : "All") + "  " + RIGHT_ARROW;
        channelItem->module = module;
        menu->addChild(channelItem);

        struct PolyphonyChannelItem : MenuItem {
            CardinalExpanderForOutputMIDI* module;
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
        PolyphonyChannelItem* const polyphonyChannelItem = new PolyphonyChannelItem;
        polyphonyChannelItem->text = "Polyphony channels";
        polyphonyChannelItem->rightText = string::f("%d", module->channels) + "  " + RIGHT_ARROW;
        polyphonyChannelItem->module = module;
        menu->addChild(polyphonyChannelItem);

        menu->addChild(createIndexPtrSubmenuItem("Polyphony mode", {
            "Rotate",
            "Reuse",
            "Reset",
            "MPE",
        }, &module->polyMode));

        menu->addChild(createMenuItem("Panic", "",
            [=]() { module->panic(); }
        ));
    }
};

// --------------------------------------------------------------------------------------------------------------------

Model* modelExpanderOutputMIDI = createModel<CardinalExpanderForOutputMIDI, CardinalExpanderForOutputMIDIWidget>("ExpanderOutputMIDI");

// --------------------------------------------------------------------------------------------------------------------
