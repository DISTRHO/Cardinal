/*
 * DISTRHO Cardinal Plugin
 * Copyright (C) 2021-2024 Filipe Coelho <falktx@falktx.com>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "simd-compat.h"

#ifdef SIMDE_X86_SSE_NATIVE
# define CARDINAL_INCLUDING_IMMINTRIN_H
# include_next <immintrin.h>
# undef CARDINAL_INCLUDING_IMMINTRIN_H
#else
# include "mmintrin.h"
# include "xmmintrin.h"
# include "emmintrin.h"
# include "pmmintrin.h"
# include "tmmintrin.h"
# include "smmintrin.h"
#endif
