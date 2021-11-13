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

#include "CarlaNativePlugin.h"
#include "CarlaBackendUtils.hpp"
#include "CarlaEngine.hpp"
#include "water/streams/MemoryOutputStream.h"
#include "water/xml/XmlDocument.h"

#ifndef CARDINAL_SYSDEPS
// private method that takes ownership, we can use it to avoid superfulous allocations
extern "C" {
json_t *jsonp_stringn_nocheck_own(const char* value, size_t len);
}
#endif

#define BUFFER_SIZE 128

// generates a warning if this is defined as anything else
#define CARLA_API

// --------------------------------------------------------------------------------------------------------------------

using namespace CarlaBackend;

static uint32_t host_get_buffer_size(NativeHostHandle);
static double host_get_sample_rate(NativeHostHandle);
static bool host_is_offline(NativeHostHandle);
static const NativeTimeInfo* host_get_time_info(NativeHostHandle handle);
static bool host_write_midi_event(NativeHostHandle handle, const NativeMidiEvent* event);
static void host_ui_parameter_changed(NativeHostHandle handle, uint32_t index, float value);
static void host_ui_midi_program_changed(NativeHostHandle handle, uint8_t channel, uint32_t bank, uint32_t program);
static void host_ui_custom_data_changed(NativeHostHandle handle, const char* key, const char* value);
static void host_ui_closed(NativeHostHandle handle);
static const char* host_ui_open_file(NativeHostHandle handle, bool isDir, const char* title, const char* filter);
static const char* host_ui_save_file(NativeHostHandle handle, bool isDir, const char* title, const char* filter);
static intptr_t host_dispatcher(NativeHostHandle handle, NativeHostDispatcherOpcode opcode, int32_t index, intptr_t value, void* ptr, float opt);

// --------------------------------------------------------------------------------------------------------------------

struct CarlaModule : Module {
    enum ParamIds {
        BIPOLAR_INPUTS,
        BIPOLAR_OUTPUTS,
        NUM_PARAMS
    };
    enum InputIds {
        AUDIO_INPUT1,
        AUDIO_INPUT2,
        CV_INPUT1,
        NUM_INPUTS = CV_INPUT1 + 8
    };
    enum OutputIds {
        AUDIO_OUTPUT1,
        AUDIO_OUTPUT2,
        CV_OUTPUT1,
        NUM_OUTPUTS = CV_OUTPUT1 + 8
    };
    enum LightIds {
        NUM_LIGHTS
    };

    CardinalPluginContext* const pcontext;

    const NativePluginDescriptor* fCarlaPluginDescriptor = nullptr;
    NativePluginHandle fCarlaPluginHandle = nullptr;

    NativeHostDescriptor fCarlaHostDescriptor = {};
    CarlaHostHandle fCarlaHostHandle = nullptr;

    mutable NativeTimeInfo fCarlaTimeInfo;

    void* fUI = nullptr;

    float dataIn[NUM_INPUTS][BUFFER_SIZE];
    float dataOut[NUM_OUTPUTS][BUFFER_SIZE];
    float* dataInPtr[NUM_INPUTS];
    float* dataOutPtr[NUM_OUTPUTS];
    unsigned audioDataFill = 0;
    std::string patchStorage;

    CarlaModule()
        : pcontext(reinterpret_cast<CardinalPluginContext*>(APP))
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam<SwitchQuantity>(BIPOLAR_INPUTS, 0.f, 1.f, 1.f, "Bipolar CV Inputs")->randomizeEnabled = false;
        configParam<SwitchQuantity>(BIPOLAR_OUTPUTS, 0.f, 1.f, 1.f, "Bipolar CV Outputs")->randomizeEnabled = false;

        for (uint i=0; i<NUM_INPUTS; ++i)
            dataInPtr[i] = dataIn[i];
        for (uint i=0; i<NUM_OUTPUTS; ++i)
            dataOutPtr[i] = dataOut[i];

