/*
 * DISTRHO Cardinal Plugin
 * Copyright (C) 2021 Filipe Coelho <falktx@falktx.com>
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

#include_next "common.hpp"

// Make binary resources work the same no matter the OS
#undef BINARY
#undef BINARY_START
#undef BINARY_END
#undef BINARY_SIZE

#define BINARY(sym) extern const unsigned char sym[]; extern const unsigned int sym##_len
#define BINARY_START(sym) ((const void*) sym)
#define BINARY_END(sym) ((const void*) sym + sym##_len)
#define BINARY_SIZE(sym) (sym##_len)
