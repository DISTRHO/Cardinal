/*
 * DISTRHO Cardinal Plugin
 * Copyright (C) 2021-2023 Filipe Coelho <falktx@falktx.com>
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

/**
 * This file is partially based on VCVRack's patch.cpp
 * Copyright (C) 2016-2021 VCV.
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 */

#include "CardinalCommon.hpp"

#include "AsyncDialog.hpp"
#include "PluginContext.hpp"
#include "DistrhoPluginUtils.hpp"

#include <asset.hpp>
#include <context.hpp>
#include <history.hpp>
#include <patch.hpp>
#include <settings.hpp>
#include <string.hpp>
#include <system.hpp>
#include <app/Browser.hpp>
#include <app/Scene.hpp>
#include <engine/Engine.hpp>
#include <window/Window.hpp>

#ifndef DISTRHO_PLUGIN_WANT_DIRECT_ACCESS
# error wrong build
#endif

#if (defined(STATIC_BUILD) && !defined(__MOD_DEVICES__)) || CARDINAL_VARIANT_MINI
# undef CARDINAL_INIT_OSC_THREAD
#endif

#ifdef NDEBUG
# undef DEBUG
#endif

// for finding special paths
#ifdef ARCH_WIN
# include <shlobj.h>
#else
# include <pwd.h>
# include <unistd.h>
#endif

#ifdef CARDINAL_INIT_OSC_THREAD
# include <lo/lo.h>
#endif

#ifdef DISTRHO_OS_WASM
# include <emscripten/emscripten.h>
#endif

#if defined(CARDINAL_COMMON_DSP_ONLY) || defined(HEADLESS)
# define HEADLESS_BEHAVIOUR
#endif

#if CARDINAL_VARIANT_FX
# define CARDINAL_VARIANT_NAME "fx"
#elif CARDINAL_VARIANT_MINI
# define CARDINAL_VARIANT_NAME "mini"
#elif CARDINAL_VARIANT_NATIVE
# define CARDINAL_VARIANT_NAME "native"
#elif CARDINAL_VARIANT_SYNTH
# define CARDINAL_VARIANT_NAME "synth"
#else
# define CARDINAL_VARIANT_NAME "main"
#endif

#ifdef DISTRHO_OS_WASM
# if CARDINAL_VARIANT_MINI
#  define CARDINAL_WASM_WELCOME_TEMPLATE_FILENAME "welcome-wasm-mini"
# else
#  define CARDINAL_WASM_WELCOME_TEMPLATE_FILENAME "welcome-wasm"
# endif
#endif

namespace rack {
namespace asset {
std::string patchesPath();
void destroy();
}
namespace plugin {
void initStaticPlugins();
void destroyStaticPlugins();
}
}

const std::string CARDINAL_VERSION = "23.07";

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------------------------------------

#ifndef HEADLESS
void handleHostParameterDrag(const CardinalPluginContext* pcontext, uint index, bool started)
{
    DISTRHO_SAFE_ASSERT_RETURN(pcontext->ui != nullptr,);

   #ifndef CARDINAL_COMMON_DSP_ONLY
    if (started)
    {
        pcontext->ui->editParameter(index, true);
        pcontext->ui->setParameterValue(index, pcontext->parameters[index]);
    }
    else
    {
        pcontext->ui->editParameter(index, false);
    }
   #endif
}
#endif

// --------------------------------------------------------------------------------------------------------------------

#ifndef HEADLESS
bool CardinalPluginContext::addIdleCallback(IdleCallback* const cb) const
{
   #ifndef CARDINAL_COMMON_DSP_ONLY
    if (ui != nullptr)
    {
        ui->addIdleCallback(cb);
        return true;
    }
   #else
    // unused
    (void)cb;
   #endif

    return false;
}

void CardinalPluginContext::removeIdleCallback(IdleCallback* const cb) const
{
   #ifndef CARDINAL_COMMON_DSP_ONLY
    if (ui != nullptr)
        ui->removeIdleCallback(cb);
   #else
    // unused
    (void)cb;
   #endif
}
#endif

