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

#if defined(__i386__) || defined(__x86_64__)
# include_next <pmmintrin.h>

#elif defined(__EMSCRIPTEN__)
# include_next <pmmintrin.h>

// NOTE these have been verified to be unused (included for ValleyAudio builds)
// static inline
// __m64 _mm_and_si64(__m64 a, __m64 b) { return a; }
// 
// static inline
// __m64 _mm_andnot_si64(__m64 a, __m64 b) { return a; }
// 
// static inline
// __m64 _mm_or_si64(__m64 a, __m64 b) { return a; }

static inline
__m64 _mm_set1_pi16(short w)
{
    return __extension__ (__m64){ static_cast<float>(w), static_cast<float>(w) };
}

#else
# include "../sse2neon/sse2neon.h"

static inline
void __builtin_ia32_pause()
{
    __asm__ __volatile__("isb\n");
}

static inline
__m64 _mm_and_si64(__m64 a, __m64 b)
{
    return vreinterpret_s64_s32(vand_s32(vreinterpret_s32_m64(a), vreinterpret_s32_m64(b)));
}

static inline
__m64 _mm_andnot_si64(__m64 a, __m64 b)
{
    // *NOTE* argument swap
    return vreinterpret_s64_s32(vbic_s32(vreinterpret_s32_m64(b), vreinterpret_s32_m64(a)));
}

static inline
__m64 _mm_or_si64(__m64 a, __m64 b)
{
    return vreinterpret_s64_s32(vorr_s32(vreinterpret_s32_m64(a), vreinterpret_s32_m64(b)));
}

static inline
__m64 _mm_set1_pi16(short w)
{
    return vreinterpret_s64_s16(vdup_n_s16(w));
}

#endif
