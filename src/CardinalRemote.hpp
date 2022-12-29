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

#pragma once

#ifdef HAVE_LIBLO
// # define REMOTE_HOST "localhost"
# define REMOTE_HOST "192.168.51.1"
# define REMOTE_HOST_PORT "2228"
#endif

// -----------------------------------------------------------------------------------------------------------

namespace remoteUtils {

struct RemoteDetails {
    void* handle;
    bool connected;
    bool autoDeploy;
};

RemoteDetails* getRemote();
bool connectToRemote();
void disconnectFromRemote(RemoteDetails* remote);
void idleRemote(RemoteDetails* remote);
void deployToRemote(RemoteDetails* remote);
void sendScreenshotToRemote(RemoteDetails* remote, const char* screenshot);

}

// -----------------------------------------------------------------------------------------------------------
