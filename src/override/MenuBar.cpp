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
 * This file is an edited version of VCVRack's app/MenuBar.cpp
 * Copyright (C) 2016-2021 VCV.
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 */

#include <thread>
#include <utility>

#include <app/MenuBar.hpp>
#include <app/TipWindow.hpp>
#include <widget/OpaqueWidget.hpp>
#include <ui/Button.hpp>
#include <ui/MenuItem.hpp>
#include <ui/MenuSeparator.hpp>
#include <ui/SequentialLayout.hpp>
#include <ui/Slider.hpp>
#include <ui/TextField.hpp>
#include <ui/ProgressBar.hpp>
#include <ui/Label.hpp>
#include <engine/Engine.hpp>
#include <widget/FramebufferWidget.hpp>
#include <window/Window.hpp>
#include <asset.hpp>
#include <context.hpp>
#include <settings.hpp>
#include <helpers.hpp>
#include <system.hpp>
#include <plugin.hpp>
#include <patch.hpp>
#include <library.hpp>

#include "../CardinalCommon.hpp"
#include "../CardinalRemote.hpp"
#include "DistrhoPlugin.hpp"
#include "DistrhoStandaloneUtils.hpp"

#ifdef HAVE_LIBLO
# include <lo/lo.h>
#endif

void switchDarkMode(bool darkMode);

namespace rack {
namespace asset {
std::string patchesPath();
}
namespace engine {
void Engine_setRemoteDetails(Engine*, remoteUtils::RemoteDetails*);
}
namespace plugin {
void updateStaticPluginsDarkMode();
}

namespace app {
namespace menuBar {


struct MenuButton : ui::Button {
	void step() override {
		box.size.x = bndLabelWidth(APP->window->vg, -1, text.c_str()) + 1.0;
		Widget::step();
	}
	void draw(const DrawArgs& args) override {
		BNDwidgetState state = BND_DEFAULT;
		if (APP->event->hoveredWidget == this)
			state = BND_HOVER;
		if (APP->event->draggedWidget == this)
			state = BND_ACTIVE;
		bndMenuItem(args.vg, 0.0, 0.0, box.size.x, box.size.y, state, -1, text.c_str());
		Widget::draw(args);
	}
};


////////////////////
// File
////////////////////


struct FileButton : MenuButton {
	const bool isStandalone;
#if ! CARDINAL_VARIANT_MINI
	std::vector<std::string> demoPatches;
#endif

	FileButton(const bool standalone)
		: MenuButton(),  isStandalone(standalone)
	{
#if ! CARDINAL_VARIANT_MINI
		const std::string patchesDir = asset::patchesPath() + DISTRHO_OS_SEP_STR "examples";

		if (system::isDirectory(patchesDir))
		{
			demoPatches = system::getEntries(patchesDir);
			std::sort(demoPatches.begin(), demoPatches.end(), [](const std::string& a, const std::string& b){
				return string::lowercase(a) < string::lowercase(b);
			});
		}
#endif
	}

