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
#include "Widgets.hpp"

#include <algorithm>

// -----------------------------------------------------------------------------------------------------------

USE_NAMESPACE_DISTRHO;

static const int MAX_MIDI_CONTROL = 120; /* 0x77 + 1 */

struct HostMIDIMap : TerminalModule {
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
    uint32_t lastProcessCounter;
    int nextLearningId;
    uint8_t channel;

    // from Rack
    bool smooth;
    /** Number of maps */
    int mapLen = 0;
    /** The mapped CC number of each channel */
    int ccs[MAX_MIDI_CONTROL];
    /** The mapped param handle of each channel */
    ParamHandle paramHandles[MAX_MIDI_CONTROL];

    /** Channel ID of the learning session */
    int learningId;
    /** Whether the CC has been set during the learning session */
    bool learnedCc;
    /** Whether the param has been set during the learning session */
    bool learnedParam;

    /** The value of each CC number */
    int8_t values[MAX_MIDI_CONTROL];
    /** The smoothing processor (normalized between 0 and 1) of each channel */
    dsp::ExponentialFilter valueFilters[MAX_MIDI_CONTROL];
    bool filterInitialized[MAX_MIDI_CONTROL] = {};
    dsp::ClockDivider divider;

    HostMIDIMap()
        : pcontext(static_cast<CardinalPluginContext*>(APP))
    {
        if (pcontext == nullptr)
            throw rack::Exception("Plugin context is null");

        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        for (int id = 0; id < MAX_MIDI_CONTROL; ++id)
        {
            paramHandles[id].color = nvgRGBf(0.76f, 0.11f, 0.22f);
            pcontext->engine->addParamHandle(&paramHandles[id]);
        }

        for (int i = 0; i < MAX_MIDI_CONTROL; i++)
            valueFilters[i].setTau(1 / 30.f);

        divider.setDivision(32);
        onReset();
    }

    ~HostMIDIMap()
    {
        if (pcontext == nullptr)
            return;

        for (int id = 0; id < MAX_MIDI_CONTROL; ++id)
            pcontext->engine->removeParamHandle(&paramHandles[id]);
    }

    void onReset() override
    {
        midiEvents = nullptr;
        midiEventsLeft = 0;
        midiEventFrame = 0;
        lastProcessCounter = 0;
        nextLearningId = -1;
        channel = 0;

        smooth = true;
        learningId = -1;
        learnedCc = false;
        learnedParam = false;
        // Use NoLock because we're already in an Engine write-lock if Engine::resetModule().
        // We also might be in the MIDIMap() constructor, which could cause problems, but when constructing, all ParamHandles will point to no Modules anyway.
        clearMaps_NoLock();
        mapLen = 1;
    }

    void processTerminalInput(const ProcessArgs& args) override
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

        if (isBypassed() || !divider.process())
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
            if (data[1] >= MAX_MIDI_CONTROL)
                continue;

            const uint8_t cc = data[1];
            const int8_t value = data[2];

            // Learn
            if (learningId >= 0 && values[cc] != value)
            {
                ccs[learningId] = cc;
                filterInitialized[cc] = false;
                valueFilters[learningId].reset();
                learnedCc = true;
                maybeCommitLearn();
                refreshParamHandleText(learningId);
                updateMapLen();
            }