void CardinalPluginContext::writeMidiMessage(const rack::midi::Message& message, const uint8_t channel)
{
    if (bypassed)
        return;

    const size_t size = message.bytes.size();
    DISTRHO_SAFE_ASSERT_RETURN(size > 0,);
    DISTRHO_SAFE_ASSERT_RETURN(message.frame >= 0,);

    MidiEvent event;
    event.frame = message.frame;

    switch (message.bytes[0] & 0xF0)
    {
    case 0x80:
    case 0x90:
    case 0xA0:
    case 0xB0:
    case 0xE0:
        event.size = 3;
        break;
    case 0xC0:
    case 0xD0:
        event.size = 2;
        break;
    case 0xF0:
        switch (message.bytes[0] & 0x0F)
        {
        case 0x0:
        case 0x4:
        case 0x5:
        case 0x7:
        case 0x9:
        case 0xD:
            // unsupported
            return;
        case 0x1:
        case 0x2:
        case 0x3:
        case 0xE:
            event.size = 3;
            break;
        case 0x6:
        case 0x8:
        case 0xA:
        case 0xB:
        case 0xC:
        case 0xF:
            event.size = 1;
            break;
        }
        break;
    default:
        // invalid
        return;
    }

    DISTRHO_SAFE_ASSERT_RETURN(size >= event.size,);

    std::memcpy(event.data, message.bytes.data(), event.size);

    if (channel != 0 && event.data[0] < 0xF0)
        event.data[0] |= channel & 0x0F;

    plugin->writeMidiEvent(event);
}

// -----------------------------------------------------------------------------------------------------------

#ifdef CARDINAL_INIT_OSC_THREAD
static void osc_error_handler(int num, const char* msg, const char* path)
{
    d_stderr("Cardinal OSC Error: code: %i, msg: \"%s\", path: \"%s\")", num, msg, path);
}

static int osc_fallback_handler(const char* const path, const char* const types, lo_arg**, int, lo_message, void*)
{
    d_stderr("Cardinal OSC unhandled message \"%s\" with types \"%s\"", path, types);
    return 0;
}

static int osc_hello_handler(const char*, const char*, lo_arg**, int, const lo_message m, void* const self)
{
    d_debug("osc_hello_handler()");
    const lo_address source = lo_message_get_source(m);
    const lo_server server = lo_server_thread_get_server(static_cast<Initializer*>(self)->oscServerThread);
    lo_send_from(source, server, LO_TT_IMMEDIATE, "/resp", "ss", "hello", "ok");
    return 0;
}

static int osc_load_handler(const char*, const char* types, lo_arg** argv, int argc, const lo_message m, void* const self)
{
    d_debug("osc_load_handler()");
    DISTRHO_SAFE_ASSERT_RETURN(argc == 1, 0);
    DISTRHO_SAFE_ASSERT_RETURN(types != nullptr && types[0] == 'b', 0);

    const int32_t size = argv[0]->blob.size;
    DISTRHO_SAFE_ASSERT_RETURN(size > 4, 0);

    const uint8_t* const blob = (uint8_t*)(&argv[0]->blob.data);
    DISTRHO_SAFE_ASSERT_RETURN(blob != nullptr, 0);

    bool ok = false;

    if (CardinalBasePlugin* const plugin = static_cast<Initializer*>(self)->remotePluginInstance)
    {
        CardinalPluginContext* const context = plugin->context;
        std::vector<uint8_t> data(size);
        std::memcpy(data.data(), blob, size);

        rack::contextSet(context);
        rack::system::removeRecursively(context->patch->autosavePath);
        rack::system::createDirectories(context->patch->autosavePath);
        try {
            rack::system::unarchiveToDirectory(data, context->patch->autosavePath);
            context->patch->loadAutosave();
            ok = true;
        }
        catch (rack::Exception& e) {
            WARN("%s", e.what());
        }
        rack::contextSet(nullptr);
    }

    const lo_address source = lo_message_get_source(m);
    const lo_server server = lo_server_thread_get_server(static_cast<Initializer*>(self)->oscServerThread);
    lo_send_from(source, server, LO_TT_IMMEDIATE, "/resp", "ss", "load", ok ? "ok" : "fail");
    return 0;
}

