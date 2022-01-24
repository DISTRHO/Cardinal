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

#include "plugincontext.hpp"

// -----------------------------------------------------------------------------------------------------------

USE_NAMESPACE_DISTRHO;

template<int numIO>
struct HostAudio : Module {
    CardinalPluginContext* const pcontext;
    const int numParams;
    const int numInputs;
    const int numOutputs;
    int dataFrame = 0;
    int64_t lastBlockFrame = -1;

    // for rack core audio module compatibility
    dsp::RCFilter dcFilters[numIO];
    bool dcFilterEnabled = (numIO == 2);

    HostAudio()
        : pcontext(static_cast<CardinalPluginContext*>(APP)),
          numParams(numIO == 2 ? 1 : 0),
          numInputs(pcontext->variant == kCardinalVariantMain ? numIO : 2),
          numOutputs(pcontext->variant == kCardinalVariantSynth ? 0 : pcontext->variant == kCardinalVariantMain ? numIO : 2)
    {
        if (pcontext == nullptr)
            throw rack::Exception("Plugin context is null");

        config(numParams, numIO, numIO, 0);

        if (numParams != 0)
            configParam(0, 0.f, 2.f, 1.f, "Level", " dB", -10, 40);

        const float sampleTime = pcontext->engine->getSampleTime();
        for (int i=0; i<numIO; ++i)
            dcFilters[i].setCutoffFreq(10.f * sampleTime);
    }

    void onReset() override
    {
        dcFilterEnabled = (numIO == 2);
    }

    void onSampleRateChange(const SampleRateChangeEvent& e) override
    {
        for (int i=0; i<numIO; ++i)
            dcFilters[i].setCutoffFreq(10.f * e.sampleTime);
    }

    void process(const ProcessArgs&) override
    {
        const float* const* const dataIns = pcontext->dataIns;
        float** const dataOuts = pcontext->dataOuts;

        const int64_t blockFrame = pcontext->engine->getBlockFrame();

        if (lastBlockFrame != blockFrame)
        {
            dataFrame = 0;
            lastBlockFrame = blockFrame;
        }

        const int k = dataFrame++;
        DISTRHO_SAFE_ASSERT_RETURN(k < pcontext->engine->getBlockFrames(),);

        const float gain = numParams != 0 ? std::pow(params[0].getValue(), 2.f) : 1.0f;

        // from host into cardinal, shows as output plug
        if (dataIns != nullptr)
        {
            for (int i=0; i<numOutputs; ++i)
                outputs[i].setVoltage(dataIns[i][k] * 10.0f);
        }

        // from cardinal into host, shows as input plug
        for (int i=0; i<numInputs; ++i)
        {
            float v = inputs[i].getVoltageSum() * 0.1f;

            // Apply DC filter
            if (dcFilterEnabled)
            {
                dcFilters[i].process(v);
                v = dcFilters[i].highpass();
            }

            dataOuts[i][k] += clamp(v * gain, -1.0f, 1.0f);
        }
    }

    json_t* dataToJson() override
    {
        json_t* const rootJ = json_object();
        DISTRHO_SAFE_ASSERT_RETURN(rootJ != nullptr, nullptr);

        json_object_set_new(rootJ, "dcFilter", json_boolean(dcFilterEnabled));
        return rootJ;
    }

    void dataFromJson(json_t* const rootJ) override
    {
        json_t* const dcFilterJ = json_object_get(rootJ, "dcFilter");
        DISTRHO_SAFE_ASSERT_RETURN(dcFilterJ != nullptr,);

        dcFilterEnabled = json_boolean_value(dcFilterJ);
    }
};

template<int numIO>
struct HostAudioWidget : ModuleWidget {
    static constexpr const float startX_In = 14.0f;
    static constexpr const float startX_Out = 96.0f;
    static constexpr const float startY = 74.0f;
    static constexpr const float padding = 29.0f;
    static constexpr const float middleX = startX_In + (startX_Out - startX_In) * 0.5f + padding * 0.25f;

    HostAudio<numIO>* const module;

    HostAudioWidget(HostAudio<numIO>* const m)
        : module(m)
    {
        setModule(m);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/HostAudio.svg")));

        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        for (uint i=0; i<numIO; ++i)
            addInput(createInput<PJ301MPort>(Vec(startX_In, startY + padding * i), m, i));

        for (uint i=0; i<numIO; ++i)
            addOutput(createOutput<PJ301MPort>(Vec(startX_Out, startY + padding * i), m, i));

        if (numIO == 2)
        {
            addParam(createParamCentered<RoundLargeBlackKnob>(Vec(middleX, 290.0f), m, 0));
        }
    }

    void drawTextLine(NVGcontext* const vg, const float offsetX, const uint posY, const char* const text)
    {
        const float y = startY + posY * padding;
        nvgBeginPath(vg);
        nvgFillColor(vg, color::WHITE);
        nvgText(vg, middleX + offsetX, y + 16, text, nullptr);
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

        nvgBeginPath(args.vg);
        nvgRoundedRect(args.vg, startX_Out - 4.0f, startY - 2.0f, padding, padding * numIO, 4);
        nvgFillColor(args.vg, nvgRGB(0xd0, 0xd0, 0xd0));
        nvgFill(args.vg);

        if (numIO == 2)
        {
            drawTextLine(args.vg, 3.0f, 0, "Left/Mono");
            drawTextLine(args.vg, 0.0f, 1, "Right");
        }
        else
        {
            for (int i=0; i<numIO; ++i)
            {
                char text[] = {'A','u','d','i','o',' ',static_cast<char>('0'+i+1),'\0'};
                drawTextLine(args.vg, 0.0f, i, text);
            }
        }

        ModuleWidget::draw(args);
    }

    void appendContextMenu(Menu* const menu) override {
        menu->addChild(new MenuSeparator);
        menu->addChild(createBoolPtrMenuItem("DC blocker", "", &module->dcFilterEnabled));
    }
};

// --------------------------------------------------------------------------------------------------------------------

Model* modelHostAudio2 = createModel<HostAudio<2>, HostAudioWidget<2>>("HostAudio2");
Model* modelHostAudio8 = createModel<HostAudio<8>, HostAudioWidget<8>>("HostAudio8");

// --------------------------------------------------------------------------------------------------------------------
