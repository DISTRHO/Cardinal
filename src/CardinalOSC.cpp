/*
 * DISTRHO Cardinal Plugin
 * Copyright (C) 2021-2026 Filipe Coelho <falktx@falktx.com>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "CardinalOSC.hpp"

#include "CardinalCommon.hpp"
#include "CardinalPluginContext.hpp"

#include <context.hpp>
#include <patch.hpp>
#include <string.hpp>
#include <system.hpp>
#include <engine/Engine.hpp>

#include <cstdlib>

#ifndef DISTRHO_PLUGIN_WANT_DIRECT_ACCESS
# error wrong build
#endif

#if (defined(STATIC_BUILD) && !defined(__MOD_DEVICES__)) || CARDINAL_VARIANT_MINI
# undef CARDINAL_INIT_OSC_THREAD
#endif

#ifdef HAVE_LIBLO
# include <lo/lo.h>
#endif

START_NAMESPACE_DISTRHO

#ifdef HAVE_LIBLO
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
    d_stdout("Hello received from OSC, saying hello back to them o/");
    const lo_address source = lo_message_get_source(m);
    const lo_server server = static_cast<Initializer*>(self)->oscServer;

    // send list of features first
   #ifdef CARDINAL_INIT_OSC_THREAD
    lo_send_from(source, server, LO_TT_IMMEDIATE, "/resp", "ss", "features", ":screenshot:");
   #else
    lo_send_from(source, server, LO_TT_IMMEDIATE, "/resp", "ss", "features", "");
   #endif

    // then finally hello reply
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

       #ifdef CARDINAL_INIT_OSC_THREAD
        rack::contextSet(context);
       #endif

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

       #ifdef CARDINAL_INIT_OSC_THREAD
        rack::contextSet(nullptr);
       #endif
    }

    const lo_address source = lo_message_get_source(m);
    const lo_server server = static_cast<Initializer*>(self)->oscServer;
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

       #ifdef CARDINAL_INIT_OSC_THREAD
        rack::contextSet(context);
       #endif

        rack::engine::Module* const module = context->engine->getModule(moduleId);
        DISTRHO_SAFE_ASSERT_RETURN(module != nullptr, 0);

        context->engine->setParamValue(module, paramId, paramValue);

       #ifdef CARDINAL_INIT_OSC_THREAD
        rack::contextSet(nullptr);
       #endif
    }

    return 0;
}

static int osc_host_param_handler(const char*, const char* types, lo_arg** argv, int argc, const lo_message, void* const self)
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

# ifdef CARDINAL_INIT_OSC_THREAD
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
    const lo_server server = static_cast<Initializer*>(self)->oscServer;
    lo_send_from(source, server, LO_TT_IMMEDIATE, "/resp", "ss", "screenshot", ok ? "ok" : "fail");
    return 0;
}
# endif
#endif

void oscInitState(Initializer* const self)
{
#if defined(CARDINAL_INIT_OSC_THREAD)
    INFO("Initializing OSC Remote control");
    const char* port;
    if (const char* const portEnv = std::getenv("CARDINAL_REMOTE_HOST_PORT"))
        port = portEnv;
    else
        port = CARDINAL_DEFAULT_REMOTE_PORT;
    self->startRemoteServer(port);
#elif defined(HAVE_LIBLO)
    if (rack::isStandalone()) {
        INFO("OSC Remote control is available on request");
    } else {
        INFO("OSC Remote control is not available on plugin variants");
    }
#else
    INFO("OSC Remote control is not enabled in this build");
    (void)self;
#endif
}

void oscShutdownState(Initializer* const self)
{
#ifdef HAVE_LIBLO
    self->stopRemoteServer();
#else
    (void)self;
#endif
}

#ifdef HAVE_LIBLO
bool Initializer::startRemoteServer(const char* const port)
{
   #ifdef CARDINAL_INIT_OSC_THREAD
    if (oscServerThread != nullptr)
        return true;

    if ((oscServerThread = lo_server_thread_new_with_proto(port, LO_UDP, osc_error_handler)) == nullptr)
        return false;

    oscServer = lo_server_thread_get_server(oscServerThread);

    lo_server_thread_add_method(oscServerThread, "/hello", "", osc_hello_handler, this);
    lo_server_thread_add_method(oscServerThread, "/host-param", "if", osc_host_param_handler, this);
    lo_server_thread_add_method(oscServerThread, "/load", "b", osc_load_handler, this);
    lo_server_thread_add_method(oscServerThread, "/param", "hif", osc_param_handler, this);
    lo_server_thread_add_method(oscServerThread, "/screenshot", "b", osc_screenshot_handler, this);
    lo_server_thread_add_method(oscServerThread, nullptr, nullptr, osc_fallback_handler, nullptr);
    lo_server_thread_start(oscServerThread);
   #else
    if (oscServer != nullptr)
        return true;

    if ((oscServer = lo_server_new_with_proto(port, LO_UDP, osc_error_handler)) == nullptr)
        return false;

    lo_server_add_method(oscServer, "/hello", "", osc_hello_handler, this);
    lo_server_add_method(oscServer, "/host-param", "if", osc_host_param_handler, this);
    lo_server_add_method(oscServer, "/load", "b", osc_load_handler, this);
    lo_server_add_method(oscServer, "/param", "hif", osc_param_handler, this);
    lo_server_add_method(oscServer, nullptr, nullptr, osc_fallback_handler, nullptr);
   #endif

    return true;
}

void Initializer::stopRemoteServer()
{
    DISTRHO_SAFE_ASSERT(remotePluginInstance == nullptr);

   #ifdef CARDINAL_INIT_OSC_THREAD
    if (oscServerThread != nullptr)
    {
        lo_server_thread_stop(oscServerThread);
        lo_server_thread_del_method(oscServerThread, nullptr, nullptr);
        lo_server_thread_free(oscServerThread);
        oscServerThread = nullptr;
        oscServer = nullptr;
    }
   #else
    if (oscServer != nullptr)
    {
        lo_server_del_method(oscServer, nullptr, nullptr);
        lo_server_free(oscServer);
        oscServer = nullptr;
    }
   #endif
}

void Initializer::stepRemoteServer()
{
    DISTRHO_SAFE_ASSERT_RETURN(oscServer != nullptr,);
    DISTRHO_SAFE_ASSERT_RETURN(remotePluginInstance != nullptr,);

   #ifndef CARDINAL_INIT_OSC_THREAD
    for (;;)
    {
        try {
            if (lo_server_recv_noblock(oscServer, 0) == 0)
                break;
        } DISTRHO_SAFE_EXCEPTION_CONTINUE("stepRemoteServer")
    }
   #endif
}
#endif // HAVE_LIBLO

END_NAMESPACE_DISTRHO