        for (uint i=0; i<2; ++i)
        {
            const char name[] = { 'A','u','d','i','o',' ','#',static_cast<char>('0'+i+1),'\0' };
            configInput(i, name);
            configOutput(i, name);
        }
        for (uint i=2; i<NUM_INPUTS; ++i)
        {
            const char name[] = { 'C','V',' ','#',static_cast<char>('0'+i-1),'\0' };
            configInput(i, name);
            configOutput(i, name);
        }

        std::memset(dataOut, 0, sizeof(dataOut));

        fCarlaPluginDescriptor = carla_get_native_patchbay_cv8_plugin();
        DISTRHO_SAFE_ASSERT_RETURN(fCarlaPluginDescriptor != nullptr,);

        memset(&fCarlaHostDescriptor, 0, sizeof(fCarlaHostDescriptor));
        memset(&fCarlaTimeInfo, 0, sizeof(fCarlaTimeInfo));

        fCarlaHostDescriptor.handle = this;
#ifdef CARLA_OS_MAC
        fCarlaHostDescriptor.resourceDir = "/Applications/Carla.app/Contents/MacOS/resources";
#else
        fCarlaHostDescriptor.resourceDir = "/usr/share/carla/resources";
#endif
        fCarlaHostDescriptor.uiName = "Carla";
        fCarlaHostDescriptor.uiParentId = 0;

        fCarlaHostDescriptor.get_buffer_size = host_get_buffer_size;
        fCarlaHostDescriptor.get_sample_rate = host_get_sample_rate;
        fCarlaHostDescriptor.is_offline = host_is_offline;

        fCarlaHostDescriptor.get_time_info = host_get_time_info;
        fCarlaHostDescriptor.write_midi_event = host_write_midi_event;
        fCarlaHostDescriptor.ui_parameter_changed = host_ui_parameter_changed;
        fCarlaHostDescriptor.ui_midi_program_changed = host_ui_midi_program_changed;
        fCarlaHostDescriptor.ui_custom_data_changed = host_ui_custom_data_changed;
        fCarlaHostDescriptor.ui_closed = host_ui_closed;
        fCarlaHostDescriptor.ui_open_file = host_ui_open_file;
        fCarlaHostDescriptor.ui_save_file = host_ui_save_file;
        fCarlaHostDescriptor.dispatcher = host_dispatcher;

        fCarlaPluginHandle = fCarlaPluginDescriptor->instantiate(&fCarlaHostDescriptor);
        DISTRHO_SAFE_ASSERT_RETURN(fCarlaPluginHandle != nullptr,);

        fCarlaHostHandle = carla_create_native_plugin_host_handle(fCarlaPluginDescriptor, fCarlaPluginHandle);
        DISTRHO_SAFE_ASSERT_RETURN(fCarlaHostHandle != nullptr,);

#ifdef CARLA_OS_MAC
        carla_set_engine_option(fCarlaHostHandle, ENGINE_OPTION_PATH_BINARIES, 0, "/Applications/Carla.app/Contents/MacOS");
        carla_set_engine_option(fCarlaHostHandle, ENGINE_OPTION_PATH_RESOURCES, 0, "/Applications/Carla.app/Contents/MacOS/resources");
#else
        carla_set_engine_option(fCarlaHostHandle, ENGINE_OPTION_PATH_BINARIES, 0, "/usr/lib/carla");
        carla_set_engine_option(fCarlaHostHandle, ENGINE_OPTION_PATH_RESOURCES, 0, "/usr/share/carla/resources");
#endif

        fCarlaPluginDescriptor->dispatcher(fCarlaPluginHandle, NATIVE_PLUGIN_OPCODE_HOST_USES_EMBED,
                                           0, 0, nullptr, 0.0f);

