/*
 * DISTRHO Cardinal Plugin
 * Copyright (C) 2021-2024 Filipe Coelho <falktx@falktx.com>
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

#include "plugin.hpp"
#include "plugincontext.hpp"
#include "ModuleWidgets.hpp"
#include "engine/TerminalModule.hpp"

// -----------------------------------------------------------------------------------------------------------

struct HostParameters : TerminalModule {
    enum ParamIds {
        NUM_PARAMS
    };
    enum InputIds {
        NUM_INPUTS
    };
    enum OutputIds {
        NUM_OUTPUTS = kModuleParameterCount
    };
    enum LightIds {
        NUM_LIGHTS
    };

    CardinalPluginContext* const pcontext;
    rack::dsp::SlewLimiter parameters[kModuleParameterCount];
    bool parametersConnected[kModuleParameterCount] = {};
    bool bypassed = false;
    bool smooth = true;
    uint32_t lastProcessCounter = 0;

    HostParameters()
        : pcontext(static_cast<CardinalPluginContext*>(APP))
    {
        if (pcontext == nullptr)
            throw rack::Exception("Plugin context is null.");

        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    }

    void processTerminalInput(const ProcessArgs& args) override
    {
        const uint32_t processCounter = pcontext->processCounter;

        if (lastProcessCounter != processCounter)
        {
            bypassed = isBypassed();
            lastProcessCounter = processCounter;

            for (uint32_t i=0; i<kModuleParameterCount; ++i)
            {
                const bool connected = outputs[i].isConnected();

                if (parametersConnected[i] != connected)
                {
                    parametersConnected[i] = connected;
                    parameters[i].reset();
                }
            }
        }

        if (bypassed)
            return;

        for (uint32_t i=0; i<kModuleParameterCount; ++i)
        {
            if (parametersConnected[i])
                outputs[i].setVoltage(smooth ? parameters[i].process(args.sampleTime, pcontext->parameters[i])
                                             : pcontext->parameters[i]);
        }
    }

    void processTerminalOutput(const ProcessArgs&) override
    {}

    void onSampleRateChange(const SampleRateChangeEvent& e) override
    {
        const double fall = 1.0 / (double(pcontext->bufferSize) / e.sampleRate);

        for (uint32_t i=0; i<kModuleParameterCount; ++i)
        {
            parameters[i].reset();
            parameters[i].setRiseFall(fall, fall);
        }
    }

    // ----------------------------------------------------------------------------------------------------------------
    // save and load json stuff

    json_t* dataToJson() override
    {
        json_t* const rootJ = json_object();
        DISTRHO_SAFE_ASSERT_RETURN(rootJ != nullptr, nullptr);

        json_object_set_new(rootJ, "smooth", json_boolean(smooth));

        return rootJ;
    }

    void dataFromJson(json_t* const rootJ) override
    {
        if (json_t* const smoothJ = json_object_get(rootJ, "smooth"))
            smooth = json_boolean_value(smoothJ);
    }
};

// --------------------------------------------------------------------------------------------------------------------

#ifndef HEADLESS
struct CardinalParameterPJ301MPort : PJ301MPort {
	void onDragStart(const DragStartEvent& e) override {
        if (const CardinalPluginContext* const pcontext = static_cast<CardinalPluginContext*>(APP))
            handleHostParameterDrag(pcontext, portId, true);
        PJ301MPort::onDragStart(e);
    }
	void onDragEnd(const DragEndEvent& e) override {
        if (const CardinalPluginContext* const pcontext = static_cast<CardinalPluginContext*>(APP))
            handleHostParameterDrag(pcontext, portId, false);
        PJ301MPort::onDragEnd(e);
    }
};

struct HostParametersWidget : ModuleWidgetWith9HP {
    static constexpr const float startX = 10.0f;
    static constexpr const float startY = 90.0f;
    static constexpr const float paddingH = 30.0f;
    static constexpr const float paddingV = 49.0f;

    HostParameters* const module;

    HostParametersWidget(HostParameters* const m)
        : module(m)
    {
        setModule(m);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/HostParameters.svg")));
        createAndAddScrews();

        for (int i=0; i<24; ++i)
        {
            const float x = startX + int(i / 6) * paddingH;
            const float y = startY + int(i % 6) * paddingV;
            addOutput(createOutput<CardinalParameterPJ301MPort>(Vec(x, y), m, i));
        }
    }

    void draw(const DrawArgs& args) override
    {
        drawBackground(args.vg);

        nvgFontFaceId(args.vg, 0);
        nvgFontSize(args.vg, 14);

        char text[] = { '0', '0', '\0' };
        for (int i=0; i<24; ++i)
        {
            const float x = startX + int(i / 6) * paddingH;
            const float y = startY + int(i % 6) * paddingV;
            nvgBeginPath(args.vg);
            nvgRoundedRect(args.vg, x - 1.0f, y - 19.0f, paddingH - 4.0f, paddingV - 4.0f, 4);
            nvgFillColor(args.vg, nvgRGB(0xd0, 0xd0, 0xd0));
            nvgFill(args.vg);

            if (text[1]++ == '9')
            {
                text[1] = '0';
                ++text[0];
            }
            nvgBeginPath(args.vg);
            nvgFillColor(args.vg, color::BLACK);
            nvgText(args.vg, x + 4.0f, y - 4.0f, text, nullptr);
        }

        ModuleWidgetWith9HP::draw(args);
    }

    void appendContextMenu(Menu* const menu) override
    {
        menu->addChild(new MenuSeparator);
        menu->addChild(createBoolPtrMenuItem("Smooth", "", &module->smooth));
    }
};
#else
struct HostParametersWidget : ModuleWidget {
    HostParametersWidget(HostParameters* const module) {
        setModule(module);
        for (uint i=0; i<HostParameters::NUM_OUTPUTS; ++i)
            addOutput(createOutput<PJ301MPort>({}, module, i));
    }
};
#endif

// --------------------------------------------------------------------------------------------------------------------

Model* modelHostParameters = createModel<HostParameters, HostParametersWidget>("HostParameters");

// --------------------------------------------------------------------------------------------------------------------
