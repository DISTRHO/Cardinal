/*
 * DISTRHO Cardinal Plugin
 * Copyright (C) 2021-2024 Filipe Coelho <falktx@falktx.com>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "../BaconPlugs/src/Style.hpp"
#include "../surgext/src/XTStyle.h"

using namespace baconpaul::rackplugs;
using namespace sst::surgext_rack::style;

void surgext_rack_initialize()
{
    BaconStyle::get()->activeStyle = rack::settings::preferDarkPanels ? BaconStyle::DARK : BaconStyle::LIGHT;
    XTStyle::initialize();
    XTStyle::setGlobalStyle(rack::settings::preferDarkPanels ? XTStyle::Style::DARK : XTStyle::Style::LIGHT);
}

void surgext_rack_update_theme()
{
    BaconStyle::get()->activeStyle = rack::settings::preferDarkPanels ? BaconStyle::DARK : BaconStyle::LIGHT;
    BaconStyle::get()->notifyStyleListeners();

    XTStyle::setGlobalStyle(rack::settings::preferDarkPanels ? XTStyle::Style::DARK : XTStyle::Style::LIGHT);
    XTStyle::notifyStyleListeners();
}
