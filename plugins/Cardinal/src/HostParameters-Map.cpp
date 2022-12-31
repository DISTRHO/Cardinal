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
 * This file contains portions from VCVRack's core/MIDIMap.cpp
 * Copyright (C) 2016-2021 VCV.
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 */

#include "plugincontext.hpp"
#include "ModuleWidgets.hpp"
#include "Widgets.hpp"

// -----------------------------------------------------------------------------------------------------------

static constexpr const uint8_t MAX_MAPPED_PARAMS = 64;

struct HostParameterMapping {
    uint8_t hostParamId = UINT8_MAX;
    bool inverted = false;
    bool smooth = true;
    ParamHandle paramHandle;
};

struct HostParametersMap : TerminalModule {
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

    HostParameterMapping mappings[MAX_MAPPED_PARAMS];
    dsp::ExponentialFilter valueFilters[MAX_MAPPED_PARAMS];
    bool filterInitialized[MAX_MAPPED_PARAMS] = {};
    bool valueReached[MAX_MAPPED_PARAMS] = {};

    uint8_t numMappedParmeters = 1;
    uint8_t learningId = UINT8_MAX;

    CardinalPluginContext* const pcontext;
    bool parametersChanged[kModuleParameterCount] = {};
    float parameterValues[kModuleParameterCount];
    bool bypassed = false;
    bool firstRun = true;
    uint32_t lastProcessCounter = 0;

    HostParametersMap()
        : pcontext(static_cast<CardinalPluginContext*>(APP))
    {
        if (pcontext == nullptr)
            throw rack::Exception("Plugin context is null.");

        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        for (uint8_t id = 0; id < MAX_MAPPED_PARAMS; ++id)
        {
            mappings[id].paramHandle.color = nvgRGBf(0.76f, 0.11f, 0.22f);
            valueFilters[id].setTau(1 / 30.f);
            pcontext->engine->addParamHandle(&mappings[id].paramHandle);
        }

        std::memcpy(parameterValues, pcontext->parameters, sizeof(parameterValues));
    }

    ~HostParametersMap()
    {
        if (pcontext == nullptr)
            return;

        for (uint8_t id = 0; id < MAX_MAPPED_PARAMS; ++id)
            pcontext->engine->removeParamHandle(&mappings[id].paramHandle);
    }

    void onReset() override
    {
        lastProcessCounter = 0;

        // Use NoLock because we're already in an Engine write-lock if Engine::resetModule().
        // We also might be in the MIDIMap() constructor, which could cause problems, but when constructing, all ParamHandles will point to no Modules anyway.
        clearMaps_NoLock();
        numMappedParmeters = 1;
    }

    void clearMaps_NoLock()
    {
        learningId = UINT8_MAX;

        for (uint8_t id = 0; id < MAX_MAPPED_PARAMS; ++id)
        {
            pcontext->engine->updateParamHandle_NoLock(&mappings[id].paramHandle, -1, 0, true);
            valueReached[id] = false;
            valueFilters[id].reset();
            mappings[id].hostParamId = UINT8_MAX;
        }

        firstRun = true;
        std::memcpy(parameterValues, pcontext->parameters, sizeof(parameterValues));
        std::memset(parametersChanged, 0, sizeof(parametersChanged));
    }

