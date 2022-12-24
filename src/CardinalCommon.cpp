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
#include <settings.hpp>
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

#ifdef DISTRHO_OS_WASM
# include <emscripten/emscripten.h>
#endif

const std::string CARDINAL_VERSION = "22.12";

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------------------------------------

void handleHostParameterDrag(const CardinalPluginContext* pcontext, uint index, bool started)
{
    DISTRHO_SAFE_ASSERT_RETURN(pcontext->ui != nullptr,);

    if (started)
    {
        pcontext->ui->editParameter(index, true);
        pcontext->ui->setParameterValue(index, pcontext->parameters[index]);
    }
    else
    {
        pcontext->ui->editParameter(index, false);
    }
}

// --------------------------------------------------------------------------------------------------------------------

bool CardinalPluginContext::addIdleCallback(IdleCallback* const cb) const
{
    if (ui == nullptr)
        return false;

    ui->addIdleCallback(cb);
    return true;
}

void CardinalPluginContext::removeIdleCallback(IdleCallback* const cb) const
{
    if (ui == nullptr)
        return;

    ui->removeIdleCallback(cb);
}

void CardinalPluginContext::writeMidiMessage(const rack::midi::Message& message, const uint8_t channel)
{
    if (bypassed)
        return;

    const size_t size = message.bytes.size();
    DISTRHO_SAFE_ASSERT_RETURN(size > 0,);
    DISTRHO_SAFE_ASSERT_RETURN(message.frame >= 0,);

    MidiEvent event;
    event.frame = message.frame;

    switch (message.bytes[0] & 0xF0)
    {
    case 0x80:
    case 0x90:
    case 0xA0:
    case 0xB0:
    case 0xE0:
        event.size = 3;
        break;
    case 0xC0:
    case 0xD0:
        event.size = 2;
        break;
    case 0xF0:
        switch (message.bytes[0] & 0x0F)
        {
        case 0x0:
        case 0x4:
        case 0x5:
        case 0x7:
        case 0x9:
        case 0xD:
            // unsupported
            return;
        case 0x1:
        case 0x2:
        case 0x3:
        case 0xE:
            event.size = 3;
            break;
        case 0x6:
        case 0x8:
        case 0xA:
        case 0xB:
        case 0xC:
        case 0xF:
            event.size = 1;
            break;
        }
        break;
    default:
        // invalid
        return;
    }

    DISTRHO_SAFE_ASSERT_RETURN(size >= event.size,);

    std::memcpy(event.data, message.bytes.data(), event.size);

    if (channel != 0 && event.data[0] < 0xF0)
        event.data[0] |= channel & 0x0F;

    plugin->writeMidiEvent(event);
}

END_NAMESPACE_DISTRHO

// --------------------------------------------------------------------------------------------------------------------

namespace rack {

bool isStandalone()
{
    return std::strstr(getPluginFormatName(), "Standalone") != nullptr;
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
    case kSpecialPathProgramFiles:
        csidl = CSIDL_PROGRAM_FILES;
        break;
    case kSpecialPathAppData:
        csidl = CSIDL_APPDATA;
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

#ifdef DISTRHO_OS_WASM
char* patchFromURL = nullptr;
char* patchRemoteURL = nullptr;
char* patchStorageSlug = nullptr;
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

// --------------------------------------------------------------------------------------------------------------------

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

        DISTRHO_NAMESPACE::FileBrowserOptions opts;
        opts.saving = ui->saving = false;
        opts.startDir = dir.c_str();
        opts.title = "Open patch";
        ui->openFileBrowser(opts);
    });
#endif
}

void loadPathDialog(const std::string& path, const bool asTemplate)
{
#ifndef HEADLESS
    promptClear("The current patch is unsaved. Clear it and open the new patch?", [path, asTemplate]() {
        APP->patch->loadAction(path);

        if (asTemplate)
        {
            APP->patch->path = "";
            APP->history->setSaved();
        }
    });
#endif
}

void loadSelectionDialog()
{
    app::RackWidget* const w = APP->scene->rack;

    std::string selectionDir = asset::user("selections");
    system::createDirectories(selectionDir);

    async_dialog_filebrowser(false, nullptr, selectionDir.c_str(), "Import selection", [w](char* pathC) {
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

    DISTRHO_NAMESPACE::FileBrowserOptions opts;
    opts.saving = ui->saving = true;
    opts.defaultName = "patch.vcv";
    opts.startDir = dir.c_str();
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

void openBrowser(const std::string& url)
{
#ifdef DISTRHO_OS_WASM
    EM_ASM({
        window.open(UTF8ToString($0), '_blank');
    }, url.c_str());
#else
    system::openBrowser(url);
#endif
}

}

// --------------------------------------------------------------------------------------------------------------------

void async_dialog_filebrowser(const bool saving,
                              const char* const defaultName,
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

    DISTRHO_NAMESPACE::FileBrowserOptions opts;
    opts.saving = saving;
    opts.defaultName = defaultName;
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
