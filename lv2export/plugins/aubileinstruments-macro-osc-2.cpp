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

#define PLAITS_ALWAYS_LOW_CPU_MODE
#include "AudibleInstruments/src/Plaits.cpp"
#include "AudibleInstruments/eurorack/stmlib/utils/random.cc"
#include "AudibleInstruments/eurorack/stmlib/dsp/units.cc"

#include "AudibleInstruments/eurorack/plaits/dsp/voice.cc"
#include "AudibleInstruments/eurorack/plaits/dsp/engine/additive_engine.cc"
#include "AudibleInstruments/eurorack/plaits/dsp/engine/bass_drum_engine.cc"
#include "AudibleInstruments/eurorack/plaits/dsp/engine/chord_engine.cc"
#include "AudibleInstruments/eurorack/plaits/dsp/engine/fm_engine.cc"
#include "AudibleInstruments/eurorack/plaits/dsp/engine/grain_engine.cc"
#include "AudibleInstruments/eurorack/plaits/dsp/engine/hi_hat_engine.cc"
#include "AudibleInstruments/eurorack/plaits/dsp/engine/modal_engine.cc"
#include "AudibleInstruments/eurorack/plaits/dsp/engine/noise_engine.cc"
#include "AudibleInstruments/eurorack/plaits/dsp/engine/particle_engine.cc"
#include "AudibleInstruments/eurorack/plaits/dsp/engine/snare_drum_engine.cc"
#include "AudibleInstruments/eurorack/plaits/dsp/engine/speech_engine.cc"
#include "AudibleInstruments/eurorack/plaits/dsp/engine/string_engine.cc"
#include "AudibleInstruments/eurorack/plaits/dsp/engine/swarm_engine.cc"
#include "AudibleInstruments/eurorack/plaits/dsp/engine/virtual_analog_engine.cc"
#include "AudibleInstruments/eurorack/plaits/dsp/engine/waveshaping_engine.cc"
#include "AudibleInstruments/eurorack/plaits/dsp/engine/wavetable_engine.cc"

#include "AudibleInstruments/eurorack/plaits/dsp/physical_modelling/modal_voice.cc"
#include "AudibleInstruments/eurorack/plaits/dsp/physical_modelling/resonator.cc"
#include "AudibleInstruments/eurorack/plaits/dsp/physical_modelling/string.cc"
#include "AudibleInstruments/eurorack/plaits/dsp/physical_modelling/string_voice.cc"

#include "AudibleInstruments/eurorack/plaits/dsp/speech/lpc_speech_synth.cc"
#include "AudibleInstruments/eurorack/plaits/dsp/speech/lpc_speech_synth_controller.cc"
#include "AudibleInstruments/eurorack/plaits/dsp/speech/lpc_speech_synth_phonemes.cc"
#include "AudibleInstruments/eurorack/plaits/dsp/speech/lpc_speech_synth_words.cc"
#include "AudibleInstruments/eurorack/plaits/dsp/speech/naive_speech_synth.cc"
#include "AudibleInstruments/eurorack/plaits/dsp/speech/sam_speech_synth.cc"

#include "AudibleInstruments/eurorack/plaits/resources.cc"

#define PLUGIN_MODEL modelPlaits
#define PLUGIN_CV_INPUTS {1,1,1,1,1,1,1,1}
#define PLUGIN_CV_OUTPUTS {0,0}

#include "lv2plugin.cpp"
#include "export.cpp"