    void processTerminalInput(const ProcessArgs& args) override
    {
        const uint32_t processCounter = pcontext->processCounter;

        if (lastProcessCounter == processCounter)
            return;

        lastProcessCounter = processCounter;

        if (isBypassed())
            return;

        for (uint32_t i = 0; i < kModuleParameterCount; ++i)
        {
            if (d_isEqual(pcontext->parameters[i], parameterValues[i]))
                continue;

            parameterValues[i] = pcontext->parameters[i];
            parametersChanged[i] = true;
        }

        for (uint id = 0; id < numMappedParmeters; ++id)
        {
            ParamHandle& paramHandle(mappings[id].paramHandle);

            if (paramHandle.module == nullptr)
                continue;

            // Get ParamQuantity from ParamHandle
            const int paramId = paramHandle.paramId;
            ParamQuantity* const paramQuantity = paramHandle.module->paramQuantities[paramId];
            if (!paramQuantity)
                continue;
            if (!paramQuantity->isBounded())
                continue;

            // Validate hostParamId
            const uint8_t hostParamId = mappings[id].hostParamId;
            if (hostParamId >= kModuleParameterCount)
                continue;

            // Set filter from param value if filter is uninitialized
            if (!filterInitialized[id])
            {
                valueFilters[id].out = paramQuantity->getScaledValue();
                filterInitialized[id] = true;
                continue;
            }

            // Check if parameter was changed by the host
            if (parametersChanged[hostParamId] && !firstRun)
                valueReached[id] = false;
            else if (valueReached[id])
                continue;

            // Apply value, smooth as needed.
            const float value = 0.1f * (mappings[id].inverted ? 10.f - parameterValues[hostParamId]
                                                              : parameterValues[hostParamId]);

            if (mappings[id].smooth && std::fabs(valueFilters[id].out - value) < 1.f)
            {
                // Smooth value with filter
                if (d_isEqual(valueFilters[id].process(args.sampleTime * pcontext->bufferSize, value), value))
                {
                    valueReached[id] = true;
                    continue;
                }
            }
            else
            {
                // Jump value
                if (d_isEqual(valueFilters[id].out, value))
                {
                    valueReached[id] = true;
                    continue;
                }

                valueFilters[id].out = value;
            }

            paramQuantity->setScaledValue(valueFilters[id].out);
        }

        firstRun = false;
        std::memset(parametersChanged, 0, sizeof(parametersChanged));
    }

    void processTerminalOutput(const ProcessArgs&) override
    {}

    // ----------------------------------------------------------------------------------------------------------------
    // save and load json stuff

    json_t* dataToJson() override
    {
        json_t* const rootJ = json_object();
        DISTRHO_SAFE_ASSERT_RETURN(rootJ != nullptr, nullptr);

        if (json_t* const mapsJ = json_array())
        {
            for (uint id = 0; id < numMappedParmeters; ++id)
            {
                json_t* const mapJ = json_object();
                DISTRHO_SAFE_ASSERT_CONTINUE(mapJ != nullptr);
                json_object_set_new(mapJ, "hostParamId", json_integer(mappings[id].hostParamId));
                json_object_set_new(mapJ, "inverted", json_boolean(mappings[id].inverted));
                json_object_set_new(mapJ, "smooth", json_boolean(mappings[id].smooth));
                json_object_set_new(mapJ, "moduleId", json_integer(mappings[id].paramHandle.moduleId));
                json_object_set_new(mapJ, "paramId", json_integer(mappings[id].paramHandle.paramId));
                json_array_append_new(mapsJ, mapJ);
            }
            json_object_set_new(rootJ, "maps", mapsJ);
        }

        return rootJ;
    }

    void dataFromJson(json_t* const rootJ) override
    {
        // Use NoLock because we're already in an Engine write-lock.
        clearMaps_NoLock();

        if (json_t* const mapsJ = json_object_get(rootJ, "maps"))
        {
            json_t* mapJ;
            size_t id;
            json_array_foreach(mapsJ, id, mapJ)
            {
                if (id >= MAX_MAPPED_PARAMS)
                    break;
                json_t* const hostParamIdJ = json_object_get(mapJ, "hostParamId");
                json_t* const invertedJ = json_object_get(mapJ, "inverted");
                json_t* const smoothJ = json_object_get(mapJ, "smooth");
                json_t* const moduleIdJ = json_object_get(mapJ, "moduleId");
                json_t* const paramIdJ = json_object_get(mapJ, "paramId");
                if (hostParamIdJ == nullptr)
                    continue;
                if (invertedJ == nullptr)
                    continue;
                if (smoothJ == nullptr)
                    continue;
                if (moduleIdJ == nullptr)
                    continue;
                if (paramIdJ == nullptr)
                    continue;

                filterInitialized[id] = false;
                valueReached[id] = true;
                valueFilters[id].reset();
                mappings[id].hostParamId = json_integer_value(hostParamIdJ);
                mappings[id].inverted = json_boolean_value(invertedJ);
                mappings[id].smooth = json_boolean_value(smoothJ);
                pcontext->engine->updateParamHandle_NoLock(&mappings[id].paramHandle,
                                                           json_integer_value(moduleIdJ),
                                                           json_integer_value(paramIdJ),
                                                           false);
            }
        }

        updateMapLen();
    }

