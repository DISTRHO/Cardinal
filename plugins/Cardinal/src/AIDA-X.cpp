/*
 * AIDA-X Cardinal plugin
 * Copyright (C) 2022-2023 Massimo Pennazio <maxipenna@libero.it>
 * Copyright (C) 2023 Filipe Coelho <falktx@falktx.com>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "plugincontext.hpp"
#include "ModuleWidgets.hpp"

#include "extra/Sleep.hpp"

#include "AIDA-X/Biquad.cpp"
#include "AIDA-X/model_variant.hpp"

#ifndef HEADLESS
# include "ImGuiWidget.hpp"
# include "ghc/filesystem.hpp"
#endif

template class RTNeural::Model<float>;
template class RTNeural::Layer<float>;

// --------------------------------------------------------------------------------------------------------------------

/* Define a constexpr for converting a gain in dB to a coefficient */
static constexpr float DB_CO(const float g) { return g > -90.f ? std::pow(10.f, g * 0.05f) : 0.f; }

/* Define a macro to re-maps a number from one range to another  */
static constexpr float MAP(const float x, const float in_min, const float in_max, const float out_min, const float out_max)
{
    return ((x - in_min) * (out_max - out_min) / (in_max - in_min)) + out_min;
}

/* Defines for tone controls */
static constexpr const float COMMON_Q = 0.707f;
static constexpr const float DEPTH_FREQ = 75.f;
static constexpr const float PRESENCE_FREQ = 900.f;

/* Defines for antialiasing filter */
static constexpr const float INLPF_MAX_CO = 0.99f * 0.5f; /* coeff * ((samplerate / 2) / samplerate) */
static constexpr const float INLPF_MIN_CO = 0.25f * 0.5f; /* coeff * ((samplerate / 2) / samplerate) */

// --------------------------------------------------------------------------------------------------------------------

struct DynamicModel {
    ModelVariantType variant;
    bool input_skip; /* Means the model has been trained with first input element skipped to the output */
    float input_gain;
    float output_gain;
};

// --------------------------------------------------------------------------------------------------------------------
// This function carries model calculations

static inline
void applyModelOffline(DynamicModel* model, float* const out, uint32_t numSamples)
{
    const bool input_skip = model->input_skip;
    const float input_gain = model->input_gain;
    const float output_gain = model->output_gain;

    std::visit(
        [&out, numSamples, input_skip, input_gain, output_gain] (auto&& custom_model)
        {
            using ModelType = std::decay_t<decltype (custom_model)>;

            if (d_isNotEqual(input_gain, 1.f))
            {
                for (uint32_t i=0; i<numSamples; ++i)
                    out[i] *= input_gain;
            }

            if constexpr (ModelType::input_size == 1)
            {
                if (input_skip)
                {
                    for (uint32_t i=0; i<numSamples; ++i)
                        out[i] += custom_model.forward(out + i);
                }
                else
                {
                    for (uint32_t i=0; i<numSamples; ++i)
                        out[i] = custom_model.forward(out + i) * output_gain;
                }
            }
            else if constexpr (ModelType::input_size == 2)
            {
                float inArray1 alignas(RTNEURAL_DEFAULT_ALIGNMENT)[2];

                if (input_skip)
                {
                    for (uint32_t i=0; i<numSamples; ++i)
                    {
                        inArray1[0] = out[i];
                        inArray1[1] = 0.f;
                        out[i] += custom_model.forward(inArray1);
                    }
                }
                else
                {
                    for (uint32_t i=0; i<numSamples; ++i)
                    {
                        inArray1[0] = out[i];
                        inArray1[1] = 0.f;
                        out[i] = custom_model.forward(inArray1) * output_gain;
                    }
                }
            }
            else if constexpr (ModelType::input_size == 3)
            {
                float inArray2 alignas(RTNEURAL_DEFAULT_ALIGNMENT)[3];

                if (input_skip)
                {
                    for (uint32_t i=0; i<numSamples; ++i)
                    {
                        inArray2[0] = out[i];
                        inArray2[1] = 0.f;
                        inArray2[2] = 0.f;
                        out[i] += custom_model.forward(inArray2);
                    }
                }
                else
                {
                    for (uint32_t i=0; i<numSamples; ++i)
                    {
                        inArray2[0] = out[i];
                        inArray2[1] = 0.f;
                        inArray2[2] = 0.f;
                        out[i] = custom_model.forward(inArray2) * output_gain;
                    }
                }
            }

            if (input_skip && d_isNotEqual(output_gain, 1.f))
            {
                for (uint32_t i=0; i<numSamples; ++i)
                    out[i] *= output_gain;
            }
        },
        model->variant
    );
}

