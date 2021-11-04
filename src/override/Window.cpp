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
#include <plugin.hpp> // used in Window::screenshot
#include <system.hpp> // used in Window::screenshot

#ifdef NDEBUG
# undef DEBUG
#endif

#include "DistrhoUI.hpp"
#include "Application.hpp"
#include "../WindowParameters.hpp"

#ifndef DGL_NO_SHARED_RESOURCES
# include "NanoVG.hpp"
# include "src/Resources.hpp"
#endif

namespace rack {
namespace window {


static const math::Vec minWindowSize = math::Vec(640, 480);


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


struct FontWithOriginalContext : Font {
	int ohandle = -1;
	std::string ofilename;
};

struct ImageWithOriginalContext : Image {
	int ohandle = -1;
	std::string ofilename;
};


struct Window::Internal {
	DISTRHO_NAMESPACE::UI* ui = nullptr;
	DISTRHO_NAMESPACE::WindowParameters params;
	DISTRHO_NAMESPACE::WindowParametersCallback* callback = nullptr;
	DGL_NAMESPACE::Application hiddenApp;
	DGL_NAMESPACE::Window hiddenWindow;
	NVGcontext* r_vg = nullptr;
	NVGcontext* r_fbVg = nullptr;
	NVGcontext* o_vg = nullptr;
	NVGcontext* o_fbVg = nullptr;

	math::Vec size = minWindowSize;
	std::string lastWindowTitle;

	int mods = 0;
	int frame = 0;
	int frameSwapInterval = 1;
	double monitorRefreshRate = 60.0; // FIXME
	double frameTime = 0.0;
	double lastFrameDuration = 0.0;

	std::map<std::string, std::shared_ptr<FontWithOriginalContext>> fontCache;
	std::map<std::string, std::shared_ptr<ImageWithOriginalContext>> imageCache;

	bool fbDirtyOnSubpixelChange = true;

