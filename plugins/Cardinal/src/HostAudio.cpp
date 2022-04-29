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
#include "ModuleWidgets.hpp"
#include "Widgets.hpp"

// -----------------------------------------------------------------------------------------------------------

USE_NAMESPACE_DISTRHO;

template<int numIO>
struct HostAudio : TerminalModule {
    CardinalPluginContext* const pcontext;
    const int numParams;
    const int numInputs;
    const int numOutputs;
    bool bypassed = false;
    bool in1connected = false;
    bool in2connected = false;
    uint32_t dataFrame = 0;
    uint32_t lastProcessCounter = 0;

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

    void processTerminalInput(const ProcessArgs&) override
    {
        const uint32_t bufferSize = pcontext->bufferSize;
        const uint32_t processCounter = pcontext->processCounter;

        // only checked on input
        if (lastProcessCounter != processCounter)
        {
            bypassed = isBypassed();
            dataFrame = 0;
            lastProcessCounter = processCounter;

            if (numIO == 2)
            {
                in1connected = inputs[0].isConnected();
                in2connected = inputs[1].isConnected();
            }
        }

        // only incremented on output
        const uint32_t k = dataFrame;
        DISTRHO_SAFE_ASSERT_INT2_RETURN(k < bufferSize, k, bufferSize,);

        // from host into cardinal, shows as output plug
        if (bypassed)
        {
            for (int i=0; i<numOutputs; ++i)
                outputs[i].setVoltage(0.0f);
        }
        else if (const float* const* const dataIns = pcontext->dataIns)
        {
            for (int i=0; i<numOutputs; ++i)
                outputs[i].setVoltage(dataIns[i][k] * 10.0f);
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

struct HostAudio2 : HostAudio<2> {
#ifndef HEADLESS
    // for stereo meter
    uint32_t internalDataFrame = 0;
    float internalDataBuffer[2][128];
    volatile bool resetMeters = true;
    float gainMeterL = 0.0f;
    float gainMeterR = 0.0f;
#endif

    HostAudio2()
        : HostAudio<2>()
    {
#ifndef HEADLESS
        std::memset(internalDataBuffer, 0, sizeof(internalDataBuffer));
#endif
    }

#ifndef HEADLESS
    void onReset() override
    {
        HostAudio<2>::onReset();
        resetMeters = true;
    }

    void onSampleRateChange(const SampleRateChangeEvent& e) override
    {
        HostAudio<2>::onSampleRateChange(e);
        resetMeters = true;
    }
#endif

    void processTerminalOutput(const ProcessArgs&) override
    {
        if (!in1connected && !in2connected)
            return;

        const uint32_t bufferSize = pcontext->bufferSize;

        // only incremented on output
        const uint32_t k = dataFrame++;
        DISTRHO_SAFE_ASSERT_INT2_RETURN(k < bufferSize, k, bufferSize,);

        if (bypassed)
            return;

        float** const dataOuts = pcontext->dataOuts;

        // gain (stereo variant only)
        const float gain = std::pow(params[0].getValue(), 2.f);

        // read stereo values
        float valueL, valueR;

        if (in1connected)
        {
            valueL = inputs[0].getVoltageSum() * 0.1f;

            if (dcFilterEnabled)
            {
                dcFilters[0].process(valueL);
                valueL = dcFilters[0].highpass();
            }

            valueL = clamp(valueL * gain, -1.0f, 1.0f);
            dataOuts[0][k] += valueL;
        }
        else
        {
            valueL = 0.0f;
        }

        if (in2connected)
        {
            valueR = inputs[1].getVoltageSum() * 0.1f;

            if (dcFilterEnabled)
            {
                dcFilters[1].process(valueR);
                valueR = dcFilters[1].highpass();
            }

            valueR = clamp(valueR * gain, -1.0f, 1.0f);
            dataOuts[1][k] += valueR;
        }
        else if (in1connected)
        {
            valueR = valueL;
            dataOuts[1][k] += valueL;
        }
#ifndef HEADLESS
        else
        {
            valueR = 0.0f;
        }

        const uint32_t j = internalDataFrame++;
        internalDataBuffer[0][j] = valueL;
        internalDataBuffer[1][j] = valueR;

        if (internalDataFrame == 128)
        {
            internalDataFrame = 0;

            if (resetMeters)
                gainMeterL = gainMeterR = 0.0f;

            gainMeterL = std::max(gainMeterL, d_findMaxNormalizedFloat(internalDataBuffer[0], 128));

            if (in2connected)
                gainMeterR = std::max(gainMeterR, d_findMaxNormalizedFloat(internalDataBuffer[1], 128));
            else
                gainMeterR = gainMeterL;

            resetMeters = false;
        }
#endif
    }
};

struct HostAudio8 : HostAudio<8> {
    // no meters in this variant

    void processTerminalOutput(const ProcessArgs&) override
    {
        const uint32_t bufferSize = pcontext->bufferSize;

        // only incremented on output
        const uint32_t k = dataFrame++;
        DISTRHO_SAFE_ASSERT_INT2_RETURN(k < bufferSize, k, bufferSize,);

        if (bypassed)
            return;

        float** const dataOuts = pcontext->dataOuts;

        for (int i=0; i<numInputs; ++i)
        {
            float v = inputs[i].getVoltageSum() * 0.1f;

            if (dcFilterEnabled)
            {
                dcFilters[i].process(v);
                v = dcFilters[i].highpass();
            }

            dataOuts[i][k] += clamp(v, -1.0f, 1.0f);
        }
    }

};

#ifndef HEADLESS
// --------------------------------------------------------------------------------------------------------------------

template<int numIO>
struct HostAudioWidget : ModuleWidgetWith8HP {
    HostAudio<numIO>* const module;

    HostAudioWidget(HostAudio<numIO>* const m)
        : module(m)
    {
        setModule(m);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/HostAudio.svg")));

        createAndAddScrews();

        for (uint i=0; i<numIO; ++i)
        {
            createAndAddInput(i);
            createAndAddOutput(i);
        }
    }

    void appendContextMenu(Menu* const menu) override {
        menu->addChild(new MenuSeparator);
        menu->addChild(createBoolPtrMenuItem("DC blocker", "", &module->dcFilterEnabled));
    }
};

// --------------------------------------------------------------------------------------------------------------------

struct HostAudioNanoMeter : NanoMeter {
    HostAudio2* const module;

    HostAudioNanoMeter(HostAudio2* const m)
        : module(m)
    {
        hasGainKnob = true;
    }

    void updateMeters() override
    {
        if (module == nullptr || module->resetMeters)
            return;

        // Only fetch new values once DSP side is updated
        gainMeterL = module->gainMeterL;
        gainMeterR = module->gainMeterR;
        module->resetMeters = true;
    }
};

// --------------------------------------------------------------------------------------------------------------------

struct HostAudioWidget2 : HostAudioWidget<2> {
    HostAudioWidget2(HostAudio2* const m)
        : HostAudioWidget<2>(m)
    {
        // FIXME
        const float middleX = box.size.x * 0.5f;
        addParam(createParamCentered<NanoKnob>(Vec(middleX, 310.0f), m, 0));

        HostAudioNanoMeter* const meter = new HostAudioNanoMeter(m);
        meter->box.pos = Vec(middleX - padding + 2.75f, startY + padding * 2);
        meter->box.size = Vec(padding * 2.0f - 4.0f, 136.0f);
        addChild(meter);
    }

    void draw(const DrawArgs& args) override
    {
        drawBackground(args.vg);
        drawOutputJacksArea(args.vg, 2);
        setupTextLines(args.vg);

        drawTextLine(args.vg, 0, "Left/M");
        drawTextLine(args.vg, 1, "Right");

        ModuleWidgetWith8HP::draw(args);
    }
};

struct HostAudioWidget8 : HostAudioWidget<8> {
    HostAudioWidget8(HostAudio8* const m)
        : HostAudioWidget<8>(m) {}

    void draw(const DrawArgs& args) override
    {
        drawBackground(args.vg);
        drawOutputJacksArea(args.vg, 8);
        setupTextLines(args.vg);

        for (int i=0; i<8; ++i)
        {
            char text[] = {'A','u','d','i','o',' ',static_cast<char>('0'+i+1),'\0'};
            drawTextLine(args.vg, i, text);
        }

        ModuleWidgetWith8HP::draw(args);
    }
};

#else
// --------------------------------------------------------------------------------------------------------------------

struct HostAudioWidget2 : ModuleWidget {
    HostAudioWidget2(HostAudio2* const module) {
        setModule(module);
        for (uint i=0; i<2; ++i) {
            addInput(createInput<PJ301MPort>({}, module, i));
            addOutput(createOutput<PJ301MPort>({}, module, i));
        }
    }
};
struct HostAudioWidget8 : ModuleWidget {
    HostAudioWidget8(HostAudio8* const module) {
        setModule(module);
        for (uint i=0; i<8; ++i) {
            addInput(createInput<PJ301MPort>({}, module, i));
            addOutput(createOutput<PJ301MPort>({}, module, i));
        }
    }
};

// --------------------------------------------------------------------------------------------------------------------
#endif

Model* modelHostAudio2 = createModel<HostAudio2, HostAudioWidget2>("HostAudio2");
Model* modelHostAudio8 = createModel<HostAudio8, HostAudioWidget8>("HostAudio8");

// --------------------------------------------------------------------------------------------------------------------
