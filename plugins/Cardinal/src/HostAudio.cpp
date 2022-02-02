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

/*
 * Find the highest absolute and normalized value within a float array.
 */
static inline
float d_findMaxNormalizedFloat(const float floats[], const std::size_t count)
{
    DISTRHO_SAFE_ASSERT_RETURN(floats != nullptr, 0.0f);
    DISTRHO_SAFE_ASSERT_RETURN(count > 0, 0.0f);

    static const float kEmptyFloats[8192] = {};

    if (count <= 8192 && std::memcmp(floats, kEmptyFloats, count) == 0)
        return 0.0f;

    float tmp, maxf2 = std::abs(floats[0]);

    for (std::size_t i=1; i<count; ++i)
    {
        tmp = std::abs(*floats++);

        if (tmp > maxf2)
            maxf2 = tmp;
    }

    if (maxf2 > 1.0f)
        maxf2 = 1.0f;

    return maxf2;
}

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
    volatile bool resetMeters = true;
    float gainMeterL = 0.0f;
    float gainMeterR = 0.0f;

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
        resetMeters = true;
    }

    void onSampleRateChange(const SampleRateChangeEvent& e) override
    {
        resetMeters = true;

        for (int i=0; i<numIO; ++i)
            dcFilters[i].setCutoffFreq(10.f * e.sampleTime);
    }

    void process(const ProcessArgs&) override
    {
        const float* const* const dataIns = pcontext->dataIns;
        float** const dataOuts = pcontext->dataOuts;

        const int blockFrames = pcontext->engine->getBlockFrames();
        const int64_t blockFrame = pcontext->engine->getBlockFrame();

        if (lastBlockFrame != blockFrame)
        {
            dataFrame = 0;
            lastBlockFrame = blockFrame;
        }

        const int k = dataFrame++;
        DISTRHO_SAFE_ASSERT_INT2_RETURN(k < blockFrames, k, blockFrames,);

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

        if (numInputs == 2)
        {
            const bool connected = inputs[1].isConnected();

            if (! connected)
                dataOuts[1][k] += dataOuts[0][k];

            if (dataFrame == blockFrames)
            {
                if (resetMeters)
                    gainMeterL = gainMeterR = 0.0f;

                gainMeterL = std::max(gainMeterL, d_findMaxNormalizedFloat(dataOuts[0], blockFrames));

                if (connected)
                    gainMeterR = std::max(gainMeterR, d_findMaxNormalizedFloat(dataOuts[1], blockFrames));
                else
                    gainMeterR = gainMeterL;

                resetMeters = false;
            }
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
struct HostAudioNanoMeter : NanoMeter {
    HostAudio<numIO>* const module;

    HostAudioNanoMeter(HostAudio<numIO>* const m)
        : module(m) {}

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

template<int numIO>
struct HostAudioWidget : ModuleWidgetWith8HP {
    HostAudio<numIO>* const module;

    HostAudioWidget(HostAudio<numIO>* const m)
        : module(m)
    {
        setModule(m);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/HostAudio.svg")));
        setSideScrews();

        for (uint i=0; i<numIO; ++i)
        {
            createAndAddInput(i);
            createAndAddOutput(i);
        }

        if (numIO == 2)
        {
            addParam(createParamCentered<NanoKnob>(Vec(middleX, 310.0f), m, 0));

            HostAudioNanoMeter<numIO>* const meter = new HostAudioNanoMeter<numIO>(m);
            meter->box.pos = Vec(middleX - padding + 2.75f, startY + padding * 2);
            meter->box.size = Vec(padding * 2.0f - 4.0f, 136.0f);
            addChild(meter);
        }
    }

    void draw(const DrawArgs& args) override
    {
        drawBackground(args.vg);
        drawOutputJacksArea(args.vg, numIO);
        setupTextLines(args.vg);

        if (numIO == 2)
        {
            drawTextLine(args.vg, 0, "Left/M");
            drawTextLine(args.vg, 1, "Right");
        }
        else
        {
            for (int i=0; i<numIO; ++i)
            {
                char text[] = {'A','u','d','i','o',' ',static_cast<char>('0'+i+1),'\0'};
                drawTextLine(args.vg, i, text);
            }
        }

        ModuleWidgetWith8HP::draw(args);
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
