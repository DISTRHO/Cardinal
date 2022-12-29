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

#include <engine/Engine.hpp>
#include <patch.hpp>
#include <system.hpp>

#ifdef NDEBUG
# undef DEBUG
#endif

#include "CardinalRemote.hpp"
#include "PluginContext.hpp"
#include "extra/Base64.hpp"

#if defined(STATIC_BUILD) || CARDINAL_VARIANT_MINI
# undef HAVE_LIBLO
#endif

#ifdef HAVE_LIBLO
# include <lo/lo.h>
#endif

// -----------------------------------------------------------------------------------------------------------

namespace remoteUtils {

#ifdef HAVE_LIBLO
static int osc_handler(const char* const path, const char* const types, lo_arg** argv, const int argc, lo_message, void* const self)
{
    d_stdout("osc_handler(\"%s\", \"%s\", %p, %i)", path, types, argv, argc);

    if (std::strcmp(path, "/resp") == 0 && argc == 2 && types[0] == 's' && types[1] == 's')
    {
        d_stdout("osc_handler(\"%s\", ...) - got resp | '%s' '%s'", path, &argv[0]->s, &argv[1]->s);

        if (std::strcmp(&argv[0]->s, "hello") == 0 && std::strcmp(&argv[1]->s, "ok") == 0)
            static_cast<RemoteDetails*>(self)->connected = true;
    }
    return 0;
}
#endif

RemoteDetails* getRemote()
{
    CardinalPluginContext* const context = static_cast<CardinalPluginContext*>(APP);
    DISTRHO_SAFE_ASSERT_RETURN(context != nullptr, nullptr);

    CardinalBaseUI* const ui = static_cast<CardinalBaseUI*>(context->ui);
    DISTRHO_SAFE_ASSERT_RETURN(ui != nullptr, nullptr);

    return ui->remoteDetails;
}

bool connectToRemote()
{
    CardinalPluginContext* const context = static_cast<CardinalPluginContext*>(APP);
    DISTRHO_SAFE_ASSERT_RETURN(context != nullptr, false);

    CardinalBaseUI* const ui = static_cast<CardinalBaseUI*>(context->ui);
    DISTRHO_SAFE_ASSERT_RETURN(ui != nullptr, false);

    RemoteDetails* remoteDetails = ui->remoteDetails;

#ifdef HAVE_LIBLO
    if (remoteDetails == nullptr)
    {
        const lo_server oscServer = lo_server_new_with_proto(nullptr, LO_UDP, nullptr);
        DISTRHO_SAFE_ASSERT_RETURN(oscServer != nullptr, false);

        remoteDetails = new RemoteDetails;
        remoteDetails->handle = oscServer;
        remoteDetails->connected = false;
        remoteDetails->autoDeploy = false;

        lo_server_add_method(oscServer, "/resp", nullptr, osc_handler, remoteDetails);
    }

    if (const lo_address addr = lo_address_new_with_proto(LO_UDP, REMOTE_HOST, REMOTE_HOST_PORT))
    {
        lo_send(addr, "/hello", "");
        lo_address_free(addr);
    }
#endif

    return remoteDetails != nullptr;
}

void disconnectFromRemote(RemoteDetails* const remote)
{
    if (remote != nullptr)
    {
       #ifdef HAVE_LIBLO
        lo_server_free(static_cast<lo_server>(remote->handle));
        delete remote;
       #endif
    }
}

void idleRemote(RemoteDetails* const remote)
{
#ifdef HAVE_LIBLO
    while (lo_server_recv_noblock(static_cast<lo_server>(remote->handle), 0) != 0) {}
#endif
}

void deployToRemote(RemoteDetails* const remote)
{
#ifdef HAVE_LIBLO
    const lo_address addr = lo_address_new_with_proto(LO_UDP, REMOTE_HOST, REMOTE_HOST_PORT);
    DISTRHO_SAFE_ASSERT_RETURN(addr != nullptr,);

    APP->engine->prepareSave();
    APP->patch->saveAutosave();
    APP->patch->cleanAutosave();
    std::vector<uint8_t> data(rack::system::archiveDirectory(APP->patch->autosavePath, 1));

    if (const lo_blob blob = lo_blob_new(data.size(), data.data()))
    {
        lo_send(addr, "/load", "b", blob);
        lo_blob_free(blob);
    }

    lo_address_free(addr);
#endif
}

void sendScreenshotToRemote(RemoteDetails* const remote, const char* const screenshot)
{
#ifdef HAVE_LIBLO
    const lo_address addr = lo_address_new_with_proto(LO_UDP, REMOTE_HOST, REMOTE_HOST_PORT);
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
