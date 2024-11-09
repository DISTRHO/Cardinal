/*
 * DISTRHO Cardinal Plugin
 * Copyright (C) 2021-2024 Filipe Coelho <falktx@falktx.com>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#define CARDINAL_DEFAULT_REMOTE_PORT "2228"
#define CARDINAL_DEFAULT_REMOTE_URL "osc.udp://192.168.51.1:2228"

// -----------------------------------------------------------------------------------------------------------

namespace remoteUtils {

struct RemoteDetails {
    void* handle;
    const char* url;
    bool autoDeploy;
    bool first;
    bool connected;
    bool screenshot;
};

RemoteDetails* getRemote();
bool connectToRemote(const char* url);
void disconnectFromRemote(RemoteDetails* remote);
void idleRemote(RemoteDetails* remote);
void sendParamChangeToRemote(RemoteDetails* remote, int64_t moduleId, int paramId, float value);
void sendFullPatchToRemote(RemoteDetails* remote);
void sendScreenshotToRemote(RemoteDetails* remote, const char* screenshot);

}

// -----------------------------------------------------------------------------------------------------------
