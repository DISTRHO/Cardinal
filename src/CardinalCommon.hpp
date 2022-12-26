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

#pragma once

#include "DistrhoUtils.hpp"

#include <string>

#ifdef HAVE_LIBLO
// # define REMOTE_HOST "localhost"
# define REMOTE_HOST "192.168.51.1"
# define REMOTE_HOST_PORT "2228"
# include "extra/Thread.hpp"
#endif

#ifdef DISTRHO_OS_WASM
# ifdef STATIC_BUILD
#  define CARDINAL_WASM_WELCOME_TEMPLATE_FILENAME "welcome-wasm-mini.vcv"
# else
#  define CARDINAL_WASM_WELCOME_TEMPLATE_FILENAME "welcome-wasm.vcv"
# endif
#endif

extern const std::string CARDINAL_VERSION;

// -----------------------------------------------------------------------------------------------------------

namespace rack {

namespace ui {
struct Menu;
}

namespace window {
void generateScreenshot();
}

bool isStandalone();

#ifdef ARCH_WIN
enum SpecialPath {
    kSpecialPathUserProfile,
    kSpecialPathCommonProgramFiles,
    kSpecialPathProgramFiles,
    kSpecialPathAppData,
};
std::string getSpecialPath(SpecialPath type);
#endif

#ifdef DISTRHO_OS_WASM
extern char* patchFromURL;
extern char* patchRemoteURL;
extern char* patchStorageSlug;
#endif

} // namespace rack

// -----------------------------------------------------------------------------------------------------------

namespace patchUtils {

void loadDialog();
void loadPathDialog(const std::string& path, bool asTemplate = false);
void loadSelectionDialog();
void loadTemplateDialog();
void revertDialog();
void saveDialog(const std::string& path);
void saveAsDialog();
void saveAsDialogUncompressed();
void appendSelectionContextMenu(rack::ui::Menu* menu);
void openBrowser(const std::string& url);

bool connectToRemote();
bool isRemoteConnected();
bool isRemoteAutoDeployed();
void setRemoteAutoDeploy(bool autoDeploy);
void deployToRemote();
void sendScreenshotToRemote(const char* screenshot);

} // namespace patchUtils

// -----------------------------------------------------------------------------------------------------------

#if defined(HAVE_LIBLO) && defined(HEADLESS) && DISTRHO_PLUGIN_WANT_DIRECT_ACCESS
# define CARDINAL_INIT_OSC_THREAD
#endif

START_NAMESPACE_DISTRHO

class CardinalBasePlugin;
class CardinalBaseUI;

struct Initializer
#ifdef CARDINAL_INIT_OSC_THREAD
    : public Thread
#endif
{
#ifdef CARDINAL_INIT_OSC_THREAD
    lo_server oscServer = nullptr;
    CardinalBasePlugin* oscPlugin = nullptr;
#endif
    std::string templatePath;
    std::string factoryTemplatePath;

    Initializer(const CardinalBasePlugin* plugin, const CardinalBaseUI* ui);
    ~Initializer();
#ifdef CARDINAL_INIT_OSC_THREAD
    void run() override;
#endif
};

END_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------------------------------------
