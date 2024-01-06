/*
 * DISTRHO Cardinal Plugin
 * Copyright (C) 2021-2024 Filipe Coelho <falktx@falktx.com>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "simd-compat.h"

#if defined(CARDINAL_INCLUDING_IMMINTRIN_H) || defined(SIMDE_X86_SSE4_1_NATIVE)
# include_next <smmintrin.h>
#else
# include "mmintrin.h"
# include "xmmintrin.h"
# include "emmintrin.h"
# include "pmmintrin.h"
# include "tmmintrin.h"
# define SIMDE_ENABLE_NATIVE_ALIASES
# include "simde/x86/sse4.1.h"
# include "simde/x86/sse4.2.h"
# undef SIMDE_ENABLE_NATIVE_ALIASES
#endif
