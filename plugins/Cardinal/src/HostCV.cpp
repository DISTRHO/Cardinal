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

USE_NAMESPACE_DISTRHO;

struct HostCV : TerminalModule {
    CardinalPluginContext* const pcontext;
    bool bypassed = false;
    int dataFrame = 0;
    uint32_t lastProcessCounter = 0;

    enum ParamIds {
        BIPOLAR_INPUTS_1_5,
        BIPOLAR_INPUTS_6_10,
        BIPOLAR_OUTPUTS_1_5,
        BIPOLAR_OUTPUTS_6_10,
        NUM_PARAMS
    };
    enum InputIds {
        NUM_INPUTS = 10
    };
    enum OutputIds {
        NUM_OUTPUTS = 10
    };
    enum LightIds {
        NUM_LIGHTS
    };

    HostCV()
        : pcontext(static_cast<CardinalPluginContext*>(APP))
    {
        if (pcontext == nullptr)
            throw rack::Exception("Plugin context is null");

        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        if (pcontext->variant == kCardinalVariantMini)
        {
            configParam<SwitchQuantity>(BIPOLAR_INPUTS_1_5, 0.f, 1.f, 0.f, "Bipolar Inputs")->randomizeEnabled = false;
            configParam<SwitchQuantity>(BIPOLAR_OUTPUTS_1_5, 0.f, 1.f, 0.f, "Bipolar Outputs")->randomizeEnabled = false;
        }
        else
        {
            configParam<SwitchQuantity>(BIPOLAR_INPUTS_1_5, 0.f, 1.f, 0.f, "Bipolar Inputs 1-5")->randomizeEnabled = false;
            configParam<SwitchQuantity>(BIPOLAR_OUTPUTS_1_5, 0.f, 1.f, 0.f, "Bipolar Outputs 1-5")->randomizeEnabled = false;
        }

        configParam<SwitchQuantity>(BIPOLAR_INPUTS_6_10, 0.f, 1.f, 0.f, "Bipolar Inputs 6-10")->randomizeEnabled = false;
        configParam<SwitchQuantity>(BIPOLAR_OUTPUTS_6_10, 0.f, 1.f, 0.f, "Bipolar Outputs 6-10")->randomizeEnabled = false;
    }

    void processTerminalInput(const ProcessArgs&) override
    {
        if (pcontext->variant != kCardinalVariantMain && pcontext->variant != kCardinalVariantMini)
            return;

        const uint8_t ioOffset = pcontext->variant == kCardinalVariantMini ? 2 : 8;
        const uint32_t bufferSize = pcontext->bufferSize;
        const uint32_t processCounter = pcontext->processCounter;

        // only checked on input
        if (lastProcessCounter != processCounter)
        {
            bypassed = isBypassed();
            dataFrame = 0;
            lastProcessCounter = processCounter;
        }

        // only incremented on output
        const uint32_t k = dataFrame;
        DISTRHO_SAFE_ASSERT_RETURN(k < bufferSize,);

        if (bypassed)
        {
            for (int i=0; i<10; ++i)
                outputs[i].setVoltage(0.0f);
        }
        else if (const float* const* const dataIns = pcontext->dataIns)
        {
            if (dataIns[ioOffset] == nullptr)
                return;

            float outputOffset;
            outputOffset = params[BIPOLAR_OUTPUTS_1_5].getValue() > 0.1f ? 5.f : 0.f;

            for (int i=0; i<5; ++i)
                outputs[i].setVoltage(dataIns[i+ioOffset][k] - outputOffset);

            if (pcontext->variant == kCardinalVariantMain)
            {
                outputOffset = params[BIPOLAR_OUTPUTS_6_10].getValue() > 0.1f ? 5.f : 0.f;

                for (int i=5; i<10; ++i)
                    outputs[i].setVoltage(dataIns[i+ioOffset][k] - outputOffset);
            }
            else
            {
                for (int i=5; i<10; ++i)
                    outputs[i].setVoltage(0.f);
            }
        }
    }

    void processTerminalOutput(const ProcessArgs&) override
    {
        if (pcontext->variant != kCardinalVariantMain && pcontext->variant != kCardinalVariantMini)
            return;
        if (pcontext->bypassed)
            return;

        const uint8_t ioOffset = pcontext->variant == kCardinalVariantMini ? 2 : 8;
        const uint32_t bufferSize = pcontext->bufferSize;

        // only incremented on output
        const uint32_t k = dataFrame++;
        DISTRHO_SAFE_ASSERT_RETURN(k < bufferSize,);

        if (bypassed)
            return;

        float** const dataOuts = pcontext->dataOuts;

        if (dataOuts[ioOffset] == nullptr)
            return;

        float inputOffset;
        inputOffset = params[BIPOLAR_INPUTS_1_5].getValue() > 0.1f ? 5.0f : 0.0f;

        for (int i=0; i<5; ++i)
        {
            if (!std::isfinite(dataOuts[i+ioOffset][k]))
                __builtin_unreachable();
            dataOuts[i+ioOffset][k] += inputs[i].getVoltage() + inputOffset;
        }

        if (pcontext->variant == kCardinalVariantMain)
        {
            inputOffset = params[BIPOLAR_INPUTS_6_10].getValue() > 0.1f ? 5.0f : 0.0f;

            for (int i=5; i<10; ++i)
            {
                if (!std::isfinite(dataOuts[i+ioOffset][k]))
                    __builtin_unreachable();
                dataOuts[i+ioOffset][k] += inputs[i].getVoltage() + inputOffset;
            }
        }
    }
};

