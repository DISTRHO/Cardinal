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

#define NANOVG_GL2 1

#include <map>
#include <queue>
#include <thread>

#include "../src/Rack/dep/glfw/deps/stb_image_write.h"
#include <GL/osmesa.h>

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

#include "DistrhoPlugin.hpp"
#include "extra/Thread.hpp"
#include "../WindowParameters.hpp"

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

struct Window::Internal : public Thread {
	DISTRHO_NAMESPACE::Plugin* plugin = nullptr;
	DISTRHO_NAMESPACE::WindowParameters params;
	DISTRHO_NAMESPACE::WindowParametersCallback* callback = nullptr;
	Context* context = nullptr;
	Window* self = nullptr;
	OSMesaContext mesa = nullptr;
	GLubyte* mesaBuffer = nullptr;

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

	void run() override {
		self->run();
		int i=0;
		while (! shouldThreadExit()) {
			// d_msleep(500);
			d_stdout("thread idle %i - start", i);
			contextSet(context);
			d_stdout("thread idle %i - context was set", i);
			self->step();
			d_stdout("thread idle %i - did step", i);
			if (i++ == 0)
			{
		// render to png
		stbi_write_png("testing.png", 1228, 666, 4, mesaBuffer, 666 * 4);
			d_stdout("thread idle %i - wrote to png", i);
			}
		}
		d_stdout("thread quit");
	}
};

Window::Window() {
	internal = new Internal;
	internal->context = APP;
	internal->self = this;
}

static void flipBitmap(uint8_t* pixels, int width, int height, int depth) {
	for (int y = 0; y < height / 2; y++) {
		int flipY = height - y - 1;
		uint8_t tmp[width * depth];
		std::memcpy(tmp, &pixels[y * width * depth], width * depth);
		std::memcpy(&pixels[y * width * depth], &pixels[flipY * width * depth], width * depth);
		std::memcpy(&pixels[flipY * width * depth], tmp, width * depth);
	}
}

void WindowInit(Window* const window, DISTRHO_NAMESPACE::Plugin* const plugin)
{
	window->internal->plugin = plugin;

	window->internal->mesa = OSMesaCreateContextExt(OSMESA_RGBA, 24, 8, 0, nullptr);
	DISTRHO_SAFE_ASSERT_RETURN(window->internal->mesa != nullptr,);

    int width = 1228;
    int height = 666;

	window->internal->mesaBuffer = new GLubyte[1228 * 666 * 4]; // 4 for RGBA
	bool ok = OSMesaMakeCurrent(window->internal->mesa, window->internal->mesaBuffer, GL_UNSIGNED_BYTE, 1228, 666);

   {
      int z, s, a;
      glGetIntegerv(GL_DEPTH_BITS, &z);
      glGetIntegerv(GL_STENCIL_BITS, &s);
      glGetIntegerv(GL_ACCUM_RED_BITS, &a);
      printf("Depth=%d Stencil=%d Accum=%d\n", z, s, a);
   }

	// Set up NanoVG
	int nvgFlags = NVG_ANTIALIAS;
// #if defined NANOVG_GL2
	window->vg = nvgCreateGL2(nvgFlags);
	window->fbVg = nvgCreateSharedGL2(window->vg, nvgFlags);
// #elif defined NANOVG_GL3
// 	window->vg = nvgCreateGL3(nvgFlags);
// #elif defined NANOVG_GLES2
// 	window->vg = nvgCreateGLES2(nvgFlags);
// 	window->fbVg = nvgCreateSharedGLES2(window->vg, nvgFlags);
// #endif

	// Load default Blendish font
	window->uiFont = window->loadFont(asset::system("res/fonts/DejaVuSans.ttf"));
	if (window->uiFont != nullptr)
		bndSetFont(window->uiFont->handle);

	// Init settings
	WindowParametersRestore(window);

	if (APP->scene) {
		widget::Widget::ContextCreateEvent e;
		APP->scene->onContextCreate(e);
	}

	d_stdout("all good with mesa and GL? %d | %p %p %p", ok, window->internal->mesa, window->vg, window->fbVg);
	// window->internal->startThread();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, static_cast<GLdouble>(width), static_cast<GLdouble>(height), 0.0, 0.0, 1.0);
    glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glColor3f(0.5f, 0.2f, 0.9f);

    glBegin(GL_QUADS);

    {
        const int x = width / 4;
        const int y = height / 4;
        const int w = width / 2;
        const int h = height / 2;

        glTexCoord2f(0.0f, 0.0f);
        glVertex2d(x, y);

        glTexCoord2f(1.0f, 0.0f);
        glVertex2d(x+w, y);

        glTexCoord2f(1.0f, 1.0f);
        glVertex2d(x+w, y+h);

        glTexCoord2f(0.0f, 1.0f);
        glVertex2d(x, y+h);
    }

    glEnd();

	for (int i=0; i<5; ++i)
	{
		d_stdout("idle %i - before step", i);
		window->step();
		d_stdout("idle %i - after step", i);
	}

	glFinish();

	// render to png
	d_stdout("idle - before png");
	// flipBitmap(window->internal->mesaBuffer, width, height, 4);
	// stbi_write_png("testing.png", 1228, 666, 4, window->internal->mesaBuffer, 666 * 4);

	// Allocate pixel color buffer
	uint8_t* pixels = new uint8_t[height * width * 4];

	// glReadPixels defaults to GL_BACK, but the back-buffer is unstable, so use the front buffer (what the user sees)
	glReadBuffer(GL_FRONT);
	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	// Write pixels to PNG
	flipBitmap(pixels, width, height, 4);
	stbi_write_png("testing.png", width, height, 4, pixels, width * 4);

	delete[] pixels;

	d_stdout("idle - after png");
}

void WindowMods(Window* const window, const int mods)
{
	window->internal->mods = mods;
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

// #if defined NANOVG_GL2
	nvgDeleteGL2(vg);
	nvgDeleteGL2(fbVg);
// #elif defined NANOVG_GL3
// 	nvgDeleteGL3(vg);
// #elif defined NANOVG_GLES2
// 	nvgDeleteGLES2(vg);
// 	nvgDeleteGLES2(fbVg);
// #endif

	if (internal->mesa != nullptr)
		OSMesaDestroyContext(internal->mesa);

	delete[] internal->mesaBuffer;
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