static int osc_param_handler(const char*, const char* types, lo_arg** argv, int argc, const lo_message m, void* const self)
{
    d_debug("osc_param_handler()");
    DISTRHO_SAFE_ASSERT_RETURN(argc == 3, 0);
    DISTRHO_SAFE_ASSERT_RETURN(types != nullptr, 0);
    DISTRHO_SAFE_ASSERT_RETURN(types[0] == 'h', 0);
    DISTRHO_SAFE_ASSERT_RETURN(types[1] == 'i', 0);
    DISTRHO_SAFE_ASSERT_RETURN(types[2] == 'f', 0);

    if (CardinalBasePlugin* const plugin = static_cast<Initializer*>(self)->remotePluginInstance)
    {
        CardinalPluginContext* const context = plugin->context;

        const int64_t moduleId = argv[0]->h;
        const int paramId = argv[1]->i;
        const float paramValue = argv[2]->f;

        rack::engine::Module* const module = context->engine->getModule(moduleId);
        DISTRHO_SAFE_ASSERT_RETURN(module != nullptr, 0);

        context->engine->setParamValue(module, paramId, paramValue);
    }

    return 0;
}

static int osc_host_param_handler(const char*, const char* types, lo_arg** argv, int argc, const lo_message m, void* const self)
{
    d_debug("osc_host_param_handler()");
    DISTRHO_SAFE_ASSERT_RETURN(argc == 2, 0);
    DISTRHO_SAFE_ASSERT_RETURN(types != nullptr, 0);
    DISTRHO_SAFE_ASSERT_RETURN(types[0] == 'i', 0);
    DISTRHO_SAFE_ASSERT_RETURN(types[1] == 'f', 0);

    if (CardinalBasePlugin* const plugin = static_cast<Initializer*>(self)->remotePluginInstance)
    {
        CardinalPluginContext* const context = plugin->context;

        const int paramId = argv[0]->i;
        DISTRHO_SAFE_ASSERT_RETURN(paramId >= 0, 0);

        const uint uparamId = static_cast<uint>(paramId);
        DISTRHO_SAFE_ASSERT_UINT2_RETURN(uparamId < kModuleParameterCount, uparamId, kModuleParameterCount, 0);

        const float paramValue = argv[1]->f;

        context->parameters[uparamId] = paramValue;
    }

    return 0;
}

static int osc_screenshot_handler(const char*, const char* types, lo_arg** argv, int argc, const lo_message m, void* const self)
{
    d_debug("osc_screenshot_handler()");
    DISTRHO_SAFE_ASSERT_RETURN(argc == 1, 0);
    DISTRHO_SAFE_ASSERT_RETURN(types != nullptr && types[0] == 'b', 0);

    const int32_t size = argv[0]->blob.size;
    DISTRHO_SAFE_ASSERT_RETURN(size > 4, 0);

    const uint8_t* const blob = (uint8_t*)(&argv[0]->blob.data);
    DISTRHO_SAFE_ASSERT_RETURN(blob != nullptr, 0);

    bool ok = false;

    if (CardinalBasePlugin* const plugin = static_cast<Initializer*>(self)->remotePluginInstance)
    {
        if (char* const screenshot = String::asBase64(blob, size).getAndReleaseBuffer())
        {
            ok = plugin->updateStateValue("screenshot", screenshot);
            std::free(screenshot);
        }
    }

    const lo_address source = lo_message_get_source(m);
    const lo_server server = lo_server_thread_get_server(static_cast<Initializer*>(self)->oscServerThread);
    lo_send_from(source, server, LO_TT_IMMEDIATE, "/resp", "ss", "screenshot", ok ? "ok" : "fail");
    return 0;
}
#endif

// -----------------------------------------------------------------------------------------------------------

#if defined(DISTRHO_OS_WASM) && !defined(CARDINAL_COMMON_UI_ONLY)
static void WebBrowserDataLoaded(void* const data)
{
    static_cast<Initializer*>(data)->loadSettings(true);
}
#endif

// -----------------------------------------------------------------------------------------------------------

