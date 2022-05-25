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
 * This file is partially based on VCVRack's patch.cpp
 * Copyright (C) 2016-2021 VCV.
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 */

#include "CardinalCommon.hpp"

#include "AsyncDialog.hpp"
#include "PluginContext.hpp"
#include "DistrhoPluginUtils.hpp"

#include <asset.hpp>
#include <context.hpp>
#include <history.hpp>
#include <patch.hpp>
#include <string.hpp>
#include <system.hpp>
#include <app/Scene.hpp>
#include <window/Window.hpp>

#ifdef NDEBUG
# undef DEBUG
#endif

// for finding special paths
#ifdef ARCH_WIN
# include <shlobj.h>
#else
# include <pwd.h>
# include <unistd.h>
#endif

const std::string CARDINAL_VERSION = "22.05";

namespace rack {

namespace settings {
int rateLimit = 0;
}

bool isStandalone()
{
    return std::strstr(getPluginFormatName(), "JACK") != nullptr;
}

#ifdef ARCH_WIN
std::string getSpecialPath(const SpecialPath type)
{
    int csidl;
    switch (type)
    {
    case kSpecialPathUserProfile:
        csidl = CSIDL_PROFILE;
        break;
    case kSpecialPathCommonProgramFiles:
        csidl = CSIDL_PROGRAM_FILES_COMMON;
        break;
    case kSpecialPathAppData:
        csidl = CSIDL_COMMON_APPDATA;
        break;
    default:
        return {};
    }

    WCHAR path[MAX_PATH + 256];

    if (SHGetSpecialFolderPathW(nullptr, path, csidl, FALSE))
        return string::UTF16toUTF8(path);

    return {};
}
#endif

std::string homeDir()
{
# ifdef ARCH_WIN
    return getSpecialPath(kSpecialPathUserProfile);
# else
    if (const char* const home = getenv("HOME"))
        return home;
    if (struct passwd* const pwd = getpwuid(getuid()))
        return pwd->pw_dir;
# endif
    return {};
}

} // namespace rack

namespace patchUtils
{

using namespace rack;

#ifndef HEADLESS
static void promptClear(const char* const message, const std::function<void()> action)
{
    if (APP->history->isSaved() || APP->scene->rack->hasModules())
        return action();

    asyncDialog::create(message, action);
}
#endif

void loadDialog()
{
#ifndef HEADLESS
    promptClear("The current patch is unsaved. Clear it and open a new patch?", []() {
        std::string dir;
        if (! APP->patch->path.empty())
            dir = system::getDirectory(APP->patch->path);
        else
            dir = homeDir();

        CardinalPluginContext* const pcontext = static_cast<CardinalPluginContext*>(APP);
        DISTRHO_SAFE_ASSERT_RETURN(pcontext != nullptr,);

        CardinalBaseUI* const ui = static_cast<CardinalBaseUI*>(pcontext->ui);
        DISTRHO_SAFE_ASSERT_RETURN(ui != nullptr,);

        FileBrowserOptions opts;
        opts.startDir = dir.c_str();
        opts.saving = ui->saving = false;
        opts.title = "Open patch";
        ui->openFileBrowser(opts);
    });
#endif
}

void loadPathDialog(const std::string& path)
{
#ifndef HEADLESS
    promptClear("The current patch is unsaved. Clear it and open the new patch?", [path]() {
        APP->patch->loadAction(path);
    });
#endif
}

void loadSelectionDialog()
{
    app::RackWidget* const w = APP->scene->rack;

    std::string selectionDir = asset::user("selections");
    system::createDirectories(selectionDir);

    async_dialog_filebrowser(false, selectionDir.c_str(), "Import selection", [w](char* pathC) {
        if (!pathC) {
            // No path selected
            return;
        }

        try {
            w->loadSelection(pathC);
        }
        catch (Exception& e) {
            async_dialog_message(e.what());
        }

        std::free(pathC);
    });
}

void loadTemplateDialog()
{
#ifndef HEADLESS
    promptClear("The current patch is unsaved. Clear it and start a new patch?", []() {
        APP->patch->loadTemplate();
    });
#endif
}

void revertDialog()
{
#ifndef HEADLESS
    if (APP->patch->path.empty())
        return;
    promptClear("Revert patch to the last saved state?", []{
        APP->patch->loadAction(APP->patch->path);
    });
#endif
}

void saveDialog(const std::string& path)
{
#ifndef HEADLESS
    if (path.empty()) {
        return;
    }

    // Note: If save() fails below, this should probably be reset. But we need it so toJson() doesn't set the "unsaved" property.
    APP->history->setSaved();

    try {
        APP->patch->save(path);
    }
    catch (Exception& e) {
        asyncDialog::create(string::f("Could not save patch: %s", e.what()).c_str());
        return;
    }
#endif
}

#ifndef HEADLESS
static void saveAsDialog(const bool uncompressed)
{
    std::string dir;
    if (! APP->patch->path.empty())
        dir = system::getDirectory(APP->patch->path);
    else
        dir = homeDir();

    CardinalPluginContext* const pcontext = static_cast<CardinalPluginContext*>(APP);
    DISTRHO_SAFE_ASSERT_RETURN(pcontext != nullptr,);

    CardinalBaseUI* const ui = static_cast<CardinalBaseUI*>(pcontext->ui);
    DISTRHO_SAFE_ASSERT_RETURN(ui != nullptr,);

    FileBrowserOptions opts;
    opts.startDir = dir.c_str();
    opts.saving = ui->saving = true;
    opts.title = "Save patch";
    ui->savingUncompressed = uncompressed;
    ui->openFileBrowser(opts);
}
#endif

void saveAsDialog()
{
#ifndef HEADLESS
    saveAsDialog(false);
#endif
}

void saveAsDialogUncompressed()
{
#ifndef HEADLESS
    saveAsDialog(true);
#endif
}

}

void async_dialog_filebrowser(const bool saving,
                              const char* const startDir,
                              const char* const title,
                              const std::function<void(char* path)> action)
{
#ifndef HEADLESS
    CardinalPluginContext* const pcontext = static_cast<CardinalPluginContext*>(APP);
    DISTRHO_SAFE_ASSERT_RETURN(pcontext != nullptr,);

    CardinalBaseUI* const ui = static_cast<CardinalBaseUI*>(pcontext->ui);
    DISTRHO_SAFE_ASSERT_RETURN(ui != nullptr,);

    // only 1 dialog possible at a time
    DISTRHO_SAFE_ASSERT_RETURN(ui->filebrowserhandle == nullptr,);

    FileBrowserOptions opts;
    opts.saving = saving;
    opts.startDir = startDir;
    opts.title = title;

    ui->filebrowseraction = action;
    ui->filebrowserhandle = fileBrowserCreate(true, pcontext->nativeWindowId, pcontext->window->pixelRatio, opts);
#endif
}

void async_dialog_message(const char* const message)
{
#ifndef HEADLESS
    asyncDialog::create(message);
#endif
}

void async_dialog_message(const char* const message, const std::function<void()> action)
{
#ifndef HEADLESS
    asyncDialog::create(message, action);
#endif
}

void async_dialog_text_input(const char* const message, const char* const text,
                             const std::function<void(char* newText)> action)
{
#ifndef HEADLESS
    asyncDialog::textInput(message, text, action);
#endif
}
