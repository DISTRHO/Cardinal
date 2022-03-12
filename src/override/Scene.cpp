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
 * This file is an edited version of VCVRack's app/Scene.cpp
 * Copyright (C) 2016-2021 VCV.
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 */

#include <thread>

#include <osdialog.h>

#include <app/Scene.hpp>
#include <app/Browser.hpp>
#include <app/TipWindow.hpp>
#include <app/MenuBar.hpp>
#include <context.hpp>
#include <engine/Engine.hpp>
#include <system.hpp>
#include <network.hpp>
#include <history.hpp>
#include <settings.hpp>
#include <patch.hpp>
#include <asset.hpp>

#ifdef NDEBUG
# undef DEBUG
#endif

#ifdef STATIC_BUILD
# undef HAVE_LIBLO
#endif

#ifdef HAVE_LIBLO
# include <lo/lo.h>
#endif

#include "../CardinalCommon.hpp"
#include "extra/Base64.hpp"
#include "DistrhoUtils.hpp"


namespace rack {
namespace app {


struct ResizeHandle : widget::OpaqueWidget {
	math::Vec size;

	void draw(const DrawArgs& args) override {
		nvgStrokeColor(args.vg, nvgRGBf(1, 1, 1));
		nvgStrokeWidth(args.vg, 1);

		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, box.size.x, 0);
		nvgLineTo(args.vg, 0, box.size.y);
		nvgStroke(args.vg);

		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, box.size.x + 5, 0);
		nvgLineTo(args.vg, 0, box.size.y + 5);
		nvgStroke(args.vg);

		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, box.size.x + 10, 0);
		nvgLineTo(args.vg, 0, box.size.y + 10);
		nvgStroke(args.vg);

		nvgStrokeColor(args.vg, nvgRGBf(0, 0, 0));

		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, box.size.x+1, 0);
		nvgLineTo(args.vg, 0, box.size.y+1);
		nvgStroke(args.vg);

		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, box.size.x + 6, 0);
		nvgLineTo(args.vg, 0, box.size.y + 6);
		nvgStroke(args.vg);

		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, box.size.x + 11, 0);
		nvgLineTo(args.vg, 0, box.size.y + 11);
		nvgStroke(args.vg);
	}

	void onHover(const HoverEvent& e) override {
		e.consume(this);
	}

	void onEnter(const EnterEvent& e) override {
		glfwSetCursor(nullptr, (GLFWcursor*)0x1);
	}

	void onLeave(const LeaveEvent& e) override {
		glfwSetCursor(nullptr, nullptr);
	}

	void onDragStart(const DragStartEvent&) override {
		size = APP->window->getSize();
	}

	void onDragMove(const DragMoveEvent& e) override {
		size = size.plus(e.mouseDelta.mult(APP->window->pixelRatio));
		APP->window->setSize(size.round());
	}
};


struct Scene::Internal {
	ResizeHandle* resizeHandle;

	bool heldArrowKeys[4] = {};

#ifdef HAVE_LIBLO
	double lastSceneChangeTime = 0.0;
	int historyActionIndex = -1;

	bool oscAutoDeploy = false;
	bool oscConnected = false;
	lo_server oscServer = nullptr;

	static int osc_handler(const char* const path, const char* const types, lo_arg** argv, const int argc, lo_message, void* const self)
	{
		d_stdout("osc_handler(\"%s\", \"%s\", %p, %i)", path, types, argv, argc);

		if (std::strcmp(path, "/resp") == 0 && argc == 2 && types[0] == 's' && types[1] == 's') {
			d_stdout("osc_handler(\"%s\", ...) - got resp | '%s' '%s'", path, &argv[0]->s, &argv[1]->s);
			if (std::strcmp(&argv[0]->s, "hello") == 0 && std::strcmp(&argv[1]->s, "ok") == 0)
				static_cast<Internal*>(self)->oscConnected = true;
		}
		return 0;
	}

	~Internal() {
		lo_server_free(oscServer);
	}
#endif
};


Scene::Scene() {
	internal = new Internal;

	rackScroll = new RackScrollWidget;
	addChild(rackScroll);

	rack = rackScroll->rackWidget;

	menuBar = createMenuBar();
	addChild(menuBar);

	browser = browserCreate();
	browser->hide();
	addChild(browser);

	internal->resizeHandle = new ResizeHandle;
	internal->resizeHandle->box.size = math::Vec(16, 16);
	addChild(internal->resizeHandle);
}


Scene::~Scene() {
	delete internal;
}


math::Vec Scene::getMousePos() {
	return mousePos;
}


