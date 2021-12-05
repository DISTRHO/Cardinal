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

/* On mingw stdio functions like printf are not inline, and thus get defined every time they are used.
 * We go through a few steps to ensure unique symbol names.
 */
#ifdef STDIO_OVERRIDE

// helper macros
# define STDIO_OVERRIDE_HELPER(NS, SEP, FN) NS ## SEP ## FN
# define STDIO_OVERRIDE_MACRO(NS, FN) STDIO_OVERRIDE_HELPER(NS, _, FN)

// step 1: prefix the needed stdio functions
# define printf STDIO_OVERRIDE_MACRO(STDIO_OVERRIDE, printf)

// step 2: inlude <stdio.h> which will use our prefixed names
# include <stdio.h>

#else // STDIO_OVERRIDE

// if STDIO_OVERRIDE is not defined, we have nothing to do
# include_next <cstdio>

#endif // STDIO_OVERRIDE
