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

#include <network.hpp>

// Define the stuff needed for VCVRack but unused for Cardinal
namespace rack {
namespace network {

std::string encodeUrl(const std::string&) { return {}; }
json_t* requestJson(Method, const std::string&, json_t*, const CookieMap&) { return nullptr; }
bool requestDownload(const std::string&, const std::string&, float*, const CookieMap&) { return false; }

}
}