    // ----------------------------------------------------------------------------------------------------------------
    // stuff called from panel side

    void clearMap(const uint8_t id)
    {
        learningId = UINT8_MAX;

        mappings[id].hostParamId = UINT8_MAX;
        pcontext->engine->updateParamHandle(&mappings[id].paramHandle, -1, 0, true);
        updateMapLen();
    }

    void learnParam(const uint8_t id, const int64_t moduleId, const int paramId)
    {
        DISTRHO_SAFE_ASSERT_UINT2_RETURN(id == learningId, id, learningId,);
        DISTRHO_SAFE_ASSERT_RETURN(id < MAX_MAPPED_PARAMS,);

        // reset for next time
        learningId = UINT8_MAX;

        // reset smoothing filters
        filterInitialized[id] = false;
        valueFilters[id].reset();
        valueReached[id] = true;

        // report mapping change to engine if needed
        if (mappings[id].paramHandle.moduleId != moduleId || mappings[id].paramHandle.paramId != paramId)
            pcontext->engine->updateParamHandle(&mappings[id].paramHandle, moduleId, paramId, true);

        updateMapLen();
    }

    void updateMapLen()
    {
        // Find last nonempty map
        int16_t id;
        for (id = MAX_MAPPED_PARAMS; --id >= 0;)
        {
            if (mappings[id].paramHandle.moduleId >= 0)
                break;
        }

        numMappedParmeters = static_cast<uint8_t>(id + 1);

        // Add an empty "Mapping..." slot
        if (numMappedParmeters < MAX_MAPPED_PARAMS)
            ++numMappedParmeters;
    }
};

// --------------------------------------------------------------------------------------------------------------------

#ifndef HEADLESS
struct ParameterIndexQuantity : Quantity {
    HostParameterMapping& mapping;
    float v;

    ParameterIndexQuantity(HostParameterMapping& m)
      : mapping(m),
        v(mapping.hostParamId) {}

    float getMinValue() override {
        return 0;
    }
    float getMaxValue() override {
        return kModuleParameterCount - 1;
    }
    float getDefaultValue() override {
        return 0;
    }
    float getValue() override {
        return v;
    }
    void setValue(float value) override {
        v = math::clamp(value, getMinValue(), getMaxValue());
        mapping.hostParamId = math::clamp(static_cast<int>(v + 0.5f), 0, kModuleParameterCount - 1);
    }
    float getDisplayValue() override {
        return mapping.hostParamId + 1;
    }
    void setDisplayValue(float displayValue) override {
        setValue(displayValue - 1);
	}
    std::string getLabel() override {
        return "Host Parameter";
    }
};

struct ParameterIndexSlider : ui::Slider {
	ParameterIndexSlider(HostParameterMapping& m) {
		quantity = new ParameterIndexQuantity(m);
	}
	~ParameterIndexSlider() {
		delete quantity;
	}
};

static HostParameterMapping& getDummyHostParameterMapping()
{
    static HostParameterMapping mapping;
    return mapping;
}

struct HostParametersMapChoice : CardinalLedDisplayChoice {
    HostParametersMap* const module;
    const uint8_t id;
    HostParameterMapping& mapping;

    HostParametersMapChoice(HostParametersMap* const m, const uint8_t i)
      : CardinalLedDisplayChoice(),
        module(m),
        id(i),
        mapping(m != nullptr ? m->mappings[i] : getDummyHostParameterMapping())
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
        if (bgColor.a > 0.0f)
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
                APP->scene->rack->touchedParam = nullptr;
                DISTRHO_SAFE_ASSERT_RETURN(mapping.hostParamId < kModuleParameterCount,);

