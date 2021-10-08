#include <map>
#include <queue>
#include <thread>

#include <stb_image_write.h>
#include <osdialog.h>

#include <window/Window.hpp>
#include <asset.hpp>
#include <widget/Widget.hpp>
#include <app/Scene.hpp>
#include <keyboard.hpp>
#include <context.hpp>
#include <patch.hpp>
#include <settings.hpp>
#include <plugin.hpp> // used in Window::screenshot
#include <system.hpp> // used in Window::screenshot

#include "DistrhoUI.hpp"

namespace rack {
namespace window {

extern DISTRHO_NAMESPACE::UI* lastUI;

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


struct Window::Internal {
	DISTRHO_NAMESPACE::UI* ui;
	math::Vec size;

	std::string lastWindowTitle;

	int frame = 0;
	bool ignoreNextMouseDelta = false;
	int frameSwapInterval = -1;
	double monitorRefreshRate = 60.0; // FIXME
	double frameTime = 0.0;
	double lastFrameDuration = 0.0;

	math::Vec lastMousePos;

	std::map<std::string, std::shared_ptr<Font>> fontCache;
	std::map<std::string, std::shared_ptr<Image>> imageCache;

	bool fbDirtyOnSubpixelChange = true;
};

Window::Window() {
	internal = new Internal;
	internal->ui = lastUI;
	internal->size = minWindowSize;

	int err;

	// Set up GLEW
	glewExperimental = GL_TRUE;
	err = glewInit();
	if (err != GLEW_OK) {
		osdialog_message(OSDIALOG_ERROR, OSDIALOG_OK, "Could not initialize GLEW. Does your graphics card support OpenGL 2.0 or greater? If so, make sure you have the latest graphics drivers installed.");
		exit(1);
	}

	const GLubyte* vendor = glGetString(GL_VENDOR);
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	INFO("Renderer: %s %s", vendor, renderer);
	INFO("OpenGL: %s", version);
	INFO("UI pointer: %p", lastUI);

	// GLEW generates GL error because it calls glGetString(GL_EXTENSIONS), we'll consume it here.
	glGetError();

	// Set up NanoVG
	int nvgFlags = NVG_ANTIALIAS;
#if defined NANOVG_GL2
	vg = nvgCreateGL2(nvgFlags);
	fbVg = nvgCreateSharedGL2(vg, nvgFlags);
#elif defined NANOVG_GL3
	vg = nvgCreateGL3(nvgFlags);
#elif defined NANOVG_GLES2
	vg = nvgCreateGLES2(nvgFlags);
#endif
	if (!vg) {
		osdialog_message(OSDIALOG_ERROR, OSDIALOG_OK, "Could not initialize NanoVG. Does your graphics card support OpenGL 2.0 or greater? If so, make sure you have the latest graphics drivers installed.");
		exit(1);
	}
	d_stderr2("framebuffer is %p", fbVg);

	// Load default Blendish font
	uiFont = loadFont(asset::system("res/fonts/DejaVuSans.ttf"));
	bndSetFont(uiFont->handle);

	if (APP->scene) {
		widget::Widget::ContextCreateEvent e;
		APP->scene->onContextCreate(e);
	}
}


Window::~Window() {
	if (APP->scene) {
		widget::Widget::ContextDestroyEvent e;
		APP->scene->onContextDestroy(e);
	}

	// Fonts and Images in the cache must be deleted before the NanoVG context is deleted
	internal->fontCache.clear();
	internal->imageCache.clear();

	// nvgDeleteClone(fbVg);

#if defined NANOVG_GL2
	nvgDeleteGL2(vg);
	nvgDeleteGL2(fbVg);
#elif defined NANOVG_GL3
	nvgDeleteGL3(vg);
#elif defined NANOVG_GLES2
	nvgDeleteGLES2(vg);
#endif

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
	double t1 = 0.0, t2 = 0.0, t3 = 0.0, t4 = 0.0, t5 = 0.0;

	// Make event handlers and step() have a clean NanoVG context
	nvgReset(vg);

	bndSetFont(uiFont->handle);

	// Poll events
	// Save and restore context because event handler set their own context based on which window they originate from.
	// Context* context = contextGet();
	// glfwPollEvents();
	// contextSet(context);

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
	t1 = system::getTime();

	if (APP->scene) {
		// DEBUG("%f %f %d %d", pixelRatio, windowRatio, fbWidth, winWidth);
		// Resize scene
		APP->scene->box.size = math::Vec(fbWidth, fbHeight).div(pixelRatio);

		// Step scene
		APP->scene->step();
		t2 = system::getTime();

		// Render scene
		bool visible = true;
		if (visible) {
			// Update and render
			nvgBeginFrame(vg, fbWidth, fbHeight, pixelRatio);
			nvgScale(vg, pixelRatio, pixelRatio);

			// Draw scene
			widget::Widget::DrawArgs args;
			args.vg = vg;
			args.clipBox = APP->scene->box.zeroPos();
			APP->scene->draw(args);
			t3 = system::getTime();

			glViewport(0, 0, fbWidth, fbHeight);
			glClearColor(0.0, 0.0, 0.0, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			nvgEndFrame(vg);
			t4 = system::getTime();
		}
	}

	t5 = system::getTime();

	// DEBUG("pre-step %6.1f step %6.1f draw %6.1f nvgEndFrame %6.1f glfwSwapBuffers %6.1f total %6.1f",
	// 	(t1 - frameTime) * 1e3f,
	// 	(t2 - t1) * 1e3f,
	// 	(t3 - t2) * 1e3f,
	// 	(t4 - t2) * 1e3f,
	// 	(t5 - t4) * 1e3f,
	// 	(t5 - frameTime) * 1e3f
	// );
	internal->frame++;
}


void Window::screenshot(const std::string&) {
}


void Window::screenshotModules(const std::string&, float) {
}


void Window::close() {
	internal->ui->getWindow().close();
}


void Window::cursorLock() {
	if (!settings::allowCursorLock)
		return;

	internal->ignoreNextMouseDelta = true;
}


void Window::cursorUnlock() {
	if (!settings::allowCursorLock)
		return;

	internal->ignoreNextMouseDelta = true;
}


bool Window::isCursorLocked() {
	return internal->ignoreNextMouseDelta;
}


int Window::getMods() {
	int mods = 0;
	/*
	if (glfwGetKey(win, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(win, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
		mods |= GLFW_MOD_SHIFT;
	if (glfwGetKey(win, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(win, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)
		mods |= GLFW_MOD_CONTROL;
	if (glfwGetKey(win, GLFW_KEY_LEFT_ALT) == GLFW_PRESS || glfwGetKey(win, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS)
		mods |= GLFW_MOD_ALT;
	if (glfwGetKey(win, GLFW_KEY_LEFT_SUPER) == GLFW_PRESS || glfwGetKey(win, GLFW_KEY_RIGHT_SUPER) == GLFW_PRESS)
		mods |= GLFW_MOD_SUPER;
	*/
	return mods;
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


void mouseButtonCallback(Window* win, int button, int action, int mods) {
	/*
#if defined ARCH_MAC
	// Remap Ctrl-left click to right click on Mac
	if (button == GLFW_MOUSE_BUTTON_LEFT && (mods & RACK_MOD_MASK) == GLFW_MOD_CONTROL) {
		button = GLFW_MOUSE_BUTTON_RIGHT;
		mods &= ~GLFW_MOD_CONTROL;
	}
	// Remap Ctrl-shift-left click to middle click on Mac
	if (button == GLFW_MOUSE_BUTTON_LEFT && (mods & RACK_MOD_MASK) == (GLFW_MOD_CONTROL | GLFW_MOD_SHIFT)) {
		button = GLFW_MOUSE_BUTTON_MIDDLE;
		mods &= ~(GLFW_MOD_CONTROL | GLFW_MOD_SHIFT);
	}
#endif
*/

	APP->event->handleButton(win->internal->lastMousePos, button, action, mods);
}

void cursorPosCallback(Window* win, double xpos, double ypos) {
	math::Vec mousePos = math::Vec(xpos, ypos).div(win->pixelRatio / win->windowRatio).round();
	math::Vec mouseDelta = mousePos.minus(win->internal->lastMousePos);

	// Workaround for GLFW warping mouse to a different position when the cursor is locked or unlocked.
	if (win->internal->ignoreNextMouseDelta) {
		win->internal->ignoreNextMouseDelta = false;
		mouseDelta = math::Vec();
	}

	win->internal->lastMousePos = mousePos;

	APP->event->handleHover(mousePos, mouseDelta);

	// Keyboard/mouse MIDI driver
	math::Vec scaledPos(xpos / win->internal->ui->getWidth(), ypos / win->internal->ui->getHeight());
	keyboard::mouseMove(scaledPos);
}

void scrollCallback(Window* win, double x, double y) {
	math::Vec scrollDelta = math::Vec(x, y);
#if defined ARCH_MAC
	scrollDelta = scrollDelta.mult(10.0);
#else
	scrollDelta = scrollDelta.mult(50.0);
#endif

	APP->event->handleScroll(win->internal->lastMousePos, scrollDelta);
}


void init() {
}

void destroy() {
}


} // namespace window
} // namespace rack
