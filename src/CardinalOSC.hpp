/*
 * DISTRHO Cardinal Plugin
 * Copyright (C) 2021-2026 Filipe Coelho <falktx@falktx.com>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "DistrhoUtils.hpp"

#if defined(HAVE_LIBLO) && defined(HEADLESS)
# define CARDINAL_INIT_OSC_THREAD
#endif

#ifdef HAVE_LIBLO
# include <lo/lo_types.h>
#endif

START_NAMESPACE_DISTRHO

class CardinalBasePlugin;
struct Initializer;

#ifdef HAVE_LIBLO
# define CARDINAL_OSC_FIELDS \
    lo_server oscServer = nullptr; \
    lo_server_thread oscServerThread = nullptr; \
    CardinalBasePlugin* remotePluginInstance = nullptr;
#else
# define CARDINAL_OSC_FIELDS
#endif

#ifdef HAVE_LIBLO
# define CARDINAL_OSC_METHODS \
    bool startRemoteServer(const char* port); \
    void stopRemoteServer(); \
    void stepRemoteServer();
#else
# define CARDINAL_OSC_METHODS
#endif

void oscInitState(Initializer* self);
void oscShutdownState(Initializer* self);

END_NAMESPACE_DISTRHO
