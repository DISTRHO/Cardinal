/*
 * DISTRHO Cardinal Plugin
 * Copyright (C) 2021-2024 Filipe Coelho <falktx@falktx.com>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "simd-compat.h"

#if defined(CARDINAL_INCLUDING_IMMINTRIN_H) || defined(SIMDE_X86_SSE_NATIVE)
# include_next <xmmintrin.h>
#else
# include "mmintrin.h"
# define SIMDE_ENABLE_NATIVE_ALIASES
# include "simde/x86/sse.h"
# undef SIMDE_ENABLE_NATIVE_ALIASES
// always use SSE2 mode, as seen in gcc
# include "emmintrin.h"
#endif
