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
void applyModel(DynamicModel* model, float* const out, uint32_t numSamples)
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
float applyModel(DynamicModel* model, float sample)
{
    const bool input_skip = model->input_skip;
    const float input_gain = model->input_gain;
    const float output_gain = model->output_gain;

    sample *= input_gain;

    std::visit(
        [&sample, input_skip, output_gain] (auto&& custom_model)
        {
            using ModelType = std::decay_t<decltype (custom_model)>;
            float* out = &sample;

            if constexpr (ModelType::input_size == 1)
            {
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
        },
        model->variant
    );
    
    return sample;
}

// --------------------------------------------------------------------------------------------------------------------

struct AidaPluginModule : Module {
    enum ParamIds {
        PARAM_INPUT_LEVEL,
        PARAM_OUTPUT_LEVEL,
        NUM_PARAMS
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

    enum Parameters {
        kParameterCount
    };

    CardinalPluginContext* const pcontext;
    bool fileChanged = false;
    std::string currentFile;

    Biquad dc_blocker { bq_type_highpass, 0.5f, COMMON_Q, 0.0f };
    Biquad in_lpf { bq_type_lowpass, 0.5f, COMMON_Q, 0.0f };
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
        configParam(PARAM_INPUT_LEVEL, -12.f, 12.f, 0.f, "Input level", " dB");
        configParam(PARAM_OUTPUT_LEVEL, -12.f, 12.f, 0.f, "Output level", " dB");

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
            if (input_size > 1) { // MAX_INPUT_SIZE
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
        applyModel(newmodel.get(), out, ARRAY_SIZE(out));

        // swap active model
        DynamicModel* const oldmodel = model;
        model = newmodel.release();

        // if processing, wait for process cycle to complete
        while (oldmodel != nullptr && activeModel.load())
            d_msleep(1);

        delete oldmodel;
    }

    void process(const ProcessArgs& args) override
    {
        const float stime = args.sampleTime;
        const float inlevelv = DB_CO(params[PARAM_INPUT_LEVEL].getValue());
        const float outlevelv = DB_CO(params[PARAM_OUTPUT_LEVEL].getValue());

        // High frequencies roll-off (lowpass)
        float sample = in_lpf.process(inputs[AUDIO_INPUT].getVoltage() * 0.1f) * inlevel.process(stime, inlevelv);

        // run model
        if (model != nullptr)
        {
            activeModel.store(true);
            sample = applyModel(model, sample);
            activeModel.store(false);
        }

        // DC blocker filter (highpass)
        outputs[AUDIO_OUTPUT].setVoltage(dc_blocker.process(sample) * outlevel.process(stime, outlevelv) * 10.f);
    }

    void onSampleRateChange(const SampleRateChangeEvent& e) override
    {
        dc_blocker.setFc(35.0f / e.sampleRate);
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
    static constexpr const float previewBoxHeight = 80.0f;
    static constexpr const float previewBoxBottom = 20.0f;
    static constexpr const float previewBoxRect[] = {8.0f,
                                                     380.0f - previewBoxHeight - previewBoxBottom,
                                                     15.0f * 23 - 16.0f,
                                                     previewBoxHeight};
    static constexpr const float startY_list = startY - 2.0f;
    static constexpr const float fileListHeight = 380.0f - startY_list - previewBoxHeight - previewBoxBottom * 1.5f;
    static constexpr const float startY_preview = startY_list + fileListHeight;

    AidaPluginModule* const module;

    AidaWidget(AidaPluginModule* const m)
        : module(m)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/AIDA-X.svg")));

        createAndAddScrews();

        addInput(createInput<PJ301MPort>(Vec(startX_In, 25), module, 0));
        addOutput(createOutput<PJ301MPort>(Vec(startX_Out, 25), module, 0));

        addChild(createParamCentered<AidaKnob>(Vec(box.size.x * 0.5f - 50, box.size.y - 60),
                                               module, AidaPluginModule::PARAM_INPUT_LEVEL));

        addChild(createParamCentered<AidaKnob>(Vec(box.size.x * 0.5f + 50, box.size.y - 60),
                                               module, AidaPluginModule::PARAM_OUTPUT_LEVEL));

        if (m != nullptr)
        {
            AidaModelListWidget* const listw = new AidaModelListWidget(m);
            listw->box.pos = Vec(0, startY_list);
            listw->box.size = Vec(box.size.x, fileListHeight);
            addChild(listw);
        }
    }

    void draw(const DrawArgs& args) override
    {
        drawBackground(args.vg);
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
