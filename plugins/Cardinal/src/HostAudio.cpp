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

struct NanoKnob : Knob {
    static const int ringSize = 4;

    std::string displayLabel = "Level";
    std::string displayString = "0 dB";
    float normalizedValue = 0.5f;

    NanoKnob()
    {
        box.size = Vec(100, 100);
    }

    void drawLayer(const DrawArgs& args, int layer) override
    {
        if (layer != 1)
            return Knob::drawLayer(args, layer);

        const float w = box.size.x;
        const float h = box.size.y;

        const int knobSize = std::min(w, h - BND_WIDGET_HEIGHT * 2) - ringSize;

        const int knobStartX = w / 2 - knobSize / 2;
        const int knobStartY = ringSize;
        const int knobCenterX = knobStartX + knobSize / 2;
        const int knobCenterY = knobStartY + knobSize / 2;

        const NVGcolor testing = nvgRGBf(0.76f, 0.11f, 0.22f);

        nvgLineCap(args.vg, NVG_ROUND);

        // outer ring value
        nvgBeginPath(args.vg);
        nvgArc(args.vg,
              knobCenterX,
              knobCenterY,
              knobSize / 2 + ringSize / 2 + 1,
              nvgDegToRad(135.0f),
              nvgDegToRad(135.0f) + nvgDegToRad(270.0f * normalizedValue),
              NVG_CW);
        nvgStrokeWidth(args.vg, ringSize);
        nvgStrokeColor(args.vg, testing);
        nvgStroke(args.vg);

        // simulate color bleeding
        nvgBeginPath(args.vg);
        nvgArc(args.vg,
              knobCenterX,
              knobCenterY,
              knobSize / 2 - 3,
              nvgDegToRad(135.0f),
              nvgDegToRad(135.0f) + nvgDegToRad(270.0f * normalizedValue),
              NVG_CW);
        nvgStrokeWidth(args.vg, 5);
        nvgStrokeColor(args.vg, nvgRGBAf(testing.r, testing.g, testing.b, 0.1f));
        nvgStroke(args.vg);

        // line indicator
        nvgStrokeWidth(args.vg, 2);
        nvgSave(args.vg);
        nvgTranslate(args.vg, knobCenterX, knobCenterY);
        nvgRotate(args.vg, nvgDegToRad(45.0f) + normalizedValue * nvgDegToRad(270.0f));
        nvgBeginPath(args.vg);
        nvgRoundedRect(args.vg, -2, knobSize / 2 - 9, 2, 6, 1);
        nvgClosePath(args.vg);
        nvgFillColor(args.vg, nvgRGBf(1.0f, 1.0f, 1.0f));
        nvgFill(args.vg);
        nvgRestore(args.vg);

        // adjusted from VCVRack's LightWidget.cpp
        if (const float halo = settings::haloBrightness)
        {
            float radius = knobSize * 0.5f;
            float oradius = radius + std::min(radius * 4.f, 15.f);

            NVGcolor icol = color::mult(nvgRGBAf(testing.r, testing.g, testing.b, 0.2f), halo);
            NVGcolor ocol = nvgRGBA(0, 0, 0, 0);
            NVGpaint paint = nvgRadialGradient(args.vg, knobCenterX, knobCenterY, radius, oradius, icol, ocol);

            nvgBeginPath(args.vg);
            nvgRect(args.vg, knobCenterX - oradius, knobCenterY - oradius, 2 * oradius, 2 * oradius);
            nvgFillPaint(args.vg, paint);
            nvgFill(args.vg);
        }

        // bottom label (value)
        bndIconLabelValue(args.vg, 0, knobSize + ringSize, w, BND_WIDGET_HEIGHT, -1,
                          testing, BND_CENTER,
                          BND_LABEL_FONT_SIZE, displayString.c_str(), nullptr);

        Knob::drawLayer(args, layer);
    }