void Scene::step() {
	if (APP->window->isFullScreen()) {
		// Expand RackScrollWidget to cover entire screen if fullscreen
		rackScroll->box.pos.y = 0;
	}
	else {
		// Always show MenuBar if not fullscreen
		menuBar->show();
		rackScroll->box.pos.y = menuBar->box.size.y;
	}

	internal->resizeHandle->box.pos = box.size.minus(internal->resizeHandle->box.size);

	// Resize owned descendants
	menuBar->box.size.x = box.size.x;
	rackScroll->box.size = box.size.minus(rackScroll->box.pos);

	// Scroll RackScrollWidget with arrow keys
	math::Vec arrowDelta;
	if (internal->heldArrowKeys[0]) {
		arrowDelta.x -= 1;
	}
	if (internal->heldArrowKeys[1]) {
		arrowDelta.x += 1;
	}
	if (internal->heldArrowKeys[2]) {
		arrowDelta.y -= 1;
	}
	if (internal->heldArrowKeys[3]) {
		arrowDelta.y += 1;
	}

	if (!arrowDelta.isZero()) {
		int mods = APP->window->getMods();
		float arrowSpeed = 32.f;
		if ((mods & RACK_MOD_MASK) == RACK_MOD_CTRL)
			arrowSpeed /= 4.f;
		if ((mods & RACK_MOD_MASK) == GLFW_MOD_SHIFT)
			arrowSpeed *= 4.f;
		if ((mods & RACK_MOD_MASK) == (RACK_MOD_CTRL | GLFW_MOD_SHIFT))
			arrowSpeed /= 16.f;

		rackScroll->offset += arrowDelta * arrowSpeed;
	}

#ifdef HAVE_LIBLO
	if (internal->oscServer != nullptr) {
		while (lo_server_recv_noblock(internal->oscServer, 0) != 0) {}

		if (internal->oscAutoDeploy) {
			const int actionIndex = APP->history->actionIndex;
			const double time = system::getTime();
			if (internal->historyActionIndex != actionIndex && time - internal->lastSceneChangeTime >= 5.0) {
				internal->historyActionIndex = actionIndex;
				internal->lastSceneChangeTime = time;
				patchUtils::deployToRemote();
				window::generateScreenshot();
			}
		}
	}
#endif

	Widget::step();
}


void Scene::draw(const DrawArgs& args) {
	Widget::draw(args);
}


void Scene::onHover(const HoverEvent& e) {
	mousePos = e.pos;
	if (mousePos.y < menuBar->box.size.y) {
		menuBar->show();
	}
	OpaqueWidget::onHover(e);
}


void Scene::onDragHover(const DragHoverEvent& e) {
	mousePos = e.pos;
	OpaqueWidget::onDragHover(e);
}


