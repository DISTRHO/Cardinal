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

#if (defined(__i386__) || defined(__x86_64__)) && !defined(CARDINAL_NOSIMD)
# include_next <pmmintrin.h>

// bring in extra SSE3 support via simde
# define SIMDE_X86_SSE2_NATIVE
# define SIMDE_X86_SSE3_ENABLE_NATIVE_ALIASES

// make sure to not include windows.h here
# ifdef _WIN32
#  define _WIN32_WAS_DEFINED
#  undef _WIN32
# endif

// assume SSE3 only on macOS
# ifndef ARCH_MAC
#  include "../simde/simde/x86/sse3.h"
# endif

# ifdef _WIN32_WAS_DEFINED
#  define _WIN32
#  undef _WIN32_WAS_DEFINED
# endif

#elif defined(__EMSCRIPTEN__) && !defined(CARDINAL_NOSIMD)
# include_next <pmmintrin.h>

static __inline__ __m64 __attribute__((__always_inline__, __nodebug__))
_mm_set1_pi16(short w)
{
    return __extension__ (__m64){ static_cast<float>(w), static_cast<float>(w) };
}

/*
#elif defined(__ARM_NEON)
# include "../sse2neon/sse2neon.h"

static inline
void __builtin_ia32_pause()
{
    __asm__ __volatile__("isb\n");
}

static inline
__m64 _mm_set1_pi16(short w)
{
    return vreinterpret_s64_s16(vdup_n_s16(w));
}
*/

#else
# define SIMDE_ACCURACY_PREFERENCE 0
# define SIMDE_ENABLE_NATIVE_ALIASES
# define SIMDE_FAST_CONVERSION_RANGE
# define SIMDE_FAST_MATH
# define SIMDE_FAST_NANS
# define SIMDE_FAST_ROUND_MODE
# define SIMDE_FAST_ROUND_TIES
# include "../simde/simde/x86/sse.h"
# include "../simde/simde/x86/sse2.h"
# include "../simde/simde/x86/sse3.h"

#endif
