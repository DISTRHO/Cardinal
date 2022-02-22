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

#include "../substation-opensource/src/Settings.hpp"

namespace slime {
namespace plugin {
namespace substation {

PluginSettings::PluginSettings(void) {}
PluginSettings::~PluginSettings(void) {}
void PluginSettings::save() {}
void PluginSettings::load() {}
void PluginSettings::appendContextMenu(rack::ui::Menu* menu) {}
void PluginSettings::updateCableColors(const bool& value) {}

PluginSettings settings;

}  // namespace substation
}  // namespace plugin
}  // namespace slime