    void draw(const DrawArgs& args) override
    {
        if (engine::ParamQuantity* const pq = getParamQuantity())
            normalizedValue = pq->getScaledValue();

        const float w = box.size.x;
        const float h = box.size.y;

        const int knobSize = std::min(w, h - BND_WIDGET_HEIGHT * 2) - ringSize;

        const int knobStartX = w / 2 - knobSize / 2;
        const int knobStartY = ringSize;
        const int knobCenterX = knobStartX + knobSize / 2;
        const int knobCenterY = knobStartY + knobSize / 2;

        // knob
        NVGcolor shade_top;
        NVGcolor shade_down;
        BNDwidgetState state;
        if (APP->event->getDraggedWidget() == this)
            state = BND_ACTIVE;
        else if (APP->event->getHoveredWidget() == this)
            state = BND_HOVER;
        else
            state = BND_DEFAULT;
        bndInnerColors(&shade_top, &shade_down, &bndGetTheme()->optionTheme, state, 0);

        // inner fill
        nvgBeginPath(args.vg);
        nvgCircle(args.vg, knobCenterX, knobCenterY, knobSize / 2);
        nvgFillPaint(args.vg, nvgLinearGradient(args.vg,
                                            knobStartX,
                                            knobStartY,
                                            knobStartX,
                                            knobStartY + knobSize,
                                            shade_top,
                                            shade_down));
        nvgFill(args.vg);

        // inner fill border (inner)
        nvgBeginPath(args.vg);
        nvgArc(args.vg, knobCenterX, knobCenterY, knobSize / 2 - 1, nvgDegToRad(0.0f), nvgDegToRad(360.0f), NVG_CCW);
        nvgClosePath(args.vg);
        nvgStrokeWidth(args.vg, 1);
        nvgStrokeColor(args.vg, nvgRGBAf(0.5f, 0.5f, 0.5f, 0.4f));
        nvgStroke(args.vg);

        // inner fill border (outer)
        nvgBeginPath(args.vg);
        nvgArc(args.vg, knobCenterX, knobCenterY, knobSize / 2, nvgDegToRad(0.0f), nvgDegToRad(360.0f), NVG_CCW);
        nvgClosePath(args.vg);
        nvgStrokeWidth(args.vg, 1);
        nvgStrokeColor(args.vg, nvgRGBAf(0.0f, 0.0f, 0.0f, 0.4f));
        nvgStroke(args.vg);

        nvgLineCap(args.vg, NVG_ROUND);

        // outer ring background
        nvgBeginPath(args.vg);
        nvgArc(args.vg,
              knobCenterX,
              knobCenterY,
              knobSize / 2 + ringSize / 2 + 1,
              nvgDegToRad(135.0f),
              nvgDegToRad(45.0f),
              NVG_CW);
        nvgStrokeWidth(args.vg, ringSize);
        nvgStrokeColor(args.vg, nvgRGBf(0.5f, 0.5f, 0.5f));
        nvgStroke(args.vg);

        // bottom label (name)
        bndIconLabelValue(args.vg, 0, knobStartY + knobSize + BND_WIDGET_HEIGHT * 0.75f, w, BND_WIDGET_HEIGHT, -1,
                          SCHEME_WHITE, BND_CENTER,
                          BND_LABEL_FONT_SIZE, displayLabel.c_str(), nullptr);

        Knob::draw(args);
    }

    void onChange(const ChangeEvent&) override
    {
        engine::ParamQuantity* const pq = getParamQuantity();
        DISTRHO_SAFE_ASSERT_RETURN(pq != nullptr,);

        displayLabel = pq->getLabel();
        displayString = pq->getDisplayValueString() + pq->getUnit();
    }
};

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
                gainMeterL = std::max(gainMeterL, d_findMaxNormalizedFloat(dataOuts[0], blockFrames));

                if (connected)
                    gainMeterR = std::max(gainMeterR, d_findMaxNormalizedFloat(dataOuts[1], blockFrames));
                else
                    gainMeterR = gainMeterL;
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
struct NanoMeter : Widget {
    HostAudio<numIO>* const module;

    NanoMeter(HostAudio<numIO>* const m)
        : module(m)
    {
        box.size = Vec(100, 100);
    }

