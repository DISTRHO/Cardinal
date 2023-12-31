/*
 * DISTRHO Cardinal Plugin
 * Copyright (C) 2021-2024 Filipe Coelho <falktx@falktx.com>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "simd-compat.h"

#ifdef SIMDE_X86_SSE3_NATIVE
# include_next <pmmintrin.h>
#else
# include "mmintrin.h"
# include "xmmintrin.h"
# include "emmintrin.h"
# define SIMDE_ENABLE_NATIVE_ALIASES
# include "simde/x86/sse3.h"
# undef SIMDE_ENABLE_NATIVE_ALIASES
#endif
