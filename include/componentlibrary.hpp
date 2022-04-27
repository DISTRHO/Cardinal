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

#define SCHEME_YELLOW SCHEME_YELLOW_OldVCV
#include_next "componentlibrary.hpp"
#undef SCHEME_YELLOW

namespace rack {
namespace componentlibrary {

// Yellow? What's that?
static const NVGcolor SCHEME_YELLOW = nvgRGBf(0.76f, 0.11f, 0.22f);

}
}