	Internal()
		: hiddenApp(false),
		  hiddenWindow(hiddenApp) { hiddenApp.idle(); }
};


#ifndef DGL_NO_SHARED_RESOURCES
static int loadFallbackFont(NVGcontext* const vg)
{
	const int font = nvgFindFont(vg, NANOVG_DEJAVU_SANS_TTF);
	if (font >= 0)
		return font;

	using namespace dpf_resources;

	return nvgCreateFontMem(vg, NANOVG_DEJAVU_SANS_TTF,
	                        (uchar*)dejavusans_ttf, dejavusans_ttf_size, 0);
}
#endif


Window::Window() {
	internal = new Internal;

	DGL_NAMESPACE::Window::ScopedGraphicsContext sgc(internal->hiddenWindow);

	// Set up NanoVG
	const int nvgFlags = NVG_ANTIALIAS;
#ifdef NANOVG_GLES2
	vg = nvgCreateGLES2(nvgFlags);
	fbVg = nvgCreateSharedGLES2(vg, nvgFlags);
#else
	vg = nvgCreateGL2(nvgFlags);
	fbVg = nvgCreateSharedGL2(vg, nvgFlags);
#endif

	// Load default Blendish font
#ifndef DGL_NO_SHARED_RESOURCES
	uiFont = std::make_shared<Font>();
	uiFont->vg = vg;
	uiFont->handle = loadFallbackFont(vg);

	std::shared_ptr<FontWithOriginalContext> uiFont2;
	uiFont2 = std::make_shared<FontWithOriginalContext>();
	uiFont2->vg = vg;
	uiFont2->handle = loadFallbackFont(vg);
	uiFont2->ofilename = asset::system("res/fonts/DejaVuSans.ttf");
	internal->fontCache[uiFont2->ofilename] = uiFont2;
#else
	uiFont = loadFont(asset::system("res/fonts/DejaVuSans.ttf"));
#endif

	if (uiFont != nullptr)
		bndSetFont(uiFont->handle);
}

void WindowSetPluginUI(Window* const window, DISTRHO_NAMESPACE::UI* const ui)
{
	if (ui != nullptr)
	{
		const GLubyte* vendor = glGetString(GL_VENDOR);
		const GLubyte* renderer = glGetString(GL_RENDERER);
		const GLubyte* version = glGetString(GL_VERSION);
		INFO("Renderer: %s %s", vendor, renderer);
		INFO("OpenGL: %s", version);

		window->internal->ui = ui;
		window->internal->size = rack::math::Vec(ui->getWidth(), ui->getHeight());

		// Set up NanoVG
		const int nvgFlags = NVG_ANTIALIAS;
#ifdef NANOVG_GLES2
		window->internal->r_vg = nvgCreateSharedGLES2(nvgFlags);
		window->internal->r_fbVg = nvgCreateSharedGLES2(window->internal->r_vg, nvgFlags);
#else
		window->internal->r_vg = nvgCreateGL2(nvgFlags);
		window->internal->r_fbVg = nvgCreateSharedGL2(window->internal->r_vg, nvgFlags);
#endif

		// swap contexts
		window->internal->o_vg = window->vg;
		window->internal->o_fbVg = window->fbVg;
		window->vg = window->internal->r_vg;
		window->fbVg = window->internal->r_fbVg;

		// also for fonts and images
		window->uiFont->vg = window->vg;
		window->uiFont->handle = loadFallbackFont(window->vg);
		for (auto& font : window->internal->fontCache)
		{
			font.second->vg = window->vg;
			font.second->ohandle = font.second->handle;
			font.second->handle = nvgCreateFont(window->vg,
			                                    font.second->ofilename.c_str(), font.second->ofilename.c_str());
		}
		for (auto& image : window->internal->imageCache)
		{
			image.second->vg = window->vg;
			image.second->ohandle = image.second->handle;
			image.second->handle = nvgCreateImage(window->vg, image.second->ofilename.c_str(),
			                                      NVG_IMAGE_REPEATX | NVG_IMAGE_REPEATY);
		}

		// Init settings
		WindowParametersRestore(window);

		widget::Widget::ContextCreateEvent e;
		APP->scene->onContextCreate(e);
	}
	else
	{
		widget::Widget::ContextDestroyEvent e;
		APP->scene->onContextDestroy(e);

		// swap contexts
		window->uiFont->vg = window->internal->o_vg;
		window->vg = window->internal->o_vg;
		window->fbVg = window->internal->o_fbVg;
		window->internal->o_vg = nullptr;
		window->internal->o_fbVg = nullptr;

		// also for fonts and images
		window->uiFont->vg = window->vg;
		window->uiFont->handle = loadFallbackFont(window->vg);
		for (auto& font : window->internal->fontCache)
		{
			font.second->vg = window->vg;
			font.second->handle = font.second->ohandle;
			font.second->ohandle = -1;
		}
		for (auto& image : window->internal->imageCache)
		{
			image.second->vg = window->vg;
			image.second->handle = image.second->ohandle;
			image.second->ohandle = -1;
		}

#if defined NANOVG_GLES2
		nvgDeleteGLES2(window->internal->r_vg);
		nvgDeleteGLES2(window->internal->r_fbVg);
#else
		nvgDeleteGL2(window->internal->r_vg);
		nvgDeleteGL2(window->internal->r_fbVg);
#endif

		window->internal->ui = nullptr;
		window->internal->callback = nullptr;
	}
}

void WindowSetMods(Window* const window, const int mods)
{
	window->internal->mods = mods;
}

Window::~Window() {
	{
		DGL_NAMESPACE::Window::ScopedGraphicsContext sgc(internal->hiddenWindow);
		internal->hiddenWindow.close();
		internal->hiddenApp.idle();

		// Fonts and Images in the cache must be deleted before the NanoVG context is deleted
		internal->fontCache.clear();
		internal->imageCache.clear();

#if defined NANOVG_GLES2
		nvgDeleteGLES2(internal->o_fbVg != nullptr ? internal->o_fbVg : fbVg);
		nvgDeleteGLES2(internal->o_vg != nullptr ? internal->o_vg : vg);
#else
		nvgDeleteGL2(internal->o_fbVg != nullptr ? internal->o_fbVg : fbVg);
		nvgDeleteGL2(internal->o_vg != nullptr ? internal->o_vg : vg);
#endif
	}

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
	DISTRHO_SAFE_ASSERT_RETURN(internal->ui != nullptr,);

	double frameTime = system::getTime();
	double lastFrameTime = internal->frameTime;
	internal->frameTime = frameTime;
	internal->lastFrameDuration = frameTime - lastFrameTime;
	// DEBUG("%.2lf Hz", 1.0 / internal->lastFrameDuration);

	// Make event handlers and step() have a clean NanoVG context
	nvgReset(vg);

	if (uiFont != nullptr)
		bndSetFont(uiFont->handle);

	// Set window title
	std::string windowTitle = APP_NAME + " " + APP_EDITION_NAME + " " + APP_VERSION;
	if (APP->patch->path != "") {
		windowTitle += " - ";
		if (!APP->history->isSaved())
			windowTitle += "*";
		windowTitle += system::getFilename(APP->patch->path);
	}
	if (windowTitle != internal->lastWindowTitle) {
		internal->ui->getWindow().setTitle(windowTitle.c_str());
		internal->lastWindowTitle = windowTitle;
	}

	// Get desired pixel ratio
	float newPixelRatio = internal->ui->getScaleFactor();
	if (newPixelRatio != pixelRatio) {
		pixelRatio = newPixelRatio;
		APP->event->handleDirty();
	}

	// Get framebuffer/window ratio
	int winWidth = internal->ui->getWidth();
	int winHeight = internal->ui->getHeight();
	int fbWidth = winWidth;// * newPixelRatio;
	int fbHeight = winHeight;// * newPixelRatio;
	windowRatio = (float)fbWidth / winWidth;

	if (APP->scene) {
		// DEBUG("%f %f %d %d", pixelRatio, windowRatio, fbWidth, winWidth);
		// Resize scene
		APP->scene->box.size = math::Vec(fbWidth, fbHeight).div(pixelRatio);

		// Step scene
		APP->scene->step();

		// Render scene
		// Update and render
		nvgBeginFrame(vg, fbWidth, fbHeight, pixelRatio);
		nvgScale(vg, pixelRatio, pixelRatio);

		// Draw scene
		widget::Widget::DrawArgs args;
		args.vg = vg;
		args.clipBox = APP->scene->box.zeroPos();
		APP->scene->draw(args);

		glViewport(0, 0, fbWidth, fbHeight);
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		nvgEndFrame(vg);
	}

	internal->frame++;
}


void Window::screenshot(const std::string&) {
}


void Window::screenshotModules(const std::string&, float) {
}


void Window::close() {
	DISTRHO_SAFE_ASSERT_RETURN(internal->ui != nullptr,);

	internal->ui->getWindow().close();
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
	std::shared_ptr<FontWithOriginalContext> font;
	try {
		font = std::make_shared<FontWithOriginalContext>();
		font->ofilename = filename;
		font->loadFile(filename, vg);
	}
	catch (Exception& e) {
		WARN("%s", e.what());
		font = nullptr;
	}
	internal->fontCache[filename] = font;
	return font;
}


std::shared_ptr<Image> Window::loadImage(const std::string& filename) {
	const auto& pair = internal->imageCache.find(filename);
	if (pair != internal->imageCache.end())
		return pair->second;

	// Load image
	std::shared_ptr<ImageWithOriginalContext> image;
	try {
		image = std::make_shared<ImageWithOriginalContext>();
		image->ofilename = filename;
		image->loadFile(filename, vg);
	}
	catch (Exception& e) {
		WARN("%s", e.what());
		image = nullptr;
	}
	internal->imageCache[filename] = image;
	return image;
}


bool& Window::fbDirtyOnSubpixelChange() {
	return internal->fbDirtyOnSubpixelChange;
}


} // namespace window
} // namespace rack


