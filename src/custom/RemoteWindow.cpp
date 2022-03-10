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
}


Font::~Font() {
}


std::shared_ptr<Font> Font::load(const std::string& filename) {
	return APP->window->loadFont(filename);
}


void Image::loadFile(const std::string& filename, NVGcontext* vg) {
}


Image::~Image() {
}


std::shared_ptr<Image> Image::load(const std::string& filename) {
	return APP->window->loadImage(filename);
}


Window::Window() {
	windowRatio = minWindowSize.x / minWindowSize.y;
	widget::Widget::ContextCreateEvent e;
	APP->scene->onContextCreate(e);
}


Window::~Window() {
	if (APP->scene) {
		widget::Widget::ContextDestroyEvent e;
		APP->scene->onContextDestroy(e);
	}
}


math::Vec Window::getSize() {
	return minWindowSize;
}


void Window::setSize(math::Vec) {
}


void Window::run() {
}


void Window::step() {
}


void Window::activateContext() {
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
	return 0;
}


void Window::setFullScreen(bool) {
}


bool Window::isFullScreen() {
	return false;
}


double Window::getMonitorRefreshRate() {
	return 60;
}


double Window::getFrameTime() {
	return 0;
}


double Window::getLastFrameDuration() {
	return 0.0;
}


double Window::getFrameDurationRemaining() {
	return 0.0;
}


std::shared_ptr<Font> Window::loadFont(const std::string& filename) {
	return std::make_shared<Font>();
}


std::shared_ptr<Image> Window::loadImage(const std::string& filename) {
	return std::make_shared<Image>();
}


bool& Window::fbDirtyOnSubpixelChange() {
	static bool _fbDirtyOnSubpixelChange;
	return _fbDirtyOnSubpixelChange;
}


int& Window::fbCount() {
	static int _fbCount;
	return _fbCount;
}


void generateScreenshot() {
}


} // namespace window
} // namespace rack