static inline
float applyModel(DynamicModel* model, float sample, const float param1, const float param2)
{
    const bool input_skip = model->input_skip;
    const float input_gain = model->input_gain;
    const float output_gain = model->output_gain;

    sample *= input_gain;

    std::visit(
        [&sample, input_skip, output_gain, param1, param2] (auto&& custom_model)
        {
            using ModelType = std::decay_t<decltype (custom_model)>;

            if constexpr (ModelType::input_size == 1)
            {
                float* out = &sample;
                if (input_skip)
                {
                    sample += custom_model.forward(out);
                    sample *= output_gain;
                }
                else
                {
                    sample = custom_model.forward(out) * output_gain;
                }
            }
            else if constexpr (ModelType::input_size == 2)
            {
                float inArray1 alignas(RTNEURAL_DEFAULT_ALIGNMENT)[2] = {sample, param1};
                if (input_skip)
                {
                    sample += custom_model.forward(inArray1);
                    sample *= output_gain;
                }
                else
                {
                    sample = custom_model.forward(inArray1) * output_gain;
                }
            }
            else if constexpr (ModelType::input_size == 3)
            {
                float inArray2 alignas(RTNEURAL_DEFAULT_ALIGNMENT)[3] = {sample, param1, param2};
                if (input_skip)
                {
                    sample += custom_model.forward(inArray2);
                    sample *= output_gain;
                }
                else
                {
                    sample = custom_model.forward(inArray2) * output_gain;
                }
            }
        },
        model->variant
    );
    
    return sample;
}

// --------------------------------------------------------------------------------------------------------------------

struct AidaPluginModule : Module {
    enum Parameters {
        kParameterINLPF,
        kParameterINLEVEL,
        kParameterNETBYPASS,
        kParameterEQBYPASS,
        kParameterEQPOS,
        kParameterBASSGAIN,
        kParameterBASSFREQ,
        kParameterMIDGAIN,
        kParameterMIDFREQ,
        kParameterMIDQ,
        kParameterMTYPE,
        kParameterTREBLEGAIN,
        kParameterTREBLEFREQ,
        kParameterDEPTH,
        kParameterPRESENCE,
        kParameterOUTLEVEL,
        kParameterPARAM1,
        kParameterPARAM2,
        NUM_PARAMS
    };
    enum EqPos {
        kEqPost,
        kEqPre
    };
    enum MidEqType {
        kMidEqPeak,
        kMidEqBandpass
    };
    enum InputIds {
        AUDIO_INPUT,
        NUM_INPUTS
    };
    enum OutputIds {
        AUDIO_OUTPUT,
        NUM_OUTPUTS
    };
    enum LightIds {
        NUM_LIGHTS
    };

    CardinalPluginContext* const pcontext;
    bool fileChanged = false;
    std::string currentFile;

    Biquad dc_blocker { bq_type_highpass, 0.5f, COMMON_Q, 0.0f };
    Biquad in_lpf { bq_type_lowpass, 0.5f, COMMON_Q, 0.0f };
    Biquad bass { bq_type_lowshelf, 0.5f, COMMON_Q, 0.0f };
    Biquad mid { bq_type_peak, 0.5f, COMMON_Q, 0.0f };
    Biquad treble { bq_type_highshelf, 0.5f, COMMON_Q, 0.0f };
    Biquad depth { bq_type_peak, 0.5f, COMMON_Q, 0.0f };
    Biquad presence { bq_type_highshelf, 0.5f, COMMON_Q, 0.0f };

    float cachedParams[NUM_PARAMS] = {};

