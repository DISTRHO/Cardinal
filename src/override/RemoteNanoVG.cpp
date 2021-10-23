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

#include "OpenGL.hpp"

#include "src/nanovg/nanovg.h"

#define NANOVG_GL2_IMPLEMENTATION
// #define NANOVG_GLES2_IMPLEMENTATION
#include "src/nanovg/nanovg_gl.h"

#define NANOVG_FBO_VALID 1
#include "src/nanovg/nanovg_gl_utils.h"

#if defined(__GNUC__) && (__GNUC__ >= 6)
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wmisleading-indentation"
# pragma GCC diagnostic ignored "-Wshift-negative-value"
#endif

#include "src/nanovg/nanovg.c"

#if defined(__GNUC__) && (__GNUC__ >= 6)
# pragma GCC diagnostic pop
#endif

// typedef struct NVGLUframebuffer {
// 	NVGcontext* ctx;
// 	GLuint fbo;
// 	GLuint rbo;
// 	GLuint texture;
// 	int image;
// } NVGLUframebuffer;
//
// void nvgluBindFramebuffer(NVGLUframebuffer* fb) {}
// NVGLUframebuffer* nvgluCreateFramebuffer(NVGcontext* ctx, int w, int h, int imageFlags) { return nullptr; }
// void nvgluDeleteFramebuffer(NVGLUframebuffer* fb) {}

#define GLFWAPI

extern "C" {

typedef struct GLFWwindow GLFWwindow;

GLFWAPI const char* glfwGetClipboardString(GLFWwindow* window) { return nullptr; }
GLFWAPI void glfwSetClipboardString(GLFWwindow* window, const char*) {}
GLFWAPI const char* glfwGetKeyName(int key, int scancode) { return nullptr; }
GLFWAPI int glfwGetKeyScancode(int key) { return 0; }

}

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../src/Rack/dep/glfw/deps/stb_image_write.h"
