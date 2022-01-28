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
 * This file contains a substantial amount of code from VCVRack's core/MIDIMap.cpp
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

static const int MAX_CHANNELS = 128;

struct HostMIDIMap : Module {
    enum ParamIds {
        NUM_PARAMS
    };
    enum InputIds {
        NUM_INPUTS
    };
    enum OutputIds {
        NUM_OUTPUTS
    };
    enum LightIds {
        NUM_LIGHTS
    };

    // Cardinal specific
    CardinalPluginContext* const pcontext;
    const MidiEvent* midiEvents;
    uint32_t midiEventsLeft;
    uint32_t midiEventFrame;
    int64_t lastBlockFrame;
    uint8_t channel;

    // from Rack
    bool smooth;
    /** Number of maps */
    int mapLen = 0;
    /** The mapped CC number of each channel */
    int ccs[MAX_CHANNELS];
    /** The mapped param handle of each channel */
    ParamHandle paramHandles[MAX_CHANNELS];

    /** Channel ID of the learning session */
    int learningId;
    /** Whether the CC has been set during the learning session */
    bool learnedCc;
    /** Whether the param has been set during the learning session */
    bool learnedParam;

    /** The value of each CC number */
    int8_t values[128];
    /** The smoothing processor (normalized between 0 and 1) of each channel */
    dsp::ExponentialFilter valueFilters[MAX_CHANNELS];
    bool filterInitialized[MAX_CHANNELS] = {};
    dsp::ClockDivider divider;

    HostMIDIMap()
        : pcontext(static_cast<CardinalPluginContext*>(APP))
    {
        if (pcontext == nullptr)
            throw rack::Exception("Plugin context is null");

        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        for (int id = 0; id < MAX_CHANNELS; ++id)
        {
            paramHandles[id].color = nvgRGB(0xff, 0xff, 0x40);
            pcontext->engine->addParamHandle(&paramHandles[id]);
        }

        for (int i = 0; i < MAX_CHANNELS; i++)
            valueFilters[i].setTau(1 / 30.f);

        divider.setDivision(32);
        onReset();
    }

    ~HostMIDIMap()
    {
        if (pcontext == nullptr)
            return;

        for (int id = 0; id < MAX_CHANNELS; ++id)
            pcontext->engine->removeParamHandle(&paramHandles[id]);
    }

    void onReset() override
    {
        midiEvents = nullptr;
        midiEventsLeft = 0;
        midiEventFrame = 0;
        lastBlockFrame = -1;
        channel = 0;

        smooth = true;
        learningId = -1;
        learnedCc = false;
        learnedParam = false;
        // Use NoLock because we're already in an Engine write-lock if Engine::resetModule().
        // We also might be in the MIDIMap() constructor, which could cause problems, but when constructing, all ParamHandles will point to no Modules anyway.
        clearMaps_NoLock();
        mapLen = 1;
        for (int i = 0; i < 128; i++) {
            values[i] = -1;
        }
    }

    void process(const ProcessArgs& args) override
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

        if (!divider.process())
        {
            ++midiEventFrame;
            return;
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
            if ((data[0] & 0xF0) != 0xB0)
                continue;

            uint8_t cc = data[1];
            int8_t value = data[2];

            // Learn
            if (0 <= learningId && values[cc] != value)
            {
                ccs[learningId] = cc;
                valueFilters[learningId].reset();
                learnedCc = true;
                commitLearn();
                updateMapLen();
                refreshParamHandleText(learningId);
            }

            // Ignore negative values generated using the nonstandard 8-bit MIDI extension from the gamepad driver
            if (value < 0)
                continue;

            values[cc] = value;
        }

        ++midiEventFrame;

