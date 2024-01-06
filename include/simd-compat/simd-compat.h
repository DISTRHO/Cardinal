/*
 * DISTRHO Cardinal Plugin
 * Copyright (C) 2021-2024 Filipe Coelho <falktx@falktx.com>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

// native up to SSE3
#if (defined(_M_X64) || defined(__amd64__) || defined(__SSE2__) || (defined(_M_IX86_FP) && _M_IX86_FP == 2)) && !defined(__EMSCRIPTEN__) && !defined(CARDINAL_NOSIMD)
# define SIMDE_X86_MMX_NATIVE
# define SIMDE_X86_SSE_NATIVE
# define SIMDE_X86_SSE2_NATIVE
# define SIMDE_X86_SSE3_NATIVE
#else
# define SIMDE_X86_MMX_NO_NATIVE
# define SIMDE_X86_SSE_NO_NATIVE
# define SIMDE_X86_SSE2_NO_NATIVE
# define SIMDE_X86_SSE3_NO_NATIVE
#endif

// everything else is emulated
#define SIMDE_X86_SSSE3_NO_NATIVE
#define SIMDE_X86_SSE4_1_NO_NATIVE
#define SIMDE_X86_SSE4_2_NO_NATIVE
#define SIMDE_X86_XOP_NO_NATIVE
#define SIMDE_X86_AVX_NO_NATIVE
#define SIMDE_X86_AVX2_NO_NATIVE
#define SIMDE_X86_FMA_NO_NATIVE
#define SIMDE_X86_AVX512F_NO_NATIVE
#define SIMDE_X86_AVX512BF16_NO_NATIVE
#define SIMDE_X86_AVX512BW_NO_NATIVE
#define SIMDE_X86_AVX512VL_NO_NATIVE
#define SIMDE_X86_AVX512DQ_NO_NATIVE
#define SIMDE_X86_AVX512CD_NO_NATIVE
#define SIMDE_X86_AVX5124VNNIW_NO_NATIVE
#define SIMDE_X86_AVX512VNNI_NO_NATIVE
#define SIMDE_X86_AVX512VBMI2_NO_NATIVE
#define SIMDE_X86_AVX512VBMI_NO_NATIVE
#define SIMDE_X86_AVX512BITALG_NO_NATIVE
#define SIMDE_X86_AVX512VPOPCNTDQ_NO_NATIVE
#define SIMDE_X86_AVX512VP2INTERSECT_NO_NATIVE
#define SIMDE_X86_SVML_NO_NATIVE

// control wasm simd state
#ifdef __EMSCRIPTEN__
# ifdef CARDINAL_NOSIMD
#  define SIMDE_WASM_SIMD128_NO_NATIVE
# else
#  define SIMDE_WASM_SIMD128_NATIVE
# endif
#endif

// fix win32 build
#ifdef _WIN32
static inline
float simde_math_roundevenf(float v) {
    float rounded = __builtin_roundf(v);
    float diff = rounded - v;
    if (__builtin_expect(!!(__builtin_fabsf(diff) == 0.5f) && ((int)rounded & 1), 0)) {
        rounded = v - diff;
    }
    return rounded;
}
#define simde_math_roundevenf simde_math_roundevenf
#endif
