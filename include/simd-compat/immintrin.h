/*
 * DISTRHO Cardinal Plugin
 * Copyright (C) 2021-2024 Filipe Coelho <falktx@falktx.com>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "simd-compat.h"

#if defined(CARDINAL_INCLUDING_EMULATED_IMMINTRIN_H) || defined(SIMDE_X86_SSE_NATIVE)
# define CARDINAL_INCLUDING_IMMINTRIN_H
# include_next <immintrin.h>
# undef CARDINAL_INCLUDING_IMMINTRIN_H
#else
# define CARDINAL_INCLUDING_EMULATED_IMMINTRIN_H
# include "mmintrin.h"
# include "xmmintrin.h"
# include "emmintrin.h"
# include "pmmintrin.h"
# include "tmmintrin.h"
# include "smmintrin.h"
# undef CARDINAL_INCLUDING_EMULATED_IMMINTRIN_H
#endif
