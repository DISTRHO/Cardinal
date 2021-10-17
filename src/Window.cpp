#include <map>
#include <queue>
#include <thread>

#include <osdialog.h>

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
	int frameSwapInterval = 1;
	double monitorRefreshRate = 60.0; // FIXME
	double frameTime = 0.0;
	double lastFrameDuration = 0.0;

	std::map<std::string, std::shared_ptr<Font>> fontCache;
	std::map<std::string, std::shared_ptr<Image>> imageCache;

	bool fbDirtyOnSubpixelChange = true;
};

Window::Window() {
	internal = new Internal;
	internal->ui = lastUI;
	internal->size = minWindowSize;

	int err;

	const GLubyte* vendor = glGetString(GL_VENDOR);
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	INFO("Renderer: %s %s", vendor, renderer);
	INFO("OpenGL: %s", version);
	INFO("UI pointer: %p", lastUI);

	vg = lastUI->getContext();
	fbVg = nvgCreateSharedGL2(vg, NVG_ANTIALIAS);

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
// 	nvgReset(vg);

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
// 			nvgBeginFrame(vg, fbWidth, fbHeight, pixelRatio);
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
// 			nvgEndFrame(vg);
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


void mouseButtonCallback(Context* ctx, int button, int action, int mods) {

}

void cursorPosCallback(Context* ctx, double xpos, double ypos) {
}

void cursorEnterCallback(Context* ctx, int entered) {
	if (!entered) {
		ctx->event->handleLeave();
	}
}

void scrollCallback(Context* ctx, double x, double y) {
}

void charCallback(Context* ctx, unsigned int codepoint) {
}

void keyCallback(Context* ctx, int key, int scancode, int action, int mods) {
}


} // namespace window
} // namespace rack
