/*
 * DISTRHO Cardinal Plugin
 * Copyright (C) 2021-2024 Filipe Coelho <falktx@falktx.com>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <engine/Engine.hpp>
#include <patch.hpp>
#include <system.hpp>

#ifdef NDEBUG
# undef DEBUG
#endif

#include "CardinalRemote.hpp"
#include "CardinalPluginContext.hpp"
#include "extra/Base64.hpp"
#include "extra/ScopedSafeLocale.hpp"

#if defined(STATIC_BUILD) || ! DISTRHO_PLUGIN_WANT_DIRECT_ACCESS
# undef HAVE_LIBLO
#endif

#if (defined(HAVE_LIBLO) || ! DISTRHO_PLUGIN_WANT_DIRECT_ACCESS) && !defined(HEADLESS)
# define CARDINAL_REMOTE_ENABLED
#endif

#ifdef HAVE_LIBLO
# include <lo/lo.h>
#endif

namespace rack {
namespace engine {
void Engine_setRemoteDetails(Engine*, remoteUtils::RemoteDetails*);
}
}

// -----------------------------------------------------------------------------------------------------------

namespace remoteUtils {

#ifdef HAVE_LIBLO
static int osc_handler(const char* const path, const char* const types, lo_arg** argv, const int argc, lo_message, void* const self)
{
    d_stdout("osc_handler(\"%s\", \"%s\", %p, %i)", path, types, argv, argc);

    if (std::strcmp(path, "/resp") == 0 && argc == 2 && types[0] == 's' && types[1] == 's')
    {
        d_stdout("osc_handler(\"%s\", ...) - got resp | '%s' '%s'", path, &argv[0]->s, &argv[1]->s);

        if (std::strcmp(&argv[0]->s, "hello") == 0)
        {
            if (std::strcmp(&argv[1]->s, "ok") == 0)
                static_cast<RemoteDetails*>(self)->connected = true;
        }
        else if (std::strcmp(&argv[0]->s, "features") == 0)
        {
            static_cast<RemoteDetails*>(self)->screenshot = std::strstr(&argv[1]->s, ":screenshot:") != nullptr;
        }
    }
    return 0;
}
#endif

RemoteDetails* getRemote()
{
#ifdef CARDINAL_REMOTE_ENABLED
    CardinalPluginContext* const context = static_cast<CardinalPluginContext*>(APP);
    DISTRHO_SAFE_ASSERT_RETURN(context != nullptr, nullptr);

    CardinalBaseUI* const ui = static_cast<CardinalBaseUI*>(context->ui);
    DISTRHO_SAFE_ASSERT_RETURN(ui != nullptr, nullptr);

    return ui->remoteDetails;
#else
    return nullptr;
#endif
}

bool connectToRemote(const char* const url)
{
#ifdef CARDINAL_REMOTE_ENABLED
    CardinalPluginContext* const context = static_cast<CardinalPluginContext*>(APP);
    DISTRHO_SAFE_ASSERT_RETURN(context != nullptr, false);

    CardinalBaseUI* const ui = static_cast<CardinalBaseUI*>(context->ui);
    DISTRHO_SAFE_ASSERT_RETURN(ui != nullptr, false);

    RemoteDetails* remoteDetails = ui->remoteDetails;

   #if ! DISTRHO_PLUGIN_WANT_DIRECT_ACCESS
    if (remoteDetails == nullptr)
    {
        ui->remoteDetails = remoteDetails = new RemoteDetails;
        remoteDetails->handle = ui;
        remoteDetails->url = strdup(url);
        remoteDetails->autoDeploy = true;
        remoteDetails->connected = true;
        remoteDetails->first = false;
        remoteDetails->screenshot = false;
    }
   #elif defined(HAVE_LIBLO)
    const lo_address addr = lo_address_new_from_url(url);
    DISTRHO_SAFE_ASSERT_RETURN(addr != nullptr, false);

    if (remoteDetails == nullptr)
    {
        const lo_server oscServer = lo_server_new_with_proto(nullptr, LO_UDP, nullptr);
        DISTRHO_SAFE_ASSERT_RETURN(oscServer != nullptr, false);

        ui->remoteDetails = remoteDetails = new RemoteDetails;
        remoteDetails->handle = oscServer;
        remoteDetails->url = strdup(url);
        remoteDetails->autoDeploy = true;
        remoteDetails->first = true;
        remoteDetails->connected = false;
        remoteDetails->screenshot = false;

        lo_server_add_method(oscServer, "/resp", nullptr, osc_handler, remoteDetails);

        sendFullPatchToRemote(remoteDetails);

        Engine_setRemoteDetails(context->engine, remoteDetails);
    }
    else if (std::strcmp(remoteDetails->url, url) != 0)
    {
        ui->remoteDetails = nullptr;
        disconnectFromRemote(remoteDetails);
        return connectToRemote(url);
    }

    lo_send(addr, "/hello", "");
    lo_address_free(addr);
   #endif

    return remoteDetails != nullptr;
#else
    return false;
#endif
}

void disconnectFromRemote(RemoteDetails* const remote)
{
    if (remote != nullptr)
    {
       #ifdef HAVE_LIBLO
        lo_server_free(static_cast<lo_server>(remote->handle));
       #endif
        std::free(const_cast<char*>(remote->url));
        delete remote;
    }
}

void idleRemote(RemoteDetails* const remote)
{
    DISTRHO_SAFE_ASSERT_RETURN(remote != nullptr,);
#ifdef HAVE_LIBLO
    while (lo_server_recv_noblock(static_cast<lo_server>(remote->handle), 0) != 0) {}
#endif
}

void sendParamChangeToRemote(RemoteDetails* const remote, int64_t moduleId, int paramId, float value)
{
#ifdef CARDINAL_REMOTE_ENABLED
#if ! DISTRHO_PLUGIN_WANT_DIRECT_ACCESS
    char paramBuf[512] = {};
    {
        const ScopedSafeLocale cssl;
        std::snprintf(paramBuf, sizeof(paramBuf), "%lld:%d:%f", (long long)moduleId, paramId, value);
    }
    static_cast<CardinalBaseUI*>(remote->handle)->setState("param", paramBuf);
#elif defined(HAVE_LIBLO)
    const lo_address addr = lo_address_new_from_url(remote->url);
    DISTRHO_SAFE_ASSERT_RETURN(addr != nullptr,);

    lo_send(addr, "/param", "hif", moduleId, paramId, value);

    lo_address_free(addr);
#endif
#endif
}

void sendFullPatchToRemote(RemoteDetails* const remote)
{
#ifdef CARDINAL_REMOTE_ENABLED
    CardinalPluginContext* const context = static_cast<CardinalPluginContext*>(APP);
    DISTRHO_SAFE_ASSERT_RETURN(context != nullptr,);

    context->engine->prepareSave();
    context->patch->saveAutosave();
    context->patch->cleanAutosave();

    std::vector<uint8_t> data;
    using namespace rack::system;

   #if ! DISTRHO_PLUGIN_WANT_DIRECT_ACCESS
    FILE* const f = std::fopen(join(context->patch->autosavePath, "patch.json").c_str(), "r");
    DISTRHO_SAFE_ASSERT_RETURN(f != nullptr,);

    DEFER({
        std::fclose(f);
    });

    std::fseek(f, 0, SEEK_END);
    const long fileSize = std::ftell(f);
    DISTRHO_SAFE_ASSERT_RETURN(fileSize > 0,);

    std::fseek(f, 0, SEEK_SET);
    char* const fileContent = new char[fileSize+1];

    DISTRHO_SAFE_ASSERT_RETURN(std::fread(fileContent, fileSize, 1, f) == 1,);
    fileContent[fileSize] = '\0';
    static_cast<CardinalBaseUI*>(remote->handle)->setState("patch", fileContent);
    delete[] fileContent;
   #elif defined(HAVE_LIBLO)
    try {
        data = archiveDirectory(context->patch->autosavePath, 1);
    } DISTRHO_SAFE_EXCEPTION_RETURN("sendFullPatchToRemote",);

    DISTRHO_SAFE_ASSERT_RETURN(data.size() >= 4,);

    const lo_address addr = lo_address_new_from_url(remote->url);
    DISTRHO_SAFE_ASSERT_RETURN(addr != nullptr,);

    if (const lo_blob blob = lo_blob_new(data.size(), data.data()))
    {
        lo_send(addr, "/load", "b", blob);
        lo_blob_free(blob);
    }

    lo_address_free(addr);
   #endif
#endif
}

void sendScreenshotToRemote(RemoteDetails* const remote, const char* const screenshot)
{
#if defined(HAVE_LIBLO) && DISTRHO_PLUGIN_WANT_DIRECT_ACCESS
    const lo_address addr = lo_address_new_from_url(remote->url);
    DISTRHO_SAFE_ASSERT_RETURN(addr != nullptr,);

    std::vector<uint8_t> data(d_getChunkFromBase64String(screenshot));

    if (const lo_blob blob = lo_blob_new(data.size(), data.data()))
    {
        lo_send(addr, "/screenshot", "b", blob);
        lo_blob_free(blob);
    }

    lo_address_free(addr);
#endif
}

}

// -----------------------------------------------------------------------------------------------------------