	void onAction(const ActionEvent& e) override {
		ui::Menu* menu = createMenu();
		menu->cornerFlags = BND_CORNER_TOP;
		menu->box.pos = getAbsoluteOffset(math::Vec(0, box.size.y));

#ifndef DISTRHO_OS_WASM
		const char* const NewShortcut = RACK_MOD_CTRL_NAME "+N";
#else
		const char* const NewShortcut = "";
#endif
		menu->addChild(createMenuItem("New", NewShortcut, []() {
			patchUtils::loadTemplateDialog();
		}));

#if ! CARDINAL_VARIANT_MINI
#ifndef DISTRHO_OS_WASM
		menu->addChild(createMenuItem("Open / Import...", RACK_MOD_CTRL_NAME "+O", []() {
			patchUtils::loadDialog();
		}));

		menu->addChild(createMenuItem("Save", RACK_MOD_CTRL_NAME "+S", []() {
			// NOTE: will do nothing if path is empty, intentionally
			patchUtils::saveDialog(APP->patch->path);
		}, APP->patch->path.empty()));

		menu->addChild(createMenuItem("Save as / Export...", RACK_MOD_CTRL_NAME "+Shift+S", []() {
			patchUtils::saveAsDialog();
		}));
#else
		menu->addChild(createMenuItem("Import patch...", RACK_MOD_CTRL_NAME "+O", []() {
			patchUtils::loadDialog();
		}));

		menu->addChild(createMenuItem("Import selection...", "", [=]() {
			patchUtils::loadSelectionDialog();
		}, false, true));

		menu->addChild(createMenuItem("Save and download compressed", RACK_MOD_CTRL_NAME "+Shift+S", []() {
			patchUtils::saveAsDialog();
		}));

		menu->addChild(createMenuItem("Save and download uncompressed", "", []() {
			patchUtils::saveAsDialogUncompressed();
		}));
#endif
#endif

		menu->addChild(createMenuItem("Revert", RACK_MOD_CTRL_NAME "+" RACK_MOD_SHIFT_NAME "+O", []() {
			patchUtils::revertDialog();
		}, APP->patch->path.empty()));

#if defined(HAVE_LIBLO) || CARDINAL_VARIANT_MINI
		menu->addChild(new ui::MenuSeparator);

		remoteUtils::RemoteDetails* const remoteDetails = remoteUtils::getRemote();

		if (remoteDetails != nullptr && remoteDetails->connected) {
			menu->addChild(createMenuItem("Deploy to MOD", "F7", [remoteDetails]() {
				remoteUtils::sendFullPatchToRemote(remoteDetails);
			}));

			menu->addChild(createCheckMenuItem("Auto deploy to MOD", "",
				[remoteDetails]() {return remoteDetails->autoDeploy;},
				[remoteDetails]() {
					remoteDetails->autoDeploy = !remoteDetails->autoDeploy;
					Engine_setRemoteDetails(APP->engine, remoteDetails->autoDeploy ? remoteDetails : nullptr);
				}
			));
		} else {
			menu->addChild(createMenuItem("Connect to MOD", "", []() {
				DISTRHO_SAFE_ASSERT(remoteUtils::connectToRemote());
			}));
		}
#endif

#if ! CARDINAL_VARIANT_MINI
#ifndef DISTRHO_OS_WASM
		menu->addChild(new ui::MenuSeparator);

		// Load selection
		menu->addChild(createMenuItem("Import selection...", "", [=]() {
			patchUtils::loadSelectionDialog();
		}, false, true));

		menu->addChild(createMenuItem("Export uncompressed json...", "", []() {
			patchUtils::saveAsDialogUncompressed();
		}));
#endif
#endif

#if ! CARDINAL_VARIANT_MINI
		if (!demoPatches.empty())
		{
			menu->addChild(new ui::MenuSeparator);

			menu->addChild(createSubmenuItem("Open Demo / Example project", "", [=](ui::Menu* const menu) {
				for (std::string path : demoPatches) {
					std::string label = system::getStem(path);

					for (size_t i=0, len=label.size(); i<len; ++i) {
						if (label[i] == '_')
							label[i] = ' ';
					}

					menu->addChild(createMenuItem(label, "", [path]() {
						patchUtils::loadPathDialog(path, true);
					}));
				}

				menu->addChild(new ui::MenuSeparator);

				menu->addChild(createMenuItem("Open PatchStorage.com for more patches", "", []() {
					patchUtils::openBrowser("https://patchstorage.com/platform/cardinal/");
				}));
			}));
		}
#endif

#ifndef DISTRHO_OS_WASM
		if (isStandalone) {
			menu->addChild(new ui::MenuSeparator);

			menu->addChild(createMenuItem("Quit", RACK_MOD_CTRL_NAME "+Q", []() {
				APP->window->close();
			}));
		}
#endif
	}
};


////////////////////
// Edit
////////////////////


struct EditButton : MenuButton {
	void onAction(const ActionEvent& e) override {
		ui::Menu* menu = createMenu();
		menu->cornerFlags = BND_CORNER_TOP;
		menu->box.pos = getAbsoluteOffset(math::Vec(0, box.size.y));

		struct UndoItem : ui::MenuItem {
			void step() override {
				text = "Undo " + APP->history->getUndoName();
				disabled = !APP->history->canUndo();
				MenuItem::step();
			}
			void onAction(const ActionEvent& e) override {
				APP->history->undo();
			}
		};
		menu->addChild(createMenuItem<UndoItem>("", RACK_MOD_CTRL_NAME "+Z"));

		struct RedoItem : ui::MenuItem {
			void step() override {
				text = "Redo " + APP->history->getRedoName();
				disabled = !APP->history->canRedo();
				MenuItem::step();
			}
			void onAction(const ActionEvent& e) override {
				APP->history->redo();
			}
		};
		menu->addChild(createMenuItem<RedoItem>("", RACK_MOD_CTRL_NAME "+" RACK_MOD_SHIFT_NAME "+Z"));

		menu->addChild(createMenuItem("Clear cables", "", [=]() {
			APP->patch->disconnectDialog();
		}));

		menu->addChild(new ui::MenuSeparator);

		patchUtils::appendSelectionContextMenu(menu);
	}
};


////////////////////
// View
////////////////////


struct ZoomQuantity : Quantity {
	void setValue(float value) override {
		APP->scene->rackScroll->setZoom(std::pow(2.f, value));
	}
	float getValue() override {
		return std::log2(APP->scene->rackScroll->getZoom());
	}
	float getMinValue() override {
		return -2.f;
	}
	float getMaxValue() override {
		return 2.f;
	}
	float getDefaultValue() override {
		return 0.0;
	}
	float getDisplayValue() override {
		return std::round(std::pow(2.f, getValue()) * 100);
	}
	void setDisplayValue(float displayValue) override {
		setValue(std::log2(displayValue / 100));
	}
	std::string getLabel() override {
		return "Zoom";
	}
	std::string getUnit() override {
		return "%";
	}
};
struct ZoomSlider : ui::Slider {
	ZoomSlider() {
		quantity = new ZoomQuantity;
	}
	~ZoomSlider() {
		delete quantity;
	}
};


struct CableOpacityQuantity : Quantity {
	void setValue(float value) override {
		settings::cableOpacity = math::clamp(value, getMinValue(), getMaxValue());
	}
	float getValue() override {
		return settings::cableOpacity;
	}
	float getDefaultValue() override {
		return 0.5;
	}
	float getDisplayValue() override {
		return getValue() * 100;
	}
	void setDisplayValue(float displayValue) override {
		setValue(displayValue / 100);
	}
	std::string getLabel() override {
		return "Cable opacity";
	}
	std::string getUnit() override {
		return "%";
	}
};
struct CableOpacitySlider : ui::Slider {
	CableOpacitySlider() {
		quantity = new CableOpacityQuantity;
	}
	~CableOpacitySlider() {
		delete quantity;
	}
};


struct CableTensionQuantity : Quantity {
	void setValue(float value) override {
		settings::cableTension = math::clamp(value, getMinValue(), getMaxValue());
	}
	float getValue() override {
		return settings::cableTension;
	}
	float getDefaultValue() override {
		return 0.75;
	}
	float getDisplayValue() override {
		return getValue() * 100;
	}
	void setDisplayValue(float displayValue) override {
		setValue(displayValue / 100);
	}
	std::string getLabel() override {
		return "Cable tension";
	}
	std::string getUnit() override {
		return "%";
	}
};
struct CableTensionSlider : ui::Slider {
	CableTensionSlider() {
		quantity = new CableTensionQuantity;
	}
	~CableTensionSlider() {
		delete quantity;
	}
};


struct RackBrightnessQuantity : Quantity {
	void setValue(float value) override {
		settings::rackBrightness = math::clamp(value, getMinValue(), getMaxValue());
	}
	float getValue() override {
		return settings::rackBrightness;
	}
	float getDefaultValue() override {
		return 1.0;
	}
	float getDisplayValue() override {
		return getValue() * 100;
	}
	void setDisplayValue(float displayValue) override {
		setValue(displayValue / 100);
	}
	std::string getUnit() override {
		return "%";
	}
	std::string getLabel() override {
		return "Room brightness";
	}
	int getDisplayPrecision() override {
		return 3;
	}
};
struct RackBrightnessSlider : ui::Slider {
	RackBrightnessSlider() {
		quantity = new RackBrightnessQuantity;
	}
	~RackBrightnessSlider() {
		delete quantity;
	}
};


struct HaloBrightnessQuantity : Quantity {
	void setValue(float value) override {
		settings::haloBrightness = math::clamp(value, getMinValue(), getMaxValue());
	}
	float getValue() override {
		return settings::haloBrightness;
	}
	float getDefaultValue() override {
		return 0.25;
	}
	float getDisplayValue() override {
		return getValue() * 100;
	}
	void setDisplayValue(float displayValue) override {
		setValue(displayValue / 100);
	}
	std::string getUnit() override {
		return "%";
	}
	std::string getLabel() override {
		return "Light bloom";
	}
	int getDisplayPrecision() override {
		return 3;
	}
};
struct HaloBrightnessSlider : ui::Slider {
	HaloBrightnessSlider() {
		quantity = new HaloBrightnessQuantity;
	}
	~HaloBrightnessSlider() {
		delete quantity;
	}
};


struct KnobScrollSensitivityQuantity : Quantity {
	void setValue(float value) override {
		value = math::clamp(value, getMinValue(), getMaxValue());
		settings::knobScrollSensitivity = std::pow(2.f, value);
	}
	float getValue() override {
		return std::log2(settings::knobScrollSensitivity);
	}
	float getMinValue() override {
		return std::log2(1e-4f);
	}
	float getMaxValue() override {
		return std::log2(1e-2f);
	}
	float getDefaultValue() override {
		return std::log2(1e-3f);
	}
	float getDisplayValue() override {
		return std::pow(2.f, getValue() - getDefaultValue());
	}
	void setDisplayValue(float displayValue) override {
		setValue(std::log2(displayValue) + getDefaultValue());
	}
	std::string getLabel() override {
		return "Scroll wheel knob sensitivity";
	}
	int getDisplayPrecision() override {
		return 2;
	}
};
struct KnobScrollSensitivitySlider : ui::Slider {
	KnobScrollSensitivitySlider() {
		quantity = new KnobScrollSensitivityQuantity;
	}
	~KnobScrollSensitivitySlider() {
		delete quantity;
	}
};


static void setAllFramebufferWidgetsDirty(widget::Widget* const widget)
{
	for (widget::Widget* child : widget->children)
	{
		if (widget::FramebufferWidget* const fbw = dynamic_cast<widget::FramebufferWidget*>(child))
		{
			fbw->setDirty();
			break;
		}
		setAllFramebufferWidgetsDirty(child);
	}
}


struct ViewButton : MenuButton {
	void onAction(const ActionEvent& e) override {
		ui::Menu* menu = createMenu();
		menu->cornerFlags = BND_CORNER_TOP;
		menu->box.pos = getAbsoluteOffset(math::Vec(0, box.size.y));

		menu->addChild(createMenuLabel("Appearance"));

		std::string darkModeText;
		if (settings::darkMode)
			darkModeText = CHECKMARK_STRING;
		menu->addChild(createMenuItem("Dark Mode", darkModeText, []() {
			switchDarkMode(!settings::darkMode);
			plugin::updateStaticPluginsDarkMode();
			setAllFramebufferWidgetsDirty(APP->scene);
		}));

		menu->addChild(createBoolPtrMenuItem("Show tooltips", "", &settings::tooltips));

		ZoomSlider* zoomSlider = new ZoomSlider;
		zoomSlider->box.size.x = 250.0;
		menu->addChild(zoomSlider);

		CableOpacitySlider* cableOpacitySlider = new CableOpacitySlider;
		cableOpacitySlider->box.size.x = 250.0;
		menu->addChild(cableOpacitySlider);

		CableTensionSlider* cableTensionSlider = new CableTensionSlider;
		cableTensionSlider->box.size.x = 250.0;
		menu->addChild(cableTensionSlider);

		RackBrightnessSlider* rackBrightnessSlider = new RackBrightnessSlider;
		rackBrightnessSlider->box.size.x = 250.0;
		menu->addChild(rackBrightnessSlider);

		HaloBrightnessSlider* haloBrightnessSlider = new HaloBrightnessSlider;
		haloBrightnessSlider->box.size.x = 250.0;
		menu->addChild(haloBrightnessSlider);

		menu->addChild(new ui::MenuSeparator);
		menu->addChild(createMenuLabel("Module dragging"));

		menu->addChild(createBoolPtrMenuItem("Lock module positions", "", &settings::lockModules));

		menu->addChild(createBoolPtrMenuItem("Auto-squeeze modules when dragging", "", &settings::squeezeModules));

		menu->addChild(new ui::MenuSeparator);
		menu->addChild(createMenuLabel("Parameters"));

#ifdef DISTRHO_OS_WASM
		menu->addChild(createBoolPtrMenuItem("Lock cursor while dragging", "", &settings::allowCursorLock));
#endif

		static const std::vector<std::string> knobModeLabels = {
			"Linear",
			"Scaled linear",
			"Absolute rotary",
			"Relative rotary",
		};
		static const std::vector<int> knobModes = {0, 2, 3};
		menu->addChild(createSubmenuItem("Knob mode", knobModeLabels[settings::knobMode], [=](ui::Menu* menu) {
			for (int knobMode : knobModes) {
				menu->addChild(createCheckMenuItem(knobModeLabels[knobMode], "",
					[=]() {return settings::knobMode == knobMode;},
					[=]() {settings::knobMode = (settings::KnobMode) knobMode;}
				));
			}
		}));

		menu->addChild(createBoolPtrMenuItem("Scroll wheel knob control", "", &settings::knobScroll));

		KnobScrollSensitivitySlider* knobScrollSensitivitySlider = new KnobScrollSensitivitySlider;
		knobScrollSensitivitySlider->box.size.x = 250.0;
		menu->addChild(knobScrollSensitivitySlider);

		menu->addChild(new ui::MenuSeparator);
		menu->addChild(createMenuLabel("Window"));

#ifdef DISTRHO_OS_WASM
		const bool fullscreen = APP->window->isFullScreen();
		std::string rightText = "F11";
		if (fullscreen)
			rightText += " " CHECKMARK_STRING;
		menu->addChild(createMenuItem("Fullscreen", rightText, [=]() {
			APP->window->setFullScreen(!fullscreen);
		}));
#endif

		menu->addChild(createBoolPtrMenuItem("Invert zoom", "", &settings::invertZoom));

		static const std::vector<std::string> rateLimitLabels = {
			"None",
			"2x",
			"4x",
		};
		static const std::vector<int> rateLimits = {0, 1, 2};
		menu->addChild(createSubmenuItem("Update rate limit", rateLimitLabels[settings::rateLimit], [=](ui::Menu* menu) {
			for (int rateLimit : rateLimits) {
				menu->addChild(createCheckMenuItem(rateLimitLabels[rateLimit], "",
					[=]() {return settings::rateLimit == rateLimit;},
					[=]() {settings::rateLimit = rateLimit;}
				));
			}
		}));
	}
};


////////////////////
// Engine
////////////////////


struct EngineButton : MenuButton {
	void onAction(const ActionEvent& e) override {
		ui::Menu* menu = createMenu();
		menu->cornerFlags = BND_CORNER_TOP;
		menu->box.pos = getAbsoluteOffset(math::Vec(0, box.size.y));

		std::string cpuMeterText = "F3";
		if (settings::cpuMeter)
			cpuMeterText += " " CHECKMARK_STRING;
		menu->addChild(createMenuItem("Performance meters", cpuMeterText, [=]() {
			settings::cpuMeter ^= true;
		}));

		if (isUsingNativeAudio()) {
			if (supportsAudioInput()) {
				const bool enabled = isAudioInputEnabled();
				std::string rightText;
				if (enabled)
					rightText = CHECKMARK_STRING;
				menu->addChild(createMenuItem("Enable Audio Input", rightText, [enabled]() {
					if (!enabled)
						requestAudioInput();
				}));
			}

			if (supportsMIDI()) {
				std::string rightText;
				if (isMIDIEnabled())
					rightText = CHECKMARK_STRING;
				menu->addChild(createMenuItem("Enable/Reconnect MIDI", rightText, []() {
					requestMIDI();
				}));
			}

			if (supportsBufferSizeChanges()) {
				static const std::vector<uint32_t> bufferSizes = {
					#ifdef DISTRHO_OS_WASM
					256, 512, 1024, 2048, 4096, 8192, 16384
					#else
					128, 256, 512, 1024, 2048, 4096, 8192
					#endif
				};
				const uint32_t currentBufferSize = getBufferSize();
				menu->addChild(createSubmenuItem("Buffer Size", std::to_string(currentBufferSize), [=](ui::Menu* menu) {
					for (uint32_t bufferSize : bufferSizes) {
						menu->addChild(createCheckMenuItem(std::to_string(bufferSize), "",
							[=]() {return currentBufferSize == bufferSize;},
							[=]() {requestBufferSizeChange(bufferSize);}
						));
					}
				}));
			}
		}
	}
};


////////////////////
// Help
////////////////////


struct HelpButton : MenuButton {
	void onAction(const ActionEvent& e) override {
		ui::Menu* menu = createMenu();
		menu->cornerFlags = BND_CORNER_TOP;
		menu->box.pos = getAbsoluteOffset(math::Vec(0, box.size.y));

		menu->addChild(createMenuItem("Rack User manual", "F1", [=]() {
			patchUtils::openBrowser("https://vcvrack.com/manual");
		}));

		menu->addChild(createMenuItem("Cardinal Project page", "", [=]() {
			patchUtils::openBrowser("https://github.com/DISTRHO/Cardinal/");
		}));

		menu->addChild(new ui::MenuSeparator);

		menu->addChild(createMenuLabel("Cardinal " + APP_EDITION + " " + CARDINAL_VERSION));
		menu->addChild(createMenuLabel("Rack " + APP_VERSION + " Compatible"));
	}
};


////////////////////
// MenuBar
////////////////////


struct MeterLabel : ui::Label {
	int frameIndex = 0;
	double frameDurationTotal = 0.0;
	double frameDurationAvg = 0.0;
	double uiLastTime = 0.0;
	double uiLastThreadTime = 0.0;
	double uiFrac = 0.0;