    dsp::ExponentialFilter inlevel;
    dsp::ExponentialFilter outlevel;
    DynamicModel* model = nullptr;
    std::atomic<bool> activeModel { false };

    AidaPluginModule()
        : pcontext(static_cast<CardinalPluginContext*>(APP))
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        configInput(AUDIO_INPUT, "Audio");
        configOutput(AUDIO_OUTPUT, "Audio");
        configParam(kParameterINLPF, -0.f, 100.f, 66.216f, "ANTIALIASING", " %");
        configParam(kParameterINLEVEL, -12.f, 12.f, 0.f, "INPUT", " dB");
        configSwitch(kParameterNETBYPASS, 0.f, 1.f, 0.f, "NETBYPASS");
        configSwitch(kParameterEQBYPASS, 0.f, 1.f, 0.f, "EQBYPASS");
        configSwitch(kParameterEQPOS, 0.f, 1.f, 0.f, "NETBYPASS");
        configParam(kParameterBASSGAIN, -8.f, 8.f, 0.f, "BASS", " dB");
        configParam(kParameterBASSFREQ, 60.f, 305.f, 75.f, "BFREQ", " Hz");
        configParam(kParameterMIDGAIN, -8.f, 8.f, 0.f, "MID", " dB");
        configParam(kParameterMIDFREQ, 150.f, 5000.f, 750.f, "MFREQ", " Hz");
        configParam(kParameterMIDQ, 0.2f, 5.f, 0.707f, "MIDQ");
        configSwitch(kParameterMTYPE, 0.f, 1.f, 0.f, "MTYPE");
        configParam(kParameterTREBLEGAIN, -8.f, 8.f, 0.f, "TREBLE", " dB");
        configParam(kParameterTREBLEFREQ, 1000.f, 4000.f, 2000.f, "TFREQ", " Hz");
        configParam(kParameterDEPTH, -8.f, 8.f, 0.f, "DEPTH", " dB");
        configParam(kParameterPRESENCE, -8.f, 8.f, 0.f, "PRESENCE", " dB");
        configParam(kParameterOUTLEVEL, -15.f, 15.f, 0.f, "OUTPUT", " dB");
        configParam(kParameterPARAM1, 0.f, 1.f, 0.f, "PARAM1");
        configParam(kParameterPARAM2, 0.f, 1.f, 0.f, "PARAM2");

        cachedParams[kParameterINLPF] = 66.216f;
        cachedParams[kParameterBASSGAIN] = 0.f;
        cachedParams[kParameterBASSFREQ] = 75.f;
        cachedParams[kParameterMIDGAIN] = 0.f;
        cachedParams[kParameterMIDFREQ] = 750.f;
        cachedParams[kParameterMIDQ] = 0.707f;
        cachedParams[kParameterTREBLEGAIN] = 0.f;
        cachedParams[kParameterTREBLEFREQ] = 2000.f;
        cachedParams[kParameterDEPTH] = 0.f;
        cachedParams[kParameterPRESENCE] = 0.f;

