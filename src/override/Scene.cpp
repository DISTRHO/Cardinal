/*
 * DISTRHO Cardinal Plugin
 * Copyright (C) 2021-2023 Filipe Coelho <falktx@falktx.com>
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
 * Copyright (C) 2016-2023 VCV.
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 */

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

#include "../CardinalCommon.hpp"
#include "../CardinalRemote.hpp"

#include <algorithm>

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
		glfwSetCursor(APP->window->win, glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR));
	}

	void onLeave(const LeaveEvent& e) override {
		glfwSetCursor(APP->window->win, nullptr);
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
	ResizeHandle* resizeHandle = nullptr;

	bool heldArrowKeys[4] = {};

	double lastSceneChangeTime = 0.0;
	int historyActionIndex = -1;
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

	if (isStandalone() || isMini())
		return;

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

	if (internal->resizeHandle != nullptr)
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

	if (remoteUtils::RemoteDetails* const remoteDetails = remoteUtils::getRemote()) {
		idleRemote(remoteDetails);

		if (remoteDetails->autoDeploy) {
			const int actionIndex = APP->history->actionIndex;
			const double time = system::getTime();

			if (internal->historyActionIndex == -1) {
				internal->historyActionIndex = actionIndex;
				internal->lastSceneChangeTime = time;
			} else if (internal->historyActionIndex != actionIndex && actionIndex > 0 && time - internal->lastSceneChangeTime >= 1.0) {
				const std::string& name(APP->history->actions[actionIndex - 1]->name);
				static const std::vector<std::string> ignoredNames = {
					"move knob",
					"move modules",
					"move switch",
				};
				if (std::find(ignoredNames.cbegin(), ignoredNames.cend(), name) == ignoredNames.cend()) {
					printf("action '%s'\n", APP->history->actions[actionIndex - 1]->name.c_str());
					remoteUtils::sendFullPatchToRemote(remoteDetails);
					window::generateScreenshot();
				}
				internal->historyActionIndex = actionIndex;
				internal->lastSceneChangeTime = time;
			}
		}
	}

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
			patchUtils::loadTemplateDialog(false);
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
#ifndef DISTRHO_OS_WASM
		if (e.keyName == "s" && (e.mods & RACK_MOD_MASK) == RACK_MOD_CTRL) {
			// NOTE: will do nothing if path is empty, intentionally
			patchUtils::saveDialog(APP->patch->path);
			e.consume(this);
		}
#endif
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
			patchUtils::openBrowser("https://vcvrack.com/manual/");
			e.consume(this);
		}
		if (e.key == GLFW_KEY_F3 && (e.mods & RACK_MOD_MASK) == 0) {
			settings::cpuMeter ^= true;
			e.consume(this);
		}
		if (e.key == GLFW_KEY_F7 && (e.mods & RACK_MOD_MASK) == 0) {
			if (remoteUtils::RemoteDetails* const remoteDetails = remoteUtils::getRemote())
			{
				remoteUtils::sendFullPatchToRemote(remoteDetails);
				window::generateScreenshot();
			}
			e.consume(this);
		}
		if (e.key == GLFW_KEY_F9 && (e.mods & RACK_MOD_MASK) == 0) {
			window::generateScreenshot();
			e.consume(this);
		}
#ifdef DISTRHO_OS_WASM
		if (e.key == GLFW_KEY_F11 && (e.mods & RACK_MOD_MASK) == 0) {
			APP->window->setFullScreen(!APP->window->isFullScreen());
			// The MenuBar will be hidden when the mouse moves over the RackScrollWidget.
			// menuBar->hide();
			e.consume(this);
		}
#endif

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
