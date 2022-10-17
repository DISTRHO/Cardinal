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

#ifndef PLUGIN_BRAND
# error PLUGIN_BRAND undefined
#endif

#ifndef PLUGIN_LABEL
# error PLUGIN_LABEL undefined
#endif

#ifndef PLUGIN_MODEL
# error PLUGIN_MODEL undefined
#endif

#ifndef PLUGIN_CV_INPUTS
# error PLUGIN_CV_INPUTS undefined
#endif

#ifndef PLUGIN_CV_OUTPUTS
# error PLUGIN_CV_OUTPUTS undefined
#endif

enum PortType {
    Audio = 0,
    Bi = 1,
    Uni = 2,
};

static constexpr const PortType kCvInputs[] = PLUGIN_CV_INPUTS;
static constexpr const PortType kCvOutputs[] = PLUGIN_CV_OUTPUTS;
