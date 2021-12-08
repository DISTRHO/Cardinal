/*
 * DISTRHO Cardinal Plugin
 * Copyright (C) 2021 Filipe Coelho <falktx@falktx.com>
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

#ifndef HEADLESS
#include "OpenGL.hpp"
#endif

#include "nanovg.h"

#ifdef HEADLESS
struct NVGLUframebuffer;
void nvgluBindFramebuffer(NVGLUframebuffer* fb) {}
NVGLUframebuffer* nvgluCreateFramebuffer(NVGcontext* ctx, int w, int h, int imageFlags) { return nullptr; }
void nvgluDeleteFramebuffer(NVGLUframebuffer* fb) {}
#else
# define NANOVG_GLES2_IMPLEMENTATION
# define NANOVG_FBO_VALID 1
# include "nanovg_gl.h"
# include "nanovg_gl_utils.h"
#endif

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

typedef struct GLFWwindow GLFWwindow;

GLFWAPI const char* glfwGetClipboardString(GLFWwindow* window) { return nullptr; }
GLFWAPI void glfwSetClipboardString(GLFWwindow* window, const char*) {}
GLFWAPI const char* glfwGetKeyName(int key, int scancode) { return nullptr; }
GLFWAPI int glfwGetKeyScancode(int key) { return 0; }
GLFWAPI double glfwGetTime(void) { return 0.0; }

}

#ifndef HEADLESS
# define STB_IMAGE_WRITE_IMPLEMENTATION
# include "../src/Rack/dep/glfw/deps/stb_image_write.h"
#endif
