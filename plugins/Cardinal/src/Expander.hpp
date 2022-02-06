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

#include "CarlaNative.h"

template<int numInputs, int numOutputs>
struct CardinalExpander : Module {
    static const constexpr int kNumInputs = numInputs;
    static const constexpr int kNumOutputs = numOutputs;
};

struct CardinalExpanderFromCVToCarlaMIDI : CardinalExpander<6, 0> {
    static const constexpr uint MAX_MIDI_EVENTS = 128;
    // continuously filled up by expander, flushed on each new block frame
    // frames are related to host block size
    uint frame, midiEventCount;
    NativeMidiEvent midiEvents[MAX_MIDI_EVENTS];
};

struct CardinalExpanderFromCarlaMIDIToCV : CardinalExpander<0, 6> {
    static const constexpr uint MAX_MIDI_EVENTS = 128;
    // filled up by connector-side in bursts, must be reset on next cycle by expander
    // frames are not related to any particular block size
    uint midiEventCount;
    NativeMidiEvent midiEvents[MAX_MIDI_EVENTS];
};
