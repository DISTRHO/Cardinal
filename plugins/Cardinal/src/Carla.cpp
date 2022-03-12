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
#include "Expander.hpp"
#include "ModuleWidgets.hpp"

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

using namespace CARLA_BACKEND_NAMESPACE;

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

    const CardinalPluginContext* const pcontext;

    const NativePluginDescriptor* fCarlaPluginDescriptor = nullptr;
    NativePluginHandle fCarlaPluginHandle = nullptr;

    NativeHostDescriptor fCarlaHostDescriptor = {};
    CarlaHostHandle fCarlaHostHandle = nullptr;

    NativeTimeInfo fCarlaTimeInfo;

    void* fUI = nullptr;

    float dataIn[NUM_INPUTS][BUFFER_SIZE];
    float dataOut[NUM_OUTPUTS][BUFFER_SIZE];
    float* dataInPtr[NUM_INPUTS];
    float* dataOutPtr[NUM_OUTPUTS];
    unsigned audioDataFill = 0;
    int64_t lastBlockFrame = -1;
    CardinalExpanderFromCarlaMIDIToCV* midiOutExpander = nullptr;
    std::string patchStorage;

#ifdef CARLA_OS_WIN
    // must keep string pointer valid
    std::string winResourceDir;
#endif

    CarlaModule()
        : pcontext(static_cast<CardinalPluginContext*>(APP))
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

        const char* binaryDir = nullptr;
        const char* resourceDir = nullptr;

#if defined(CARLA_OS_MAC)
        if (system::exists("~/Applications/Carla.app"))
        {
            binaryDir = "~/Applications/Carla.app/Contents/MacOS";
            resourceDir = "~/Applications/Carla.app/Contents/MacOS/resources";
        }
        else if (system::exists("/Applications/Carla.app"))
        {
            binaryDir = "/Applications/Carla.app/Contents/MacOS";
            resourceDir = "/Applications/Carla.app/Contents/MacOS/resources";
        }
#elif defined(CARLA_OS_WIN)
        const std::string winBinaryDir = system::join(asset::systemDir, "Carla");

        if (system::exists(winBinaryDir))
        {
            winResourceDir = system::join(winBinaryDir, "resources");
            binaryDir = winBinaryDir.c_str();
            resourceDir = winResourceDir.c_str();
        }
#else
        if (system::exists("/usr/local/lib/carla"))
        {
            binaryDir = "/usr/local/lib/carla";
            resourceDir = "/usr/local/share/carla/resources";
        }
        else if (system::exists("/usr/lib/carla"))
        {
            binaryDir = "/usr/lib/carla";
            resourceDir = "/usr/share/carla/resources";
        }