        // Step channels
        for (int id = 0; id < mapLen; ++id)
        {
            int cc = ccs[id];
            if (cc < 0)
                continue;
            // Get Module
            Module* module = paramHandles[id].module;
            if (!module)
                continue;
            // Get ParamQuantity from ParamHandle
            int paramId = paramHandles[id].paramId;
            ParamQuantity* paramQuantity = module->paramQuantities[paramId];
            if (!paramQuantity)
                continue;
            if (!paramQuantity->isBounded())
                continue;
            // Set filter from param value if filter is uninitialized
            if (!filterInitialized[id]) {
                valueFilters[id].out = paramQuantity->getScaledValue();
                filterInitialized[id] = true;
                continue;
            }
            // Check if CC has been set by the MIDI device
            if (values[cc] < 0)
                continue;
            float value = values[cc] / 127.f;
            // Detect behavior from MIDI buttons.
            if (smooth && std::fabs(valueFilters[id].out - value) < 1.f) {
                // Smooth value with filter
                valueFilters[id].process(args.sampleTime * divider.getDivision(), value);
            }
            else {
                // Jump value
                valueFilters[id].out = value;
            }
            paramQuantity->setScaledValue(valueFilters[id].out);
        }
    }

    void clearMap(int id)
    {
        learningId = -1;
        ccs[id] = -1;
        pcontext->engine->updateParamHandle(&paramHandles[id], -1, 0, true);
        valueFilters[id].reset();
        updateMapLen();
        refreshParamHandleText(id);
    }

    void clearMaps_NoLock()
    {
        learningId = -1;
        for (int id = 0; id < MAX_CHANNELS; id++) {
            ccs[id] = -1;
            pcontext->engine->updateParamHandle_NoLock(&paramHandles[id], -1, 0, true);
            valueFilters[id].reset();
            refreshParamHandleText(id);
        }
        mapLen = 0;
    }

    void updateMapLen()
    {
        // Find last nonempty map
        int id;
        for (id = MAX_CHANNELS - 1; id >= 0; id--) {
            if (ccs[id] >= 0 || paramHandles[id].moduleId >= 0)
                break;
        }
        mapLen = id + 1;
        // Add an empty "Mapping..." slot
        if (mapLen < MAX_CHANNELS)
            mapLen++;
    }

    void commitLearn()
    {
        if (learningId < 0)
            return;
        if (!learnedCc)
            return;
        if (!learnedParam)
            return;
        // Reset learned state
        learnedCc = false;
        learnedParam = false;
        // Find next incomplete map
        while (++learningId < MAX_CHANNELS) {
            if (ccs[learningId] < 0 || paramHandles[learningId].moduleId < 0)
                return;
        }
        learningId = -1;
    }

    void enableLearn(int id)
    {
        if (learningId != id) {
            learningId = id;
            learnedCc = false;
            learnedParam = false;
        }
    }

    void disableLearn(int id)
    {
        if (learningId == id) {
            learningId = -1;
        }
    }

    void learnParam(int id, int64_t moduleId, int paramId)
    {
        pcontext->engine->updateParamHandle(&paramHandles[id], moduleId, paramId, true);
        learnedParam = true;
        commitLearn();
        updateMapLen();
    }

    void refreshParamHandleText(int id) {
        std::string text;
        if (ccs[id] >= 0)
            text = string::f("CC%02d", ccs[id]);
        else
            text = "MIDI-Map";
        paramHandles[id].text = text;
    }

    json_t* dataToJson() override
    {
        json_t* const rootJ = json_object();
        DISTRHO_SAFE_ASSERT_RETURN(rootJ != nullptr, nullptr);

        if (json_t* const mapsJ = json_array())
        {
            for (int id = 0; id < mapLen; id++)
            {
                json_t* const mapJ = json_object();
                DISTRHO_SAFE_ASSERT_CONTINUE(mapJ != nullptr);
                json_object_set_new(mapJ, "cc", json_integer(ccs[id]));
                json_object_set_new(mapJ, "moduleId", json_integer(paramHandles[id].moduleId));
                json_object_set_new(mapJ, "paramId", json_integer(paramHandles[id].paramId));
                json_array_append_new(mapsJ, mapJ);
            }
            json_object_set_new(rootJ, "maps", mapsJ);
        }

        json_object_set_new(rootJ, "smooth", json_boolean(smooth));

        // FIXME use "midi" object?
        json_object_set_new(rootJ, "channel", json_integer(channel));
        return rootJ;
    }

    void dataFromJson(json_t* const rootJ) override
    {
        // Use NoLock because we're already in an Engine write-lock.
        clearMaps_NoLock();

        if (json_t* const mapsJ = json_object_get(rootJ, "maps"))
        {
            json_t* mapJ;
            size_t mapIndex;
            json_array_foreach(mapsJ, mapIndex, mapJ)
            {
                json_t* ccJ = json_object_get(mapJ, "cc");
                json_t* moduleIdJ = json_object_get(mapJ, "moduleId");
                json_t* paramIdJ = json_object_get(mapJ, "paramId");
                if (!(ccJ && moduleIdJ && paramIdJ))
                    continue;
                if (mapIndex >= MAX_CHANNELS)
                    continue;
                ccs[mapIndex] = json_integer_value(ccJ);
                pcontext->engine->updateParamHandle_NoLock(&paramHandles[mapIndex],
                                                           json_integer_value(moduleIdJ),
                                                           json_integer_value(paramIdJ),
                                                           false);
                refreshParamHandleText(mapIndex);
            }
        }

        updateMapLen();

        if (json_t* const smoothJ = json_object_get(rootJ, "smooth"))
            smooth = json_boolean_value(smoothJ);

        // FIXME use "midi" object?
        if (json_t* const channelJ = json_object_get(rootJ, "channel"))
            channel = json_integer_value(channelJ);
    }
};

// --------------------------------------------------------------------------------------------------------------------

struct HostMIDIMapWidget : ModuleWidget {
    static constexpr const float startX_In = 14.0f;
    static constexpr const float startX_Out = 96.0f;
    static constexpr const float startY = 74.0f;
    static constexpr const float padding = 29.0f;
    static constexpr const float middleX = startX_In + (startX_Out - startX_In) * 0.5f + padding * 0.35f;

    HostMIDIMap* const module;

    HostMIDIMapWidget(HostMIDIMap* const m)
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
        menu->addChild(createBoolPtrMenuItem("Smooth CC", "", &module->smooth));

        struct InputChannelItem : MenuItem {
            HostMIDIMap* module;
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
        InputChannelItem* const inputChannelItem = new InputChannelItem;
        inputChannelItem->text = "MIDI channel";
        inputChannelItem->rightText = (module->channel ? string::f("%d", module->channel) : "All")
                                    + "  " + RIGHT_ARROW;
        inputChannelItem->module = module;
        menu->addChild(inputChannelItem);
    }
};

// --------------------------------------------------------------------------------------------------------------------

Model* modelHostMIDIMap = createModel<HostMIDIMap, HostMIDIMapWidget>("HostMIDIMap");

// --------------------------------------------------------------------------------------------------------------------