Initializer::Initializer(const CardinalBasePlugin* const plugin, const CardinalBaseUI* const ui)
{
    using namespace rack;

    // Cardinal default settings, potentially overriding VCV Rack ones
    settings::allowCursorLock = false;
    settings::tooltips = true;
    settings::cableOpacity = 0.5f;
    settings::cableTension = 0.75f;
    settings::rackBrightness = 1.0f;
    settings::haloBrightness = 0.25f;
    settings::knobMode = settings::KNOB_MODE_LINEAR;
    settings::knobScroll = false;
    settings::knobScrollSensitivity = 0.001f;
    settings::lockModules = false;
    settings::browserSort = settings::BROWSER_SORT_UPDATED;
    settings::browserZoom = -1.f;
    settings::invertZoom = false;
    settings::squeezeModules = true;
    settings::darkMode = true;
    settings::uiTheme = "dark";

    // runtime behaviour
    settings::devMode = true;
    settings::isPlugin = true;
   #ifdef HEADLESS_BEHAVIOUR
    settings::headless = true;
   #endif

    // copied from https://community.vcvrack.com/t/16-colour-cable-palette/15951
    settings::cableColors = {
        color::fromHexString("#ff5252"),
        color::fromHexString("#ff9352"),
        color::fromHexString("#ffd452"),
        color::fromHexString("#e8ff52"),
        color::fromHexString("#a8ff52"),
        color::fromHexString("#67ff52"),
        color::fromHexString("#52ff7d"),
        color::fromHexString("#52ffbe"),
        color::fromHexString("#52ffff"),
        color::fromHexString("#52beff"),
        color::fromHexString("#527dff"),
        color::fromHexString("#6752ff"),
        color::fromHexString("#a852ff"),
        color::fromHexString("#e952ff"),
        color::fromHexString("#ff52d4"),
        color::fromHexString("#ff5293"),
    };

    system::init();
    logger::init();
    random::init();
    ui::init();

   #ifdef CARDINAL_COMMON_UI_ONLY
    constexpr const bool isRealInstance = true;
   #else
    const bool isRealInstance = !plugin->isDummyInstance();
   #endif

    if (asset::systemDir.empty())
    {
        if (const char* const bundlePath = (plugin != nullptr ? plugin->getBundlePath() :
                                           #if DISTRHO_PLUGIN_HAS_UI
                                            ui != nullptr ? ui->getBundlePath() :
                                           #endif
                                            nullptr))
        {
            if (const char* const resourcePath = getResourcePath(bundlePath))
            {
                asset::systemDir = resourcePath;
                asset::bundlePath = system::join(asset::systemDir, "PluginManifests");
            }
        }

        if (asset::systemDir.empty() || ! system::exists(asset::systemDir) || ! system::exists(asset::bundlePath))
        {
           #ifdef CARDINAL_PLUGIN_SOURCE_DIR
            // Make system dir point to source code location as fallback
            asset::systemDir = CARDINAL_PLUGIN_SOURCE_DIR DISTRHO_OS_SEP_STR "Rack";
            asset::bundlePath.clear();

            // If source code dir does not exist use install target prefix as system dir
            if (!system::exists(system::join(asset::systemDir, "res")))
           #endif
            {
               #if defined(DISTRHO_OS_WASM)
                asset::systemDir = "/resources";
               #elif defined(ARCH_MAC)
                asset::systemDir = "/Library/Application Support/Cardinal";
               #elif defined(ARCH_WIN)
                asset::systemDir = system::join(getSpecialPath(kSpecialPathCommonProgramFiles), "Cardinal");
               #else
                asset::systemDir = CARDINAL_PLUGIN_PREFIX "/share/cardinal";
               #endif
                asset::bundlePath = system::join(asset::systemDir, "PluginManifests");
            }
        }
    }

    if (asset::userDir.empty())
    {
       #if defined(DISTRHO_OS_WASM)
        asset::userDir = "/userfiles";
       #elif defined(ARCH_MAC)
        asset::userDir = system::join(homeDir(), "Documents", "Cardinal");
       #elif defined(ARCH_WIN)
        asset::userDir = system::join(getSpecialPath(kSpecialPathMyDocuments), "Cardinal");
       #else
        if (const char* const xdgEnv = getenv("XDG_DOCUMENTS_DIR"))
            asset::userDir = system::join(xdgEnv, "Cardinal");
        else
            asset::userDir = system::join(homeDir(), "Documents", "Cardinal");
       #endif

        if (isRealInstance)
        {
            system::createDirectory(asset::userDir);
           #if defined(DISTRHO_OS_WASM) && !defined(CARDINAL_COMMON_UI_ONLY)
            EM_ASM({
                Module.FS.mount(Module.IDBFS, {}, '/userfiles');
                Module.FS.syncfs(true, function(err) { if (!err) { dynCall('vi', $0, [$1]) } });
            }, WebBrowserDataLoaded, this);
           #endif
        }
    }

   #ifndef CARDINAL_COMMON_DSP_ONLY
    if (asset::configDir.empty())
    {
       #if defined(ARCH_MAC) || defined(ARCH_WIN) || defined(DISTRHO_OS_WASM)
        asset::configDir = asset::userDir;
       #else
        if (const char* const xdgEnv = getenv("XDG_CONFIG_HOME"))
            asset::configDir = system::join(xdgEnv, "Cardinal");
        else
            asset::configDir = system::join(homeDir(), ".config", "Cardinal");

        if (isRealInstance)
            system::createDirectory(asset::configDir);
       #endif
    }
   #endif
   
    if (settings::settingsPath.empty())
        settings::settingsPath = asset::config(CARDINAL_VARIANT_NAME ".json");

    templatePath = asset::user("templates/" CARDINAL_VARIANT_NAME ".vcv");
   #ifdef DISTRHO_OS_WASM
    factoryTemplatePath = system::join(asset::patchesPath(), CARDINAL_WASM_WELCOME_TEMPLATE_FILENAME ".vcv");
   #else
    factoryTemplatePath = system::join(asset::patchesPath(), "templates/" CARDINAL_VARIANT_NAME ".vcv");
   #endif

    // Log environment
    INFO("%s %s %s, compatible with Rack version %s", APP_NAME.c_str(), APP_EDITION.c_str(), CARDINAL_VERSION.c_str(), APP_VERSION.c_str());
    INFO("%s", system::getOperatingSystemInfo().c_str());
    INFO("Binary filename: %s", getBinaryFilename());
    if (plugin != nullptr) {
        INFO("Bundle path: %s", plugin->getBundlePath());
   #if DISTRHO_PLUGIN_HAS_UI
    } else if (ui != nullptr) {
        INFO("Bundle path: %s", ui->getBundlePath());
   #endif
    }
    INFO("System directory: %s", asset::systemDir.c_str());
    INFO("User directory: %s", asset::userDir.c_str());
    INFO("Template patch: %s", templatePath.c_str());
    INFO("System template patch: %s", factoryTemplatePath.c_str());

    // Report to user if something is wrong with the installation
    if (asset::systemDir.empty())
    {
        d_stderr2("Failed to locate Cardinal plugin bundle.\n"
                    "Install Cardinal with its bundle folder intact and try again.");
    }
    else if (! system::exists(asset::systemDir))
    {
        d_stderr2("System directory \"%s\" does not exist.\n"
                    "Make sure Cardinal was downloaded and installed correctly.", asset::systemDir.c_str());
    }

    INFO("Initializing plugins");
    plugin::initStaticPlugins();

    INFO("Initializing plugin browser DB");
    app::browserInit();

    loadSettings(isRealInstance);

#ifdef CARDINAL_INIT_OSC_THREAD
    INFO("Initializing OSC Remote control");
    const char* port;
    if (const char* const portEnv = std::getenv("CARDINAL_REMOTE_HOST_PORT"))
        port = portEnv;
    else
        port = CARDINAL_DEFAULT_REMOTE_PORT;
    oscServerThread = lo_server_thread_new_with_proto(port, LO_UDP, osc_error_handler);
    DISTRHO_SAFE_ASSERT_RETURN(oscServerThread != nullptr,);

    lo_server_thread_add_method(oscServerThread, "/hello", "", osc_hello_handler, this);
    lo_server_thread_add_method(oscServerThread, "/host-param", "if", osc_host_param_handler, this);
    lo_server_thread_add_method(oscServerThread, "/load", "b", osc_load_handler, this);
    lo_server_thread_add_method(oscServerThread, "/param", "hif", osc_param_handler, this);
    lo_server_thread_add_method(oscServerThread, "/screenshot", "b", osc_screenshot_handler, this);
    lo_server_thread_add_method(oscServerThread, nullptr, nullptr, osc_fallback_handler, nullptr);
    lo_server_thread_start(oscServerThread);
#else
    INFO("OSC Remote control is not enabled in this build");
#endif
}