        in_lpf.setFc(MAP(66.216f, 0.0f, 100.0f, INLPF_MAX_CO, INLPF_MIN_CO));
        inlevel.setTau(1 / 30.f);
        outlevel.setTau(1 / 30.f);
    }

    ~AidaPluginModule() override
    {
        delete model;
    }

    json_t* dataToJson() override
    {
        json_t* const rootJ = json_object();
        DISTRHO_SAFE_ASSERT_RETURN(rootJ != nullptr, nullptr);

        json_object_set_new(rootJ, "filepath", json_string(currentFile.c_str()));

        return rootJ;
    }

    void dataFromJson(json_t* const rootJ) override
    {
        fileChanged = false;

        if (json_t* const filepathJ = json_object_get(rootJ, "filepath"))
        {
            const char* const filepath = json_string_value(filepathJ);

            if (filepath[0] != '\0')
            {
                currentFile = filepath;
                fileChanged = true;

                loadModelFromFile(filepath);
            }
        }

        if (! fileChanged)
        {
            currentFile.clear();
            fileChanged = true;
        }
    }

    void loadModelFromFile(const char* const filename)
    {
        try {
            std::ifstream jsonStream(filename, std::ifstream::binary);
            loadModelFromStream(jsonStream);
        }
        catch (const std::exception& e) {
            d_stderr2("Unable to load json file: %s\nError: %s", filename, e.what());
        };
    }

    void loadModelFromStream(std::istream& jsonStream)
    {
        int input_size;
        int input_skip;
        float input_gain;
        float output_gain;
        nlohmann::json model_json;

        try {
            jsonStream >> model_json;

            /* Understand which model type to load */
            input_size = model_json["in_shape"].back().get<int>();
            if (input_size > MAX_INPUT_SIZE) {
                throw std::invalid_argument("Value for input_size not supported");
            }

            if (model_json["in_skip"].is_number()) {
                input_skip = model_json["in_skip"].get<int>();
                if (input_skip > 1)
                    throw std::invalid_argument("Values for in_skip > 1 are not supported");
            }
            else {
                input_skip = 0;
            }

            if (model_json["in_gain"].is_number()) {
                input_gain = DB_CO(model_json["in_gain"].get<float>());
            }
            else {
                input_gain = 1.0f;
            }

            if (model_json["out_gain"].is_number()) {
                output_gain = DB_CO(model_json["out_gain"].get<float>());
            }
            else {
                output_gain = 1.0f;
            }
        }
        catch (const std::exception& e) {
            d_stderr2("Unable to load json, error: %s", e.what());
            return;
        }

        std::unique_ptr<DynamicModel> newmodel = std::make_unique<DynamicModel>();

        try {
            if (! custom_model_creator (model_json, newmodel->variant))
                throw std::runtime_error ("Unable to identify a known model architecture!");

            std::visit (
                [&model_json] (auto&& custom_model)
                {
                    using ModelType = std::decay_t<decltype (custom_model)>;
                    if constexpr (! std::is_same_v<ModelType, NullModel>)
                    {
                        custom_model.parseJson (model_json, true);
                        custom_model.reset();
                    }
                },
                newmodel->variant);
        }
        catch (const std::exception& e) {
            d_stderr2("Error loading model: %s", e.what());
            return;
        }

        // save extra info
        newmodel->input_skip = input_skip != 0;
        newmodel->input_gain = input_gain;
        newmodel->output_gain = output_gain;

        // Pre-buffer to avoid "clicks" during initialization
        float out[2048] = {};
        applyModelOffline(newmodel.get(), out, ARRAY_SIZE(out));

        // swap active model
        DynamicModel* const oldmodel = model;
        model = newmodel.release();

        // if processing, wait for process cycle to complete
        while (oldmodel != nullptr && activeModel.load())
            d_msleep(1);

        delete oldmodel;
    }

    MidEqType getMidType() const
    {
        return cachedParams[kParameterMTYPE] > 0.5f ? kMidEqBandpass : kMidEqPeak;
    }

    float applyToneControls(float sample)
    {
        return getMidType() == kMidEqBandpass
            ? mid.process(sample)
            : presence.process(
                treble.process(
                    mid.process(
                        bass.process(
                            depth.process(sample)))));
    }

    void process(const ProcessArgs& args) override
    {
        const float stime = args.sampleTime;
        const float inlevelv = DB_CO(params[kParameterINLEVEL].getValue());
        const float outlevelv = DB_CO(params[kParameterOUTLEVEL].getValue());

        const bool net_bypass = params[kParameterNETBYPASS].getValue() > 0.5f;
        const bool eq_bypass = params[kParameterEQBYPASS].getValue() > 0.5f;
        const EqPos eq_pos = params[kParameterEQPOS].getValue() > 0.5f ? kEqPre : kEqPost;

        // update tone controls
        bool changed = false;
        float value;

        value = params[kParameterINLPF].getValue();
        if (d_isNotEqual(cachedParams[kParameterINLPF], value))
        {
            cachedParams[kParameterINLPF] = value;
            in_lpf.setFc(MAP(value, 0.0f, 100.0f, INLPF_MAX_CO, INLPF_MIN_CO));
        }

        value = params[kParameterBASSGAIN].getValue();
        if (d_isNotEqual(cachedParams[kParameterBASSGAIN], value))
        {
            cachedParams[kParameterBASSGAIN] = value;
            changed = true;
        }

        value = params[kParameterBASSFREQ].getValue();
        if (d_isNotEqual(cachedParams[kParameterBASSFREQ], value))
        {
            cachedParams[kParameterBASSFREQ] = value;
            changed = true;
        }

        if (changed)
        {
            changed = false;
            bass.setBiquad(bq_type_lowshelf,
                           cachedParams[kParameterBASSFREQ] / args.sampleRate,
                           COMMON_Q,
                           cachedParams[kParameterBASSGAIN]);
        }

        value = params[kParameterMIDGAIN].getValue();
        if (d_isNotEqual(cachedParams[kParameterMIDGAIN], value))
        {
            cachedParams[kParameterMIDGAIN] = value;
            changed = true;
        }

        value = params[kParameterMIDFREQ].getValue();
        if (d_isNotEqual(cachedParams[kParameterMIDFREQ], value))
        {
            cachedParams[kParameterMIDFREQ] = value;
            changed = true;
        }

        value = params[kParameterMIDQ].getValue();
        if (d_isNotEqual(cachedParams[kParameterMIDQ], value))
        {
            cachedParams[kParameterMIDQ] = value;
            changed = true;
        }

        value = params[kParameterMTYPE].getValue();
        if (d_isNotEqual(cachedParams[kParameterMTYPE], value))
        {
            cachedParams[kParameterMTYPE] = value;
            changed = true;
        }

        if (changed)
        {
            changed = false;
            mid.setBiquad(getMidType() == kMidEqBandpass ? bq_type_bandpass : bq_type_peak,
                          cachedParams[kParameterMIDFREQ] / args.sampleRate,
                          cachedParams[kParameterMIDQ],
                          cachedParams[kParameterMIDGAIN]);
        }

        value = params[kParameterTREBLEGAIN].getValue();
        if (d_isNotEqual(cachedParams[kParameterTREBLEGAIN], value))
        {
            cachedParams[kParameterTREBLEGAIN] = value;
            changed = true;
        }

        value = params[kParameterTREBLEFREQ].getValue();
        if (d_isNotEqual(cachedParams[kParameterTREBLEFREQ], value))
        {
            cachedParams[kParameterTREBLEFREQ] = value;
            changed = true;
        }

        if (changed)
        {
            changed = false;
            treble.setBiquad(bq_type_highshelf,
                             cachedParams[kParameterTREBLEFREQ] / args.sampleRate,
                             COMMON_Q,
                             cachedParams[kParameterTREBLEGAIN]);
        }

        value = params[kParameterDEPTH].getValue();
        if (d_isNotEqual(cachedParams[kParameterDEPTH], value))
        {
            cachedParams[kParameterDEPTH] = value;
            depth.setPeakGain(value);
        }

        value = params[kParameterPRESENCE].getValue();
        if (d_isNotEqual(cachedParams[kParameterPRESENCE], value))
        {
            cachedParams[kParameterPRESENCE] = value;
            presence.setPeakGain(value);
        }

        // High frequencies roll-off (lowpass)
        float sample = in_lpf.process(inputs[AUDIO_INPUT].getVoltage() * 0.1f) * inlevel.process(stime, inlevelv);

        // Equalizer section
        if (!eq_bypass && eq_pos == kEqPre)
            sample = applyToneControls(sample);

        // run model
        if (!net_bypass && model != nullptr)
        {
            activeModel.store(true);
            sample = applyModel(model, sample,
                                params[kParameterPARAM1].getValue(),
                                params[kParameterPARAM2].getValue());
            activeModel.store(false);
        }

        // DC blocker filter (highpass)
        sample = dc_blocker.process(sample);

        // Equalizer section
        if (!eq_bypass && eq_pos == kEqPost)
            sample = applyToneControls(sample);

        // Output volume
        outputs[AUDIO_OUTPUT].setVoltage(sample * outlevel.process(stime, outlevelv) * 10.f);
    }

    void onSampleRateChange(const SampleRateChangeEvent& e) override
    {
        cachedParams[kParameterBASSGAIN] = params[kParameterBASSGAIN].getValue();
        cachedParams[kParameterBASSFREQ] = params[kParameterBASSFREQ].getValue();
        cachedParams[kParameterMIDGAIN] = params[kParameterMIDGAIN].getValue();
        cachedParams[kParameterMIDFREQ] = params[kParameterMIDFREQ].getValue();
        cachedParams[kParameterMIDQ] = params[kParameterMIDQ].getValue();
        cachedParams[kParameterMTYPE] = params[kParameterMTYPE].getValue();
        cachedParams[kParameterTREBLEGAIN] = params[kParameterTREBLEGAIN].getValue();
        cachedParams[kParameterTREBLEFREQ] = params[kParameterTREBLEFREQ].getValue();
        cachedParams[kParameterDEPTH] = params[kParameterDEPTH].getValue();
        cachedParams[kParameterPRESENCE] = params[kParameterPRESENCE].getValue();

        dc_blocker.setFc(35.0f / e.sampleRate);

        bass.setBiquad(bq_type_lowshelf,
                       cachedParams[kParameterBASSFREQ] / e.sampleRate,
                       COMMON_Q,
                       cachedParams[kParameterBASSGAIN]);

        mid.setBiquad(getMidType() == kMidEqBandpass ? bq_type_bandpass : bq_type_peak,
                      cachedParams[kParameterMIDFREQ] / e.sampleRate,
                      cachedParams[kParameterMIDQ],
                      cachedParams[kParameterMIDGAIN]);

        treble.setBiquad(bq_type_highshelf,
                         cachedParams[kParameterTREBLEFREQ] / e.sampleRate,
                         COMMON_Q,
                         cachedParams[kParameterTREBLEGAIN]);

        depth.setBiquad(bq_type_peak,
                        DEPTH_FREQ / e.sampleRate,
                        COMMON_Q,
                        cachedParams[kParameterDEPTH]);

        presence.setBiquad(bq_type_highshelf,
                           PRESENCE_FREQ / e.sampleRate,
                           COMMON_Q,
                           cachedParams[kParameterPRESENCE]);
    }

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AidaPluginModule)
};

