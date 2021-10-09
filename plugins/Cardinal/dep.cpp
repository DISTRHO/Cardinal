// This source file compiles those nice implementation-in-header little libraries

#include <common.hpp> // for fopen_u8

#ifdef NDEBUG
# undef DEBUG
#endif
#include "OpenGL.hpp"

#define BLENDISH_IMPLEMENTATION
#include <blendish.h>

#define NANOSVG_IMPLEMENTATION
#define NANOSVG_ALL_COLOR_KEYWORDS
#include <nanosvg.h>
