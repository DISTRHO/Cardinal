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
 * This file is an edited version of VCVRack's Window.cpp
 * Copyright (C) 2016-2021 VCV.
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 */

#include <map>
#include <queue>
#include <thread>

#include <window/Window.hpp>
#include <asset.hpp>
#include <widget/Widget.hpp>
#include <app/Scene.hpp>
#include <context.hpp>
#include <patch.hpp>
#include <settings.hpp>
#include <system.hpp>

#ifdef NDEBUG
# undef DEBUG
#endif


namespace rack {
namespace window {


static const math::Vec minWindowSize = math::Vec(1228, 666);


void Font::loadFile(const std::string& filename, NVGcontext* vg) {
	this->vg = vg;
	handle = nvgCreateFont(vg, filename.c_str(), filename.c_str());
	if (handle < 0)
		throw Exception("Failed to load font %s", filename.c_str());
	INFO("Loaded font %s", filename.c_str());
}


Font::~Font() {
	// There is no NanoVG deleteFont() function yet, so do nothing
}


std::shared_ptr<Font> Font::load(const std::string& filename) {
	return APP->window->loadFont(filename);
}


void Image::loadFile(const std::string& filename, NVGcontext* vg) {
	this->vg = vg;
	handle = nvgCreateImage(vg, filename.c_str(), NVG_IMAGE_REPEATX | NVG_IMAGE_REPEATY);
	if (handle <= 0)
		throw Exception("Failed to load image %s", filename.c_str());
	INFO("Loaded image %s", filename.c_str());
}


Image::~Image() {
	// TODO What if handle is invalid?
	if (handle >= 0)
		nvgDeleteImage(vg, handle);
}


std::shared_ptr<Image> Image::load(const std::string& filename) {
	return APP->window->loadImage(filename);
}


struct WindowParams {
	float rackBrightness = 1.0f;
};

struct Window::Internal
{
	Context* context = nullptr;
	Window* self = nullptr;

	math::Vec size = minWindowSize;
	std::string lastWindowTitle;

	int mods = 0;
	int frame = 0;
	int frameSwapInterval = 1;
	double monitorRefreshRate = 60.0; // FIXME
	double frameTime = 0.0;
	double lastFrameDuration = 0.0;

	std::map<std::string, std::shared_ptr<Font>> fontCache;
	std::map<std::string, std::shared_ptr<Image>> imageCache;

	bool fbDirtyOnSubpixelChange = true;
	int fbCount = 0;
};

Window::Window() {
	internal = new Internal;
	internal->context = APP;
	internal->self = this;
}

Window::~Window() {
	// internal->stopThread(5000);

	if (APP->scene) {
		widget::Widget::ContextDestroyEvent e;
		APP->scene->onContextDestroy(e);
	}

	// Fonts and Images in the cache must be deleted before the NanoVG context is deleted
	internal->fontCache.clear();
	internal->imageCache.clear();

	delete internal;
}


math::Vec Window::getSize() {
	return internal->size;
}


void Window::setSize(math::Vec size) {
	internal->size = size.max(minWindowSize);
}


void Window::run() {
	internal->frame = 0;
}


void Window::step() {
	double frameTime = system::getTime();
	double lastFrameTime = internal->frameTime;
	internal->frameTime = frameTime;
	internal->lastFrameDuration = frameTime - lastFrameTime;
	internal->fbCount = 0;
	// DEBUG("%.2lf Hz", 1.0 / internal->lastFrameDuration);

	// Make event handlers and step() have a clean NanoVG context
	nvgReset(vg);

	if (uiFont != nullptr)
		bndSetFont(uiFont->handle);

	// Get framebuffer/window ratio
	windowRatio = internal->size.x / internal->size.y;

	if (APP->scene) {
		// Resize scene
		APP->scene->box.size = internal->size;

		// Step scene
		APP->scene->step();

		// Update and render
		nvgBeginFrame(vg, internal->size.x, internal->size.y, pixelRatio);
		nvgScale(vg, pixelRatio, pixelRatio);

		// Draw scene
		widget::Widget::DrawArgs args;
		args.vg = vg;
		args.clipBox = APP->scene->box.zeroPos();
		APP->scene->draw(args);

		glViewport(0, 0, internal->size.x, internal->size.y);
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		nvgEndFrame(vg);
	}

	internal->frame++;
}


void Window::activateContext() {
    // glfwMakeContextCurrent(win);
}


void Window::screenshot(const std::string&) {
}


void Window::screenshotModules(const std::string&, float) {
}


void Window::close() {
}


void Window::cursorLock() {
}


void Window::cursorUnlock() {
}


bool Window::isCursorLocked() {
	return false;
}


int Window::getMods() {
	return internal->mods;
}


void Window::setFullScreen(bool) {
}


bool Window::isFullScreen() {
	return false;
}


double Window::getMonitorRefreshRate() {
	return internal->monitorRefreshRate;
}


double Window::getFrameTime() {
	return internal->frameTime;
}


double Window::getLastFrameDuration() {
	return internal->lastFrameDuration;
}


double Window::getFrameDurationRemaining() {
	double frameDurationDesired = internal->frameSwapInterval / internal->monitorRefreshRate;
	return frameDurationDesired - (system::getTime() - internal->frameTime);
}


std::shared_ptr<Font> Window::loadFont(const std::string& filename) {
	const auto& pair = internal->fontCache.find(filename);
	if (pair != internal->fontCache.end())
		return pair->second;

	// Load font
	std::shared_ptr<Font> font;
	try {
		font = std::make_shared<Font>();
		font->loadFile(filename, vg);
	}
	catch (Exception& e) {
		WARN("%s", e.what());
		font = NULL;
	}
	internal->fontCache[filename] = font;
	return font;
}


std::shared_ptr<Image> Window::loadImage(const std::string& filename) {
	const auto& pair = internal->imageCache.find(filename);
	if (pair != internal->imageCache.end())
		return pair->second;

	// Load image
	std::shared_ptr<Image> image;
	try {
		image = std::make_shared<Image>();
		image->loadFile(filename, vg);
	}
	catch (Exception& e) {
		WARN("%s", e.what());
		image = NULL;
	}
	internal->imageCache[filename] = image;
	return image;
}


bool& Window::fbDirtyOnSubpixelChange() {
	return internal->fbDirtyOnSubpixelChange;
}


int& Window::fbCount() {
	return internal->fbCount;
}


} // namespace window
} // namespace rack
