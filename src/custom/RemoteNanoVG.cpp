/*
 * DISTRHO Cardinal Plugin
 * Copyright (C) 2021-2025 Filipe Coelho <falktx@falktx.com>
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

#include "DistrhoPluginInfo.h"

#ifndef DISTRHO_PLUGIN_WANT_DIRECT_ACCESS
# error wrong build 1
#endif
#if DISTRHO_PLUGIN_WANT_DIRECT_ACCESS && !defined(HEADLESS)
# error wrong build 2
#endif

#include "nanovg.h"

struct NVGLUframebuffer;
void nvgluBindFramebuffer(NVGLUframebuffer* fb) {}
NVGLUframebuffer* nvgluCreateFramebuffer(NVGcontext* ctx, int w, int h, int imageFlags) { return nullptr; }
void nvgluDeleteFramebuffer(NVGLUframebuffer* fb) {}

#if defined(__GNUC__) && (__GNUC__ >= 6)
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wmisleading-indentation"
# pragma GCC diagnostic ignored "-Wshift-negative-value"
#endif

#include "nanovg.c"

#if defined(__GNUC__) && (__GNUC__ >= 6)
# pragma GCC diagnostic pop
#endif

#define GLFWAPI

extern "C" {

typedef struct GLFWcursor GLFWcursor;
typedef struct GLFWwindow GLFWwindow;

GLFWAPI const char* glfwGetClipboardString(GLFWwindow*) { return nullptr; }
GLFWAPI void glfwSetClipboardString(GLFWwindow*, const char*) {}
GLFWAPI GLFWcursor* glfwCreateStandardCursor(int) { return nullptr; }
GLFWAPI void glfwSetCursor(GLFWwindow*, GLFWcursor*) {}
GLFWAPI const char* glfwGetKeyName(int, int) { return nullptr; }
GLFWAPI int glfwGetKey(GLFWwindow*, int) { return 0; }
GLFWAPI int glfwGetKeyScancode(int) { return 0; }
GLFWAPI double glfwGetTime(void) { return 0.0; }

}