void Scene::onHoverKey(const HoverKeyEvent& e) {
	// Key commands that override children
	if (e.action == GLFW_PRESS || e.action == GLFW_REPEAT) {
		// DEBUG("key '%d '%c' scancode %d '%c' keyName '%s'", e.key, e.key, e.scancode, e.scancode, e.keyName.c_str());
		if (e.keyName == "n" && (e.mods & RACK_MOD_MASK) == RACK_MOD_CTRL) {
			patchUtils::loadTemplateDialog();
			e.consume(this);
		}
		if (e.keyName == "q" && (e.mods & RACK_MOD_MASK) == RACK_MOD_CTRL) {
			APP->window->close();
			e.consume(this);
		}
		if (e.keyName == "o" && (e.mods & RACK_MOD_MASK) == RACK_MOD_CTRL) {
			patchUtils::loadDialog();
			e.consume(this);
		}
		if (e.keyName == "o" && (e.mods & RACK_MOD_MASK) == (RACK_MOD_CTRL | GLFW_MOD_SHIFT)) {
			patchUtils::revertDialog();
			e.consume(this);
		}
		if (e.keyName == "s" && (e.mods & RACK_MOD_MASK) == RACK_MOD_CTRL) {
			// NOTE: will do nothing if path is empty, intentionally
			patchUtils::saveDialog(APP->patch->path);
			e.consume(this);
		}
		if (e.keyName == "s" && (e.mods & RACK_MOD_MASK) == (RACK_MOD_CTRL | GLFW_MOD_SHIFT)) {
			patchUtils::saveAsDialog();
			e.consume(this);
		}
		if (e.keyName == "z" && (e.mods & RACK_MOD_MASK) == RACK_MOD_CTRL) {
			APP->history->undo();
			e.consume(this);
		}
		if (e.keyName == "z" && (e.mods & RACK_MOD_MASK) == (RACK_MOD_CTRL | GLFW_MOD_SHIFT)) {
			APP->history->redo();
			e.consume(this);
		}
		if (e.keyName == "-" && (e.mods & RACK_MOD_MASK) == RACK_MOD_CTRL) {
			float zoom = std::log2(APP->scene->rackScroll->getZoom());
			zoom *= 2;
			zoom = std::ceil(zoom - 0.01f) - 1;
			zoom /= 2;
			APP->scene->rackScroll->setZoom(std::pow(2.f, zoom));
			e.consume(this);
		}
		// Numpad has a "+" key, but the main keyboard section hides it under "="
		if ((e.keyName == "=" || e.keyName == "+") && (e.mods & RACK_MOD_MASK) == RACK_MOD_CTRL) {
			float zoom = std::log2(APP->scene->rackScroll->getZoom());
			zoom *= 2;
			zoom = std::floor(zoom + 0.01f) + 1;
			zoom /= 2;
			APP->scene->rackScroll->setZoom(std::pow(2.f, zoom));
			e.consume(this);
		}
		if ((e.keyName == "0" || e.keyName == "1") && (e.mods & RACK_MOD_MASK) == RACK_MOD_CTRL) {
			APP->scene->rackScroll->setZoom(1.f);
			e.consume(this);
		}
		if (e.keyName == "2" && (e.mods & RACK_MOD_MASK) == RACK_MOD_CTRL) {
			APP->scene->rackScroll->setZoom(2.f);
			e.consume(this);
		}
		if (e.key == GLFW_KEY_F1 && (e.mods & RACK_MOD_MASK) == 0) {
			system::openBrowser("https://vcvrack.com/manual/");
			e.consume(this);
		}
		if (e.key == GLFW_KEY_F3 && (e.mods & RACK_MOD_MASK) == 0) {
			settings::cpuMeter ^= true;
			e.consume(this);
		}
		if (e.key == GLFW_KEY_F7 && (e.mods & RACK_MOD_MASK) == 0) {
			patchUtils::deployToRemote();
			window::generateScreenshot();
			e.consume(this);
		}
		if (e.key == GLFW_KEY_F9 && (e.mods & RACK_MOD_MASK) == 0) {
			window::generateScreenshot();
			e.consume(this);
		}

		// Module selections
		if (e.keyName == "a" && (e.mods & RACK_MOD_MASK) == RACK_MOD_CTRL) {
			rack->selectAll();
			e.consume(this);
		}
		if (e.keyName == "a" && (e.mods & RACK_MOD_MASK) == (RACK_MOD_CTRL | GLFW_MOD_SHIFT)) {
			rack->deselectAll();
			e.consume(this);
		}
		if (e.keyName == "c" && (e.mods & RACK_MOD_MASK) == RACK_MOD_CTRL) {
			if (rack->hasSelection()) {
				rack->copyClipboardSelection();
				e.consume(this);
			}
		}
		if (e.keyName == "i" && (e.mods & RACK_MOD_MASK) == RACK_MOD_CTRL) {
			if (rack->hasSelection()) {
				rack->resetSelectionAction();
				e.consume(this);
			}
		}
		if (e.keyName == "r" && (e.mods & RACK_MOD_MASK) == RACK_MOD_CTRL) {
			if (rack->hasSelection()) {
				rack->randomizeSelectionAction();
				e.consume(this);
			}
		}
		if (e.keyName == "u" && (e.mods & RACK_MOD_MASK) == RACK_MOD_CTRL) {
			if (rack->hasSelection()) {
				rack->disconnectSelectionAction();
				e.consume(this);
			}
		}
		if (e.keyName == "e" && (e.mods & RACK_MOD_MASK) == RACK_MOD_CTRL) {
			if (rack->hasSelection()) {
				rack->bypassSelectionAction(!rack->isSelectionBypassed());
				e.consume(this);
			}
		}
		if (e.keyName == "d" && (e.mods & RACK_MOD_MASK) == RACK_MOD_CTRL) {
			if (rack->hasSelection()) {
				rack->cloneSelectionAction(false);
				e.consume(this);
			}
		}
		if (e.keyName == "d" && (e.mods & RACK_MOD_MASK) == (RACK_MOD_CTRL | GLFW_MOD_SHIFT)) {
			if (rack->hasSelection()) {
				rack->cloneSelectionAction(true);
				e.consume(this);
			}
		}
		if ((e.key == GLFW_KEY_DELETE || e.key == GLFW_KEY_BACKSPACE) && (e.mods & RACK_MOD_MASK) == 0) {
			if (rack->hasSelection()) {
				rack->deleteSelectionAction();
				e.consume(this);
			}
		}
	}

	// Scroll RackScrollWidget with arrow keys
	if (e.action == GLFW_PRESS || e.action == GLFW_RELEASE) {
		if (e.key == GLFW_KEY_LEFT) {
			internal->heldArrowKeys[0] = (e.action == GLFW_PRESS);
			e.consume(this);
		}
		if (e.key == GLFW_KEY_RIGHT) {
			internal->heldArrowKeys[1] = (e.action == GLFW_PRESS);
			e.consume(this);
		}
		if (e.key == GLFW_KEY_UP) {
			internal->heldArrowKeys[2] = (e.action == GLFW_PRESS);
			e.consume(this);
		}
		if (e.key == GLFW_KEY_DOWN) {
			internal->heldArrowKeys[3] = (e.action == GLFW_PRESS);
			e.consume(this);
		}
	}

	if (e.isConsumed())
		return;
	OpaqueWidget::onHoverKey(e);
	if (e.isConsumed())
		return;

	// Key commands that can be overridden by children
	if (e.action == GLFW_PRESS || e.action == GLFW_REPEAT) {
		if (e.keyName == "v" && (e.mods & RACK_MOD_MASK) == RACK_MOD_CTRL) {
			rack->pasteClipboardAction();
			e.consume(this);
		}
		if ((e.key == GLFW_KEY_ENTER || e.key == GLFW_KEY_KP_ENTER) && (e.mods & RACK_MOD_MASK) == 0) {
			browser->show();
			e.consume(this);
		}
	}
}