// --------------------------------------------------------------------------------------------------------------------

#ifndef HEADLESS
struct AidaModelListWidget : ImGuiWidget {
    AidaPluginModule* const module;

    /*
    bool showError = false;
    String errorMessage;
    */

    struct ghcFile {
        std::string full, base;
        bool operator<(const ghcFile& other) const noexcept { return base < other.base; }
    };
    std::string currentDirectory;
    std::vector<ghcFile> currentFiles;
    size_t selectedFile = (size_t)-1;

    AidaModelListWidget(AidaPluginModule* const m)
        : ImGuiWidget(),
          module(m)
    {
        if (module->fileChanged)
            reloadDir();
    }

    void drawImGui() override
    {
        const float scaleFactor = getScaleFactor();

        const int flags = ImGuiWindowFlags_NoSavedSettings
                        | ImGuiWindowFlags_NoTitleBar
                        | ImGuiWindowFlags_NoResize
                        | ImGuiWindowFlags_NoCollapse
                        | ImGuiWindowFlags_NoScrollbar
                        | ImGuiWindowFlags_NoScrollWithMouse;

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(box.size.x * scaleFactor, box.size.y * scaleFactor));

        if (ImGui::Begin("Model File List", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize))
        {
            /*
            if (showError)
            {
                showError = false;
                ImGui::OpenPopup("Audio File Error");
            }

            if (ImGui::BeginPopupModal("Model File Error", nullptr, flags))
            {
                ImGui::TextWrapped("Failed to load model file, error was:\n%s", errorMessage.buffer());

                ImGui::Separator();

                if (ImGui::Button("Ok"))
                    ImGui::CloseCurrentPopup();

                ImGui::EndPopup();
            }
            else
            */
            if (ImGui::BeginTable("modellist", 1, ImGuiTableFlags_NoSavedSettings))
            {
                for (size_t i=0, count=currentFiles.size(); i < count; ++i)
                {
                    bool wasSelected = selectedFile == i;
                    bool selected = wasSelected;
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Selectable(currentFiles[i].base.c_str(), &selected);

                    if (selected && ! wasSelected)
                    {
                        selectedFile = i;
                        module->currentFile = currentFiles[i].full;
                        module->loadModelFromFile(currentFiles[i].full.c_str());
                    }
                }

                ImGui::EndTable();
            }
        }

