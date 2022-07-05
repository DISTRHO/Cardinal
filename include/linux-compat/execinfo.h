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

#ifdef __HAIKU__
// these are missing from HaikuOS
#define pthread_setname_np(...)

static int pthread_getcpuclockid(pthread_t, clockid_t* const clock_id)
{
    *clock_id = CLOCK_REALTIME;
    return 0;
}
#endif

static int backtrace(void**, int)
{
    return 0;
}

char** backtrace_symbols(void* const*, int)
{
    return nullptr;
}