Initializer::~Initializer()
{
    using namespace rack;

#ifdef CARDINAL_INIT_OSC_THREAD
    if (oscServerThread != nullptr)
    {
        lo_server_thread_stop(oscServerThread);
        lo_server_thread_del_method(oscServerThread, nullptr, nullptr);
        lo_server_thread_free(oscServerThread);
        oscServerThread = nullptr;
    }
#endif

    if (shouldSaveSettings)
    {
        INFO("Save settings");
        settings::save();
    }

    INFO("Clearing asset paths");
    asset::bundlePath.clear();
    asset::systemDir.clear();
    asset::userDir.clear();

    INFO("Destroying plugins");
    plugin::destroyStaticPlugins();

    INFO("Destroying colourized assets");
    asset::destroy();

    INFO("Destroying settings");
    settings::destroy();

    INFO("Destroying logger");
    logger::destroy();
}

void Initializer::loadSettings(const bool isRealInstance)
{
    using namespace rack;

    if (isRealInstance)
    {
        INFO("Loading settings");
        settings::load();
        shouldSaveSettings = true;
    }

    // enforce settings that do not make sense as anything else
    settings::safeMode = false;
    settings::token.clear();
    settings::windowMaximized = false;
    settings::windowPos = math::Vec(0, 0);
    settings::pixelRatio = 0.0;
    settings::sampleRate = 0;
    settings::threadCount = 1;
    settings::autosaveInterval = 0;
    settings::skipLoadOnLaunch = true;
    settings::autoCheckUpdates = false;
    settings::showTipsOnLaunch = false;
    settings::tipIndex = -1;

    if (settings::uiTheme != "dark" && settings::uiTheme != "light")
    {
        settings::uiTheme = "dark";
        rack::ui::refreshTheme();
    }

    // reload dark/light mode as necessary
    switchDarkMode(settings::uiTheme == "dark");
}

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO

