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

#include <osdialog.h>

char* osdialog_file(osdialog_file_action action, const char* path, const char* filename, osdialog_filters* filters)
{
    return nullptr;
}

int osdialog_message(osdialog_message_level level, osdialog_message_buttons buttons, const char* message)
{
    return 0;
}

char* osdialog_prompt(osdialog_message_level level, const char* message, const char* text)
{
    return nullptr;
}
