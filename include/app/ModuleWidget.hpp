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

#ifdef BUILDING_PLUGIN_MODULES
# include <plugin/Model.hpp>
# undef ModuleWidget
#endif

#include_next <app/ModuleWidget.hpp>

#ifdef BUILDING_PLUGIN_MODULES
namespace rack {
namespace app {
struct CardinalModuleWidget : ModuleWidget {
    CardinalModuleWidget() : ModuleWidget() {}
    DEPRECATED CardinalModuleWidget(engine::Module* module) : ModuleWidget() {
        setModule(module);
    }
    void onButton(const ButtonEvent& e) override;
};
}
}
# define ModuleWidget CardinalModuleWidget
#endif