        fCarlaPluginDescriptor->activate(fCarlaPluginHandle);
    }

    ~CarlaModule() override
    {
        if (fCarlaPluginHandle != nullptr)
            fCarlaPluginDescriptor->deactivate(fCarlaPluginHandle);

        if (fCarlaHostHandle != nullptr)
            carla_host_handle_free(fCarlaHostHandle);

        if (fCarlaPluginHandle != nullptr)
            fCarlaPluginDescriptor->cleanup(fCarlaPluginHandle);
    }

    const NativeTimeInfo* hostGetTimeInfo() const noexcept
    {
        if (pcontext != nullptr)
        {
            fCarlaTimeInfo.playing = pcontext->playing;
            fCarlaTimeInfo.frame = pcontext->frame;
            fCarlaTimeInfo.bbt.valid = pcontext->bbtValid;
            fCarlaTimeInfo.bbt.bar = pcontext->bar;
            fCarlaTimeInfo.bbt.beat = pcontext->beat;
            fCarlaTimeInfo.bbt.tick = pcontext->tick;
            fCarlaTimeInfo.bbt.barStartTick = pcontext->barStartTick;
            fCarlaTimeInfo.bbt.beatsPerBar = pcontext->beatsPerBar;
            fCarlaTimeInfo.bbt.beatType = pcontext->beatType;
            fCarlaTimeInfo.bbt.ticksPerBeat = pcontext->ticksPerBeat;
            fCarlaTimeInfo.bbt.beatsPerMinute = pcontext->beatsPerMinute;
        }

        return &fCarlaTimeInfo;
    }

    intptr_t hostDispatcher(const NativeHostDispatcherOpcode opcode,
                            const int32_t index, const intptr_t value, void* const ptr, const float opt)
    {
        switch (opcode)
        {
        // cannnot be supported
        case NATIVE_HOST_OPCODE_HOST_IDLE:
            break;
        // other stuff
        case NATIVE_HOST_OPCODE_NULL:
        case NATIVE_HOST_OPCODE_UPDATE_PARAMETER:
        case NATIVE_HOST_OPCODE_UPDATE_MIDI_PROGRAM:
        case NATIVE_HOST_OPCODE_RELOAD_PARAMETERS:
        case NATIVE_HOST_OPCODE_RELOAD_MIDI_PROGRAMS:
        case NATIVE_HOST_OPCODE_RELOAD_ALL:
        case NATIVE_HOST_OPCODE_UI_UNAVAILABLE:
        case NATIVE_HOST_OPCODE_INTERNAL_PLUGIN:
        case NATIVE_HOST_OPCODE_QUEUE_INLINE_DISPLAY:
        case NATIVE_HOST_OPCODE_UI_TOUCH_PARAMETER:
        case NATIVE_HOST_OPCODE_REQUEST_IDLE:
        case NATIVE_HOST_OPCODE_UI_RESIZE:
        case NATIVE_HOST_OPCODE_PREVIEW_BUFFER_DATA:
            // TESTING
            d_stdout("dispatcher %i, %i, %li, %p, %f", opcode, index, value, ptr, opt);
            break;
        case NATIVE_HOST_OPCODE_GET_FILE_PATH:
            return (intptr_t)(void*)patchStorage.c_str();
            break;
        }

        return 0;
    }

    json_t* dataToJson() override
    {
        if (fCarlaHostHandle == nullptr)
            return nullptr;

        CarlaEngine* const engine = carla_get_engine_from_handle(fCarlaHostHandle);

        water::MemoryOutputStream projectState;
        engine->saveProjectInternal(projectState);

        const size_t dataSize = projectState.getDataSize();
#ifndef CARDINAL_SYSDEPS
        return jsonp_stringn_nocheck_own(static_cast<const char*>(projectState.getDataAndRelease()), dataSize);
#else
        return json_stringn(static_cast<const char*>(projectState.getData()), dataSize);
#endif
    }

    void dataFromJson(json_t* const rootJ) override
    {
        if (fCarlaHostHandle == nullptr)
            return;

        const char* const projectState = json_string_value(rootJ);
        DISTRHO_SAFE_ASSERT_RETURN(projectState != nullptr,);

        CarlaEngine* const engine = carla_get_engine_from_handle(fCarlaHostHandle);

        water::XmlDocument xml(projectState);
        engine->loadProjectInternal(xml, true);
    }

    void onAdd(const AddEvent&) override
    {
        patchStorage = getPatchStorageDirectory();
    }

    void process(const ProcessArgs&) override
    {
        if (fCarlaPluginHandle == nullptr)
            return;

        const float inputOffset = params[BIPOLAR_INPUTS].getValue() > 0.1f ? -5.0f : 0.0f;
        const float outputOffset = params[BIPOLAR_OUTPUTS].getValue() > 0.1f ? -5.0f : 0.0f;

        const unsigned k = audioDataFill++;

        for (uint i=0; i<2; ++i)
            dataIn[i][k] = inputs[i].getVoltage() * 0.1f;
        for (uint i=2; i<NUM_INPUTS; ++i)
            dataIn[i][k] = inputs[i].getVoltage() + inputOffset;

        for (uint i=0; i<2; ++i)
            outputs[i].setVoltage(dataOut[i][k] * 10.0f);
        for (uint i=2; i<NUM_OUTPUTS; ++i)
            outputs[i].setVoltage(dataOut[i][k] + outputOffset);

        if (audioDataFill == BUFFER_SIZE)
        {
            audioDataFill = 0;
            fCarlaPluginDescriptor->process(fCarlaPluginHandle, dataInPtr, dataOutPtr, BUFFER_SIZE, nullptr, 0);
        }
    }

    void onSampleRateChange(const SampleRateChangeEvent& e) override
    {
        if (fCarlaPluginHandle == nullptr)
            return;

        fCarlaPluginDescriptor->deactivate(fCarlaPluginHandle);
        fCarlaPluginDescriptor->dispatcher(fCarlaPluginHandle, NATIVE_PLUGIN_OPCODE_SAMPLE_RATE_CHANGED,
                                           0, 0, nullptr, e.sampleRate);
        fCarlaPluginDescriptor->activate(fCarlaPluginHandle);
    }
};