START_NAMESPACE_DISTRHO

void WindowParametersSave(rack::window::Window* const window)
{
	if (d_isNotEqual(window->internal->params.cableOpacity, rack::settings::cableOpacity))
	{
		window->internal->params.cableOpacity = rack::settings::cableOpacity;
		if (window->internal->callback != nullptr)
			window->internal->callback->WindowParametersChanged(kWindowParameterCableOpacity,
			                                                    rack::settings::cableOpacity);
	}
	if (d_isNotEqual(window->internal->params.cableTension, rack::settings::cableTension))
	{
		window->internal->params.cableTension = rack::settings::cableTension;
		if (window->internal->callback != nullptr)
			window->internal->callback->WindowParametersChanged(kWindowParameterCableTension,
			                                                    rack::settings::cableTension);
	}
	if (d_isNotEqual(window->internal->params.rackBrightness, rack::settings::rackBrightness))
	{
		window->internal->params.rackBrightness = rack::settings::rackBrightness;
		if (window->internal->callback != nullptr)
			window->internal->callback->WindowParametersChanged(kWindowParameterRackBrightness,
			                                                    rack::settings::rackBrightness);
	}
	if (d_isNotEqual(window->internal->params.haloBrightness, rack::settings::haloBrightness))
	{
		window->internal->params.haloBrightness = rack::settings::haloBrightness;
		if (window->internal->callback != nullptr)
			window->internal->callback->WindowParametersChanged(kWindowParameterHaloBrightness,
			                                                    rack::settings::haloBrightness);
	}
	if (d_isNotEqual(window->internal->params.knobScrollSensitivity, rack::settings::knobScrollSensitivity))
	{
		window->internal->params.knobScrollSensitivity = rack::settings::knobScrollSensitivity;
		if (window->internal->callback != nullptr)
			window->internal->callback->WindowParametersChanged(kWindowParameterWheelSensitivity,
			                                                    rack::settings::knobScrollSensitivity);
	}
	if (window->internal->params.knobMode != rack::settings::knobMode)
	{
		window->internal->params.knobMode = rack::settings::knobMode;
		if (window->internal->callback != nullptr)
			window->internal->callback->WindowParametersChanged(kWindowParameterKnobMode,
			                                                    rack::settings::knobMode);
	}
	if (window->internal->params.tooltips != rack::settings::tooltips)
	{
		window->internal->params.tooltips = rack::settings::tooltips;
		if (window->internal->callback != nullptr)
			window->internal->callback->WindowParametersChanged(kWindowParameterShowTooltips,
			                                                    rack::settings::tooltips);
	}
	if (window->internal->params.knobScroll != rack::settings::knobScroll)
	{
		window->internal->params.knobScroll = rack::settings::knobScroll;
		if (window->internal->callback != nullptr)
			window->internal->callback->WindowParametersChanged(kWindowParameterWheelKnobControl,
			                                                    rack::settings::knobScroll);
	}
	if (window->internal->params.lockModules != rack::settings::lockModules)
	{
		window->internal->params.lockModules = rack::settings::lockModules;
		if (window->internal->callback != nullptr)
			window->internal->callback->WindowParametersChanged(kWindowParameterLockModulePositions,
			                                                    rack::settings::lockModules);
	}
}

void WindowParametersRestore(rack::window::Window* const window)
{
	rack::settings::cableOpacity = window->internal->params.cableOpacity;
	rack::settings::cableTension = window->internal->params.cableTension;
	rack::settings::rackBrightness = window->internal->params.rackBrightness;
	rack::settings::haloBrightness = window->internal->params.haloBrightness;
	rack::settings::knobScrollSensitivity = window->internal->params.knobScrollSensitivity;
	rack::settings::knobMode = static_cast<rack::settings::KnobMode>(window->internal->params.knobMode);
	rack::settings::tooltips = window->internal->params.tooltips;
	rack::settings::knobScroll = window->internal->params.knobScroll;
	rack::settings::lockModules = window->internal->params.lockModules;
}

void WindowParametersSetCallback(rack::window::Window* const window, WindowParametersCallback* const callback)
{
	window->internal->callback = callback;
}

void WindowParametersSetValues(rack::window::Window* const window, const WindowParameters& params)
{
	window->internal->params = params;
}

END_NAMESPACE_DISTRHO

