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

#include "rackwindows/src/mv.cpp"

#define PLUGIN_BRAND "Rackwindows"
#define PLUGIN_LABEL "MV"
#define PLUGIN_MODEL modelMv
#define PLUGIN_CV_INPUTS {Bi,Bi,Bi,Bi,Audio,Audio}
#define PLUGIN_CV_OUTPUTS {Audio,Audio}
#define PLUGIN_LV2_CATEGORY "lv2:ReverbPlugin"

#include "lv2plugin.cpp"
#include "export.cpp"