static_assert((int)CarlaModule::NUM_INPUTS == (int)CarlaModule::NUM_OUTPUTS, "inputs must match outputs");

// -----------------------------------------------------------------------------------------------------------

static uint32_t host_get_buffer_size(const NativeHostHandle handle)
{
    return BUFFER_SIZE;
}

static double host_get_sample_rate(const NativeHostHandle handle)
{
    CardinalPluginContext* const pcontext = static_cast<CarlaModule*>(handle)->pcontext;
    DISTRHO_SAFE_ASSERT_RETURN(pcontext != nullptr, 48000.0);
    return pcontext->sampleRate;
}

static bool host_is_offline(NativeHostHandle)
{
    return false;
}

static const NativeTimeInfo* host_get_time_info(const NativeHostHandle handle)
{
    return static_cast<CarlaModule*>(handle)->hostGetTimeInfo();
}

static bool host_write_midi_event(const NativeHostHandle handle, const NativeMidiEvent* const event)
{
    return false;
}

static void host_ui_midi_program_changed(NativeHostHandle handle, uint8_t channel, uint32_t bank, uint32_t program)
{
    d_stdout("%s %p %u %u %u", __FUNCTION__, handle, channel, bank, program);
}

static void host_ui_custom_data_changed(NativeHostHandle handle, const char* key, const char* value)
{
    d_stdout("%s %p %s %s", __FUNCTION__, handle, key, value);
}

static const char* host_ui_save_file(NativeHostHandle, bool, const char*, const char*)
{
    return nullptr;
}

static intptr_t host_dispatcher(const NativeHostHandle handle, const NativeHostDispatcherOpcode opcode,
                                const int32_t index, const intptr_t value, void* const ptr, const float opt)
{
    return static_cast<CarlaModule*>(handle)->hostDispatcher(opcode, index, value, ptr, opt);
}

