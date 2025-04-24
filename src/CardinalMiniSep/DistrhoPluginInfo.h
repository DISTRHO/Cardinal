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

#ifndef DISTRHO_PLUGIN_INFO_H_INCLUDED
#define DISTRHO_PLUGIN_INFO_H_INCLUDED

#define CARDINAL_VARIANT_MAIN   0
#define CARDINAL_VARIANT_MINI   1
#define CARDINAL_VARIANT_FX     0
#define CARDINAL_VARIANT_NATIVE 0
#define CARDINAL_VARIANT_SYNTH  0

#define CARDINAL_NUM_AUDIO_INPUTS  2
#define CARDINAL_NUM_AUDIO_OUTPUTS 2

#define DISTRHO_PLUGIN_BRAND   "DISTRHO"
#define DISTRHO_PLUGIN_URI     "https://distrho.kx.studio/plugins/cardinal#mini"

#define DISTRHO_PLUGIN_NAME  "Cardinal Mini"
#define DISTRHO_PLUGIN_LABEL "CardinalMini"

#define DISTRHO_PLUGIN_HAS_UI             1
#define DISTRHO_PLUGIN_WANT_DIRECT_ACCESS 0
#define DISTRHO_UI_FILE_BROWSER           1
#define DISTRHO_UI_USE_NANOVG             1
#define DISTRHO_UI_USER_RESIZABLE         1
#define DISTRHO_UI_DEFAULT_WIDTH          1000
#define DISTRHO_UI_DEFAULT_HEIGHT         600
#define DISTRHO_PLUGIN_IS_SYNTH           0
#define DISTRHO_PLUGIN_NUM_INPUTS         CARDINAL_NUM_AUDIO_INPUTS + 5
#define DISTRHO_PLUGIN_NUM_OUTPUTS        CARDINAL_NUM_AUDIO_OUTPUTS + 5
#define DISTRHO_PLUGIN_WANT_MIDI_AS_MPE   1
#define DISTRHO_PLUGIN_WANT_MIDI_INPUT    1
#define DISTRHO_PLUGIN_WANT_MIDI_OUTPUT   1
#define DISTRHO_PLUGIN_WANT_FULL_STATE    1
#define DISTRHO_PLUGIN_WANT_STATE         1
#define DISTRHO_PLUGIN_WANT_TIMEPOS       1
#define DISTRHO_PLUGIN_USES_CUSTOM_MODGUI 0
#define DISTRHO_PLUGIN_LV2_CATEGORY       "mod:ControlVoltagePlugin, lv2:UtilityPlugin"

#define DISTRHO_PLUGIN_USES_MODGUI 1

#endif // DISTRHO_PLUGIN_INFO_H_INCLUDED
