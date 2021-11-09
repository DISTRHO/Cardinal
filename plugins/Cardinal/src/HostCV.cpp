/*
 * DISTRHO Cardinal Plugin
 * Copyright (C) 2021 Filipe Coelho <falktx@falktx.com>
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

#include "plugincontext.hpp"

// -----------------------------------------------------------------------------------------------------------

USE_NAMESPACE_DISTRHO;

struct HostCV : Module {
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
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam<SwitchQuantity>(BIPOLAR_INPUTS_1_5, 0.f, 1.f, 0.f, "Bipolar Inputs 1-5")->randomizeEnabled = false;
        configParam<SwitchQuantity>(BIPOLAR_INPUTS_6_10, 0.f, 1.f, 0.f, "Bipolar Inputs 6-10")->randomizeEnabled = false;
        configParam<SwitchQuantity>(BIPOLAR_OUTPUTS_1_5, 0.f, 1.f, 0.f, "Bipolar Outputs 1-5")->randomizeEnabled = false;
        configParam<SwitchQuantity>(BIPOLAR_OUTPUTS_6_10, 0.f, 1.f, 0.f, "Bipolar Outputs 6-10")->randomizeEnabled = false;

        CardinalPluginContext* const pcontext = reinterpret_cast<CardinalPluginContext*>(APP);

        if (pcontext == nullptr)
            throw rack::Exception("Plugin context is null.");

        if (pcontext->loadedHostCV)
            throw rack::Exception("Another instance of a Host CV module is already loaded, only one can be used at a time.");

        pcontext->loadedHostCV = true;
    }

    ~HostCV() override
    {
        CardinalPluginContext* const pcontext = reinterpret_cast<CardinalPluginContext*>(APP);
        DISTRHO_SAFE_ASSERT_RETURN(pcontext != nullptr,);

        pcontext->loadedHostCV = false;
    }

    void process(const ProcessArgs&) override
    {
        if (CardinalPluginContext* const pcontext = reinterpret_cast<CardinalPluginContext*>(APP))
        {
            const float** dataIns = pcontext->dataIns;
            float** dataOuts = pcontext->dataOuts;

            if (dataIns == nullptr || dataOuts == nullptr)
                return;

            const uint32_t dataFrame = pcontext->dataFrame++;
            float inputOffset, outputOffset;

            inputOffset = params[BIPOLAR_INPUTS_1_5].getValue() > 0.1f ? 5.0f : 0.0f;
            outputOffset = params[BIPOLAR_OUTPUTS_1_5].getValue() > 0.1f ? 5.0f : 0.0f;

            for (int i=0; i<5; ++i)
            {
                outputs[i].setVoltage(dataIns[i+2][dataFrame] - outputOffset);
                dataOuts[i+2][dataFrame] = inputs[i].getVoltage() + inputOffset;
            }

            inputOffset = params[BIPOLAR_INPUTS_6_10].getValue() > 0.1f ? 5.0f : 0.0f;
            outputOffset = params[BIPOLAR_OUTPUTS_6_10].getValue() > 0.1f ? 5.0f : 0.0f;

            for (int i=5; i<10; ++i)
            {
                outputs[i].setVoltage(dataIns[i+2][dataFrame] - outputOffset);
                dataOuts[i+2][dataFrame] = inputs[i].getVoltage() + inputOffset;
            }
        }
    }
};

struct HostCVWidget : ModuleWidget {
    static constexpr const float startX_In = 14.0f;
    static constexpr const float startX_Out = 96.0f;
    static constexpr const float startY = 74.0f;
    static constexpr const float padding = 29.0f;
    static constexpr const float middleX = startX_In + (startX_Out - startX_In) * 0.5f + padding * 0.25f;

    HostCVWidget(HostCV* const module)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/HostCV.svg")));

        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        for (uint i=0; i<HostCV::NUM_INPUTS; ++i)
            addInput(createInput<PJ301MPort>(Vec(startX_In, startY + padding * i), module, i));

        for (uint i=0; i<HostCV::NUM_OUTPUTS; ++i)
            addOutput(createOutput<PJ301MPort>(Vec(startX_Out, startY + padding * i), module, i));
    }

    void drawTextLine(NVGcontext* const vg, const uint offset, const char* const text)
    {
        const float y = startY + offset * padding;
        nvgBeginPath(vg);
        nvgFillColor(vg, color::WHITE);
        nvgText(vg, middleX, y + 16, text, nullptr);
    }

    void draw(const DrawArgs& args) override
    {
        nvgBeginPath(args.vg);
        nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
        nvgFillPaint(args.vg, nvgLinearGradient(args.vg, 0, 0, 0, box.size.y,
                                                nvgRGB(0x18, 0x19, 0x19), nvgRGB(0x21, 0x22, 0x22)));
        nvgFill(args.vg);

        nvgFontFaceId(args.vg, 0);
        nvgFontSize(args.vg, 11);
        nvgTextAlign(args.vg, NVG_ALIGN_CENTER);
        // nvgTextBounds(vg, 0, 0, text, nullptr, nullptr);

        nvgBeginPath(args.vg);
        nvgRoundedRect(args.vg, startX_Out - 4.0f, startY - 2.0f, padding, padding * HostCV::NUM_INPUTS, 4);
        nvgFillColor(args.vg, nvgRGB(0xd0, 0xd0, 0xd0));
        nvgFill(args.vg);

        drawTextLine(args.vg, 0, "CV 1");
        drawTextLine(args.vg, 1, "CV 2");
        drawTextLine(args.vg, 2, "CV 3");
        drawTextLine(args.vg, 3, "CV 4");
        drawTextLine(args.vg, 4, "CV 5");
        drawTextLine(args.vg, 5, "CV 6");
        drawTextLine(args.vg, 6, "CV 7");
        drawTextLine(args.vg, 7, "CV 8");
        drawTextLine(args.vg, 8, "CV 9");
        drawTextLine(args.vg, 9, "CV 10");

        ModuleWidget::draw(args);
    }

    void appendContextMenu(ui::Menu* const menu) override
    {
        menu->addChild(new ui::MenuSeparator);

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
};

// --------------------------------------------------------------------------------------------------------------------

Model* modelHostCV = createModel<HostCV, HostCVWidget>("HostCV");

// --------------------------------------------------------------------------------------------------------------------
