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

#include "ValleyAudio/src/Common/DSP/OnePoleFilters.cpp"
#include "ValleyAudio/src/Common/DSP/LinearEnvelope.cpp"
#include "ValleyAudio/src/Plateau/Dattorro.cpp"
#include "ValleyAudio/src/Plateau/Plateau.cpp"

#define PLUGIN_BRAND "Valley Audio"
#define PLUGIN_LABEL "Plateau"
#define PLUGIN_MODEL modelPlateau
#define PLUGIN_CV_INPUTS {Audio,Audio,Bi,Bi,Bi,Bi,Bi,Bi,Bi,Bi,Bi,Bi,Bi,Bi,Bi,Bi,Bi}
#define PLUGIN_CV_OUTPUTS {Audio,Audio}
#define PLUGIN_LV2_CATEGORY "lv2:ReverbPlugin"

#include "lv2plugin.cpp"
#include "export.cpp"
