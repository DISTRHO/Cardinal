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

#include <string>

#pragma once

#ifdef HAVE_LIBLO
// # define REMOTE_HOST "localhost"
# define REMOTE_HOST "192.168.51.1"
# define REMOTE_HOST_PORT "2228"
#endif

namespace rack
{
namespace ui {
struct Menu;
}
}

namespace patchUtils
{

void loadDialog();
void loadPathDialog(const std::string& path);
void loadSelectionDialog();
void loadTemplateDialog();
void revertDialog();
void saveDialog(const std::string& path);
void saveAsDialog();
void appendSelectionContextMenu(rack::ui::Menu* menu);
void deployToMOD();

}
