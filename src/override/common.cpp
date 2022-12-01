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

#ifdef NDEBUG
# undef DEBUG
#endif

#include "DistrhoPluginUtils.hpp"

#if defined(ARCH_WIN)
#include <windows.h>

FILE* fopen_u8(const char* filename, const char* mode) {
	if (FILE* const f = _wfopen(rack::string::UTF8toUTF16(filename).c_str(), rack::string::UTF8toUTF16(mode).c_str()))
		return f;
	if (std::strncmp(filename, "\\\\?\\", 4) == 0 && std::getenv("CARDINAL_UNDER_WINE") != nullptr)
		return _wfopen(L"Z:\\dev\\null", rack::string::UTF8toUTF16(mode).c_str());
	return nullptr;
}

#elif defined(DISTRHO_OS_WASM)
#include <sys/stat.h>
#undef fopen

FILE* fopen_wasm(const char* filename, const char* mode) {
	chmod(filename, 0777);
	return std::fopen(filename, mode);
}

#endif


namespace rack {

const std::string APP_NAME = "Cardinal";
const std::string APP_EDITION = getPluginFormatName();
const std::string APP_EDITION_NAME = "Audio Plugin";
const std::string APP_VERSION_MAJOR = "2";
const std::string APP_VERSION = "2.1.2";
#if defined ARCH_WIN
	const std::string APP_OS = "win";
#elif defined ARCH_MAC
	const std::string APP_OS = "mac";
#elif defined ARCH_LIN
	const std::string APP_OS = "lin";
#else
	#error ARCH_LIN undefined
#endif
const std::string API_URL = "";


Exception::Exception(const char* format, ...) {
	va_list args;
	va_start(args, format);
	msg = string::fV(format, args);
	va_end(args);
}


} // namespace rack