// --------------------------------------------------------------------------------------------------------------------

namespace rack {

bool isMini()
{
#if CARDINAL_VARIANT_MINI
    return true;
#else
    return false;
#endif
}

bool isStandalone()
{
    static const bool standalone = std::strstr(getPluginFormatName(), "Standalone") != nullptr;
    return standalone;
}

#ifdef ARCH_WIN
std::string getSpecialPath(const SpecialPath type)
{
    int csidl;
    switch (type)
    {
    case kSpecialPathUserProfile:
        csidl = CSIDL_PROFILE;
        break;
    case kSpecialPathCommonProgramFiles:
        csidl = CSIDL_PROGRAM_FILES_COMMON;
        break;
    case kSpecialPathProgramFiles:
        csidl = CSIDL_PROGRAM_FILES;
        break;
    case kSpecialPathAppData:
        csidl = CSIDL_APPDATA;
        break;
    case kSpecialPathMyDocuments:
        csidl = CSIDL_MYDOCUMENTS;
    default:
        return {};
    }

    WCHAR path[MAX_PATH + 256];

    if (SHGetSpecialFolderPathW(nullptr, path, csidl, FALSE))
        return string::UTF16toUTF8(path);

    return {};
}
#endif

#ifdef DISTRHO_OS_WASM
char* patchFromURL = nullptr;
char* patchRemoteURL = nullptr;
char* patchStorageSlug = nullptr;

void syncfs()
{
    settings::save();

   #ifndef CARDINAL_COMMON_UI_ONLY
    EM_ASM({
        Module.FS.syncfs(false, function(){} );
    });
   #endif
}
#endif

std::string homeDir()
{
   #ifdef ARCH_WIN
    return getSpecialPath(kSpecialPathUserProfile);
   #else
    if (const char* const home = getenv("HOME"))
        return home;
    if (struct passwd* const pwd = getpwuid(getuid()))
        return pwd->pw_dir;
   #endif
    return {};
}

} // namespace rack

// --------------------------------------------------------------------------------------------------------------------

