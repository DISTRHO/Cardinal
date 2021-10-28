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

#pragma once

#ifdef HEADLESS
# define GL_COLOR_BUFFER_BIT 0
# define GL_DEPTH_BUFFER_BIT 0
# define GL_STENCIL_BUFFER_BIT 0
# define GL_PROJECTION 0
# define GL_TRIANGLES 0
static inline void glBegin(int) {}
static inline void glEnd() {}
static inline void glColor3f(float, float, float) {}
static inline void glVertex3f(float, float, float) {}
static inline void glClear(int) {}
static inline void glClearColor(double, double, double, double) {}
static inline void glLoadIdentity() {}
static inline void glMatrixMode(int) {}
static inline void glOrtho(double, double, double, double, double, double) {}
static inline void glViewport(double, double, double, double) {}
typedef unsigned int GLuint;
#else
# include_next <GL/gl.h>
#endif
