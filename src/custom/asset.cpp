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

#include <asset.hpp>
#include <string.hpp>
#include <system.hpp>
#include <plugin/Plugin.hpp>

#ifdef NDEBUG
# undef DEBUG
#endif

#include <algorithm>

#include "DistrhoUtils.hpp"

namespace rack {
namespace asset {

std::string userDir; // ignored
std::string systemDir; // points to plugin resources dir (or installed/local Rack dir)
std::string bundlePath; // points to plugin manifests dir (or empty)

// get rid of "res/" prefix
static inline std::string& trim(std::string& s)
{
    if (std::strncmp(s.c_str(), "res/", 4) == 0)
        s = s.substr(4, s.size()-4);
    return s;
}

// ignored, returns the same as `system`
std::string user(std::string filename) {
    return system(filename);
}

// get system resource, trimming "res/" prefix if we are loaded as a plugin bundle
std::string system(std::string filename) {
    return system::join(systemDir, bundlePath.empty() ? filename : trim(filename));
}

// get plugin resource
std::string plugin(plugin::Plugin* plugin, std::string filename) {
    DISTRHO_SAFE_ASSERT_RETURN(plugin != nullptr, {});
    return system::join(plugin->path, filename);
}

// path to demo patch files
std::string patchesPath() {
    // no bundlePath set, assume local source build
    if (bundlePath.empty())
        return system::join(systemDir, "..", "..", "patches");
    // bundlePath is present, use resources from bundle
    return system::join(systemDir, "patches");
}

// path to plugin manifest
std::string pluginManifest(const std::string& dirname) {
    // no bundlePath set, assume local source build
    if (bundlePath.empty())
        return system::join(systemDir, "..", "..", "plugins", dirname, "plugin.json");
    // bundlePath is present, use resources from bundle
    return system::join(bundlePath, dirname + ".json");
}

// path to plugin files
std::string pluginPath(const std::string& dirname) {
    // no bundlePath set, assume local source build
    if (bundlePath.empty())
        return system::join(systemDir, "..", "..", "plugins", dirname);
    // bundlePath is present, use resources from bundle
    return system::join(systemDir, dirname);
}

}
}
