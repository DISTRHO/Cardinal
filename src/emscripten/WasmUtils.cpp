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

#include <emscripten/html5.h>

// -----------------------------------------------------------------------------------------------------------

namespace CardinalDISTRHO {

long d_emscripten_set_interval(void (*cb)(void* userData), double intervalMsecs, void* userData)
{
    return emscripten_set_interval(cb, intervalMsecs, userData);
}

void d_emscripten_clear_interval(long setIntervalId)
{
    emscripten_clear_interval(setIntervalId);
}

}

// -----------------------------------------------------------------------------------------------------------