namespace patchUtils
{

using namespace rack;

#ifndef HEADLESS_BEHAVIOUR
static void promptClear(const char* const message, const std::function<void()> action)
{
    if (APP->history->isSaved() || APP->scene->rack->hasModules())
        return action();

    asyncDialog::create(message, action);
}
#endif

void loadDialog()
{
#ifndef HEADLESS_BEHAVIOUR
    promptClear("The current patch is unsaved. Clear it and open a new patch?", []() {
        std::string dir;
        if (! APP->patch->path.empty())
            dir = system::getDirectory(APP->patch->path);
        else
            dir = homeDir();

        CardinalPluginContext* const pcontext = static_cast<CardinalPluginContext*>(APP);
        DISTRHO_SAFE_ASSERT_RETURN(pcontext != nullptr,);

        CardinalBaseUI* const ui = static_cast<CardinalBaseUI*>(pcontext->ui);
        DISTRHO_SAFE_ASSERT_RETURN(ui != nullptr,);

        DISTRHO_NAMESPACE::FileBrowserOptions opts;
        opts.saving = ui->saving = false;
        opts.startDir = dir.c_str();
        opts.title = "Open patch";
        ui->openFileBrowser(opts);
    });
#endif
}

void loadPathDialog(const std::string& path, const bool asTemplate)
{
#ifndef HEADLESS_BEHAVIOUR
    promptClear("The current patch is unsaved. Clear it and open the new patch?", [path, asTemplate]() {
        APP->patch->loadAction(path);

        if (asTemplate)
        {
            APP->patch->path = "";
            APP->history->setSaved();
        }

       #ifdef DISTRHO_OS_WASM
        syncfs();
       #endif

        if (remoteUtils::RemoteDetails* const remoteDetails = remoteUtils::getRemote())
            if (remoteDetails->autoDeploy)
                remoteUtils::sendFullPatchToRemote(remoteDetails);
    });
#endif
}

void loadSelectionDialog()
{
    app::RackWidget* const w = APP->scene->rack;

    std::string selectionDir = asset::user("selections");
    system::createDirectories(selectionDir);

    async_dialog_filebrowser(false, nullptr, selectionDir.c_str(), "Import selection", [w](char* pathC) {
        if (!pathC) {
            // No path selected
            return;
        }

        try {
            w->loadSelection(pathC);
        }
        catch (Exception& e) {
            async_dialog_message(e.what());
        }

        std::free(pathC);

       #ifdef DISTRHO_OS_WASM
        syncfs();
       #endif

        if (remoteUtils::RemoteDetails* const remoteDetails = remoteUtils::getRemote())
            if (remoteDetails->autoDeploy)
                remoteUtils::sendFullPatchToRemote(remoteDetails);
    });
}

void loadTemplate(const bool factory)
{
    try {
        APP->patch->load(factory ? APP->patch->factoryTemplatePath : APP->patch->templatePath);
    }
    catch (Exception& e) {
        // if user template failed, try the factory one
        if (!factory)
            return loadTemplate(true);

        const std::string message = string::f("Could not load template patch, clearing rack: %s", e.what());
        asyncDialog::create(message.c_str());

        APP->patch->clear();
        APP->patch->clearAutosave();
    }

    // load() sets the patch's original patch, but we don't want to use that.
    APP->patch->path.clear();
    APP->history->setSaved();

   #ifdef DISTRHO_OS_WASM
    syncfs();
   #endif

    if (remoteUtils::RemoteDetails* const remoteDetails = remoteUtils::getRemote())
        if (remoteDetails->autoDeploy)
            remoteUtils::sendFullPatchToRemote(remoteDetails);
}


void loadTemplateDialog(const bool factory)
{
#ifndef HEADLESS_BEHAVIOUR
    promptClear("The current patch is unsaved. Clear it and start a new patch?", [factory]() {
        loadTemplate(factory);
    });
#endif
}

void revertDialog()
{
#ifndef HEADLESS_BEHAVIOUR
    if (APP->patch->path.empty())
        return;
    promptClear("Revert patch to the last saved state?", []{
        APP->patch->loadAction(APP->patch->path);

       #ifdef DISTRHO_OS_WASM
        syncfs();
       #endif

        if (remoteUtils::RemoteDetails* const remoteDetails = remoteUtils::getRemote())
            if (remoteDetails->autoDeploy)
                remoteUtils::sendFullPatchToRemote(remoteDetails);
    });
#endif
}

void saveDialog(const std::string& path)
{
#ifndef HEADLESS_BEHAVIOUR
    if (path.empty()) {
        return;
    }

    // Note: If save() fails below, this should probably be reset. But we need it so toJson() doesn't set the "unsaved" property.
    APP->history->setSaved();

    try {
        APP->patch->save(path);
    }
    catch (Exception& e) {
        asyncDialog::create(string::f("Could not save patch: %s", e.what()).c_str());
        return;
    }

    APP->patch->pushRecentPath(path);

   #ifdef DISTRHO_OS_WASM
    syncfs();
   #else
    rack::settings::save();
   #endif
#endif
}

#ifndef HEADLESS_BEHAVIOUR
static void saveAsDialog(const bool uncompressed)
{
    std::string dir;
    if (! APP->patch->path.empty())
    {
        dir = system::getDirectory(APP->patch->path);
    }
    else
    {
        dir = asset::user("patches");
        system::createDirectories(dir);
    }

    CardinalPluginContext* const pcontext = static_cast<CardinalPluginContext*>(APP);
    DISTRHO_SAFE_ASSERT_RETURN(pcontext != nullptr,);

    CardinalBaseUI* const ui = static_cast<CardinalBaseUI*>(pcontext->ui);
    DISTRHO_SAFE_ASSERT_RETURN(ui != nullptr,);

    DISTRHO_NAMESPACE::FileBrowserOptions opts;
    opts.saving = ui->saving = true;
    opts.defaultName = "patch.vcv";
    opts.startDir = dir.c_str();
    opts.title = "Save patch";
    ui->savingUncompressed = uncompressed;
    ui->openFileBrowser(opts);
}
#endif

void saveAsDialog()
{
#ifndef HEADLESS_BEHAVIOUR
    saveAsDialog(false);
#endif
}

void saveAsDialogUncompressed()
{
#ifndef HEADLESS_BEHAVIOUR
    saveAsDialog(true);
#endif
}

void saveTemplateDialog()
{
    asyncDialog::create("Overwrite template patch?", []{
        rack::system::createDirectories(system::getDirectory(APP->patch->templatePath));

        try {
            APP->patch->save(APP->patch->templatePath);
        }
        catch (Exception& e) {
            asyncDialog::create(string::f("Could not save template patch: %s", e.what()).c_str());
            return;
        }

       #ifdef DISTRHO_OS_WASM
        syncfs();
       #endif
    });
}

void openBrowser(const std::string& url)
{
#ifdef DISTRHO_OS_WASM
    EM_ASM({
        window.open(UTF8ToString($0), '_blank');
    }, url.c_str());
#else
    system::openBrowser(url);
#endif
}

}

