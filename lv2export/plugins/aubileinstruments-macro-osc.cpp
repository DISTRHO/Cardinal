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

#define BRAIDS_ALWAYS_LOW_CPU_MODE
#include "AudibleInstruments/src/Braids.cpp"
#include "AudibleInstruments/eurorack/stmlib/utils/random.cc"

#define kHighestNote kHighestNoteAnalog
#define kPitchTableStart kPitchTableStartAnalog
#define kOctave kOctaveAnalog
#include "AudibleInstruments/eurorack/braids/analog_oscillator.cc"
#undef kHighestNote
#undef kPitchTableStart
#undef kOctave

#include "AudibleInstruments/eurorack/braids/digital_oscillator.cc"
#include "AudibleInstruments/eurorack/braids/macro_oscillator.cc"
// #include "AudibleInstruments/eurorack/braids/quantizer.cc"
#include "AudibleInstruments/eurorack/braids/resources.cc"

#define PLUGIN_MODEL modelBraids
#define PLUGIN_CV_INPUTS {1,1,1,1,1}
#define PLUGIN_CV_OUTPUTS {0}

#include "lv2plugin.cpp"
#include "export.cpp"
