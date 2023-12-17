/*
 * DISTRHO Cardinal Plugin
 * Copyright (C) 2021-2023 Filipe Coelho <falktx@falktx.com>
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

#include "../BaconPlugs/src/Style.hpp"
#include "../surgext/src/XTStyle.h"

using namespace baconpaul::rackplugs;
using namespace sst::surgext_rack::style;

void surgext_rack_initialize()
{
    BaconStyle::get()->activeStyle = rack::settings::preferDarkPanels ? BaconStyle::DARK : BaconStyle::LIGHT;
    XTStyle::initialize();
}

void surgext_rack_update_theme()
{
    BaconStyle::get()->activeStyle = rack::settings::preferDarkPanels ? BaconStyle::DARK : BaconStyle::LIGHT;
    BaconStyle::get()->notifyStyleListeners();

    XTStyle::setGlobalStyle(rack::settings::preferDarkPanels ? XTStyle::Style::DARK : XTStyle::Style::LIGHT);
    XTStyle::notifyStyleListeners();
}