        ImGui::End();
    }

    void step() override
    {
        if (module->fileChanged)
            reloadDir();

        ImGuiWidget::step();
    }

    void reloadDir()
    {
        module->fileChanged = false;

        currentFiles.clear();
        selectedFile = (size_t)-1;

        static constexpr const char* const supportedExtensions[] = {
            ".json"
        };

        using namespace ghc::filesystem;
        const path currentFile = u8path(module->currentFile);
        currentDirectory = currentFile.parent_path().generic_u8string();

        directory_iterator it;

        try {
            it = directory_iterator(u8path(currentDirectory));
        } DISTRHO_SAFE_EXCEPTION_RETURN("Failed to open current directory",);

        for (directory_iterator itb = begin(it), ite=end(it); itb != ite; ++itb)
        {
            if (! itb->is_regular_file())
                continue;
            const path filepath = itb->path();
            const path extension = filepath.extension();
            for (size_t i=0; i<ARRAY_SIZE(supportedExtensions); ++i)
            {
                if (extension.compare(supportedExtensions[i]) == 0)
                {
                    currentFiles.push_back({ filepath.generic_u8string(), filepath.filename().generic_u8string() });
                    break;
                }
            }
        }

        std::sort(currentFiles.begin(), currentFiles.end());

        for (size_t index = 0; index < currentFiles.size(); ++index)
        {
            if (currentFiles[index].full.compare(currentFile) == 0)
            {
                selectedFile = index;
                break;
            }
        }
    }
};

