/*
 * DISTRHO Cardinal Plugin
 * Copyright (C) 2021-2024 Filipe Coelho <falktx@falktx.com>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "DistrhoUtils.hpp"
#include "CardinalOSC.hpp"

#include <string>

extern const std::string CARDINAL_VERSION;

struct CardinalPluginContext;

// -----------------------------------------------------------------------------------------------------------

namespace rack {

namespace ui {
struct Menu;
}

namespace window {
void generateScreenshot();
}

bool isMini();
bool isStandalone();

#ifdef ARCH_WIN
enum SpecialPath {
    kSpecialPathUserProfile,
    kSpecialPathCommonProgramFiles,
    kSpecialPathProgramFiles,
    kSpecialPathAppData,
    kSpecialPathMyDocuments,
};
std::string getSpecialPath(SpecialPath type);
#endif

#ifdef DISTRHO_OS_WASM
extern char* patchFromURL;
extern char* patchRemoteURL;
extern char* patchStorageSlug;
void syncfs();
#endif

std::string homeDir();

void switchDarkMode(bool darkMode);

} // namespace rack

// -----------------------------------------------------------------------------------------------------------

namespace patchUtils {

void loadDialog();
void loadPathDialog(const std::string& path, bool asTemplate = false);
void loadSelectionDialog();
void loadTemplate(bool factory);
void loadTemplateDialog(bool factory);
void revertDialog();
void saveDialog(const std::string& path);
void saveAsDialog();
void saveAsDialogUncompressed();
void saveTemplateDialog();
void appendSelectionContextMenu(rack::ui::Menu* menu);
void openBrowser(const std::string& url);

} // namespace patchUtils

// -----------------------------------------------------------------------------------------------------------

START_NAMESPACE_DISTRHO

class CardinalBasePlugin;
class CardinalBaseUI;

struct Initializer
{
    std::string templatePath;
    std::string factoryTemplatePath;
    bool shouldSaveSettings = false;
    CARDINAL_OSC_FIELDS

    Initializer(const CardinalBasePlugin* plugin, const CardinalBaseUI* ui);
    ~Initializer();
    void loadSettings(bool isRealInstance);
    CARDINAL_OSC_METHODS
};

#ifndef HEADLESS
void handleHostParameterDrag(const CardinalPluginContext* pcontext, uint index, bool started);
#endif

END_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------------------------------------
