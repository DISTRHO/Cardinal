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

/**
 * This file is an edited version of osdialog.h
 * Originally licensed under CC0 1.0 Universal.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

/** Linked list of patterns. */
typedef struct osdialog_filter_patterns {
	char* pattern;
	struct osdialog_filter_patterns* next;
} osdialog_filter_patterns;

/** Linked list of file filters. */
typedef struct osdialog_filters {
	char* name;
	osdialog_filter_patterns* patterns;
	struct osdialog_filters* next;
} osdialog_filters;

/** Parses a filter string. */
static inline osdialog_filters* osdialog_filters_parse(const char* str) { return NULL; }
static inline void osdialog_filters_free(osdialog_filters* filters) {}

#ifdef __cplusplus
}
#endif