    void drawLayer(const DrawArgs& args, int layer) override
    {
        if (layer != 1)
            return;

        const float usableHeight = box.size.y - 10.0f;

        // draw background
        nvgBeginPath(args.vg);
        nvgRect(args.vg,
                0,
                0,
                box.size.x,
                usableHeight);
        nvgFillColor(args.vg, nvgRGB(26, 26, 26));
        nvgFill(args.vg);

        nvgFillColor(args.vg, nvgRGBAf(0.76f, 0.11f, 0.22f, 0.5f));
        nvgStrokeColor(args.vg, nvgRGBf(0.76f, 0.11f, 0.22f));

        if (module != nullptr)
        {
            float gainMeterL = 0.0f;
            float gainMeterR = 0.0f;
            std::swap(gainMeterL, module->gainMeterL);
            std::swap(gainMeterR, module->gainMeterR);

            const float heightL = 1.0f + std::sqrt(gainMeterL) * (usableHeight - 1.0f);
            nvgBeginPath(args.vg);
            nvgRect(args.vg, 0.0f, usableHeight - heightL, box.size.x * 0.5f - 1.0f, heightL);
            nvgFill(args.vg);
            nvgStroke(args.vg);

            const float heightR = 1.0f + std::sqrt(gainMeterR) * (usableHeight - 1.0f);
            nvgBeginPath(args.vg);
            nvgRect(args.vg, box.size.x * 0.5f + 1.0f, usableHeight - heightR, box.size.x * 0.5f - 2.0f, heightR);
            nvgFill(args.vg);
            nvgStroke(args.vg);
        }

        nvgLineCap(args.vg, NVG_ROUND);

        nvgBeginPath(args.vg);
        nvgMoveTo(args.vg, 0.0f, usableHeight + 2.0f);
        nvgLineTo(args.vg, box.size.x * 0.5f - 11.0f, usableHeight + 2.0f);
        nvgStroke(args.vg);

        nvgBeginPath(args.vg);
        nvgMoveTo(args.vg, 0.0f, usableHeight + 4.0f);
        nvgLineTo(args.vg, box.size.x * 0.5f - 16.0f, usableHeight + 4.0f);
        nvgStroke(args.vg);

        nvgBeginPath(args.vg);
        nvgMoveTo(args.vg, 0.0f, usableHeight + 6.0f);
        nvgLineTo(args.vg, box.size.x * 0.5f - 19.0f, usableHeight + 6.0f);
        nvgStroke(args.vg);

        nvgBeginPath(args.vg);
        nvgMoveTo(args.vg, 0.0f, usableHeight + 8.0f);
        nvgLineTo(args.vg, box.size.x * 0.5f - 22.0f, usableHeight + 8.0f);
        nvgStroke(args.vg);

        nvgBeginPath(args.vg);
        nvgMoveTo(args.vg, 0.0f, usableHeight + 10.0f);
        nvgLineTo(args.vg, box.size.x * 0.5f - 24.0f, usableHeight + 10.0f);
        nvgStroke(args.vg);

        nvgBeginPath(args.vg);
        nvgMoveTo(args.vg, 0.0f, usableHeight + 12.0f);
        nvgLineTo(args.vg, box.size.x * 0.5f - 26.0f, usableHeight + 12.0f);
        nvgStroke(args.vg);

        nvgBeginPath(args.vg);
        nvgMoveTo(args.vg, box.size.x * 0.5f + 10.0f, usableHeight + 2.0f);
        nvgLineTo(args.vg, box.size.x - 1.0f, usableHeight + 2.0f);
        nvgStroke(args.vg);

        nvgBeginPath(args.vg);
        nvgMoveTo(args.vg, box.size.x * 0.5f + 15.0f, usableHeight + 4.0f);
        nvgLineTo(args.vg, box.size.x - 1.0f, usableHeight + 4.0f);
        nvgStroke(args.vg);

        nvgBeginPath(args.vg);
        nvgMoveTo(args.vg, box.size.x * 0.5f + 18.0f, usableHeight + 6.0f);
        nvgLineTo(args.vg, box.size.x - 1.0f, usableHeight + 6.0f);
        nvgStroke(args.vg);

        nvgBeginPath(args.vg);
        nvgMoveTo(args.vg, box.size.x * 0.5f + 20.0f, usableHeight + 8.0f);
        nvgLineTo(args.vg, box.size.x - 1.0f, usableHeight + 8.0f);
        nvgStroke(args.vg);

        nvgBeginPath(args.vg);
        nvgMoveTo(args.vg, box.size.x * 0.5f + 22.0f, usableHeight + 10.0f);
        nvgLineTo(args.vg, box.size.x - 1.0f, usableHeight + 10.0f);
        nvgStroke(args.vg);

        nvgBeginPath(args.vg);
        nvgMoveTo(args.vg, box.size.x * 0.5f + 24.0f, usableHeight + 12.0f);
        nvgLineTo(args.vg, box.size.x - 1.0f, usableHeight + 12.0f);
        nvgStroke(args.vg);
    }
};

template<int numIO>
struct HostAudioWidget : ModuleWidget {
    static constexpr const float startX_In = 14.0f;
    static constexpr const float startX_Out = 81.0f;
    static constexpr const float startY = 74.0f;
    static constexpr const float padding = 29.0f;
    static constexpr const float middleX = startX_In + (startX_Out - startX_In) * 0.5f + padding * 0.35f;

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
            addParam(createParamCentered<NanoKnob>(Vec(middleX, 310.0f), m, 0));

            NanoMeter<numIO>* const meter = new NanoMeter<numIO>(m);
            meter->box.pos = Vec(middleX - padding + 2.75f, startY + padding * 2);
            meter->box.size = Vec(padding * 2.0f - 4.0f, 136.0f);
            addChild(meter);
        }
    }

    void drawTextLine(NVGcontext* const vg, const uint posY, const char* const text)
    {
        const float y = startY + posY * padding;
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

        nvgBeginPath(args.vg);
        nvgRoundedRect(args.vg, startX_Out - 2.5f, startY - 2.0f, padding, padding * numIO, 4);
        nvgFillColor(args.vg, nvgRGB(0xd0, 0xd0, 0xd0));
        nvgFill(args.vg);

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
