/*
 * DISTRHO Cardinal Plugin
 * Copyright (C) 2021-2024 Filipe Coelho <falktx@falktx.com>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/**
 * This file is an edited version of VCVRack's window/Window.cpp
 * Copyright (C) 2016-2023 VCV.
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

#include "DistrhoUI.hpp"
#include "Application.hpp"
#include "extra/String.hpp"
#include "../CardinalCommon.hpp"
#include "../CardinalPluginContext.hpp"
#include "../WindowParameters.hpp"

#ifndef DGL_NO_SHARED_RESOURCES
# include "src/Resources.hpp"
#endif

#if !(defined(DGL_USE_GLES) || CARDINAL_VARIANT_MINI)

#define CARDINAL_WINDOW_CAN_GENERATE_SCREENSHOTS

// comment out if wanting to generate a local screenshot.png
#define STBI_WRITE_NO_STDIO

// uncomment to generate screenshots without the rack rail background (ie, transparent)
// #define CARDINAL_TRANSPARENT_SCREENSHOTS

// used in Window::screenshot
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#endif

#ifdef DISTRHO_OS_WASM
# include <emscripten/html5.h>
#endif

namespace rack {
namespace window {


static const math::Vec WINDOW_SIZE_MIN = math::Vec(648, 538);


struct FontWithOriginalContext : Font {
	int ohandle = -1;
	std::string ofilename;
};

struct ImageWithOriginalContext : Image {
	int ohandle = -1;
	std::string ofilename;
};


Font::~Font() {
	// There is no NanoVG deleteFont() function yet, so do nothing
}


void Font::loadFile(const std::string& filename, NVGcontext* vg) {
	this->vg = vg;
	std::string name = system::getStem(filename);
	size_t size;
	// Transfer ownership of font data to font object
	uint8_t* data = system::readFile(filename, &size);
	// Don't use nvgCreateFont because it doesn't properly handle UTF-8 filenames on Windows.
	handle = nvgCreateFontMem(vg, name.c_str(), data, size, 1);
	if (handle < 0) {
		throw Exception("Failed to load font %s", filename.c_str());
	}
	INFO("Loaded font %s", filename.c_str());
}


std::shared_ptr<Font> Font::load(const std::string& filename) {
	return APP->window->loadFont(filename);
}


Image::~Image() {
	// TODO What if handle is invalid?
	if (handle >= 0)
		nvgDeleteImage(vg, handle);
}


void Image::loadFile(const std::string& filename, NVGcontext* vg) {
	this->vg = vg;
	std::vector<uint8_t> data = system::readFile(filename);
	// Don't use nvgCreateImage because it doesn't properly handle UTF-8 filenames on Windows.
	handle = nvgCreateImageMem(vg, NVG_IMAGE_REPEATX | NVG_IMAGE_REPEATY, data.data(), data.size());
	if (handle <= 0)
		throw Exception("Failed to load image %s", filename.c_str());
	INFO("Loaded image %s", filename.c_str());
}


std::shared_ptr<Image> Image::load(const std::string& filename) {
	return APP->window->loadImage(filename);
}


#ifdef CARDINAL_WINDOW_CAN_GENERATE_SCREENSHOTS
enum ScreenshotStep {
	kScreenshotStepNone,
	kScreenshotStepStarted,
	kScreenshotStepFirstPass,
	kScreenshotStepSecondPass,
	kScreenshotStepSaving
};
#endif


struct Window::Internal {
	std::string lastWindowTitle;

	CardinalBaseUI* ui = nullptr;
	DGL_NAMESPACE::NanoTopLevelWidget* tlw = nullptr;
	DISTRHO_NAMESPACE::WindowParameters params;
	DISTRHO_NAMESPACE::WindowParametersCallback* callback = nullptr;
#if DISTRHO_PLUGIN_WANT_DIRECT_ACCESS
	DGL_NAMESPACE::Application hiddenApp;
	DGL_NAMESPACE::Window hiddenWindow;
	NVGcontext* r_vg = nullptr;
	NVGcontext* r_fbVg = nullptr;
	NVGcontext* o_vg = nullptr;
	NVGcontext* o_fbVg = nullptr;
#endif

	math::Vec size = WINDOW_SIZE_MIN;

	int mods = 0;
	int currentRateLimit = 0;

	int frame = 0;
#ifdef CARDINAL_WINDOW_CAN_GENERATE_SCREENSHOTS
	int generateScreenshotStep = kScreenshotStepNone;
#endif
	double monitorRefreshRate = 60.0;
	double frameTime = NAN;
	double lastFrameDuration = NAN;

	std::map<std::string, std::shared_ptr<FontWithOriginalContext>> fontCache;
	std::map<std::string, std::shared_ptr<ImageWithOriginalContext>> imageCache;

	bool fbDirtyOnSubpixelChange = true;
	int fbCount = 0;

	Internal()
#if DISTRHO_PLUGIN_WANT_DIRECT_ACCESS
		: hiddenApp(false),
		  hiddenWindow(hiddenApp, 0, DISTRHO_UI_DEFAULT_WIDTH, DISTRHO_UI_DEFAULT_HEIGHT, 0.0, true)
	{
		hiddenWindow.setIgnoringKeyRepeat(true);
		hiddenApp.idle();
	}
#else
	{}
#endif
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

	// Set up NanoVG
	const int nvgFlags = NVG_ANTIALIAS;

#if DISTRHO_PLUGIN_WANT_DIRECT_ACCESS
	const DGL_NAMESPACE::Window::ScopedGraphicsContext sgc(internal->hiddenWindow);
	vg = nvgCreateGL(nvgFlags);
#else
	vg = static_cast<CardinalPluginContext*>(APP)->tlw->getContext();
#endif
	DISTRHO_SAFE_ASSERT_RETURN(vg != nullptr,);

#ifdef NANOVG_GLES2
	fbVg = nvgCreateSharedGLES2(vg, nvgFlags);
#else
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

#ifdef DISTRHO_OS_WASM
	emscripten_lock_orientation(EMSCRIPTEN_ORIENTATION_LANDSCAPE_PRIMARY);
#endif
}

void WindowSetPluginRemote(Window* const window, NanoTopLevelWidget* const tlw)
{
	// if nanovg context failed, init only bare minimum
	if (window->vg == nullptr)
	{
		if (tlw != nullptr)
		{
			window->internal->tlw = tlw;
			window->internal->size = rack::math::Vec(tlw->getWidth(), tlw->getHeight());
		}
		else
		{
			window->internal->tlw = nullptr;
			window->internal->callback = nullptr;
		}
		return;
	}

	if (tlw != nullptr)
	{
		const GLubyte* vendor = glGetString(GL_VENDOR);
		const GLubyte* renderer = glGetString(GL_RENDERER);
		const GLubyte* version = glGetString(GL_VERSION);
		INFO("Renderer: %s %s", vendor, renderer);
		INFO("OpenGL: %s", version);

		window->internal->tlw = tlw;
		window->internal->size = rack::math::Vec(tlw->getWidth(), tlw->getHeight());

#if DISTRHO_PLUGIN_WANT_DIRECT_ACCESS
		// Set up NanoVG
		window->internal->r_vg = tlw->getContext();
#ifdef NANOVG_GLES2
		window->internal->r_fbVg = nvgCreateSharedGLES2(window->internal->r_vg, NVG_ANTIALIAS);
#else
		window->internal->r_fbVg = nvgCreateSharedGL2(window->internal->r_vg, NVG_ANTIALIAS);
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
#endif

		// Init settings
		WindowParametersRestore(window);

		widget::Widget::ContextCreateEvent e = {};
		e.vg = window->vg;
		APP->scene->onContextCreate(e);
	}
	else
	{
		widget::Widget::ContextDestroyEvent e = {};
		e.vg = window->vg;
		APP->scene->onContextDestroy(e);

#if DISTRHO_PLUGIN_WANT_DIRECT_ACCESS
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
		nvgDeleteGLES2(window->internal->r_fbVg);
#else
		nvgDeleteGL2(window->internal->r_fbVg);
#endif
#endif

		window->internal->tlw = nullptr;
		window->internal->callback = nullptr;
	}
}

void WindowSetPluginUI(Window* const window, CardinalBaseUI* const ui)
{
	// if nanovg context failed, init only bare minimum
	if (window->vg == nullptr)
	{
		if (ui != nullptr)
		{
			window->internal->ui = ui;
			window->internal->size = rack::math::Vec(ui->getWidth(), ui->getHeight());
		}
		else
		{
			window->internal->ui = nullptr;
			window->internal->callback = nullptr;
		}
		return;
	}

	if (ui != nullptr)
	{
		const GLubyte* vendor = glGetString(GL_VENDOR);
		const GLubyte* renderer = glGetString(GL_RENDERER);
		const GLubyte* version = glGetString(GL_VERSION);
		INFO("Renderer: %s %s", vendor, renderer);
		INFO("OpenGL: %s", version);

		window->internal->tlw = ui;
		window->internal->ui = ui;
		window->internal->size = rack::math::Vec(ui->getWidth(), ui->getHeight());

#if DISTRHO_PLUGIN_WANT_DIRECT_ACCESS
		// Set up NanoVG
		window->internal->r_vg = ui->getContext();
#ifdef NANOVG_GLES2
		window->internal->r_fbVg = nvgCreateSharedGLES2(window->internal->r_vg, NVG_ANTIALIAS);
#else
		window->internal->r_fbVg = nvgCreateSharedGL2(window->internal->r_vg, NVG_ANTIALIAS);
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
#endif

		// Init settings
		WindowParametersRestore(window);

		widget::Widget::ContextCreateEvent e = {};
		e.vg = window->vg;
		APP->scene->onContextCreate(e);
	}
	else
	{
		widget::Widget::ContextDestroyEvent e = {};
		e.vg = window->vg;
		APP->scene->onContextDestroy(e);

#if DISTRHO_PLUGIN_WANT_DIRECT_ACCESS
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
		nvgDeleteGLES2(window->internal->r_fbVg);
#else
		nvgDeleteGL2(window->internal->r_fbVg);
#endif
#endif

		window->internal->tlw = nullptr;
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
#if DISTRHO_PLUGIN_WANT_DIRECT_ACCESS
		DGL_NAMESPACE::Window::ScopedGraphicsContext sgc(internal->hiddenWindow);
		internal->hiddenWindow.close();
		internal->hiddenApp.idle();
#endif

		// Fonts and Images in the cache must be deleted before the NanoVG context is deleted
		internal->fontCache.clear();
		internal->imageCache.clear();

		if (vg != nullptr)
		{
#if DISTRHO_PLUGIN_WANT_DIRECT_ACCESS
#if defined NANOVG_GLES2
			nvgDeleteGLES2(internal->o_fbVg != nullptr ? internal->o_fbVg : fbVg);
			nvgDeleteGLES2(internal->o_vg != nullptr ? internal->o_vg : vg);
#else
			nvgDeleteGL2(internal->o_fbVg != nullptr ? internal->o_fbVg : fbVg);
			nvgDeleteGL2(internal->o_vg != nullptr ? internal->o_vg : vg);
#endif
#else
#if defined NANOVG_GLES2
			nvgDeleteGLES2(fbVg);
#else
			nvgDeleteGL2(fbVg);
#endif
#endif
		}
	}

	delete internal;
}


math::Vec Window::getSize() {
	return internal->size;
}


void Window::setSize(math::Vec size) {
	size = size.max(WINDOW_SIZE_MIN);
	internal->size = size;

	if (DGL_NAMESPACE::NanoTopLevelWidget* const tlw = internal->ui)
		tlw->setSize(internal->size.x, internal->size.y);
}

void WindowSetInternalSize(rack::window::Window* const window, math::Vec size) {
	size = size.max(WINDOW_SIZE_MIN);
	window->internal->size = size;
}


void Window::run() {
	internal->frame = 0;
}


#ifdef CARDINAL_WINDOW_CAN_GENERATE_SCREENSHOTS
static void Window__flipBitmap(uint8_t* pixels, const int width, const int height, const int depth) {
	for (int y = 0; y < height / 2; y++) {
		const int flipY = height - y - 1;
		uint8_t tmp[width * depth];
		std::memcpy(tmp, &pixels[y * width * depth], width * depth);
		std::memmove(&pixels[y * width * depth], &pixels[flipY * width * depth], width * depth);
		std::memcpy(&pixels[flipY * width * depth], tmp, width * depth);
	}
}


#ifdef STBI_WRITE_NO_STDIO
static void Window__downscaleBitmap(uint8_t* pixels, int& width, int& height) {
	int targetWidth = width;
	int targetHeight = height;
	double scale = 1.0;

	if (targetWidth > 340) {
		scale = width / 340.0;
		targetWidth = 340;
		targetHeight = height / scale;
	}
	if (targetHeight > 210) {
		scale = height / 210.0;
		targetHeight = 210;
		targetWidth = width / scale;
	}
	DISTRHO_SAFE_ASSERT_INT_RETURN(targetWidth <= 340, targetWidth,);
	DISTRHO_SAFE_ASSERT_INT_RETURN(targetHeight <= 210, targetHeight,);

	// FIXME worst possible quality :/
	for (int y = 0; y < targetHeight; ++y) {
		const int ys = static_cast<int>(y * scale);
		for (int x = 0; x < targetWidth; ++x) {
			const int xs = static_cast<int>(x * scale);
			std::memmove(pixels + (width * y + x) * 3, pixels + (width * ys + xs) * 3, 3);
		}
	}

	width = targetWidth;
	height = targetHeight;
}

static void Window__writeImagePNG(void* context, void* data, int size) {
	USE_NAMESPACE_DISTRHO
	CardinalBaseUI* const ui = static_cast<CardinalBaseUI*>(context);
	if (char* const screenshot = String::asBase64(data, size).getAndReleaseBuffer()) {
		ui->setState("screenshot", screenshot);
		if (ui->remoteDetails != nullptr && ui->remoteDetails->connected && ui->remoteDetails->screenshot)
			remoteUtils::sendScreenshotToRemote(ui->remoteDetails, screenshot);
		std::free(screenshot);
	}
}
#endif
#endif


void Window::step() {
	if (internal->tlw == nullptr || vg == nullptr)
		return;

	double frameTime = system::getTime();
	if (std::isfinite(internal->frameTime)) {
		internal->lastFrameDuration = frameTime - internal->frameTime;
	}
	internal->frameTime = frameTime;
	internal->fbCount = 0;
	// double t1 = 0.0, t2 = 0.0, t3 = 0.0, t4 = 0.0, t5 = 0.0;

	// Make event handlers and step() have a clean NanoVG context
	nvgReset(vg);

	if (uiFont != nullptr)
		bndSetFont(uiFont->handle);

	// Set window title
	if (isStandalone()) {
#if CARDINAL_VARIANT_MINI
		std::string windowTitle = "Cardinal Mini";
#else
		std::string windowTitle = "Cardinal";
#endif
		if (APP->patch->path != "") {
			windowTitle += " - ";
			if (!APP->history->isSaved())
				windowTitle += "*";
			windowTitle += system::getFilename(APP->patch->path);
		}
		if (windowTitle != internal->lastWindowTitle) {
			internal->tlw->getWindow().setTitle(windowTitle.c_str());
			internal->lastWindowTitle = windowTitle;
		}
	}

	// Get desired pixel ratio
	float newPixelRatio = internal->tlw->getScaleFactor();
	if (newPixelRatio != pixelRatio) {
		pixelRatio = newPixelRatio;
		APP->event->handleDirty();
	}

#ifdef CARDINAL_WINDOW_CAN_GENERATE_SCREENSHOTS
	// Hide menu and background if generating screenshot
	if (internal->generateScreenshotStep == kScreenshotStepStarted) {
#ifdef CARDINAL_TRANSPARENT_SCREENSHOTS
		APP->scene->menuBar->hide();
		APP->scene->rack->children.front()->hide();
#else
		internal->generateScreenshotStep = kScreenshotStepSecondPass;
#endif
	}
#endif

	// Get framebuffer/window ratio
	int winWidth = internal->tlw->getWidth();
	int winHeight = internal->tlw->getHeight();
	int fbWidth = winWidth;
	int fbHeight = winHeight;
	windowRatio = (float)fbWidth / winWidth;
	// t1 = system::getTime();

	if (APP->scene) {
		// DEBUG("%f %f %d %d", pixelRatio, windowRatio, fbWidth, winWidth);
		// Resize scene
		APP->scene->box.size = math::Vec(fbWidth, fbHeight).div(pixelRatio);

		// Step scene
		APP->scene->step();
		// t2 = system::getTime();

		// Render scene
		{
			// Update and render
			nvgScale(vg, pixelRatio, pixelRatio);

			// Draw scene
			widget::Widget::DrawArgs args;
			args.vg = vg;
			args.clipBox = APP->scene->box.zeroPos();
			APP->scene->draw(args);
			// t3 = system::getTime();

			glViewport(0, 0, fbWidth, fbHeight);
#ifdef CARDINAL_TRANSPARENT_SCREENSHOTS
			glClearColor(0.0, 0.0, 0.0, 0.0);
#else
			glClearColor(0.0, 0.0, 0.0, 1.0);
#endif
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		}
		// t4 = system::getTime();
	}

	// t5 = system::getTime();
	// DEBUG("pre-step %6.1f step %6.1f draw %6.1f nvgEndFrame %6.1f glfwSwapBuffers %6.1f total %6.1f",
	// 	(t1 - frameTime) * 1e3f,
	// 	(t2 - t1) * 1e3f,
	// 	(t3 - t2) * 1e3f,
	// 	(t4 - t3) * 1e3f,
	// 	(t5 - t4) * 1e3f,
	// 	(t5 - frameTime) * 1e3f
	// );
	++internal->frame;

#ifdef CARDINAL_WINDOW_CAN_GENERATE_SCREENSHOTS
	if (internal->generateScreenshotStep != kScreenshotStepNone) {
		++internal->generateScreenshotStep;

		int y = 0;
#ifdef CARDINAL_TRANSPARENT_SCREENSHOTS
		constexpr const int depth = 4;
#else
		y = APP->scene->menuBar->box.size.y * newPixelRatio;
		constexpr const int depth = 3;
#endif

		// Allocate pixel color buffer
		uint8_t* const pixels = new uint8_t[winHeight * winWidth * 4];

		// glReadPixels defaults to GL_BACK, but the back-buffer is unstable, so use the front buffer (what the user sees)
		glReadBuffer(GL_FRONT);
		glReadPixels(0, 0, winWidth, winHeight, depth == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, pixels);

		if (internal->generateScreenshotStep == kScreenshotStepSaving)
		{
			// Write pixels to PNG
			Window__flipBitmap(pixels, winWidth, winHeight, depth);
			winHeight -= y;
			const int stride = winWidth * depth;
			uint8_t* const pixelsWithOffset = pixels + (stride * y);
#ifdef STBI_WRITE_NO_STDIO
			Window__downscaleBitmap(pixelsWithOffset, winWidth, winHeight);
			stbi_write_png_to_func(Window__writeImagePNG, internal->ui,
			                       winWidth, winHeight, depth, pixelsWithOffset, stride);
#else
			stbi_write_png("screenshot.png", winWidth, winHeight, depth, pixelsWithOffset, stride);
#endif

			internal->generateScreenshotStep = kScreenshotStepNone;
#ifdef CARDINAL_TRANSPARENT_SCREENSHOTS
			APP->scene->menuBar->show();
			APP->scene->rack->children.front()->show();
#endif
		}

		delete[] pixels;
	}
#endif
}


void Window::screenshot(const std::string& screenshotPath) {
}


void Window::screenshotModules(const std::string& screenshotsDir, float zoom) {
}


void Window::close() {
	DISTRHO_SAFE_ASSERT_RETURN(internal->tlw != nullptr,);

	internal->tlw->getWindow().close();
}


void Window::cursorLock() {
#ifdef DISTRHO_OS_WASM
	if (!settings::allowCursorLock)
		return;

	emscripten_request_pointerlock(internal->tlw->getWindow().getApp().getClassName(), false);
#endif
}


void Window::cursorUnlock() {
#ifdef DISTRHO_OS_WASM
	if (!settings::allowCursorLock)
		return;

	emscripten_exit_pointerlock();
#endif
}


bool Window::isCursorLocked() {
#ifdef DISTRHO_OS_WASM
	EmscriptenPointerlockChangeEvent status;
	if (emscripten_get_pointerlock_status(&status) == EMSCRIPTEN_RESULT_SUCCESS)
		return status.isActive;
#endif
	return false;
}


int Window::getMods() {
	return internal->mods;
}


void Window::setFullScreen(bool fullScreen) {
#ifdef DISTRHO_OS_WASM
	if (fullScreen)
	{
		try {
			emscripten_request_fullscreen(internal->tlw->getWindow().getApp().getClassName(), false);
		} DISTRHO_SAFE_EXCEPTION("fullscreen");
	}
	else
	{
		emscripten_exit_fullscreen();
	}
#endif
}


bool Window::isFullScreen() {
#ifdef DISTRHO_OS_WASM
	EmscriptenFullscreenChangeEvent status;
	if (emscripten_get_fullscreen_status(&status) == EMSCRIPTEN_RESULT_SUCCESS)
		return status.isFullscreen;
	return false;
#elif defined(CARDINAL_WINDOW_CAN_GENERATE_SCREENSHOTS) && defined(CARDINAL_TRANSPARENT_SCREENSHOTS)
	return internal->generateScreenshotStep != kScreenshotStepNone;
#else
	return false;
#endif
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
	double frameDuration = 1.f / internal->monitorRefreshRate;
	return frameDuration - (system::getTime() - internal->frameTime);
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


int& Window::fbCount() {
	return internal->fbCount;
}


void generateScreenshot() {
#ifdef CARDINAL_WINDOW_CAN_GENERATE_SCREENSHOTS
	APP->window->internal->generateScreenshotStep = kScreenshotStepStarted;
#endif
}


void init() {
}


void destroy() {
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
	if (d_isNotEqual(window->internal->params.browserZoom, rack::settings::browserZoom))
	{
		window->internal->params.browserZoom = rack::settings::browserZoom;
		if (window->internal->callback != nullptr)
			window->internal->callback->WindowParametersChanged(kWindowParameterBrowserZoom,
			                                                    rack::settings::browserZoom);
	}
	if (window->internal->params.knobMode != rack::settings::knobMode)
	{
		window->internal->params.knobMode = rack::settings::knobMode;
		if (window->internal->callback != nullptr)
			window->internal->callback->WindowParametersChanged(kWindowParameterKnobMode,
			                                                    rack::settings::knobMode);
	}
	if (window->internal->params.browserSort != rack::settings::browserSort)
	{
		window->internal->params.browserSort = rack::settings::browserSort;
		if (window->internal->callback != nullptr)
			window->internal->callback->WindowParametersChanged(kWindowParameterBrowserSort,
			                                                    rack::settings::browserSort);
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
	if (window->internal->params.squeezeModules != rack::settings::squeezeModules)
	{
		window->internal->params.squeezeModules = rack::settings::squeezeModules;
		if (window->internal->callback != nullptr)
			window->internal->callback->WindowParametersChanged(kWindowParameterSqueezeModulePositions,
			                                                    rack::settings::squeezeModules);
	}
	if (window->internal->params.invertZoom != rack::settings::invertZoom)
	{
		window->internal->params.invertZoom = rack::settings::invertZoom;
		if (window->internal->callback != nullptr)
			window->internal->callback->WindowParametersChanged(kWindowParameterInvertZoom,
			                                                    rack::settings::invertZoom);
	}
	if (window->internal->params.rateLimit != rack::settings::rateLimit)
	{
		window->internal->params.rateLimit = rack::settings::rateLimit;
		if (window->internal->callback != nullptr)
			window->internal->callback->WindowParametersChanged(kWindowParameterUpdateRateLimit,
			                                                    rack::settings::rateLimit);
	}
}

void WindowParametersRestore(rack::window::Window* const window)
{
	rack::settings::cableOpacity = window->internal->params.cableOpacity;
	rack::settings::cableTension = window->internal->params.cableTension;
	rack::settings::rackBrightness = window->internal->params.rackBrightness;
	rack::settings::haloBrightness = window->internal->params.haloBrightness;
	rack::settings::knobScrollSensitivity = window->internal->params.knobScrollSensitivity;
	rack::settings::browserZoom = window->internal->params.browserZoom;
	rack::settings::knobMode = static_cast<rack::settings::KnobMode>(window->internal->params.knobMode);
	rack::settings::browserSort = static_cast<rack::settings::BrowserSort>(window->internal->params.browserSort);
	rack::settings::tooltips = window->internal->params.tooltips;
	rack::settings::knobScroll = window->internal->params.knobScroll;
	rack::settings::lockModules = window->internal->params.lockModules;
	rack::settings::squeezeModules = window->internal->params.squeezeModules;
	rack::settings::invertZoom = window->internal->params.invertZoom;
	rack::settings::rateLimit = window->internal->params.rateLimit;
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

