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

#include <common.hpp>
#include <string.hpp>

#ifdef NDEBUG
# undef DEBUG
#endif

#include "OpenGL.hpp"

// fix blendish build, missing symbol in debug mode
#ifdef DEBUG
extern "C" {
float bnd_clamp(float v, float mn, float mx) {
    return (v > mx)?mx:(v < mn)?mn:v;
}
}
#endif

// fopen_u8
#ifdef ARCH_WIN
#include <windows.h>
FILE* fopen_u8(const char* filename, const char* mode)
{
    return _wfopen(rack::string::UTF8toUTF16(filename).c_str(), rack::string::UTF8toUTF16(mode).c_str());
}
#endif

// Compile those nice implementation-in-header little libraries
#define NANOSVG_IMPLEMENTATION
#define NANOSVG_ALL_COLOR_KEYWORDS
#include <nanosvg.h>

// Define the global names to indicate this is Cardinal and not VCVRack
namespace rack {
const std::string APP_NAME = "Cardinal";
const std::string APP_EDITION = "";
const std::string APP_EDITION_NAME = "Audio Plugin";
const std::string APP_VERSION_MAJOR = "2";
const std::string APP_VERSION = "2.0";
#if defined(ARCH_WIN)
const std::string APP_ARCH = "win";
#elif defined(ARCH_MAC)
const std::string APP_ARCH = "mac";
#else
const std::string APP_ARCH = "lin";
#endif
const std::string API_URL = "";
Exception::Exception(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    msg = string::fV(format, args);
    va_end(args);
}
}

// Custom assets location
#include <algorithm>
#include <asset.hpp>
#include <system.hpp>
#include <plugin/Plugin.hpp>
namespace rack {
namespace asset {
std::string systemDir;
std::string userDir;
std::string bundlePath;
static inline std::string& trim(std::string& s) {
    if (std::strncmp(s.c_str(), "res" DISTRHO_OS_SEP_STR, 4) == 0)
        s = s.substr(4, s.size()-4);
    return s;
}
std::string system(std::string filename) {
	return system::join(systemDir, trim(filename));
}
std::string user(std::string filename) {
	return system::join(userDir, trim(filename));
}
std::string plugin(plugin::Plugin* plugin, std::string filename) {
	DISTRHO_SAFE_ASSERT_RETURN(plugin != nullptr, {});
	return system::join(systemDir, plugin->path, trim(filename));
}
}
}

// Define the stuff needed for VCVRack but unused for Cardinal
#include <library.hpp>
#include <network.hpp>
namespace rack {
namespace library {
    std::string appChangelogUrl;
    std::string appDownloadUrl;
    std::string appVersion;
    std::string loginStatus;
    std::map<std::string, UpdateInfo> updateInfos;
    std::string updateStatus;
    std::string updateSlug;
    float updateProgress = 0.f;
    bool isSyncing = false;
    bool restartRequested = false;
    void checkAppUpdate() {}
    void checkUpdates() {}
    bool hasUpdates() { return false; }
    bool isAppUpdateAvailable() { return false; }
    bool isLoggedIn() { return false; }
    void logIn(const std::string&, const std::string&) {}
    void logOut() {}
    void syncUpdate(const std::string&) {}
    void syncUpdates() {}
}
namespace network {
    std::string encodeUrl(const std::string&) { return {}; }
    json_t* requestJson(Method, const std::string&, json_t*, const CookieMap&) { return nullptr; }
    bool requestDownload(const std::string&, const std::string&, float*, const CookieMap&) { return false; }
}
}