struct AidaKnob : app::SvgKnob {
    AidaKnob()
    {
        minAngle = -0.76 * M_PI;
        maxAngle = 0.76 * M_PI;
        shadow->opacity = 0;
        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/aida-x-knob.svg")));
    }
};

struct AidaWidget : ModuleWidgetWithSideScrews<23> {
    static constexpr const uint kPedalMargin = 10;
    static constexpr const uint kPedalMarginTop = 50;

    static constexpr const float startY_list = startY - 2.0f;
    static constexpr const float fileListHeight = 380.0f - startY_list - 110.0f;

    AidaPluginModule* const module;

    AidaWidget(AidaPluginModule* const m)
        : module(m)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/AIDA-X.svg")));

        createAndAddScrews();

        addInput(createInput<PJ301MPort>(Vec(startX_In, 25), module, 0));
        addOutput(createOutput<PJ301MPort>(Vec(startX_Out, 25), module, 0));

        addChild(createParamCentered<AidaKnob>(Vec(50, box.size.y - 60),
                                               module, AidaPluginModule::kParameterINLEVEL));

        addChild(createParamCentered<AidaKnob>(Vec(100, box.size.y - 60),
                                               module, AidaPluginModule::kParameterBASSGAIN));

        addChild(createParamCentered<AidaKnob>(Vec(150, box.size.y - 60),
                                               module, AidaPluginModule::kParameterMIDGAIN));

        addChild(createParamCentered<AidaKnob>(Vec(200, box.size.y - 60),
                                               module, AidaPluginModule::kParameterTREBLEGAIN));

        addChild(createParamCentered<AidaKnob>(Vec(250, box.size.y - 60),
                                               module, AidaPluginModule::kParameterDEPTH));

        addChild(createParamCentered<AidaKnob>(Vec(300, box.size.y - 60),
                                               module, AidaPluginModule::kParameterPRESENCE));

        addChild(createParamCentered<AidaKnob>(Vec(350, box.size.y - 60),
                                               module, AidaPluginModule::kParameterOUTLEVEL));

