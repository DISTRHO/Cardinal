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

#include "Fundamental/src/SEQ3.cpp"

#define PLUGIN_BRAND "VCV Fundamental"
#define PLUGIN_LABEL "SEQ3"
#define PLUGIN_MODEL modelSEQ3
#define PLUGIN_CV_INPUTS {Bi,Uni,Uni,Uni,Uni}
#define PLUGIN_CV_OUTPUTS {          \
    /* trigger */                    \
    Uni,                             \
    /* 3 cv outs */                  \
    Bi,Bi,Bi,                        \
    /* 8 steps */                    \
    Uni,Uni,Uni,Uni,Uni,Uni,Uni,Uni, \
    /* steps, clk, run, reset */     \
    Uni,Uni,Uni,Uni                  \
}
#define PLUGIN_LV2_CATEGORY "mod:CVPlugin"

#include "lv2plugin.cpp"
#include "export.cpp"