// --------------------------------------------------------------------------------------------------------------------

void async_dialog_filebrowser(const bool saving,
                              const char* const defaultName,
                              const char* const startDir,
                              const char* const title,
                              const std::function<void(char* path)> action)
{
#ifndef HEADLESS_BEHAVIOUR
    CardinalPluginContext* const pcontext = static_cast<CardinalPluginContext*>(APP);
    DISTRHO_SAFE_ASSERT_RETURN(pcontext != nullptr,);

    CardinalBaseUI* const ui = static_cast<CardinalBaseUI*>(pcontext->ui);
    DISTRHO_SAFE_ASSERT_RETURN(ui != nullptr,);

    // only 1 dialog possible at a time
    DISTRHO_SAFE_ASSERT_RETURN(ui->filebrowserhandle == nullptr,);

    DISTRHO_NAMESPACE::FileBrowserOptions opts;
    opts.saving = saving;
    opts.defaultName = defaultName;
    opts.startDir = startDir;
    opts.title = title;

    ui->filebrowseraction = action;
    ui->filebrowserhandle = fileBrowserCreate(true, pcontext->nativeWindowId, pcontext->window->pixelRatio, opts);
#endif
}

void async_dialog_message(const char* const message)
{
#ifndef HEADLESS_BEHAVIOUR
    asyncDialog::create(message);
#endif
}

void async_dialog_message(const char* const message, const std::function<void()> action)
{
#ifndef HEADLESS_BEHAVIOUR
    asyncDialog::create(message, action);
#endif
}

void async_dialog_text_input(const char* const message, const char* const text,
                             const std::function<void(char* newText)> action)
{
#ifndef HEADLESS_BEHAVIOUR
    asyncDialog::textInput(message, text, action);
#endif
}