	void step() override {
		// Compute frame rate
		double frameDuration = APP->window->getLastFrameDuration();
		frameDurationTotal += frameDuration;
		frameIndex++;
		if (frameDurationTotal >= 1.0) {
			frameDurationAvg = frameDurationTotal / frameIndex;
			frameDurationTotal = 0.0;
			frameIndex = 0;
		}

		// Compute UI thread CPU
		// double time = system::getTime();
		// double uiDuration = time - uiLastTime;
		// if (uiDuration >= 1.0) {
		// 	double threadTime = system::getThreadTime();
		// 	uiFrac = (threadTime - uiLastThreadTime) / uiDuration;
		// 	uiLastThreadTime = threadTime;
		// 	uiLastTime = time;
		// }

#if CARDINAL_VARIANT_MINI
		text = string::f("%.1f fps", 1.0 / frameDurationAvg);
#else
		double meterAverage = APP->engine->getMeterAverage();
		double meterMax = APP->engine->getMeterMax();
		text = string::f("%.1f fps  %.1f%% avg  %.1f%% max", 1.0 / frameDurationAvg, meterAverage * 100, meterMax * 100);
#endif

		Label::step();
	}
};


struct MenuBar : widget::OpaqueWidget {
	MeterLabel* meterLabel;

	MenuBar(const bool isStandalone)
		: widget::OpaqueWidget()
	{
		const float margin = 5;
		box.size.y = BND_WIDGET_HEIGHT + 2 * margin;

		ui::SequentialLayout* layout = new ui::SequentialLayout;
		layout->margin = math::Vec(margin, margin);
		layout->spacing = math::Vec(0, 0);
		addChild(layout);

		FileButton* fileButton = new FileButton(isStandalone);
		fileButton->text = "File";
		layout->addChild(fileButton);

		EditButton* editButton = new EditButton;
		editButton->text = "Edit";
		layout->addChild(editButton);

		ViewButton* viewButton = new ViewButton;
		viewButton->text = "View";
		layout->addChild(viewButton);

#if ! CARDINAL_VARIANT_MINI
		EngineButton* engineButton = new EngineButton;
		engineButton->text = "Engine";
		layout->addChild(engineButton);
#endif

		HelpButton* helpButton = new HelpButton;
		helpButton->text = "Help";
		layout->addChild(helpButton);

		// ui::Label* titleLabel = new ui::Label;
		// titleLabel->color.a = 0.5;
		// layout->addChild(titleLabel);

		meterLabel = new MeterLabel;
		meterLabel->box.pos.y = margin;
		meterLabel->box.size.x = 300;
		meterLabel->alignment = ui::Label::RIGHT_ALIGNMENT;
		meterLabel->color.a = 0.5;
		addChild(meterLabel);
	}

	void draw(const DrawArgs& args) override {
		bndMenuBackground(args.vg, 0.0, 0.0, box.size.x, box.size.y, BND_CORNER_ALL);
		bndBevel(args.vg, 0.0, 0.0, box.size.x, box.size.y);

		Widget::draw(args);
	}

	void step() override {
		meterLabel->box.pos.x = box.size.x - meterLabel->box.size.x - 5;
		Widget::step();
	}
};


} // namespace menuBar


widget::Widget* createMenuBar() {
	menuBar::MenuBar* menuBar = new menuBar::MenuBar(isStandalone());
	return menuBar;
}


} // namespace app
} // namespace rack
