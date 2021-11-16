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
 * This file is an edited version of VCVRack's context.cpp
 * Copyright (C) 2016-2021 VCV.
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 */

#include <context.hpp>
#include <window/Window.hpp>
#include <patch.hpp>
#include <engine/Engine.hpp>
#include <app/Scene.hpp>
#include <history.hpp>
#include <settings.hpp>

#ifdef NDEBUG
# undef DEBUG
#endif

#include "DistrhoUtils.hpp"

namespace rack {

Context::~Context() {
	// Deleting NULL is safe in C++.

	// Set pointers to NULL so other objects will segfault when attempting to access them

	delete window;
	window = NULL;

	delete patch;
	patch = NULL;

	delete scene;
	scene = NULL;

	delete event;
	event = NULL;

	delete history;
	history = NULL;

	delete engine;
	engine = NULL;
}

static thread_local Context* threadContext = nullptr;

Context* contextGet() {
	DISTRHO_SAFE_ASSERT(threadContext != nullptr);
	return threadContext;
}

// Apple's clang incorrectly compiles this function when -O2 or higher is enabled.
#ifdef ARCH_MAC
__attribute__((optnone))
#endif
void contextSet(Context* const context) {
	// DISTRHO_SAFE_ASSERT(threadContext == nullptr);
	threadContext = context;
}

} // namespace rack
