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

// this file matches the top of `OpenGL.hpp` provided by DPF

#pragma once

#ifdef NDEBUG
# undef DEBUG
#endif

/* Check OS */
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
# define DISTRHO_OS_WINDOWS 1
#else
# if defined(__APPLE__)
#  define DISTRHO_OS_MAC 1
# endif
#endif

// -----------------------------------------------------------------------
// Fix OpenGL includes for Windows, based on glfw code (part 1)

#undef DGL_CALLBACK_DEFINED
#undef DGL_WINGDIAPI_DEFINED

#ifdef DISTRHO_OS_WINDOWS

#ifndef APIENTRY
# define APIENTRY __stdcall
#endif // APIENTRY

/* We need WINGDIAPI defined */
#ifndef WINGDIAPI
# if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__POCC__)
#  define WINGDIAPI __declspec(dllimport)
# elif defined(__LCC__)
#  define WINGDIAPI __stdcall
# else
#  define WINGDIAPI extern
# endif
# define DGL_WINGDIAPI_DEFINED
#endif // WINGDIAPI

/* Some <GL/glu.h> files also need CALLBACK defined */
#ifndef CALLBACK
# if defined(_MSC_VER)
#  if (defined(_M_MRX000) || defined(_M_IX86) || defined(_M_ALPHA) || defined(_M_PPC)) && !defined(MIDL_PASS)
#   define CALLBACK __stdcall
#  else
#   define CALLBACK
#  endif
# else
#  define CALLBACK __stdcall
# endif
# define DGL_CALLBACK_DEFINED
#endif // CALLBACK

/* Most GL/glu.h variants on Windows need wchar_t */
#include <cstddef>

#endif // DISTRHO_OS_WINDOWS

// -----------------------------------------------------------------------
// OpenGL includes

#ifdef DISTRHO_OS_MAC
# ifdef DGL_USE_OPENGL3
#  include <OpenGL/gl3.h>
#  include <OpenGL/gl3ext.h>
# else
#  include <OpenGL/gl.h>
# endif
#else
# ifdef DISTRHO_OS_WINDOWS
#  define GL_GLEXT_PROTOTYPES
# endif
# ifndef __GLEW_H__
#  include <GL/gl.h>
#  include <GL/glext.h>
# endif
#endif

// -----------------------------------------------------------------------
// Missing OpenGL defines

#if defined(GL_BGR_EXT) && !defined(GL_BGR)
# define GL_BGR GL_BGR_EXT
#endif

#if defined(GL_BGRA_EXT) && !defined(GL_BGRA)
# define GL_BGRA GL_BGRA_EXT
#endif

#ifndef GL_CLAMP_TO_BORDER
# define GL_CLAMP_TO_BORDER 0x812D
#endif

// -----------------------------------------------------------------------
// Fix OpenGL includes for Windows, based on glfw code (part 2)

#ifdef DGL_CALLBACK_DEFINED
# undef CALLBACK
# undef DGL_CALLBACK_DEFINED
#endif

#ifdef DGL_WINGDIAPI_DEFINED
# undef WINGDIAPI
# undef DGL_WINGDIAPI_DEFINED
#endif