// --------------------------------------------------------------------------------------------------------------------

#ifndef HEADLESS
struct CarlaModuleWidget : ModuleWidget, IdleCallback {
    static constexpr const float startX_In = 14.0f;
    static constexpr const float startX_Out = 96.0f;
    static constexpr const float startY = 74.0f;
    static constexpr const float padding = 29.0f;
    static constexpr const float middleX = startX_In + (startX_Out - startX_In) * 0.5f + padding * 0.25f;

    CarlaModule* const module;
    bool idleCallbackActive = false;
    bool visible = false;

    CarlaModuleWidget(CarlaModule* const m)
        : ModuleWidget(),
          module(m)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Carla.svg")));

        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        for (uint i=0; i<CarlaModule::NUM_INPUTS; ++i)
            addInput(createInput<PJ301MPort>(Vec(startX_In, startY + padding * i), module, i));

        for (uint i=0; i<CarlaModule::NUM_OUTPUTS; ++i)
            addOutput(createOutput<PJ301MPort>(Vec(startX_Out, startY + padding * i), module, i));
    }

    ~CarlaModuleWidget() override
    {
        if (module != nullptr && module->fCarlaHostHandle != nullptr)
        {
            module->fUI = nullptr;

            if (visible)
                module->fCarlaPluginDescriptor->ui_show(module->fCarlaPluginHandle, false);

            module->fCarlaHostDescriptor.uiParentId = 0;
            carla_set_engine_option(module->fCarlaHostHandle, ENGINE_OPTION_FRONTEND_WIN_ID, 0, "0");
        }
    }

    void onContextCreate(const ContextCreateEvent& e) override
    {
        ModuleWidget::onContextCreate(e);
        widgetCreated();
    }

    void onContextDestroy(const ContextDestroyEvent& e) override
    {
        widgetDestroyed();
        ModuleWidget::onContextDestroy(e);
    }

    void onAdd(const AddEvent& e) override
    {
        ModuleWidget::onAdd(e);
        widgetCreated();
    }

    void onRemove(const RemoveEvent& e) override
    {
        widgetDestroyed();
        ModuleWidget::onRemove(e);
    }

    void widgetCreated()
    {
        if (module == nullptr || module->pcontext == nullptr || module->fCarlaHostHandle == nullptr)
            return;

        const CarlaHostHandle handle = module->fCarlaHostHandle;
        CardinalPluginContext* const pcontext = module->pcontext;

        char winIdStr[24];
        std::snprintf(winIdStr, sizeof(winIdStr), "%llx", (ulonglong)pcontext->nativeWindowId);

        module->fCarlaHostDescriptor.uiParentId = pcontext->nativeWindowId;
        carla_set_engine_option(handle, ENGINE_OPTION_FRONTEND_WIN_ID, 0, winIdStr);

        if (pcontext->window != nullptr)
            carla_set_engine_option(handle, ENGINE_OPTION_FRONTEND_UI_SCALE, pcontext->window->pixelRatio*1000, nullptr);

        if (! idleCallbackActive)
            idleCallbackActive = pcontext->addIdleCallback(this);

        module->fUI = this;
    }

    void widgetDestroyed()
    {
        if (module == nullptr || module->pcontext == nullptr || module->fCarlaHostHandle == nullptr)
            return;

        const CarlaHostHandle handle = module->fCarlaHostHandle;
        CardinalPluginContext* const pcontext = module->pcontext;

        module->fUI = nullptr;

        if (visible)
        {
            visible = false;
            module->fCarlaPluginDescriptor->ui_show(module->fCarlaPluginHandle, false);
        }

        if (idleCallbackActive)
        {
            idleCallbackActive = false;
            pcontext->removeIdleCallback(this);
        }

        module->fCarlaHostDescriptor.uiParentId = 0;
        carla_set_engine_option(handle, ENGINE_OPTION_FRONTEND_WIN_ID, 0, "0");
    }

    void idleCallback() override
    {
        if (module != nullptr && module->fCarlaHostHandle != nullptr && visible)
            module->fCarlaPluginDescriptor->ui_idle(module->fCarlaPluginHandle);
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
        nvgRoundedRect(args.vg, startX_Out - 4.0f, startY - 2.0f, padding, padding * CarlaModule::NUM_INPUTS, 4);
        nvgFillColor(args.vg, nvgRGB(0xd0, 0xd0, 0xd0));
        nvgFill(args.vg);

        drawTextLine(args.vg, 0, "Audio 1");
        drawTextLine(args.vg, 1, "Audio 2");
        drawTextLine(args.vg, 2, "CV 1");
        drawTextLine(args.vg, 3, "CV 2");
        drawTextLine(args.vg, 4, "CV 3");
        drawTextLine(args.vg, 5, "CV 4");
        drawTextLine(args.vg, 6, "CV 5");
        drawTextLine(args.vg, 7, "CV 6");
        drawTextLine(args.vg, 8, "CV 7");
        drawTextLine(args.vg, 9, "CV 8");

        ModuleWidget::draw(args);
    }

    void showUI()
    {
        visible = true;
        module->fCarlaPluginDescriptor->ui_show(module->fCarlaPluginHandle, true);
    }

    void appendContextMenu(ui::Menu* const menu) override
    {
        menu->addChild(new ui::MenuSeparator);

        menu->addChild(createCheckMenuItem(visible ? "Bring GUI to Front" : "Show GUI", "",
            [=]() {return visible;},
            [=]() {showUI();}
        ));

        menu->addChild(createCheckMenuItem("Bipolar CV Inputs", "",
            [=]() {return module->params[CarlaModule::BIPOLAR_INPUTS].getValue() > 0.1f;},
            [=]() {module->params[CarlaModule::BIPOLAR_INPUTS].setValue(1.0f - module->params[CarlaModule::BIPOLAR_INPUTS].getValue());}
        ));

        menu->addChild(createCheckMenuItem("Bipolar CV Outputs", "",
            [=]() {return module->params[CarlaModule::BIPOLAR_OUTPUTS].getValue() > 0.1f;},
            [=]() {module->params[CarlaModule::BIPOLAR_OUTPUTS].setValue(1.0f - module->params[CarlaModule::BIPOLAR_OUTPUTS].getValue());}
        ));
    }

    void onDoubleClick(const DoubleClickEvent& e) override
    {
        e.consume(this);
        showUI();
    }
};

static void host_ui_closed(NativeHostHandle handle)
{
    if (CarlaModuleWidget* const ui = static_cast<CarlaModuleWidget*>(static_cast<CarlaModule*>(handle)->fUI))
        ui->visible = false;
}
#else
static void host_ui_closed(handle) {}
typedef ModuleWidget CarlaModuleWidget;
#endif


// --------------------------------------------------------------------------------------------------------------------

static void host_ui_parameter_changed(const NativeHostHandle handle, const uint32_t index, const float value)
{
//     if (CarlaWidget* const ui = static_cast<CarlaWidget*>(static_cast<CarlaModule*>(handle)->fUI))
//         ui->changeParameterFromDSP(index, value);
}

static const char* host_ui_open_file(const NativeHostHandle handle,
                                     const bool isDir, const char* const title, const char* const filter)
{
//     if (CarlaWidget* const ui = static_cast<CarlaWidget*>(static_cast<CarlaModule*>(handle)->fUI))
//         ui->openFileFromDSP(isDir, title, filter);

    return nullptr;
}

// --------------------------------------------------------------------------------------------------------------------

Model* modelCarla = createModel<CarlaModule, CarlaModuleWidget>("Carla");

// --------------------------------------------------------------------------------------------------------------------
