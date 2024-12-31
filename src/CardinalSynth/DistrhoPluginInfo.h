/*
 * DISTRHO Cardinal Plugin
 * Copyright (C) 2021-2024 Filipe Coelho <falktx@falktx.com>
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

#ifndef DISTRHO_PLUGIN_INFO_H_INCLUDED
#define DISTRHO_PLUGIN_INFO_H_INCLUDED

#define CARDINAL_VARIANT_MAIN   0
#define CARDINAL_VARIANT_MINI   0
#define CARDINAL_VARIANT_FX     0
#define CARDINAL_VARIANT_NATIVE 0
#define CARDINAL_VARIANT_SYNTH  1

#define CARDINAL_NUM_AUDIO_INPUTS  0
#define CARDINAL_NUM_AUDIO_OUTPUTS 2

#define DISTRHO_PLUGIN_BRAND   "DISTRHO"
#define DISTRHO_PLUGIN_NAME    "Cardinal Synth"
#define DISTRHO_PLUGIN_LABEL   "CardinalSynth"
#define DISTRHO_PLUGIN_URI     "https://distrho.kx.studio/plugins/cardinal#synth"
#define DISTRHO_PLUGIN_CLAP_ID "studio.kx.distrho.cardinal#synth"

#define DISTRHO_PLUGIN_AU_TYPE   aumu
#define DISTRHO_PLUGIN_BRAND_ID  Dstr
#define DISTRHO_PLUGIN_UNIQUE_ID DcnS

#ifdef HEADLESS
#define DISTRHO_PLUGIN_HAS_UI             0
#define DISTRHO_PLUGIN_WANT_DIRECT_ACCESS 0
#else
#define DISTRHO_PLUGIN_HAS_UI             1
#define DISTRHO_PLUGIN_WANT_DIRECT_ACCESS 1
#define DISTRHO_UI_FILE_BROWSER           1
#define DISTRHO_UI_USE_NANOVG             1
#define DISTRHO_UI_USER_RESIZABLE         1
#define DISTRHO_UI_DEFAULT_WIDTH          1228
#define DISTRHO_UI_DEFAULT_HEIGHT         666
#endif
#define DISTRHO_PLUGIN_IS_SYNTH           1
#define DISTRHO_PLUGIN_NUM_INPUTS         CARDINAL_NUM_AUDIO_INPUTS
#define DISTRHO_PLUGIN_NUM_OUTPUTS        CARDINAL_NUM_AUDIO_OUTPUTS
#define DISTRHO_PLUGIN_WANT_MIDI_AS_MPE   1
#define DISTRHO_PLUGIN_WANT_MIDI_INPUT    1
#define DISTRHO_PLUGIN_WANT_MIDI_OUTPUT   1
#define DISTRHO_PLUGIN_WANT_FULL_STATE    1
#define DISTRHO_PLUGIN_WANT_STATE         1
#define DISTRHO_PLUGIN_WANT_TIMEPOS       1

#define DPF_VST3_DONT_USE_BRAND_ID

#endif // DISTRHO_PLUGIN_INFO_H_INCLUDED