#ifndef HEADLESS
struct HostCVWidget : ModuleWidgetWith8HP {
    CardinalPluginContext* const pcontext;

    HostCVWidget(HostCV* const module)
        : pcontext(static_cast<CardinalPluginContext*>(APP))
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/HostCV.svg")));

        createAndAddScrews();

        uint8_t maxVisible;
        switch (pcontext->variant)
        {
        case kCardinalVariantMain:
            maxVisible = HostCV::NUM_INPUTS;
            break;
        case kCardinalVariantMini:
            maxVisible = 5;
            break;
        default:
            maxVisible = 0;
            break;
        }

        for (uint i=0; i<HostCV::NUM_INPUTS; ++i)
            createAndAddInput(i, i, i<maxVisible);

        for (uint i=0; i<HostCV::NUM_OUTPUTS; ++i)
            createAndAddOutput(i, i, i<maxVisible);
    }

    void draw(const DrawArgs& args) override
    {
        drawBackground(args.vg);

        if (pcontext->variant == kCardinalVariantMain || pcontext->variant == kCardinalVariantMini)
        {
            drawOutputJacksArea(args.vg, pcontext->variant == kCardinalVariantMini ? 5 : HostCV::NUM_INPUTS);
            setupTextLines(args.vg);

            switch (pcontext->variant)
            {
            case kCardinalVariantMain:
                drawTextLine(args.vg, 5, "CV 6");
                drawTextLine(args.vg, 6, "CV 7");
                drawTextLine(args.vg, 7, "CV 8");
                drawTextLine(args.vg, 8, "CV 9");
                drawTextLine(args.vg, 9, "CV 10");
                // fall through
            case kCardinalVariantMini:
                drawTextLine(args.vg, 0, "CV 1");
                drawTextLine(args.vg, 1, "CV 2");
                drawTextLine(args.vg, 2, "CV 3");
                drawTextLine(args.vg, 3, "CV 4");
                drawTextLine(args.vg, 4, "CV 5");
                break;
            default:
                break;
            }
        }

        ModuleWidgetWith8HP::draw(args);
    }

    void appendContextMenu(ui::Menu* const menu) override
    {
        menu->addChild(new ui::MenuSeparator);

        if (pcontext->variant == kCardinalVariantMini)
        {
            menu->addChild(createCheckMenuItem("Bipolar Inputs", "",
                [=]() {return module->params[HostCV::BIPOLAR_INPUTS_1_5].getValue() > 0.1f;},
                [=]() {module->params[HostCV::BIPOLAR_INPUTS_1_5].setValue(1.0f - module->params[HostCV::BIPOLAR_INPUTS_1_5].getValue());}
            ));

            menu->addChild(createCheckMenuItem("Bipolar Outputs", "",
                [=]() {return module->params[HostCV::BIPOLAR_OUTPUTS_1_5].getValue() > 0.1f;},
                [=]() {module->params[HostCV::BIPOLAR_OUTPUTS_1_5].setValue(1.0f - module->params[HostCV::BIPOLAR_OUTPUTS_1_5].getValue());}
            ));
        }
        else
        {
            menu->addChild(createCheckMenuItem("Bipolar Inputs 1-5", "",
                [=]() {return module->params[HostCV::BIPOLAR_INPUTS_1_5].getValue() > 0.1f;},
                [=]() {module->params[HostCV::BIPOLAR_INPUTS_1_5].setValue(1.0f - module->params[HostCV::BIPOLAR_INPUTS_1_5].getValue());}
            ));

            menu->addChild(createCheckMenuItem("Bipolar Inputs 6-10", "",
                [=]() {return module->params[HostCV::BIPOLAR_INPUTS_6_10].getValue() > 0.1f;},
                [=]() {module->params[HostCV::BIPOLAR_INPUTS_6_10].setValue(1.0f - module->params[HostCV::BIPOLAR_INPUTS_6_10].getValue());}
            ));

            menu->addChild(createCheckMenuItem("Bipolar Outputs 1-5", "",
                [=]() {return module->params[HostCV::BIPOLAR_OUTPUTS_1_5].getValue() > 0.1f;},
                [=]() {module->params[HostCV::BIPOLAR_OUTPUTS_1_5].setValue(1.0f - module->params[HostCV::BIPOLAR_OUTPUTS_1_5].getValue());}
            ));

            menu->addChild(createCheckMenuItem("Bipolar Outputs 6-10", "",
                [=]() {return module->params[HostCV::BIPOLAR_OUTPUTS_6_10].getValue() > 0.1f;},
                [=]() {module->params[HostCV::BIPOLAR_OUTPUTS_6_10].setValue(1.0f - module->params[HostCV::BIPOLAR_OUTPUTS_6_10].getValue());}
            ));
        }
    }
};
#else
struct HostCVWidget : ModuleWidget {
    HostCVWidget(HostCV* const module) {
        setModule(module);
        for (uint i=0; i<HostCV::NUM_INPUTS; ++i)
            addInput(createInput<PJ301MPort>({}, module, i));
        for (uint i=0; i<HostCV::NUM_OUTPUTS; ++i)
            addOutput(createOutput<PJ301MPort>({}, module, i));
    }
};
#endif

// --------------------------------------------------------------------------------------------------------------------

Model* modelHostCV = createModel<HostCV, HostCVWidget>("HostCV");

// --------------------------------------------------------------------------------------------------------------------