#endif

        if (binaryDir == nullptr)
        {
            static bool warningShown = false;
            if (! warningShown)
            {
                warningShown = true;
                async_dialog_message("Carla is not installed on this system, the Carla module will do nothing");
            }
            return;
        }

        std::memset(dataOut, 0, sizeof(dataOut));

        fCarlaPluginDescriptor = carla_get_native_patchbay_cv8_plugin();
        DISTRHO_SAFE_ASSERT_RETURN(fCarlaPluginDescriptor != nullptr,);

        memset(&fCarlaHostDescriptor, 0, sizeof(fCarlaHostDescriptor));
        memset(&fCarlaTimeInfo, 0, sizeof(fCarlaTimeInfo));

        fCarlaHostDescriptor.handle = this;
        fCarlaHostDescriptor.resourceDir = resourceDir;
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

        carla_set_engine_option(fCarlaHostHandle, ENGINE_OPTION_PATH_BINARIES, 0, binaryDir);
        carla_set_engine_option(fCarlaHostHandle, ENGINE_OPTION_PATH_RESOURCES, 0, resourceDir);

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

    void process(const ProcessArgs& args) override
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
            const int64_t blockFrame = pcontext->engine->getBlockFrame();

            // Update time position if running a new audio block
            if (lastBlockFrame != blockFrame)
            {
                lastBlockFrame = blockFrame;
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
            // or advance time by BUFFER_SIZE frames if still under the same audio block
            else if (fCarlaTimeInfo.playing)
            {
                fCarlaTimeInfo.frame += BUFFER_SIZE;

                // adjust BBT as well
                if (fCarlaTimeInfo.bbt.valid)
                {
                    const double samplesPerTick = 60.0 * args.sampleRate
                                                / fCarlaTimeInfo.bbt.beatsPerMinute
                                                / fCarlaTimeInfo.bbt.ticksPerBeat;

                    int32_t newBar = fCarlaTimeInfo.bbt.bar;
                    int32_t newBeat = fCarlaTimeInfo.bbt.beat;
                    double newTick = fCarlaTimeInfo.bbt.tick + (double)BUFFER_SIZE / samplesPerTick;

                    while (newTick >= fCarlaTimeInfo.bbt.ticksPerBeat)
                    {
                        newTick -= fCarlaTimeInfo.bbt.ticksPerBeat;

                        if (++newBeat > fCarlaTimeInfo.bbt.beatsPerBar)
                        {
                            newBeat = 1;

                            ++newBar;
                            fCarlaTimeInfo.bbt.barStartTick += fCarlaTimeInfo.bbt.beatsPerBar * fCarlaTimeInfo.bbt.ticksPerBeat;
                        }
                    }

                    fCarlaTimeInfo.bbt.bar = newBar;
                    fCarlaTimeInfo.bbt.beat = newBeat;
                    fCarlaTimeInfo.bbt.tick = newTick;
                }
            }

            NativeMidiEvent* midiEvents;
            uint midiEventCount;

            if (CardinalExpanderFromCVToCarlaMIDI* const midiInExpander = leftExpander.module != nullptr && leftExpander.module->model == modelExpanderInputMIDI
                                                                        ? static_cast<CardinalExpanderFromCVToCarlaMIDI*>(leftExpander.module)
                                                                        : nullptr)
            {
                midiEvents = midiInExpander->midiEvents;
                midiEventCount = midiInExpander->midiEventCount;
                midiInExpander->midiEventCount = midiInExpander->frame = 0;
            }
            else
            {
                midiEvents = nullptr;
                midiEventCount = 0;
            }

            if ((midiOutExpander = rightExpander.module != nullptr && rightExpander.module->model == modelExpanderOutputMIDI
                                 ? static_cast<CardinalExpanderFromCarlaMIDIToCV*>(rightExpander.module)
                                 : nullptr))
                midiOutExpander->midiEventCount = 0;

            audioDataFill = 0;
            fCarlaPluginDescriptor->process(fCarlaPluginHandle, dataInPtr, dataOutPtr, BUFFER_SIZE, midiEvents, midiEventCount);
        }
    }

    void onReset() override
    {
        midiOutExpander = nullptr;
    }

    void onSampleRateChange(const SampleRateChangeEvent& e) override
    {
        if (fCarlaPluginHandle == nullptr)
            return;

        midiOutExpander = nullptr;

        fCarlaPluginDescriptor->deactivate(fCarlaPluginHandle);
        fCarlaPluginDescriptor->dispatcher(fCarlaPluginHandle, NATIVE_PLUGIN_OPCODE_SAMPLE_RATE_CHANGED,
                                           0, 0, nullptr, e.sampleRate);
        fCarlaPluginDescriptor->activate(fCarlaPluginHandle);
    }

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CarlaModule)
};

static_assert((int)CarlaModule::NUM_INPUTS == (int)CarlaModule::NUM_OUTPUTS, "inputs must match outputs");

// -----------------------------------------------------------------------------------------------------------

static uint32_t host_get_buffer_size(const NativeHostHandle handle)
{
    return BUFFER_SIZE;
}

