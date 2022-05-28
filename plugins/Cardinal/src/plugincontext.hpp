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

#include "plugin.hpp"
#include "DistrhoUtils.hpp"

#ifndef HEADLESS
# include "../dgl/Base.hpp"
#else
# include "extra/LeakDetector.hpp"
#endif

// -----------------------------------------------------------------------------------------------------------
// from PluginContext.hpp

START_NAMESPACE_DISTRHO

static constexpr const uint32_t kModuleParameters = 24;

enum CardinalVariant {
    kCardinalVariantMain,
    kCardinalVariantFX,
    kCardinalVariantSynth,
};

class Plugin;
class UI;

struct MidiEvent {
    static const uint32_t kDataSize = 4;
    uint32_t frame;
    uint32_t size;
    uint8_t data[kDataSize];
    const uint8_t* dataExt;
};

struct CardinalPluginContext : rack::Context {
    uint32_t bufferSize, processCounter;
    double sampleRate;
    float parameters[kModuleParameters];
    CardinalVariant variant;
    bool bypassed, playing, reset, bbtValid;
    int32_t bar, beat, beatsPerBar, beatType;
    uint64_t frame;
    double barStartTick, beatsPerMinute;
    double tick, tickClock, ticksPerBeat, ticksPerClock, ticksPerFrame;
    uintptr_t nativeWindowId;
    const float* const* dataIns;
    float** dataOuts;
    const MidiEvent* midiEvents;
    uint32_t midiEventCount;
    Plugin* const plugin;
#ifndef HEADLESS
    UI* ui;
#endif
    CardinalPluginContext(Plugin* const p);
    void writeMidiMessage(const rack::midi::Message& message, uint8_t channel);
#ifndef HEADLESS
    bool addIdleCallback(IdleCallback* cb) const;
    void removeIdleCallback(IdleCallback* cb) const;
#endif
};

#ifndef HEADLESS
void handleHostParameterDrag(const CardinalPluginContext* pcontext, uint index, bool started);
#endif

END_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------------------------------------