                const int64_t moduleId = touchedParam->module->id;
                const int paramId = touchedParam->paramId;
                module->learnParam(id, moduleId, paramId);
            }
        }
        else
        {
            bgColor = nvgRGB(0, 0, 0);
        }

        // Set text
        text.clear();

        // mapped
        if (module->mappings[id].hostParamId < kModuleParameterCount)
            text += string::f("P%02d: ", module->mappings[id].hostParamId + 1);
        if (module->mappings[id].paramHandle.moduleId >= 0)
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
                text = module->numMappedParmeters == 1 ? "Click here to map" : "Unmapped";
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
            APP->scene->rack->touchedParam = nullptr;
            module->clearMap(id);
            e.consume(this);
            break;
        case GLFW_MOUSE_BUTTON_LEFT:
            APP->scene->rack->touchedParam = nullptr;
            e.consume(this);
            createMappingMenu();
            break;
        }
    }

    std::string getParamName() const
    {
        DISTRHO_SAFE_ASSERT_RETURN(module != nullptr, "error");
        DISTRHO_SAFE_ASSERT_RETURN(id < module->numMappedParmeters, "error");

        const ParamHandle& paramHandle(module->mappings[id].paramHandle);

        Module* const paramModule = paramHandle.module;
        DISTRHO_CUSTOM_SAFE_ASSERT_ONCE_RETURN("paramModule is null", paramModule != nullptr, "error");

        const int paramId = paramHandle.paramId;
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

    void createMappingMenu()
    {
        ui::Menu* const menu = createMenu();
        menu->cornerFlags = BND_CORNER_TOP;
        menu->box.pos = getAbsoluteOffset(math::Vec(0, box.size.y));

        if (mapping.hostParamId == UINT8_MAX)
        {
            mapping.hostParamId = 0;
            module->learningId = id;
        }

        ParameterIndexSlider* const paramIndexSlider = new ParameterIndexSlider(mapping);
        paramIndexSlider->box.size.x = RACK_GRID_WIDTH * 11;
        menu->addChild(paramIndexSlider);

        menu->addChild(createBoolPtrMenuItem("Inverted", "", &mapping.inverted));
        menu->addChild(createBoolPtrMenuItem("Smooth", "", &mapping.smooth));
    }
};

struct HostParametersMapDisplay : Widget {
    HostParametersMap* module;
    ScrollWidget* scroll;
    HostParametersMapChoice* choices[MAX_MAPPED_PARAMS];
    LedDisplaySeparator* separators[MAX_MAPPED_PARAMS];

    void drawLayer(const DrawArgs& args, int layer) override
    {
        nvgScissor(args.vg, RECT_ARGS(args.clipBox));
        Widget::drawLayer(args, layer);
        nvgResetScissor(args.vg);
    }

    void setModule(HostParametersMap* const module)
    {
        this->module = module;

        scroll = new ScrollWidget;
        scroll->box.size = box.size;
        addChild(scroll);

        float posY = 0.0f;
        for (uint8_t id = 0; id < MAX_MAPPED_PARAMS; ++id)
        {
            if (id != 0)
            {
                LedDisplaySeparator* separator = createWidget<LedDisplaySeparator>(Vec(0.0f, posY));
                separator->box.size = Vec(box.size.x, 1.0f);
                separator->visible = false;
                scroll->container->addChild(separator);
                separators[id] = separator;
            }

            HostParametersMapChoice* const choice = new HostParametersMapChoice(module, id);
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
            const uint8_t numMappedParmeters = module->numMappedParmeters;

            for (uint8_t id = 1; id < MAX_MAPPED_PARAMS; ++id)
            {
                separators[id]->visible = (id < numMappedParmeters);
                choices[id]->visible = (id < numMappedParmeters);
            }
        }

        Widget::step();
    }
};

struct HostParametersMapWidget : ModuleWidgetWith11HP {
    HostParametersMap* const module;

    HostParametersMapWidget(HostParametersMap* const m)
        : module(m)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/HostParamsMap.svg")));
        createAndAddScrews();

        HostParametersMapDisplay* const display = createWidget<HostParametersMapDisplay>(Vec(1.0f, 71.0f));
        display->box.size = Vec(box.size.x - 2.0f, box.size.y - 89.0f);
        display->setModule(m);
        addChild(display);
    }

    void draw(const DrawArgs& args) override
    {
        drawBackground(args.vg);
        ModuleWidgetWith11HP::draw(args);
    }
};
#else
struct HostParametersMapWidget : ModuleWidget {
    HostParametersMapWidget(HostParametersMap* const module) {
        setModule(module);
    }
};
#endif

// --------------------------------------------------------------------------------------------------------------------

Model* modelHostParametersMap = createModel<HostParametersMap, HostParametersMapWidget>("HostParametersMap");

// --------------------------------------------------------------------------------------------------------------------
