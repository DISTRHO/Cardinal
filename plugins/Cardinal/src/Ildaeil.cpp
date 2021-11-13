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

#ifndef HEADLESS
# include "ImGuiWidget.hpp"
# include "extra/ScopedPointer.hpp"
# include "extra/Thread.hpp"
#endif

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
// strcasestr

#ifdef DISTRHO_OS_WINDOWS
# include <shlwapi.h>
namespace ildaeil {
    inline const char* strcasestr(const char* const haystack, const char* const needle)
    {
        return StrStrIA(haystack, needle);
    }
    // using strcasestr = StrStrIA;
}
#else
namespace ildaeil {
    using ::strcasestr;
}
#endif

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

struct IldaeilModule : Module {
    enum ParamIds {
        NUM_PARAMS
    };
    enum InputIds {
        INPUT1,
        INPUT2,
        NUM_INPUTS
    };
    enum OutputIds {
        OUTPUT1,
        OUTPUT2,
        NUM_OUTPUTS
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
    // mutable water::MemoryOutputStream fLastProjectState;

    void* fUI = nullptr;

    float audioDataIn1[BUFFER_SIZE];
    float audioDataIn2[BUFFER_SIZE];
    float audioDataOut1[BUFFER_SIZE];
    float audioDataOut2[BUFFER_SIZE];
    unsigned audioDataFill = 0;

    IldaeilModule()
        : pcontext(reinterpret_cast<CardinalPluginContext*>(APP))
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        std::memset(audioDataOut1, 0, sizeof(audioDataOut1));
        std::memset(audioDataOut2, 0, sizeof(audioDataOut2));

        fCarlaPluginDescriptor = carla_get_native_rack_plugin();
        DISTRHO_SAFE_ASSERT_RETURN(fCarlaPluginDescriptor != nullptr,);

        memset(&fCarlaHostDescriptor, 0, sizeof(fCarlaHostDescriptor));
        memset(&fCarlaTimeInfo, 0, sizeof(fCarlaTimeInfo));

        fCarlaHostDescriptor.handle = this;
        fCarlaHostDescriptor.resourceDir = carla_get_library_folder();
        fCarlaHostDescriptor.uiName = "Ildaeil";
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

        if (const char* const path = std::getenv("LV2_PATH"))
            carla_set_engine_option(fCarlaHostHandle, ENGINE_OPTION_PLUGIN_PATH, PLUGIN_LV2, path);

        fCarlaPluginDescriptor->dispatcher(fCarlaPluginHandle, NATIVE_PLUGIN_OPCODE_HOST_USES_EMBED,
                                           0, 0, nullptr, 0.0f);

        fCarlaPluginDescriptor->activate(fCarlaPluginHandle);
    }

    ~IldaeilModule() override
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
        case NATIVE_HOST_OPCODE_GET_FILE_PATH:
        case NATIVE_HOST_OPCODE_UI_RESIZE:
        case NATIVE_HOST_OPCODE_PREVIEW_BUFFER_DATA:
            // TESTING
            d_stdout("dispatcher %i, %i, %li, %p, %f", opcode, index, value, ptr, opt);
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

