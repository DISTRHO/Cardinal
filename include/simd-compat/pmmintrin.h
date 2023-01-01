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
// add missing calls, see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=95399
# ifdef __arm__
//#  define SIMDE_ARM_NEON_A64V8_NO_NATIVE
#  if 0
#  include <arm_neon.h>
// custom vcvtnq_s32_f32 implementation for armv7, based on _mm_cvtps_epi32 from sse2neon
/*
 * sse2neon is freely redistributable under the MIT License.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
static __inline__ int32x4_t __attribute__((__always_inline__, __nodebug__))
vcvtnq_s32_f32(const float32x4_t a)
{
    const uint32x4_t signmask = vdupq_n_u32(0x80000000);
    const float32x4_t half = vbslq_f32(signmask, a, vdupq_n_f32(0.5f)); /* +/- 0.5 */
    const int32x4_t r_normal = vcvtq_s32_f32(vaddq_f32(a, half)); /* round to integer: [a + 0.5]*/
    const int32x4_t r_trunc = vcvtq_s32_f32(a); /* truncate to integer: [a] */
    const int32x4_t plusone = vreinterpretq_s32_u32(vshrq_n_u32(vreinterpretq_u32_s32(vnegq_s32(r_trunc)), 31)); /* 1 or 0 */
    const int32x4_t r_even = vbicq_s32(vaddq_s32(r_trunc, plusone), vdupq_n_s32(1)); /* ([a] + {0,1}) & ~1 */
    const float32x4_t delta = vsubq_f32(a, vcvtq_f32_s32(r_trunc)); /* compute delta: delta = (a - [a]) */
    const uint32x4_t is_delta_half = vceqq_f32(delta, half); /* delta == +/- 0.5 */
    return vbslq_s32(is_delta_half, r_even, r_normal);
}
#  endif
# endif
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