void Scene::onPathDrop(const PathDropEvent& e) {
	if (e.paths.size() >= 1) {
		const std::string& path = e.paths[0];
		std::string extension = system::getExtension(path);

		if (extension == ".vcv") {
			patchUtils::loadPathDialog(path);
			e.consume(this);
			return;
		}
		if (extension == ".vcvs") {
			APP->scene->rack->loadSelection(path);
			e.consume(this);
			return;
		}
	}

	OpaqueWidget::onPathDrop(e);
}


} // namespace app
} // namespace rack


namespace patchUtils {


bool connectToRemote() {
#ifdef HAVE_LIBLO
	rack::app::Scene::Internal* const internal = APP->scene->internal;

	if (internal->oscServer == nullptr) {
		const lo_server oscServer = lo_server_new_with_proto(nullptr, LO_UDP, nullptr);
		DISTRHO_SAFE_ASSERT_RETURN(oscServer != nullptr, false);
		lo_server_add_method(oscServer, "/resp", nullptr, rack::app::Scene::Internal::osc_handler, internal);
		internal->oscServer = oscServer;
	}

	const lo_address addr = lo_address_new_with_proto(LO_UDP, REMOTE_HOST, REMOTE_HOST_PORT);
	DISTRHO_SAFE_ASSERT_RETURN(addr != nullptr, false);
	lo_send(addr, "/hello", "");
	lo_address_free(addr);

	return true;
#else
	return false;
#endif
}


bool isRemoteConnected() {
#ifdef HAVE_LIBLO
	return APP->scene->internal->oscConnected;
#else
	return false;
#endif
}


bool isRemoteAutoDeployed() {
#ifdef HAVE_LIBLO
	return APP->scene->internal->oscAutoDeploy;
#else
	return false;
#endif
}


void setRemoteAutoDeploy(bool autoDeploy) {
#ifdef HAVE_LIBLO
	APP->scene->internal->oscAutoDeploy = autoDeploy;
#endif
}


void deployToRemote() {
#ifdef HAVE_LIBLO
	const lo_address addr = lo_address_new_with_proto(LO_UDP, REMOTE_HOST, REMOTE_HOST_PORT);
	DISTRHO_SAFE_ASSERT_RETURN(addr != nullptr,);

	APP->engine->prepareSave();
	APP->patch->saveAutosave();
	APP->patch->cleanAutosave();
	std::vector<uint8_t> data(rack::system::archiveDirectory(APP->patch->autosavePath, 1));

	if (const lo_blob blob = lo_blob_new(data.size(), data.data())) {
		lo_send(addr, "/load", "b", blob);
		lo_blob_free(blob);
	}

	lo_address_free(addr);
#endif
}


void sendScreenshotToRemote(const char* const screenshot) {
#ifdef HAVE_LIBLO
	const lo_address addr = lo_address_new_with_proto(LO_UDP, REMOTE_HOST, REMOTE_HOST_PORT);
	DISTRHO_SAFE_ASSERT_RETURN(addr != nullptr,);

	std::vector<uint8_t> data(d_getChunkFromBase64String(screenshot));

	if (const lo_blob blob = lo_blob_new(data.size(), data.data())) {
		lo_send(addr, "/screenshot", "b", blob);
		lo_blob_free(blob);
	}

	lo_address_free(addr);
#endif
}


} // namespace patchUtils