        if (m != nullptr)
        {
            AidaModelListWidget* const listw = new AidaModelListWidget(m);
            listw->box.pos = Vec(kPedalMargin, startY_list);
            listw->box.size = Vec(box.size.x - kPedalMargin * 2, fileListHeight);
            addChild(listw);
        }
    }

    void draw(const DrawArgs& args) override
    {
        const double widthPedal = box.size.x - kPedalMargin * 2;
        const double heightPedal = box.size.y - kPedalMargin - kPedalMarginTop;
        const int cornerRadius = 12;

        // outer bounds gradient
        nvgBeginPath(args.vg);
        nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
        nvgFillPaint(args.vg,
                     nvgLinearGradient(args.vg,
                                       0, 0, 0, box.size.y,
                                       nvgRGB(0xff - 0xcd + 50, 0xff - 0xff + 50, 0xff),
                                       nvgRGB(0xff - 0x8b + 50, 0xff - 0xf7 + 50, 0xff)));
        nvgFill(args.vg);

        // outer bounds pattern
        // TODO

        // box shadow
        nvgBeginPath(args.vg);
        nvgRect(args.vg,
                kPedalMargin / 2,
                kPedalMarginTop / 2,
                kPedalMargin + widthPedal,
                kPedalMarginTop + heightPedal);
        nvgFillPaint(args.vg,
                     nvgBoxGradient(args.vg,
                                    kPedalMargin,
                                    kPedalMarginTop,
                                    widthPedal,
                                    heightPedal,
                                    cornerRadius,
                                    cornerRadius,
                                    nvgRGBAf(0,0,0,1.f),
                                    nvgRGBAf(0,0,0,0.f)));
        nvgFill(args.vg);

        // .rt-neural .grid
        nvgBeginPath(args.vg);
        nvgRoundedRect(args.vg, kPedalMargin, kPedalMarginTop, widthPedal, heightPedal, cornerRadius);
        nvgFillPaint(args.vg,
                     nvgLinearGradient(args.vg,
                                       kPedalMargin, kPedalMarginTop,
                                       kPedalMargin + box.size.x * 0.52f, 0,
                                       nvgRGB(28, 23, 12),
                                       nvgRGB(42, 34, 15)));
        nvgFill(args.vg);

        nvgFillPaint(args.vg,
                     nvgLinearGradient(args.vg,
                                       kPedalMargin + box.size.x * 0.5f, 0,
                                       kPedalMargin + box.size.x, 0,
                                       nvgRGB(42, 34, 15),
                                       nvgRGB(19, 19, 19)));
        nvgFill(args.vg);

        // extra
        nvgStrokeColor(args.vg, nvgRGBA(150, 150, 150, 60));
        nvgStroke(args.vg);

        drawOutputJacksArea(args.vg);

        ModuleWidget::draw(args);
    }

    void drawOutputJacksArea(NVGcontext* const vg)
    {
        nvgBeginPath(vg);
        nvgRoundedRect(vg, startX_Out - 2.5f, startY_list * 0.5f - padding * 0.5f, padding, padding, 4);
        nvgFillColor(vg, nvgRGB(0xd0, 0xd0, 0xd0));
        nvgFill(vg);
    }

    void appendContextMenu(ui::Menu* const menu) override
    {
        menu->addChild(new ui::MenuSeparator);

        struct LoadModelFileItem : MenuItem {
            AidaPluginModule* const module;

            LoadModelFileItem(AidaPluginModule* const m)
                : module(m)
            {
                text = "Load model file...";
            }

            void onAction(const event::Action&) override
            {
                AidaPluginModule* const module = this->module;
                async_dialog_filebrowser(false, nullptr, nullptr, text.c_str(), [module](char* path)
                {
                    if (path == nullptr)
                        return;

                    module->currentFile = path;
                    module->fileChanged = true;
                    module->loadModelFromFile(path);
                    std::free(path);
                });
            }
        };

        menu->addChild(new LoadModelFileItem(module));
    }

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AidaWidget)
};
#else
struct AidaWidget : ModuleWidget {
    AidaWidget(AidaPluginModule* const module) {
        setModule(module);

        addInput(createInput<PJ301MPort>({}, module, 0));
        addOutput(createOutput<PJ301MPort>({}, module, 0));
    }
};
#endif

// --------------------------------------------------------------------------------------------------------------------

Model* modelAidaX = createModel<AidaPluginModule, AidaWidget>("AIDA-X");

// --------------------------------------------------------------------------------------------------------------------