static double host_get_sample_rate(const NativeHostHandle handle)
{
    const CardinalPluginContext* const pcontext = static_cast<CarlaModule*>(handle)->pcontext;
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
    if (CardinalExpanderFromCarlaMIDIToCV* const expander = static_cast<CarlaModule*>(handle)->midiOutExpander)
    {
        if (expander->midiEventCount == CardinalExpanderFromCarlaMIDIToCV::MAX_MIDI_EVENTS)
            return false;

        NativeMidiEvent& expanderEvent(expander->midiEvents[expander->midiEventCount++]);
        carla_copyStruct(expanderEvent, *event);
        return true;
    }

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
struct CarlaModuleWidget : ModuleWidgetWith9HP, IdleCallback {
    CarlaModule* const module;
    bool hasLeftSideExpander = false;
    bool hasRightSideExpander = false;
    bool idleCallbackActive = false;
    bool visible = false;

    CarlaModuleWidget(CarlaModule* const m)
        : module(m)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Carla.svg")));

        createAndAddScrews();

        for (uint i=0; i<CarlaModule::NUM_INPUTS; ++i)
            createAndAddInput(i);

        for (uint i=0; i<CarlaModule::NUM_OUTPUTS; ++i)
            createAndAddOutput(i);
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
        const CardinalPluginContext* const pcontext = module->pcontext;

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
        const CardinalPluginContext* const pcontext = module->pcontext;

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

    void draw(const DrawArgs& args) override
    {
        drawBackground(args.vg);

        if (hasLeftSideExpander)
        {
            nvgBeginPath(args.vg);
            nvgRect(args.vg, 1, 90 - 19, 18, 49 * 6 - 4);
            nvgFillPaint(args.vg, nvgLinearGradient(args.vg, 0, 0, 18, 0, nvgRGB(0xd0, 0xd0, 0xd0), nvgRGBA(0xd0, 0xd0, 0xd0, 0)));
            nvgFill(args.vg);

            for (int i=1; i<6; ++i)
            {
                const float y = 90 + 49 * i - 23;
                const int col1 = 0x18 + static_cast<int>((y / box.size.y) * (0x21 - 0x18) + 0.5f);
                const int col2 = 0x19 + static_cast<int>((y / box.size.y) * (0x22 - 0x19) + 0.5f);
                nvgBeginPath(args.vg);
                nvgRect(args.vg, 1, y, 18, 4);
                nvgFillColor(args.vg, nvgRGB(col1, col2, col2));
                nvgFill(args.vg);
            }
        }

        if (hasRightSideExpander)
        {
            nvgFillColor(args.vg, nvgRGB(0xd0, 0xd0, 0xd0));

            for (int i=0; i<6; ++i)
            {
                const float y = 90 + 49 * i - 19;
                nvgBeginPath(args.vg);
                nvgRect(args.vg, box.size.x - 19, y, 18, 49 - 4);
                nvgFill(args.vg);
            }
        }

        drawOutputJacksArea(args.vg, CarlaModule::NUM_INPUTS);

        setupTextLines(args.vg);

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

        ModuleWidgetWith9HP::draw(args);
    }

    void step() override
    {
        hasLeftSideExpander = module != nullptr
                            && module->leftExpander.module != nullptr
                            && module->leftExpander.module->model == modelExpanderInputMIDI;

        hasRightSideExpander = module != nullptr
                             && module->rightExpander.module != nullptr
                             && module->rightExpander.module->model == modelExpanderOutputMIDI;

        ModuleWidgetWith9HP::step();
    }

    void showUI()
    {
        visible = true;
        module->fCarlaPluginDescriptor->ui_show(module->fCarlaPluginHandle, true);
    }

    void appendContextMenu(ui::Menu* const menu) override
    {
        if (module == nullptr || module->pcontext == nullptr || module->fCarlaHostHandle == nullptr)
            return;

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
        if (module == nullptr || module->pcontext == nullptr || module->fCarlaHostHandle == nullptr)
            return;

        e.consume(this);
        showUI();
    }

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CarlaModuleWidget)
};

static void host_ui_closed(NativeHostHandle handle)
{
    if (CarlaModuleWidget* const ui = static_cast<CarlaModuleWidget*>(static_cast<CarlaModule*>(handle)->fUI))
        ui->visible = false;
}
#else
static void host_ui_closed(NativeHostHandle) {}
struct CarlaModuleWidget : ModuleWidget {
    CarlaModuleWidget(CarlaModule* const module) {
        setModule(module);

        for (uint i=0; i<CarlaModule::NUM_INPUTS; ++i)
            addInput(createInput<PJ301MPort>({}, module, i));

        for (uint i=0; i<CarlaModule::NUM_OUTPUTS; ++i)
            addOutput(createOutput<PJ301MPort>({}, module, i));
    }
};
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