            values[cc] = value;
        }

        ++midiEventFrame;

        // Step channels
        for (int id = 0; id < mapLen; ++id)
        {
            const int cc = ccs[id];
            if (cc < 0)
                continue;

            // Get Module
            Module* const module = paramHandles[id].module;
            if (!module)
                continue;

            // Get ParamQuantity from ParamHandle
            const int paramId = paramHandles[id].paramId;
            ParamQuantity* const paramQuantity = module->paramQuantities[paramId];
            if (!paramQuantity)
                continue;
            if (!paramQuantity->isBounded())
                continue;

            // Set filter from param value if filter is uninitialized
            if (!filterInitialized[id])
            {
                valueFilters[id].out = paramQuantity->getScaledValue();
                filterInitialized[id] = true;
                continue;
            }

            // Check if CC has been set by the MIDI device
            if (values[cc] < 0)
                continue;

            const float value = values[cc] / 127.f;

            // Detect behavior from MIDI buttons.
            if (smooth && std::fabs(valueFilters[id].out - value) < 1.f)
            {
                // Smooth value with filter
                if (d_isEqual(valueFilters[id].process(args.sampleTime * divider.getDivision(), value), value))
                {
                    values[cc] = -1;
                    continue;
                }
            }
            else
            {
                // Jump value
                if (d_isEqual(valueFilters[id].out, value))
                {
                    values[cc] = -1;
                    continue;
                }

                valueFilters[id].out = value;
            }

            paramQuantity->setScaledValue(valueFilters[id].out);
        }
    }

    void processTerminalOutput(const ProcessArgs&) override
    {}

    void clearMap(int id)
    {
        nextLearningId = -1;
        learningId = -1;
        learnedCc = false;
        learnedParam = false;

        ccs[id] = -1;
        values[id] = -1;
        pcontext->engine->updateParamHandle(&paramHandles[id], -1, 0, true);
        valueFilters[id].reset();
        refreshParamHandleText(id);
        updateMapLen();
    }

    // ----------------------------------------------------------------------------------------------------------------
    // stuff for resetting state

    void clearMaps_NoLock()
    {
        nextLearningId = -1;
        learningId = -1;
        learnedCc = false;
        learnedParam = false;

        for (int id = 0; id < MAX_MIDI_CONTROL; ++id)
        {
            ccs[id] = -1;
            values[id] = -1;
            pcontext->engine->updateParamHandle_NoLock(&paramHandles[id], -1, 0, true);
            valueFilters[id].reset();
            refreshParamHandleText(id);
        }
    }

    void setChannel(uint8_t channel)
    {
        this->channel = channel;

        for (int i = 0; i < MAX_MIDI_CONTROL; ++i)
            values[i] = -1;
    }

    // ----------------------------------------------------------------------------------------------------------------
    // stuff called from panel side, must lock engine

    // called from onSelect
    void enableLearn(const int id)
    {
        if (learningId == id)
            return;

        ccs[id] = -1;
        nextLearningId = -1;
        learningId = id;
        learnedCc = false;
        learnedParam = false;
    }

    // called from onDeselect
    void disableLearn(const int id)
    {
        nextLearningId = -1;

        if (learningId == id)
            learningId = -1;
    }

    // called from onDeselect
    void learnParam(const int id, const int64_t moduleId, const int paramId)
    {
        pcontext->engine->updateParamHandle(&paramHandles[id], moduleId, paramId, true);
        learnedParam = true;
        maybeCommitLearn();
        updateMapLen();
    }

    // ----------------------------------------------------------------------------------------------------------------
    // common utils

    void maybeCommitLearn()
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
        while (++learningId < MAX_MIDI_CONTROL)
        {
            if (ccs[learningId] < 0 || paramHandles[learningId].moduleId < 0)
            {
                nextLearningId = learningId;
                return;
            }
        }

        nextLearningId = learningId = -1;
    }

    // FIXME this allocates string during RT!!
    void refreshParamHandleText(const int id)
    {
        std::string text;
        if (ccs[id] >= 0)
            text = string::f("CC%02d", ccs[id]);
        else
            text = "MIDI-Map";
        paramHandles[id].text = text;
    }

    void updateMapLen()
    {
        // Find last nonempty map
        int id;
        for (id = MAX_MIDI_CONTROL; --id >= 0;)
        {
            if (ccs[id] >= 0 || paramHandles[id].moduleId >= 0)
                break;
        }

        mapLen = id + 1;

        // Add an empty "Mapping..." slot
        if (mapLen < MAX_MIDI_CONTROL)
            ++mapLen;
    }

    // ----------------------------------------------------------------------------------------------------------------
    // save and load json stuff

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
                if (mapIndex >= MAX_MIDI_CONTROL)
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

#ifndef HEADLESS
struct CardinalMIDIMapChoice : CardinalLedDisplayChoice {
    HostMIDIMap* const module;
    const int id;
    int disableLearnFrames = -1;
    ParamWidget* lastTouchedParam = nullptr;

    CardinalMIDIMapChoice(HostMIDIMap* const m, const int i)
      : CardinalLedDisplayChoice(),
        module(m),
        id(i)
    {
        alignTextCenter = false;

        // Module browser setup
        if (m == nullptr)
        {
            bgColor = nvgRGB(0, 0, 0);
            color.a = 0.75f;
            text = "Click here to map";
        }
    }

    void draw(const DrawArgs& args) override
    {
        if (bgColor.a > 0.0)
        {
            nvgBeginPath(args.vg);
            nvgRoundedRect(args.vg, 0, 0, box.size.x, box.size.y, 4);
            nvgFillColor(args.vg, bgColor);
            nvgFill(args.vg);
        }

        Widget::draw(args);
    }

    void step() override
    {
        if (module == nullptr)
            return;

        // Set bgColor and selected state
        if (module->learningId == id)
        {
            bgColor = color;
            bgColor.a = 0.125f;

            if (ParamWidget* const touchedParam = APP->scene->rack->touchedParam)
            {
                if (module->nextLearningId == id)
                {
                    module->nextLearningId = -1;
                    lastTouchedParam = touchedParam;
                }
                else if (lastTouchedParam != touchedParam)
                {
                    const int64_t moduleId = touchedParam->module->id;
                    const int paramId = touchedParam->paramId;
                    module->learnParam(id, moduleId, paramId);
                    lastTouchedParam = touchedParam;
                }
            }
            else
            {
                lastTouchedParam = nullptr;
            }
        }
        else
        {
            bgColor = nvgRGB(0, 0, 0);
        }

        // Set text
        text.clear();

        // mapped
        if (module->ccs[id] >= 0)
            text += string::f("CC%02d: ", module->ccs[id]);
        if (module->paramHandles[id].moduleId >= 0)
            text += getParamName();

        // Set text color
        if (text.empty() && module->learningId != id)
            color.a = 0.75f;
        else
            color.a = 1.0f;

        // unmapped
        if (text.empty())
        {
            if (module->learningId == id)
                text = "Mapping...";
            else
                text = module->mapLen == 1 ? "Click here to map" : "Unmapped";
        }
    }

