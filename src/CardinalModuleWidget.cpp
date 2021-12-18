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

#include <app/ModuleWidget.hpp>
#include <app/RackWidget.hpp>
#include <app/Scene.hpp>
#include <engine/Engine.hpp>
#include <ui/MenuSeparator.hpp>
#include <asset.hpp>
#include <context.hpp>
#include <helpers.hpp>
#include <system.hpp>

namespace rack {
namespace app {

struct CardinalModuleWidget : ModuleWidget {
    CardinalModuleWidget() : ModuleWidget() {}
    DEPRECATED CardinalModuleWidget(engine::Module* module) : ModuleWidget() {
        setModule(module);
    }
    void onButton(const ButtonEvent& e) override;
};

struct ModuleWidget::Internal {
    math::Vec dragOffset;
    math::Vec dragRackPos;
    bool dragEnabled;
    math::Vec oldPos;
    widget::Widget* panel;
};

static void CardinalModuleWidget__loadDialog(ModuleWidget* const w)
{
    std::string presetDir = w->model->getUserPresetDirectory();
    system::createDirectories(presetDir);

    WeakPtr<ModuleWidget> weakThis = w;

    async_dialog_filebrowser(false, presetDir.c_str(), "Load preset", [=](char* pathC) {
        // Delete directories if empty
        DEFER({
            try {
                system::remove(presetDir);
                system::remove(system::getDirectory(presetDir));
            }
            catch (Exception& e) {
                // Ignore exceptions if directory cannot be removed.
            }
        });

        if (!weakThis)
            return;
        if (!pathC)
            return;

        try {
            weakThis->loadAction(pathC);
        }
        catch (Exception& e) {
            async_dialog_message(e.what());
        }

        std::free(pathC);
    });
}

void CardinalModuleWidget__saveDialog(ModuleWidget* const w)
{
    const std::string presetDir = w->model->getUserPresetDirectory();
    system::createDirectories(presetDir);

    WeakPtr<ModuleWidget> weakThis = w;

    async_dialog_filebrowser(true, presetDir.c_str(), "Save preset", [=](char* pathC) {
        // Delete directories if empty
        DEFER({
            try {
                system::remove(presetDir);
                system::remove(system::getDirectory(presetDir));
            }
            catch (Exception& e) {
                // Ignore exceptions if directory cannot be removed.
            }
        });

        if (!weakThis)
            return;
        if (!pathC)
            return;

        std::string path = pathC;
        std::free(pathC);

        // Automatically append .vcvm extension
        if (system::getExtension(path) != ".vcvm")
            path += ".vcvm";

        weakThis->save(path);
    });
}

static void CardinalModuleWidget__createContextMenu(ModuleWidget* const w,
                                                    plugin::Model* const model,
                                                    engine::Module* const module) {
    DISTRHO_SAFE_ASSERT_RETURN(model != nullptr,);

    ui::Menu* menu = createMenu();

    WeakPtr<ModuleWidget> weakThis = w;

    // Brand and module name
    menu->addChild(createMenuLabel(model->name));
    menu->addChild(createMenuLabel(model->plugin->brand));

    // Info
    menu->addChild(createSubmenuItem("Info", "", [model](ui::Menu* menu) {
        model->appendContextMenu(menu);
    }));

    // Preset
    menu->addChild(createSubmenuItem("Preset", "", [weakThis](ui::Menu* menu) {
        menu->addChild(createMenuItem("Copy", RACK_MOD_CTRL_NAME "+C", [weakThis]() {
            if (!weakThis)
                return;
            weakThis->copyClipboard();
        }));

        menu->addChild(createMenuItem("Paste", RACK_MOD_CTRL_NAME "+V", [weakThis]() {
            if (!weakThis)
                return;
            weakThis->pasteClipboardAction();
        }));

        menu->addChild(createMenuItem("Open", "", [weakThis]() {
            if (!weakThis)
                return;
            CardinalModuleWidget__loadDialog(weakThis);
        }));

        menu->addChild(createMenuItem("Save as", "", [weakThis]() {
            if (!weakThis)
                return;
            CardinalModuleWidget__saveDialog(weakThis);
        }));

        /* TODO
        // Scan `<user dir>/presets/<plugin slug>/<module slug>` for presets.
        menu->addChild(new ui::MenuSeparator);
        menu->addChild(createMenuLabel("User presets"));
        appendPresetItems(menu, weakThis, weakThis->model->getUserPresetDirectory());

        // Scan `<plugin dir>/presets/<module slug>` for presets.
        menu->addChild(new ui::MenuSeparator);
        menu->addChild(createMenuLabel("Factory presets"));
        appendPresetItems(menu, weakThis, weakThis->model->getFactoryPresetDirectory());
        */
    }));

    // Initialize
    menu->addChild(createMenuItem("Initialize", RACK_MOD_CTRL_NAME "+I", [weakThis]() {
        if (!weakThis)
            return;
        weakThis->resetAction();
    }));

    // Randomize
    menu->addChild(createMenuItem("Randomize", RACK_MOD_CTRL_NAME "+R", [weakThis]() {
        if (!weakThis)
            return;
        weakThis->randomizeAction();
    }));

    // Disconnect cables
    menu->addChild(createMenuItem("Disconnect cables", RACK_MOD_CTRL_NAME "+U", [weakThis]() {
        if (!weakThis)
            return;
        weakThis->disconnectAction();
    }));

    // Bypass
    std::string bypassText = RACK_MOD_CTRL_NAME "+E";
    bool bypassed = module && module->isBypassed();
    if (bypassed)
        bypassText += " " CHECKMARK_STRING;
    menu->addChild(createMenuItem("Bypass", bypassText, [weakThis, bypassed]() {
        if (!weakThis)
            return;
        weakThis->bypassAction(!bypassed);
    }));

    // Duplicate
    menu->addChild(createMenuItem("Duplicate", RACK_MOD_CTRL_NAME "+D", [weakThis]() {
        if (!weakThis)
            return;
        weakThis->cloneAction(false);
    }));

    // Duplicate with cables
    menu->addChild(createMenuItem("└ with cables", RACK_MOD_SHIFT_NAME "+" RACK_MOD_CTRL_NAME "+D", [weakThis]() {
        if (!weakThis)
            return;
        weakThis->cloneAction(true);
    }));

    // Delete
    menu->addChild(createMenuItem("Delete", "Backspace/Delete", [weakThis]() {
        if (!weakThis)
            return;
        weakThis->removeAction();
    }, false, true));

    w->appendContextMenu(menu);
}

static void CardinalModuleWidget__saveSelectionDialog(RackWidget* const w)
{
    std::string selectionDir = asset::user("selections");
    system::createDirectories(selectionDir);

    async_dialog_filebrowser(true, selectionDir.c_str(), "Save selection as", [w](char* pathC) {
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

void CardinalModuleWidget::onButton(const ButtonEvent& e)
{
    bool selected = APP->scene->rack->isSelected(this);

    if (selected) {
        if (e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_RIGHT) {
            ui::Menu* menu = createMenu();
            patchUtils::appendSelectionContextMenu(menu);
        }

        e.consume(this);
    }

    OpaqueWidget::onButton(e);

    if (e.getTarget() == this) {
        // Set starting drag position
        if (e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_LEFT) {
            internal->dragOffset = e.pos;
        }
        // Toggle selection on Shift-click
        if (e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_LEFT && (e.mods & RACK_MOD_MASK) == GLFW_MOD_SHIFT) {
            APP->scene->rack->select(this, !selected);
        }
    }

    if (!e.isConsumed() && !selected) {
        // Open context menu on right-click
        if (e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_RIGHT) {
            CardinalModuleWidget__createContextMenu(this, model, module);
            e.consume(this);
        }
    }
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
    menu->addChild(createMenuItem("Save selection as", "", [w]() {
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
