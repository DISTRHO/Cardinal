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

// check if PRIVATE is defined already before including any headers
#ifdef PRIVATE
# define PRIVATE_WAS_DEFINED
#endif

// use mingw print format
#if defined(ARCH_WIN) && !defined(SKIP_MINGW_FORMAT)
# include <regex>
# define format(f,a,b) format(__MINGW_PRINTF_FORMAT,a,b)
#endif

#include_next "common.hpp"

// Make binary resources work the same no matter the OS
#undef BINARY
#undef BINARY_START
#undef BINARY_END
#undef BINARY_SIZE

#define BINARY(sym) extern const unsigned char sym[]; extern const unsigned int sym##_len
#define BINARY_START(sym) ((const void*) sym)
#define BINARY_END(sym) ((const void*) sym + sym##_len)
#define BINARY_SIZE(sym) (sym##_len)

// undefine PRIVATE if needed
#if defined(PRIVATE) && !defined(PRIVATE_WAS_DEFINED)
# undef PRIVATE
#endif

// and also our flag
#undef PRIVATE_WAS_DEFINED

// Cardinal specific API
#include <functional>
#define USING_CARDINAL_NOT_RACK

// OS separator macros
#ifdef ARCH_WIN
# define CARDINAL_OS_SEP       '\\'
# define CARDINAL_OS_SEP_STR   "\\"
# define CARDINAL_OS_SPLIT     ';'
# define CARDINAL_OS_SPLIT_STR ";"
#else
# define CARDINAL_OS_SEP       '/'
# define CARDINAL_OS_SEP_STR   "/"
# define CARDINAL_OS_SPLIT     ':'
# define CARDINAL_OS_SPLIT_STR ":"
#endif

// opens a file browser, startDir and title can be null
// action is always triggered on close (path can be null), must be freed if not null
void async_dialog_filebrowser(bool saving, const char* defaultName, const char* startDir, const char* title,
                              std::function<void(char* path)> action);

// opens a message dialog with only an "ok" button
void async_dialog_message(const char* message);

// opens a message dialog with "ok" and "cancel" buttons
// action is triggered if user presses "ok"
void async_dialog_message(const char* message, std::function<void()> action);

// opens a text input dialog, message and text can be null
// action is always triggered on close (newText can be null), must be freed if not null
void async_dialog_text_input(const char* message, const char* text, std::function<void(char* newText)> action);