    void onButton(const ButtonEvent& e) override
    {
        DISTRHO_SAFE_ASSERT_RETURN(module != nullptr,);

        e.stopPropagating();

        if (e.action != GLFW_PRESS)
            return;

        switch (e.button)
        {
        case GLFW_MOUSE_BUTTON_RIGHT:
            module->clearMap(id);
            e.consume(this);
            break;
            // fall-through
        case GLFW_MOUSE_BUTTON_LEFT:
            APP->scene->rack->touchedParam = lastTouchedParam = nullptr;
            module->enableLearn(id);
            e.consume(this);
            break;
        }
    }

    /*
    void onSelect(const SelectEvent& e) override
    {
        DISTRHO_SAFE_ASSERT_RETURN(module != nullptr,);

        ScrollWidget* scroll = getAncestorOfType<ScrollWidget>();
        scroll->scrollTo(box);
    }
    */

    std::string getParamName() const
    {
        DISTRHO_SAFE_ASSERT_RETURN(module != nullptr, "error");
        DISTRHO_SAFE_ASSERT_RETURN(id < module->mapLen, "error");

        ParamHandle* const paramHandle = &module->paramHandles[id];

        Module* const paramModule = paramHandle->module;
        DISTRHO_CUSTOM_SAFE_ASSERT_ONCE_RETURN("paramModule is null", paramModule != nullptr, "error");

        const int paramId = paramHandle->paramId;
        DISTRHO_CUSTOM_SAFE_ASSERT_ONCE_RETURN("paramId is out of bounds", paramId < (int) paramModule->params.size(), "error");

        ParamQuantity* const paramQuantity = paramModule->paramQuantities[paramId];
        std::string s = paramQuantity->name;
        if (s.empty())
            s = "Unnamed";
        s += " (";
        s += paramModule->model->name;
        s += ")";
        return s;
    }
};

struct HostMIDIMapDisplay : Widget {
    HostMIDIMap* module;
    ScrollWidget* scroll;
    CardinalMIDIMapChoice* choices[MAX_MIDI_CONTROL];
    LedDisplaySeparator* separators[MAX_MIDI_CONTROL];

    void drawLayer(const DrawArgs& args, int layer) override
    {
        nvgScissor(args.vg, RECT_ARGS(args.clipBox));
        Widget::drawLayer(args, layer);
        nvgResetScissor(args.vg);
    }

    void setModule(HostMIDIMap* const module)
    {
        this->module = module;

        scroll = new ScrollWidget;
        scroll->box.size = box.size;
        addChild(scroll);

        float posY = 0.0f;
        for (int id = 0; id < MAX_MIDI_CONTROL; ++id)
        {
            if (id != 0)
            {
                LedDisplaySeparator* separator = createWidget<LedDisplaySeparator>(Vec(0.0f, posY));
                separator->box.size = Vec(box.size.x, 1.0f);
                separator->visible = false;
                scroll->container->addChild(separator);
                separators[id] = separator;
            }

            CardinalMIDIMapChoice* const choice = new CardinalMIDIMapChoice(module, id);
            choice->box.pos = Vec(0.0f, posY);
            choice->box.size = Vec(box.size.x, 20.0f);
            choice->visible = id == 0;
            scroll->container->addChild(choice);
            choices[id] = choice;

            posY += choice->box.size.y;
        }
    }

    void step() override
    {
        if (module != nullptr)
        {
            const int mapLen = module->mapLen;

            for (int id = 1; id < MAX_MIDI_CONTROL; ++id)
            {
                separators[id]->visible = (id < mapLen);
                choices[id]->visible = (id < mapLen);
            }
        }

        Widget::step();
    }
};

struct HostMIDIMapWidget : ModuleWidget {
    HostMIDIMap* const module;

    HostMIDIMapWidget(HostMIDIMap* const m)
        : module(m)
    {
        setModule(m);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/HostMIDIMap.svg")));

        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        HostMIDIMapDisplay* const display = createWidget<HostMIDIMapDisplay>(Vec(1.0f, 71.0f));
        display->box.size = Vec(box.size.x - 2.0f, box.size.y - 89.0f);
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
                        [=]() {module->setChannel(c);}
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
#else
struct HostMIDIMapWidget : ModuleWidget {
    HostMIDIMapWidget(HostMIDIMap* const module) {
        setModule(module);
    }
};
#endif

// --------------------------------------------------------------------------------------------------------------------

Model* modelHostMIDIMap = createModel<HostMIDIMap, HostMIDIMapWidget>("HostMIDIMap");

// --------------------------------------------------------------------------------------------------------------------
