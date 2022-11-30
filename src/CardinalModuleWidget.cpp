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
 * This file is partially based on VCVRack's ModuleWidget.cpp
 * Copyright (C) 2016-2021 VCV.
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 */

#include "CardinalCommon.hpp"

#include <regex>

#include <app/ModuleWidget.hpp>
#include <app/RackWidget.hpp>
#include <app/Scene.hpp>
#include <engine/Engine.hpp>
#include <ui/MenuSeparator.hpp>
#include <asset.hpp>
#include <context.hpp>
#include <helpers.hpp>
#include <settings.hpp>
#include <system.hpp>

#undef ModuleWidget

namespace rack {
namespace app {

// Create ModulePresetPathItems for each patch in a directory.
static void appendPresetItems(ui::Menu* menu, WeakPtr<ModuleWidget> moduleWidget, std::string presetDir) {
    bool foundPresets = false;

    if (system::isDirectory(presetDir))
    {
        // Note: This is not cached, so opening this menu each time might have a bit of latency.
        std::vector<std::string> entries = system::getEntries(presetDir);
        std::sort(entries.begin(), entries.end());
        for (std::string path : entries) {
            std::string name = system::getStem(path);
            // Remove "1_", "42_", "001_", etc at the beginning of preset filenames
            std::regex r("^\\d+_");
            name = std::regex_replace(name, r, "");

            if (system::getExtension(path) == ".vcvm" && name != "template")
            {
                if (!foundPresets)
                    menu->addChild(new ui::MenuSeparator);

                foundPresets = true;

                menu->addChild(createMenuItem(name, "", [=]() {
                    if (!moduleWidget)
                        return;
                    try {
                        moduleWidget->loadAction(path);
                    }
                    catch (Exception& e) {
                        async_dialog_message(e.what());
                    }
                }));
            }
        }
    }
};

static void CardinalModuleWidget__saveSelectionDialog(RackWidget* const w)
{
    std::string selectionDir = asset::user("selections");
    system::createDirectories(selectionDir);

    async_dialog_filebrowser(true, "selection.vcvs", selectionDir.c_str(),
                            #ifdef DISTRHO_OS_WASM
                             "Save selection",
                            #else
                             "Save selection as...",
                            #endif
                             [w](char* pathC) {
        if (!pathC) {
            // No path selected
            return;
        }

        std::string path = pathC;
        std::free(pathC);

        // Automatically append .vcvs extension
        if (system::getExtension(path) != ".vcvs")
            path += ".vcvs";

        w->saveSelection(path);
    });
}

}
}

namespace patchUtils
{

using namespace rack;

void appendSelectionContextMenu(ui::Menu* const menu)
{
    app::RackWidget* const w = APP->scene->rack;

    int n = w->getSelected().size();
    menu->addChild(createMenuLabel(string::f("%d selected %s", n, n == 1 ? "module" : "modules")));

    // Enable alwaysConsume of menu items if the number of selected modules changes

    // Select all
    menu->addChild(createMenuItem("Select all", RACK_MOD_CTRL_NAME "+A", [w]() {
        w->selectAll();
    }, false, true));

    // Deselect
    menu->addChild(createMenuItem("Deselect", RACK_MOD_CTRL_NAME "+" RACK_MOD_SHIFT_NAME "+A", [w]() {
        w->deselectAll();
    }, n == 0, true));

    // Copy
    menu->addChild(createMenuItem("Copy", RACK_MOD_CTRL_NAME "+C", [w]() {
        w->copyClipboardSelection();
    }, n == 0));

    // Paste
    menu->addChild(createMenuItem("Paste", RACK_MOD_CTRL_NAME "+V", [w]() {
        w->pasteClipboardAction();
    }, false, true));

    // Save
    menu->addChild(createMenuItem(
       #ifdef DISTRHO_OS_WASM
        "Save selection",
       #else
        "Save selection as...",
       #endif
        "", [w]() {
        CardinalModuleWidget__saveSelectionDialog(w);
    }, n == 0));

    // Initialize
    menu->addChild(createMenuItem("Initialize", RACK_MOD_CTRL_NAME "+I", [w]() {
        w->resetSelectionAction();
    }, n == 0));

    // Randomize
    menu->addChild(createMenuItem("Randomize", RACK_MOD_CTRL_NAME "+R", [w]() {
        w->randomizeSelectionAction();
    }, n == 0));

    // Disconnect cables
    menu->addChild(createMenuItem("Disconnect cables", RACK_MOD_CTRL_NAME "+U", [w]() {
        w->disconnectSelectionAction();
    }, n == 0));

    // Bypass
    std::string bypassText = RACK_MOD_CTRL_NAME "+E";
    bool bypassed = (n > 0) && w->isSelectionBypassed();
    if (bypassed)
        bypassText += " " CHECKMARK_STRING;
    menu->addChild(createMenuItem("Bypass", bypassText, [w, bypassed]() {
        w->bypassSelectionAction(!bypassed);
    }, n == 0, true));

    // Duplicate
    menu->addChild(createMenuItem("Duplicate", RACK_MOD_CTRL_NAME "+D", [w]() {
        w->cloneSelectionAction(false);
    }, n == 0));

    // Duplicate with cables
    menu->addChild(createMenuItem("└ with cables", RACK_MOD_SHIFT_NAME "+" RACK_MOD_CTRL_NAME "+D", [w]() {
        w->cloneSelectionAction(true);
    }, n == 0));

    // Delete
    menu->addChild(createMenuItem("Delete", "Backspace/Delete", [w]() {
        w->deleteSelectionAction();
    }, n == 0, true));
}

}
