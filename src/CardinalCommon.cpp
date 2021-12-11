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

#include "CardinalCommon.hpp"

#include "AsyncDialog.hpp"
#include "PluginContext.hpp"

#include <context.hpp>
#include <history.hpp>
#include <patch.hpp>
#include <string.hpp>
#include <system.hpp>
#include <app/Scene.hpp>

#ifdef NDEBUG
# undef DEBUG
#endif

// for finding home dir
#ifndef ARCH_WIN
# include <pwd.h>
# include <unistd.h>
#endif

namespace patchUtils
{

static void promptClear(const char* const message, const std::function<void()> action)
{
	if (APP->history->isSaved() || APP->scene->rack->hasModules())
		return action();

	asyncDialog::create(message, action);
}

static std::string homeDir()
{
#ifdef ARCH_WIN
	if (const char* const userprofile = getenv("USERPROFILE"))
	{
		return userprofile;
	}
	else if (const char* const homedrive = getenv("HOMEDRIVE"))
	{
		if (const char* const homepath = getenv("HOMEPATH"))
			return system::join(homedrive, homepath);
	}
#else
	if (const char* const home = getenv("HOME"))
		return home;
	else if (struct passwd* const pwd = getpwuid(getuid()))
		return pwd->pw_dir;
#endif
	return {};
}

using namespace rack;

void loadDialog()
{
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
        ui->openFileBrowser(opts);
    });
}

void loadPathDialog(const std::string& path)
{
    promptClear("The current patch is unsaved. Clear it and open the new patch?", [path]() {
        APP->patch->loadAction(path);
    });
}

void loadTemplateDialog()
{
    promptClear("The current patch is unsaved. Clear it and start a new patch?", []() {
        APP->patch->loadTemplate();
    });
}

void revertDialog()
{
    if (APP->patch->path.empty())
        return;
    promptClear("Revert patch to the last saved state?", []{
        APP->patch->loadAction(APP->patch->path);
    });
}

void saveDialog(const std::string& path)
{
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
}

void saveAsDialog()
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
    ui->openFileBrowser(opts);
}

}
