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

#pragma once

#include "plugin.hpp"
#include "DistrhoUtils.hpp"

// -----------------------------------------------------------------------------------------------------------
// from PluginContext.hpp

START_NAMESPACE_DISTRHO

static constexpr const uint32_t kModuleParameters = 24;

class Plugin;

struct CardinalPluginContext : rack::Context {
    uint32_t bufferSize;
    double sampleRate;
    float parameters[kModuleParameters];
    bool playing, reset, bbtValid, loadedHostCV;
    int32_t bar, beat, beatsPerBar, beatType;
    uint64_t frame;
    double barStartTick, beatsPerMinute;
    double tick, tickClock, ticksPerBeat, ticksPerClock, ticksPerFrame;
    uintptr_t nativeWindowId;
    uint32_t dataFrame;
    const float** dataIns;
    float** dataOuts;
    Plugin* const plugin;
    CardinalPluginContext(Plugin* const p);
};

END_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------------------------------------
