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
 * This file is an edited version of VCVRack's common.cpp
 * Copyright (C) 2016-2021 VCV.
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 */

#include <common.hpp>
#include <string.hpp>

#include "DistrhoPluginUtils.hpp"

// fopen_u8
#ifdef ARCH_WIN
#include <windows.h>

FILE* fopen_u8(const char* filename, const char* mode)
{
    return _wfopen(rack::string::UTF8toUTF16(filename).c_str(), rack::string::UTF8toUTF16(mode).c_str());
}
#endif

// Define the global names to indicate this is Cardinal and not VCVRack
namespace rack {

const std::string APP_NAME = "Cardinal";
const std::string APP_EDITION = getPluginFormatName();
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