    void process(const ProcessArgs&) override
    {
        if (fCarlaPluginHandle == nullptr)
            return;

        const unsigned i = audioDataFill++;

        audioDataIn1[i] = inputs[INPUT1].getVoltage() * 0.1f;
        audioDataIn2[i] = inputs[INPUT2].getVoltage() * 0.1f;
        outputs[OUTPUT1].setVoltage(audioDataOut1[i] * 10.0f);
        outputs[OUTPUT2].setVoltage(audioDataOut2[i] * 10.0f);

        if (audioDataFill == BUFFER_SIZE)
        {
            audioDataFill = 0;
            float* ins[2] = { audioDataIn1, audioDataIn2 };
            float* outs[2] = { audioDataOut1, audioDataOut2 };
            fCarlaPluginDescriptor->process(fCarlaPluginHandle, ins, outs, BUFFER_SIZE, nullptr, 0);
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

// -----------------------------------------------------------------------------------------------------------

static uint32_t host_get_buffer_size(const NativeHostHandle handle)
{
    return BUFFER_SIZE;
}

static double host_get_sample_rate(const NativeHostHandle handle)
{
    CardinalPluginContext* const pcontext = static_cast<IldaeilModule*>(handle)->pcontext;
    DISTRHO_SAFE_ASSERT_RETURN(pcontext != nullptr, 48000.0);
    return pcontext->sampleRate;
}

static bool host_is_offline(NativeHostHandle)
{
    return false;
}

static const NativeTimeInfo* host_get_time_info(const NativeHostHandle handle)
{
    return static_cast<IldaeilModule*>(handle)->hostGetTimeInfo();
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

static void host_ui_closed(NativeHostHandle handle)
{
    d_stdout("%s %p", __FUNCTION__, handle);
}

static const char* host_ui_save_file(NativeHostHandle, bool, const char*, const char*)
{
    return nullptr;
}

static intptr_t host_dispatcher(const NativeHostHandle handle, const NativeHostDispatcherOpcode opcode,
                                const int32_t index, const intptr_t value, void* const ptr, const float opt)
{
    return static_cast<IldaeilModule*>(handle)->hostDispatcher(opcode, index, value, ptr, opt);
}

// --------------------------------------------------------------------------------------------------------------------

#ifndef HEADLESS
struct IldaeilWidget : ImGuiWidget, IdleCallback, Thread {
    static constexpr const uint kButtonHeight = 20;

    struct PluginInfoCache {
        char* name;
        char* label;

        PluginInfoCache()
            : name(nullptr),
              label(nullptr) {}

        ~PluginInfoCache()
        {
            std::free(name);
            std::free(label);
        }
    };

    struct PluginGenericUI {
        char* title;
        uint parameterCount;
        struct Parameter {
            char* name;
            char* format;
            uint32_t rindex;
            bool boolean, bvalue, log;
            float min, max, power;
            Parameter()
                : name(nullptr),
                  format(nullptr),
                  rindex(0),
                  boolean(false),
                  bvalue(false),
                  log(false),
                  min(0.0f),
                  max(1.0f) {}
            ~Parameter()
            {
                std::free(name);
                std::free(format);
            }
        }* parameters;
        float* values;

        PluginGenericUI()
            : title(nullptr),
              parameterCount(0),
              parameters(nullptr),
              values(nullptr) {}

        ~PluginGenericUI()
        {
            std::free(title);
            delete[] parameters;
            delete[] values;
        }
    };

    enum {
        kDrawingInit,
        kDrawingErrorInit,
        kDrawingErrorDraw,
        kDrawingLoading,
        kDrawingPluginList,
        kDrawingPluginGenericUI,
        kDrawingPluginPendingFromInit
    } fDrawingState = kDrawingInit;

    PluginType fPluginType = PLUGIN_LV2;
    uint fPluginCount = 0;
    uint fPluginSelected = false;
    bool fPluginScanningFinished = false;
    bool fPluginHasCustomUI = false;
    bool fPluginWillRunInBridgeMode = false;
    PluginInfoCache* fPlugins = nullptr;
    ScopedPointer<PluginGenericUI> fPluginGenericUI;

    bool fPluginSearchActive = false;
    bool fPluginSearchFirstShow = false;
    char fPluginSearchString[0xff] = {};

    String fPopupError;

    bool idleCallbackActive = false;
    IldaeilModule* const module;

    IldaeilWidget(IldaeilModule* const m)
        : ImGuiWidget(),
          module(m)
    {
        if (module->fCarlaHostHandle == nullptr)
        {
            fDrawingState = kDrawingErrorInit;
            fPopupError = "Ildaeil backend failed to init properly, cannot continue.";
            return;
        }

        std::strcpy(fPluginSearchString, "Search...");

        ImGuiStyle& style(ImGui::GetStyle());
        style.FrameRounding = 4;

        const CarlaHostHandle handle = module->fCarlaHostHandle;

        if (carla_get_current_plugin_count(handle) != 0)
        {
            const uint hints = carla_get_plugin_info(handle, 0)->hints;
            fDrawingState = kDrawingPluginPendingFromInit;
            fPluginHasCustomUI = hints & PLUGIN_HAS_CUSTOM_UI;
        }

        module->fUI = this;
    }

    ~IldaeilWidget() override
    {
        if (module->fCarlaHostHandle != nullptr)
        {
            module->fUI = nullptr;
            carla_set_engine_option(module->fCarlaHostHandle, ENGINE_OPTION_FRONTEND_WIN_ID, 0, "0");

            module->pcontext->removeIdleCallback(this);
        }

        if (isThreadRunning())
            stopThread(-1);

        hidePluginUI();

        fPluginGenericUI = nullptr;

        delete[] fPlugins;
    }

    void changeParameterFromDSP(const uint32_t index, const float value)
    {
        if (PluginGenericUI* const ui = fPluginGenericUI)
        {
            for (uint32_t i=0; i < ui->parameterCount; ++i)
            {
                if (ui->parameters[i].rindex != index)
                    continue;

                ui->values[i] = value;

                if (ui->parameters[i].boolean)
                    ui->parameters[i].bvalue = value > ui->parameters[i].min;

                break;
            }
        }
    }

    void openFileFromDSP(const bool isDir, const char* const title, const char* const filter)
    {
        /*
        Window::FileBrowserOptions opts;
        opts.title = title;
        getWindow().openFileBrowser(opts);
        */
    }

    void showPluginUI(const CarlaHostHandle handle)
    {
        const CarlaPluginInfo* const info = carla_get_plugin_info(handle, 0);

        fDrawingState = kDrawingPluginGenericUI;
        fPluginHasCustomUI = info->hints & PLUGIN_HAS_CUSTOM_UI;
        if (fPluginGenericUI == nullptr)
            createPluginGenericUI(handle, info);
        else
            updatePluginGenericUI(handle);

        setDirty(true);
    }

    void hidePluginUI()
    {
        if (module->fCarlaHostHandle == nullptr)
            return;

        if (fDrawingState == kDrawingPluginGenericUI)
            carla_show_custom_ui(module->fCarlaHostHandle, 0, false);
    }

    void createPluginGenericUI(const CarlaHostHandle handle, const CarlaPluginInfo* const info)
    {
        PluginGenericUI* const ui = new PluginGenericUI;

        String title(info->name);
        title += " by ";
        title += info->maker;
        ui->title = title.getAndReleaseBuffer();

        const uint32_t pcount = ui->parameterCount = carla_get_parameter_count(handle, 0);

        // make count of valid parameters
        for (uint32_t i=0; i < pcount; ++i)
        {
            const ParameterData* const pdata = carla_get_parameter_data(handle, 0, i);

            if (pdata->type != PARAMETER_INPUT ||
                (pdata->hints & PARAMETER_IS_ENABLED) == 0x0 ||
                (pdata->hints & PARAMETER_IS_READ_ONLY) != 0x0)
            {
                --ui->parameterCount;
                continue;
            }
        }

        ui->parameters = new PluginGenericUI::Parameter[ui->parameterCount];
        ui->values = new float[ui->parameterCount];

        // now safely fill in details
        for (uint32_t i=0, j=0; i < pcount; ++i)
        {
            const ParameterData* const pdata = carla_get_parameter_data(handle, 0, i);

            if (pdata->type != PARAMETER_INPUT ||
                (pdata->hints & PARAMETER_IS_ENABLED) == 0x0 ||
                (pdata->hints & PARAMETER_IS_READ_ONLY) != 0x0)
                continue;

            const CarlaParameterInfo* const pinfo = carla_get_parameter_info(handle, 0, i);
            const ::ParameterRanges* const pranges = carla_get_parameter_ranges(handle, 0, i);

            String format;

            if (pdata->hints & PARAMETER_IS_INTEGER)
                format = "%.0f ";
            else
                format = "%.3f ";

            format += pinfo->unit;

            PluginGenericUI::Parameter& param(ui->parameters[j]);
            param.name = strdup(pinfo->name);
            param.format = format.getAndReleaseBuffer();
            param.rindex = i;
            param.boolean = pdata->hints & PARAMETER_IS_BOOLEAN;
            param.log = pdata->hints & PARAMETER_IS_LOGARITHMIC;
            param.min = pranges->min;
            param.max = pranges->max;
            ui->values[j] = carla_get_current_parameter_value(handle, 0, i);

            if (param.boolean)
                param.bvalue = ui->values[j] > param.min;
            else
                param.bvalue = false;

            ++j;
        }

        fPluginGenericUI = ui;
    }

    void updatePluginGenericUI(const CarlaHostHandle handle)
    {
        PluginGenericUI* const ui = fPluginGenericUI;
        DISTRHO_SAFE_ASSERT_RETURN(ui != nullptr,);

        for (uint32_t i=0; i < ui->parameterCount; ++i)
        {
            ui->values[i] = carla_get_current_parameter_value(handle, 0, ui->parameters[i].rindex);

            if (ui->parameters[i].boolean)
                ui->parameters[i].bvalue = ui->values[i] > ui->parameters[i].min;
        }
    }

    bool loadPlugin(const CarlaHostHandle handle, const char* const label)
    {
        if (carla_get_current_plugin_count(handle) != 0)
        {
            hidePluginUI();
            carla_replace_plugin(handle, 0);
        }

        carla_set_engine_option(handle, ENGINE_OPTION_PREFER_PLUGIN_BRIDGES, fPluginWillRunInBridgeMode, nullptr);

        if (carla_add_plugin(handle, BINARY_NATIVE, fPluginType, nullptr, nullptr,
                             label, 0, 0x0, PLUGIN_OPTIONS_NULL))
        {
            fPluginGenericUI = nullptr;
            showPluginUI(handle);
            return true;
        }
        else
        {
            fPopupError = carla_get_last_error(handle);
            d_stdout("got error: %s", fPopupError.buffer());
            ImGui::OpenPopup("Plugin Error");
        }

        return false;
    }

    void onContextCreate(const ContextCreateEvent& e) override
    {
        ImGuiWidget::onContextCreate(e);
        widgetCreated();
    }

    void onContextDestroy(const ContextDestroyEvent& e) override
    {
        widgetDestroyed();
        ImGuiWidget::onContextDestroy(e);
    }

    void onAdd(const AddEvent& e) override
    {
        ImGuiWidget::onAdd(e);
        widgetCreated();
    }

    void onRemove(const RemoveEvent& e) override
    {
        widgetDestroyed();
        ImGuiWidget::onRemove(e);
    }

    void widgetCreated()
    {
        if (const CarlaHostHandle handle = module->fCarlaHostHandle)
        {
            CardinalPluginContext* const pcontext = module->pcontext;

            char winIdStr[24];
            std::snprintf(winIdStr, sizeof(winIdStr), "%llx", (ulonglong)pcontext->nativeWindowId);

            module->fCarlaHostDescriptor.uiParentId = pcontext->nativeWindowId;
            carla_set_engine_option(handle, ENGINE_OPTION_FRONTEND_WIN_ID, 0, winIdStr);

            if (pcontext->window != nullptr)
                carla_set_engine_option(handle, ENGINE_OPTION_FRONTEND_UI_SCALE, pcontext->window->pixelRatio*1000, nullptr);

            if (! idleCallbackActive)
                idleCallbackActive = pcontext->addIdleCallback(this);
        }
    }

    void widgetDestroyed()
    {
        if (const CarlaHostHandle handle = module->fCarlaHostHandle)
        {
            CardinalPluginContext* const pcontext = module->pcontext;

            module->fCarlaHostDescriptor.uiParentId = 0;
            carla_set_engine_option(handle, ENGINE_OPTION_FRONTEND_WIN_ID, 0, "0");

            if (idleCallbackActive)
            {
                idleCallbackActive = false;
                pcontext->removeIdleCallback(this);
            }
        }
    }

    void idleCallback() override
    {
        switch (fDrawingState)
        {
        case kDrawingInit:
            fDrawingState = kDrawingLoading;
            startThread();
            break;

        case kDrawingPluginPendingFromInit:
            showPluginUI(module->fCarlaHostHandle);
            startThread();
            break;

        case kDrawingPluginGenericUI:
            module->fCarlaPluginDescriptor->ui_idle(module->fCarlaPluginHandle);
            break;

        default:
            break;
        }
    }

    /*
    void uiFileBrowserSelected(const char* const filename) override
    {
        if (fPlugin != nullptr && fPlugin->fCarlaHostHandle != nullptr && filename != nullptr)
            carla_set_custom_data(fPlugin->fCarlaHostHandle, 0, CUSTOM_DATA_TYPE_STRING, "file", filename);
    }
    */

    void run() override
    {
        const char* path;

        switch (fPluginType)
        {
        case PLUGIN_LV2:
            path = std::getenv("LV2_PATH");
            break;
        default:
            path = nullptr;
            break;
        }

        if (path != nullptr)
            carla_set_engine_option(module->fCarlaHostHandle, ENGINE_OPTION_PLUGIN_PATH, fPluginType, path);

        if (const uint count = carla_get_cached_plugin_count(fPluginType, nullptr))
        {
            fPluginCount = 0;
            fPlugins = new PluginInfoCache[count];

            if (fDrawingState == kDrawingLoading)
            {
                fDrawingState = kDrawingPluginList;
                fPluginSearchFirstShow = true;
            }

            for (uint i=0, j; i < count && ! shouldThreadExit(); ++i)
            {
                const CarlaCachedPluginInfo* const info = carla_get_cached_plugin_info(fPluginType, i);
                DISTRHO_SAFE_ASSERT_CONTINUE(info != nullptr);

                if (! info->valid)
                    continue;
                if (info->audioIns != 2 || info->audioOuts != 2)
                    continue;

                j = fPluginCount;
                fPlugins[j].name = strdup(info->name);
                fPlugins[j].label = strdup(info->label);
                ++fPluginCount;
            }
        }
        else
        {
            String error("There are no ");
            error += getPluginTypeAsString(fPluginType);
            error += " audio plugins on this system.";
            fPopupError = error;
            fDrawingState = kDrawingErrorInit;
        }

        if (! shouldThreadExit())
            fPluginScanningFinished = true;
    }

    void drawImGui() override
    {
        switch (fDrawingState)
        {
        case kDrawingInit:
        case kDrawingLoading:
        case kDrawingPluginPendingFromInit:
            drawLoading();
            break;
        case kDrawingPluginList:
            drawPluginList();
            break;
        case kDrawingErrorInit:
            fDrawingState = kDrawingErrorDraw;
            drawError(true);
            break;
        case kDrawingErrorDraw:
            drawError(false);
            break;
        case kDrawingPluginGenericUI:
            drawTopBar();
            drawGenericUI();
            break;
        }
    }

    void drawError(const bool open)
    {
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(box.size.x, box.size.y));

        const int flags = ImGuiWindowFlags_NoSavedSettings
                        | ImGuiWindowFlags_NoTitleBar
                        | ImGuiWindowFlags_NoResize
                        | ImGuiWindowFlags_NoCollapse
                        | ImGuiWindowFlags_NoScrollbar
                        | ImGuiWindowFlags_NoScrollWithMouse
                        | ImGuiWindowFlags_NoCollapse;

        if (ImGui::Begin("Error Window", nullptr, flags))
        {
            if (open)
                ImGui::OpenPopup("Engine Error");

            const int pflags = ImGuiWindowFlags_NoSavedSettings
                             | ImGuiWindowFlags_NoResize
                             | ImGuiWindowFlags_NoCollapse
                             | ImGuiWindowFlags_NoScrollbar
                             | ImGuiWindowFlags_NoScrollWithMouse
                             | ImGuiWindowFlags_NoCollapse
                             | ImGuiWindowFlags_AlwaysAutoResize
                             | ImGuiWindowFlags_AlwaysUseWindowPadding;

            if (ImGui::BeginPopupModal("Engine Error", nullptr, pflags))
            {
                ImGui::TextUnformatted(fPopupError.buffer(), nullptr);
                ImGui::EndPopup();
            }
        }

        ImGui::End();
    }

    void drawTopBar()
    {
        const float padding = ImGui::GetStyle().WindowPadding.y * 2;

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(box.size.x, kButtonHeight + padding));

        const int flags = ImGuiWindowFlags_NoSavedSettings
                        | ImGuiWindowFlags_NoTitleBar
                        | ImGuiWindowFlags_NoResize
                        | ImGuiWindowFlags_NoCollapse
                        | ImGuiWindowFlags_NoScrollbar
                        | ImGuiWindowFlags_NoScrollWithMouse
                        | ImGuiWindowFlags_NoCollapse;

        if (ImGui::Begin("Current Plugin", nullptr, flags))
        {
            const CarlaHostHandle handle = module->fCarlaHostHandle;

            if (ImGui::Button("Pick Another..."))
            {
                hidePluginUI();
                fDrawingState = kDrawingPluginList;
            }

            ImGui::SameLine();

            if (ImGui::Button("Reset"))
            {
                loadPlugin(handle, carla_get_plugin_info(handle, 0)->label);
            }

            if (fDrawingState == kDrawingPluginGenericUI && fPluginHasCustomUI)
            {
                ImGui::SameLine();

                if (ImGui::Button("Show Custom GUI"))
                {
                    carla_show_custom_ui(handle, 0, true);
                    ImGui::End();
                }
            }
        }

        ImGui::End();
    }

    void setupMainWindowPos()
    {
        float y = 0;
        float height = box.size.y;

        if (fDrawingState == kDrawingPluginGenericUI)
        {
            y = kButtonHeight + ImGui::GetStyle().WindowPadding.y * 2 - 1;
            height -= y;
        }

        ImGui::SetNextWindowPos(ImVec2(0, y));
        ImGui::SetNextWindowSize(ImVec2(box.size.x, height));
    }

    void drawGenericUI()
    {
        setupMainWindowPos();

        PluginGenericUI* const ui = fPluginGenericUI;
        DISTRHO_SAFE_ASSERT_RETURN(ui != nullptr,);

        // ImGui::SetNextWindowFocus();

        if (ImGui::Begin(ui->title, nullptr, ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse))
        {
            const CarlaHostHandle handle = module->fCarlaHostHandle;

            for (uint32_t i=0; i < ui->parameterCount; ++i)
            {
                PluginGenericUI::Parameter& param(ui->parameters[i]);

                if (param.boolean)
                {
                    if (ImGui::Checkbox(param.name, &ui->parameters[i].bvalue))
                    {
                        if (ImGui::IsItemActivated())
                        {
                            carla_set_parameter_touch(handle, 0, param.rindex, true);
                            // editParameter(0, true);
                        }

                        ui->values[i] = ui->parameters[i].bvalue ? ui->parameters[i].max : ui->parameters[i].min;
                        carla_set_parameter_value(handle, 0, param.rindex, ui->values[i]);
                        // setParameterValue(0, ui->values[i]);
                    }
                }
                else
                {
                    const bool ret = param.log
                                   ? ImGui::SliderFloat(param.name, &ui->values[i], param.min, param.max, param.format, 2.0f)
                                   : ImGui::SliderFloat(param.name, &ui->values[i], param.min, param.max, param.format);
                    if (ret)
                    {
                        if (ImGui::IsItemActivated())
                        {
                            carla_set_parameter_touch(handle, 0, param.rindex, true);
                            // editParameter(0, true);
                        }

                        carla_set_parameter_value(handle, 0, param.rindex, ui->values[i]);
                        // setParameterValue(0, ui->values[i]);
                    }
                }

                if (ImGui::IsItemDeactivated())
                {
                    carla_set_parameter_touch(handle, 0, param.rindex, false);
                    // editParameter(0, false);
                }
            }
        }

        ImGui::End();
    }

    void drawLoading()
    {
        setupMainWindowPos();

        if (ImGui::Begin("Plugin List", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize))
        {
            ImGui::TextUnformatted("Loading...", nullptr);
        }

        ImGui::End();
    }

    void drawPluginList()
    {
        setupMainWindowPos();

        const CarlaHostHandle handle = module->fCarlaHostHandle;

        if (ImGui::Begin("Plugin List", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize))
        {
            const int pflags = ImGuiWindowFlags_NoSavedSettings
                             | ImGuiWindowFlags_NoResize
                             | ImGuiWindowFlags_NoCollapse
                             | ImGuiWindowFlags_NoScrollbar
                             | ImGuiWindowFlags_NoScrollWithMouse
                             | ImGuiWindowFlags_NoCollapse
                             | ImGuiWindowFlags_AlwaysAutoResize
                             | ImGuiWindowFlags_AlwaysUseWindowPadding;

            if (ImGui::BeginPopupModal("Plugin Error", nullptr, pflags))
            {
                ImGui::TextWrapped("Failed to load plugin, error was:\n%s", fPopupError.buffer());

                ImGui::Separator();

                if (ImGui::Button("Ok"))
                {
                    ImGui::CloseCurrentPopup();
                }

                ImGui::SameLine();
                ImGui::Dummy(ImVec2(500, 1));
                ImGui::EndPopup();
            }
            else if (fPluginSearchFirstShow)
            {
                fPluginSearchFirstShow = false;
                ImGui::SetKeyboardFocusHere();
            }

            if (ImGui::InputText("", fPluginSearchString, sizeof(fPluginSearchString)-1, ImGuiInputTextFlags_CharsNoBlank|ImGuiInputTextFlags_AutoSelectAll))
                fPluginSearchActive = true;

            if (ImGui::IsKeyDown(ImGuiKey_Escape))
                fPluginSearchActive = false;

            ImGui::BeginDisabled(!fPluginScanningFinished);

            if (ImGui::Button("Load Plugin"))
            {
                do {
                    const PluginInfoCache& info(fPlugins[fPluginSelected]);

                    const char* label = nullptr;

                    switch (fPluginType)
                    {
                    case PLUGIN_INTERNAL:
                    // case PLUGIN_JSFX:
                    case PLUGIN_SFZ:
                        label = info.label;
                        break;
                    case PLUGIN_LV2: {
                        const char* const slash = std::strchr(info.label, DISTRHO_OS_SEP);
                        DISTRHO_SAFE_ASSERT_BREAK(slash != nullptr);
                        label = slash+1;
                        break;
                    }
                    default:
                        break;
                    }

                    DISTRHO_SAFE_ASSERT_BREAK(label != nullptr);

                    d_stdout("Loading %s...", info.name);

                    if (loadPlugin(handle, label))
                    {
                        ImGui::EndDisabled();
                        ImGui::End();
                        return;
                    }
                } while (false);
            }

            ImGui::SameLine();
            ImGui::Checkbox("Run in bridge mode", &fPluginWillRunInBridgeMode);

            if (carla_get_current_plugin_count(handle) != 0)
            {
                ImGui::SameLine();

                if (ImGui::Button("Cancel"))
                {
                    showPluginUI(handle);
                }
            }

            ImGui::EndDisabled();

            if (ImGui::BeginChild("pluginlistwindow"))
            {
                if (ImGui::BeginTable("pluginlist", 2, ImGuiTableFlags_NoSavedSettings))
                {
                    const char* const search = fPluginSearchActive && fPluginSearchString[0] != '\0' ? fPluginSearchString : nullptr;

                    switch (fPluginType)
                    {
                    case PLUGIN_INTERNAL:
                    // case PLUGIN_JSFX:
                    case PLUGIN_SFZ:
                        ImGui::TableSetupColumn("Name");
                        ImGui::TableSetupColumn("Label");
                        ImGui::TableHeadersRow();
                        break;
                    case PLUGIN_LV2:
                        ImGui::TableSetupColumn("Name");
                        ImGui::TableSetupColumn("URI");
                        ImGui::TableHeadersRow();
                        break;
                    default:
                        break;
                    }

                    for (uint i=0; i<fPluginCount; ++i)
                    {
                        const PluginInfoCache& info(fPlugins[i]);

                        if (search != nullptr && ildaeil::strcasestr(info.name, search) == nullptr)
                            continue;

                        bool selected = fPluginSelected == i;

                        switch (fPluginType)
                        {
                        case PLUGIN_INTERNAL:
                        // case PLUGIN_JSFX:
                        case PLUGIN_SFZ:
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::Selectable(info.name, &selected);
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Selectable(info.label, &selected);
                            break;
                        case PLUGIN_LV2: {
                            const char* const slash = std::strchr(info.label, DISTRHO_OS_SEP);
                            DISTRHO_SAFE_ASSERT_CONTINUE(slash != nullptr);
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::Selectable(info.name, &selected);
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Selectable(slash+1, &selected);
                            break;
                        }
                        default:
                            break;
                        }

                        if (selected)
                            fPluginSelected = i;
                    }

                    ImGui::EndTable();
                }
                ImGui::EndChild();
            }
        }

        ImGui::End();
    }
};

// --------------------------------------------------------------------------------------------------------------------

static void host_ui_parameter_changed(const NativeHostHandle handle, const uint32_t index, const float value)
{
    if (IldaeilWidget* const ui = static_cast<IldaeilWidget*>(static_cast<IldaeilModule*>(handle)->fUI))
        ui->changeParameterFromDSP(index, value);
}

static const char* host_ui_open_file(const NativeHostHandle handle,
                                     const bool isDir, const char* const title, const char* const filter)
{
    if (IldaeilWidget* const ui = static_cast<IldaeilWidget*>(static_cast<IldaeilModule*>(handle)->fUI))
        ui->openFileFromDSP(isDir, title, filter);

    return nullptr;
}

// --------------------------------------------------------------------------------------------------------------------

struct IldaeilModuleWidget : ModuleWidget {
    IldaeilWidget* ildaeilWidget = nullptr;

    IldaeilModuleWidget(IldaeilModule* const module)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Ildaeil.svg")));

        if (module != nullptr && module->pcontext != nullptr)
        {
            ildaeilWidget = new IldaeilWidget(module);
            ildaeilWidget->box.pos = Vec(2 * RACK_GRID_WIDTH, 0);
            ildaeilWidget->box.size = Vec(box.size.x - 2 * RACK_GRID_WIDTH, box.size.y);
            addChild(ildaeilWidget);
        }

        addChild(createWidget<ScrewBlack>(Vec(0, 0)));
        addChild(createWidget<ScrewBlack>(Vec(0, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addInput(createInput<PJ301MPort>(Vec(3, 54), module, IldaeilModule::INPUT1));
        addInput(createInput<PJ301MPort>(Vec(3, 54 + 30), module, IldaeilModule::INPUT2));
        addOutput(createOutput<PJ301MPort>(Vec(3, 54 + 60), module, IldaeilModule::OUTPUT1));
        addOutput(createOutput<PJ301MPort>(Vec(3, 54 + 90), module, IldaeilModule::OUTPUT2));
    }
};
#else
static void host_ui_parameter_changed(NativeHostHandle, uint32_t, float) {}
static const char* host_ui_open_file(NativeHostHandle, bool, const char*, const char*) { return nullptr; }
typedef ModuleWidget IldaeilModuleWidget;
#endif

// --------------------------------------------------------------------------------------------------------------------

Model* modelIldaeil = createModel<IldaeilModule, IldaeilModuleWidget>("Ildaeil");

// --------------------------------------------------------------------------------------------------------------------
