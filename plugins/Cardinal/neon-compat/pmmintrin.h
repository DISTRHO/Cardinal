#pragma once

#ifdef NDEBUG
# undef DEBUG
#endif

#include "../sse2neon/sse2neon.h"

static inline
void __builtin_ia32_pause()
{
    __asm__ __volatile__("isb\n");
}

static inline
uint32_t _mm_getcsr()
{
    return 0;
}
