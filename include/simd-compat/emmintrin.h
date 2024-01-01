/*
 * DISTRHO Cardinal Plugin
 * Copyright (C) 2021-2024 Filipe Coelho <falktx@falktx.com>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "simd-compat.h"

#if defined(CARDINAL_INCLUDING_IMMINTRIN_H) || defined(SIMDE_X86_SSE2_NATIVE)
# include_next <emmintrin.h>
#else
# include "mmintrin.h"
# include "xmmintrin.h"
# define SIMDE_ENABLE_NATIVE_ALIASES
# include "simde/x86/sse2.h"
# undef SIMDE_ENABLE_NATIVE_ALIASES
#endif
