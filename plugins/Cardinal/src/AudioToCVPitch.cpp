/*
 * DISTRHO Cardinal Plugin
 * Copyright (C) 2021-2022 Bram Giesen
 * Copyright (C) 2022 Filipe Coelho <falktx@falktx.com>
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

extern "C" {
#include "aubio.h"
}

USE_NAMESPACE_DISTRHO;

// --------------------------------------------------------------------------------------------------------------------

// aubio setup values (tested under 48 kHz sample rate)
static constexpr const uint32_t kAubioHopSize = 1;
static constexpr const uint32_t kAubioBufferSize = (1024 + 256 + 128) / kAubioHopSize;

// default values
static constexpr const float kDefaultSensitivity = 50.f;
static constexpr const float kDefaultTolerance = 6.25f;
static constexpr const float kDefaultThreshold = 12.5f;

// static checks
static_assert(sizeof(smpl_t) == sizeof(float), "smpl_t is float");
static_assert(kAubioBufferSize % kAubioHopSize == 0, "kAubioBufferSize / kAubioHopSize has no remainder");

// --------------------------------------------------------------------------------------------------------------------

struct AudioToCVPitch : Module {
    enum ParamIds {
        PARAM_SENSITIVITY,
        PARAM_CONFIDENCETHRESHOLD,
        PARAM_TOLERANCE,
        PARAM_OCTAVE,
        NUM_PARAMS
    };
    enum InputIds {
        AUDIO_INPUT,
        NUM_INPUTS
    };
    enum OutputIds {
        CV_PITCH,
        CV_GATE,
        NUM_OUTPUTS
    };
    enum LightIds {
        NUM_LIGHTS
    };

    bool holdOutputPitch = true;
    bool smooth = true;
    int octave = 0;

    float lastKnownPitchInHz = 0.f;
    float lastKnownPitchConfidence = 0.f;

    float lastUsedTolerance = kDefaultTolerance;
    float lastUsedOutputPitch = 0.f;
    float lastUsedOutputSignal = 0.f;

    fvec_t* const detectedPitch = new_fvec(1);
    fvec_t* const inputBuffer = new_fvec(kAubioBufferSize);
    uint32_t inputBufferPos = 0;

    aubio_pitch_t* pitchDetector = nullptr;

    dsp::SlewLimiter smoothOutputSignal;

    AudioToCVPitch()
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        configInput(AUDIO_INPUT, "Audio");
        configOutput(CV_PITCH, "Pitch");
        configOutput(CV_GATE, "Gate");
        configParam(PARAM_SENSITIVITY, 0.1f, 99.f, kDefaultSensitivity, "Sensitivity", " %");
        configParam(PARAM_CONFIDENCETHRESHOLD, 0.f, 99.f, kDefaultThreshold, "Confidence Threshold", " %");
        configParam(PARAM_TOLERANCE, 0.f, 99.f,  kDefaultTolerance, "Tolerance", " %");
    }

    void process(const ProcessArgs& args) override
    {
        float cvPitch = lastUsedOutputPitch;
        float cvSignal = lastUsedOutputSignal;

        inputBuffer->data[inputBufferPos] = inputs[AUDIO_INPUT].getVoltage() * 0.1f
                                          * params[PARAM_SENSITIVITY].getValue();

        if (++inputBufferPos == kAubioBufferSize)
        {
            inputBufferPos = 0;

            const float tolerance = params[PARAM_TOLERANCE].getValue();
            if (d_isNotEqual(lastUsedTolerance, tolerance))
            {
                lastUsedTolerance = tolerance;
                aubio_pitch_set_tolerance(pitchDetector, tolerance * 0.01f);
            }

            aubio_pitch_do(pitchDetector, inputBuffer, detectedPitch);
            const float detectedPitchInHz = fvec_get_sample(detectedPitch, 0);
            const float pitchConfidence = aubio_pitch_get_confidence(pitchDetector);

            if (detectedPitchInHz > 0.f && pitchConfidence >=  params[PARAM_CONFIDENCETHRESHOLD].getValue() * 0.01f)
            {
                const float linearPitch = 12.f * (log2f(detectedPitchInHz / 440.f) + octave - 6) + 69.f;
                cvPitch = std::max(-10.f, std::min(10.f, linearPitch * (1.f/12.f)));
                lastKnownPitchInHz = detectedPitchInHz;
                cvSignal = 10.f;
            }
            else
            {
                if (! holdOutputPitch)
                    lastKnownPitchInHz = cvPitch = 0.0f;

                cvSignal = 0.f;
            }

            lastKnownPitchConfidence = pitchConfidence;
            lastUsedOutputPitch = cvPitch;
            lastUsedOutputSignal = cvSignal;
        }

        outputs[CV_PITCH].setVoltage(smooth ? smoothOutputSignal.process(args.sampleTime, cvPitch) : cvPitch);
        outputs[CV_GATE].setVoltage(cvSignal);
    }

    void onReset() override
    {
        inputBufferPos = 0;
        smooth = true;
        holdOutputPitch = true;
        octave = 0;
    }

    void onSampleRateChange(const SampleRateChangeEvent& e) override
    {
        float tolerance;

        if (pitchDetector != nullptr)
        {
            tolerance = aubio_pitch_get_tolerance(pitchDetector);
            del_aubio_pitch(pitchDetector);
        }
        else
        {
            tolerance = kDefaultTolerance * 0.01f;
        }

        pitchDetector = new_aubio_pitch("yinfast", kAubioBufferSize, kAubioHopSize, e.sampleRate);
        DISTRHO_SAFE_ASSERT_RETURN(pitchDetector != nullptr,);

        aubio_pitch_set_silence(pitchDetector, -30.0f);
        aubio_pitch_set_tolerance(pitchDetector, tolerance);
        aubio_pitch_set_unit(pitchDetector, "Hz");

        const double fall = 1.0 / (double(kAubioBufferSize) / e.sampleRate);
        smoothOutputSignal.reset();
        smoothOutputSignal.setRiseFall(fall, fall);
    }

    json_t* dataToJson() override
    {
        json_t* const rootJ = json_object();
        DISTRHO_SAFE_ASSERT_RETURN(rootJ != nullptr, nullptr);

        json_object_set_new(rootJ, "holdOutputPitch", json_boolean(holdOutputPitch));
        json_object_set_new(rootJ, "smooth", json_boolean(smooth));
        json_object_set_new(rootJ, "octave", json_integer(octave));

        return rootJ;
    }

    void dataFromJson(json_t* const rootJ) override
    {
        if (json_t* const holdOutputPitchJ = json_object_get(rootJ, "holdOutputPitch"))
            holdOutputPitch = json_boolean_value(holdOutputPitchJ);

        if (json_t* const smoothJ = json_object_get(rootJ, "smooth"))
            smooth = json_boolean_value(smoothJ);

        if (json_t* const octaveJ = json_object_get(rootJ, "octave"))
            octave = json_integer_value(octaveJ);
    }
};

struct SmallPercentageNanoKnob : NanoKnob<2, 0> {
    SmallPercentageNanoKnob() {
        box.size = Vec(32, 32);
        displayLabel = "";
    }

    void onChange(const ChangeEvent&) override
    {
        engine::ParamQuantity* const pq = getParamQuantity();
        DISTRHO_SAFE_ASSERT_RETURN(pq != nullptr,);

        displayString = string::f("%.1f %%", pq->getDisplayValue());
    }
};

struct AudioToCVPitchWidget : ModuleWidgetWith9HP {
    static constexpr const float startX = 10.0f;
    static constexpr const float startY_top = 71.0f;
    static constexpr const float startY_cv1 = 115.0f;
    static constexpr const float startY_cv2 = 145.0f;
    static constexpr const float padding = 32.0f;

    AudioToCVPitch* const module;
    std::string monoFontPath;

    AudioToCVPitchWidget(AudioToCVPitch* const m)
        : module(m)
    {
        setModule(m);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/AudioToCVPitch.svg")));
        monoFontPath = asset::system("res/fonts/ShareTechMono-Regular.ttf");

        createAndAddScrews();

        addInput(createInput<PJ301MPort>(Vec(startX, startY_cv1 + 0 * padding), m, AudioToCVPitch::AUDIO_INPUT));
        addOutput(createOutput<PJ301MPort>(Vec(startX, startY_cv2 + 0 * padding), m, AudioToCVPitch::CV_PITCH));
        addOutput(createOutput<PJ301MPort>(Vec(startX, startY_cv2 + 1 * padding), m, AudioToCVPitch::CV_GATE));

        SmallPercentageNanoKnob* knobSens = createParamCentered<SmallPercentageNanoKnob>(Vec(box.size.x * 0.5f, startY_cv2 + 85.f),
                                                                                         module, AudioToCVPitch::PARAM_SENSITIVITY);
        knobSens->displayString = "50 %";
        addChild(knobSens);

        SmallPercentageNanoKnob* knobTolerance = createParamCentered<SmallPercentageNanoKnob>(Vec(box.size.x * 0.5f, startY_cv2 + 135.f),
                                                                                              module, AudioToCVPitch::PARAM_TOLERANCE);
        knobTolerance->displayString = "6.25 %";
        addChild(knobTolerance);

        SmallPercentageNanoKnob* knobThres = createParamCentered<SmallPercentageNanoKnob>(Vec(box.size.x * 0.5f, startY_cv2 + 185.f),
                                                                                          module, AudioToCVPitch::PARAM_CONFIDENCETHRESHOLD);
        knobThres->displayString = "12.5 %";
        addChild(knobThres);
    }

    void drawInputLine(NVGcontext* const vg, const uint offset, const char* const text)
    {
        const float y = startY_cv1 + offset * padding;
        nvgBeginPath(vg);
        nvgFillColor(vg, nvgRGB(0xd0, 0xd0, 0xd0));
        nvgText(vg, startX + 28, y + 16, text, nullptr);
    }

    void drawOutputLine(NVGcontext* const vg, const uint offset, const char* const text)
    {
        const float y = startY_cv2 + offset * padding;
        nvgBeginPath(vg);
        nvgRoundedRect(vg, startX - 1.f, y - 2.f, box.size.x - startX * 2 + 2.f, 28.f, 4);
        nvgFillColor(vg, nvgRGB(0xd0, 0xd0, 0xd0));
        nvgFill(vg);
        nvgBeginPath(vg);
        nvgFillColor(vg, color::BLACK);
        nvgText(vg, startX + 28, y + 16, text, nullptr);
    }

    void draw(const DrawArgs& args) override
    {
        drawBackground(args.vg);

        nvgFontFaceId(args.vg, 0);
        nvgFontSize(args.vg, 14);

        drawInputLine(args.vg, 0, "Input");
        drawOutputLine(args.vg, 0, "Pitch");
        drawOutputLine(args.vg, 1, "Gate");

        nvgFontSize(args.vg, 11);
        nvgBeginPath(args.vg);
        nvgFillColor(args.vg, nvgRGB(0xd0, 0xd0, 0xd0));
        nvgTextLineHeight(args.vg, 0.8f);
        nvgTextAlign(args.vg, NVG_ALIGN_CENTER);
        nvgTextBox(args.vg, startX + 6.f, startY_cv2 + 75.f, 11.f, "S\ne\nn\ns", nullptr);
        nvgTextBox(args.vg, box.size.x - startX - 16.f, startY_cv2 + 130.f, 11.f, "T\no\nl", nullptr);
        nvgTextBox(args.vg, startX + 6.f, startY_cv2 + 175.f, 11.f, "T\nh\nr\ne\ns", nullptr);

        nvgBeginPath(args.vg);
        nvgRoundedRect(args.vg, 10.0f, startY_top, box.size.x - 20.f, 38.0f, 4);
        nvgFillColor(args.vg, color::BLACK);
        nvgFill(args.vg);

        ModuleWidgetWith9HP::draw(args);
    }

    void drawLayer(const DrawArgs& args, int layer) override
    {
        if (layer == 1)
        {
            nvgFontSize(args.vg, 17);
            nvgFillColor(args.vg, nvgRGBf(0.76f, 0.11f, 0.22f));

            char pitchConfString[24];
            char pitchFreqString[24];

            std::shared_ptr<Font> monoFont = APP->window->loadFont(monoFontPath);

            if (module != nullptr && monoFont != nullptr)
            {
                nvgFontFaceId(args.vg, monoFont->handle);

                std::snprintf(pitchConfString, sizeof(pitchConfString), "%5.1f %%", module->lastKnownPitchConfidence * 100.f);
                std::snprintf(pitchFreqString, sizeof(pitchFreqString), "%5.0f Hz", module->lastKnownPitchInHz);
            }
            else
            {
                std::strcpy(pitchConfString, "0.0 %");
                std::strcpy(pitchFreqString, "0 Hz");
            }

            nvgTextAlign(args.vg, NVG_ALIGN_CENTER);
            nvgText(args.vg, box.size.x * 0.5f, startY_top + 15.0f, pitchConfString, nullptr);
            nvgText(args.vg, box.size.x * 0.5f, startY_top + 33.0f, pitchFreqString, nullptr);
        }

        ModuleWidgetWith9HP::drawLayer(args, layer);
    }

    void appendContextMenu(Menu* const menu) override
    {
        menu->addChild(new MenuSeparator);

        menu->addChild(createBoolPtrMenuItem("Hold Output Pitch", "", &module->holdOutputPitch));
        menu->addChild(createBoolPtrMenuItem("Smooth Output Pitch", "", &module->smooth));

        static const std::vector<int> octaves = {-4, -3, -2, -1, 0, 1, 2, 3, 4};
        menu->addChild(createSubmenuItem("Octave", string::f("%d", module->octave), [=](Menu* menu) {
            for (size_t i = 0; i < octaves.size(); i++) {
                menu->addChild(createCheckMenuItem(string::f("%d", octaves[i]), "",
                    [=]() {return module->octave == octaves[i];},
                    [=]() {module->octave = octaves[i];}
                ));
            }
        }));
    }
};

// --------------------------------------------------------------------------------------------------------------------

Model* modelAudioToCVPitch = createModel<AudioToCVPitch, AudioToCVPitchWidget>("AudioToCVPitch");

// --------------------------------------------------------------------------------------------------------------------
