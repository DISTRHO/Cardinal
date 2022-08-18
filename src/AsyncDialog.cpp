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

#include "AsyncDialog.hpp"

#include <context.hpp>
#include <app/Scene.hpp>
#include <ui/Button.hpp>
#include <ui/Label.hpp>
#include <ui/MenuOverlay.hpp>
#include <ui/SequentialLayout.hpp>
#include <ui/TextField.hpp>
#include <widget/OpaqueWidget.hpp>

namespace asyncDialog
{

using namespace rack;
using namespace rack::ui;
using namespace rack::widget;

struct AsyncDialog : OpaqueWidget
{
	static const constexpr float margin = 10;
	static const constexpr float buttonWidth = 100;

	SequentialLayout* layout;
	SequentialLayout* contentLayout;
	SequentialLayout* buttonLayout;
	Label* label;

	AsyncDialog(const char* const message)
	{
		setup(message);

		struct AsyncDismissButton : Button {
			AsyncDialog* dialog;
			void onAction(const ActionEvent& e) override {
				dialog->getParent()->requestDelete();
			}
		};
		AsyncDismissButton* const dismissButton = new AsyncDismissButton;
		dismissButton->box.size.x = buttonWidth;
		dismissButton->text = "Dismiss";
		dismissButton->dialog = this;
		buttonLayout->addChild(dismissButton);
	}

	AsyncDialog(const char* const message, const std::function<void()> action)
	{
		setup(message);

		struct AsyncCancelButton : Button {
			AsyncDialog* dialog;
			void onAction(const ActionEvent& e) override {
				dialog->getParent()->requestDelete();
			}
		};
		AsyncCancelButton* const cancelButton = new AsyncCancelButton;
		cancelButton->box.size.x = buttonWidth;
		cancelButton->text = "Cancel";
		cancelButton->dialog = this;
		buttonLayout->addChild(cancelButton);

		struct AsyncOkButton : Button {
			AsyncDialog* dialog;
			std::function<void()> action;
			void onAction(const ActionEvent& e) override {
				action();
				dialog->getParent()->requestDelete();
			}
		};
		AsyncOkButton* const okButton = new AsyncOkButton;
		okButton->box.size.x = buttonWidth;
		okButton->text = "Ok";
		okButton->dialog = this;
		okButton->action = action;
		buttonLayout->addChild(okButton);
	}

	void setup(const char* const message)
	{
		box.size = math::Vec(400, 120);

		layout = new SequentialLayout;
		layout->box.pos = math::Vec(0, 0);
		layout->box.size = box.size;
		layout->orientation = SequentialLayout::VERTICAL_ORIENTATION;
		layout->margin = math::Vec(margin, margin);
		layout->spacing = math::Vec(margin, margin);
		layout->wrap = false;
		addChild(layout);

		contentLayout = new SequentialLayout;
		contentLayout->spacing = math::Vec(margin, margin);
		layout->addChild(contentLayout);

		buttonLayout = new SequentialLayout;
		buttonLayout->alignment = SequentialLayout::CENTER_ALIGNMENT;
		buttonLayout->box.size = box.size;
		buttonLayout->spacing = math::Vec(margin, margin);
		layout->addChild(buttonLayout);

		label = new Label;
		label->box.size.x = box.size.x - 2*margin;
		label->box.size.y = box.size.y - 2*margin - 40;
		label->fontSize = 16;
		label->text = message;
		contentLayout->addChild(label);
	}

	void step() override
    {
		OpaqueWidget::step();

		box.pos = parent->box.size.minus(box.size).div(2).round();
	}

	void draw(const DrawArgs& args) override
	{
		bndMenuBackground(args.vg, 0.0, 0.0, box.size.x, box.size.y, 0);
		Widget::draw(args);
	}
};

void create(const char* const message)
{
	MenuOverlay* const overlay = new MenuOverlay;
	overlay->bgColor = nvgRGBAf(0, 0, 0, 0.33);

	AsyncDialog* const dialog = new AsyncDialog(message);
	overlay->addChild(dialog);

	APP->scene->addChild(overlay);
}

void create(const char* const message, const std::function<void()> action)
{
	MenuOverlay* const overlay = new MenuOverlay;
	overlay->bgColor = nvgRGBAf(0, 0, 0, 0.33);

	AsyncDialog* const dialog = new AsyncDialog(message, action);
	overlay->addChild(dialog);

	APP->scene->addChild(overlay);
}

struct AsyncTextInput : OpaqueWidget
{
	static const constexpr float margin = 10;
	static const constexpr float buttonWidth = 100;

	AsyncTextInput(const char* const message, const char* const text, const std::function<void(char* newText)> action)
	{
		box.size = math::Vec(400, 80);

		SequentialLayout* const layout = new SequentialLayout;
		layout->box.pos = math::Vec(0, 0);
		layout->box.size = box.size;
		layout->orientation = SequentialLayout::VERTICAL_ORIENTATION;
		layout->margin = math::Vec(margin, margin);
		layout->spacing = math::Vec(margin, margin);
		layout->wrap = false;
		addChild(layout);

		SequentialLayout* const contentLayout = new SequentialLayout;
		contentLayout->box.size.x = box.size.x - 2*margin;
		contentLayout->box.size.y = box.size.y / 2 - margin;
		contentLayout->spacing = math::Vec(margin, margin);
		layout->addChild(contentLayout);

		SequentialLayout* const buttonLayout = new SequentialLayout;
		buttonLayout->alignment = SequentialLayout::CENTER_ALIGNMENT;
		buttonLayout->box.size.x = box.size.x - 2*margin;
		buttonLayout->box.size.y = box.size.y / 2 - margin;
		buttonLayout->spacing = math::Vec(margin, margin);
		layout->addChild(buttonLayout);

		Label* label;
		if (message != nullptr)
		{
			label = new Label;
			nvgFontSize(APP->window->vg, 14);
			label->box.size.x = std::min(bndLabelWidth(APP->window->vg, -1, message) + margin,
			                             box.size.x / 2 - margin);
			label->box.size.y = contentLayout->box.size.y;
			label->fontSize = 14;
			label->text = message;
			contentLayout->addChild(label);
		}
		else
		{
			label = nullptr;
		}

		struct AsyncTextField : TextField {
			AsyncTextInput* dialog;
			std::function<void(char*)> action;
			void onSelectKey(const SelectKeyEvent& e) override {
				if (e.key == GLFW_KEY_ENTER || e.key == GLFW_KEY_KP_ENTER)
				{
					e.consume(this);
					action(strdup(text.c_str()));
					dialog->getParent()->requestDelete();
					return;
				}
				TextField::onSelectKey(e);
			}
		};
		AsyncTextField* const textField = new AsyncTextField;
		textField->box.size.x = contentLayout->box.size.x - (label != nullptr ? label->box.size.x + margin : 0);
		textField->box.size.y = 24;
		textField->dialog = this;
		textField->action = action;
		if (text != nullptr)
			textField->text = text;
		contentLayout->addChild(textField);

		struct AsyncCancelButton : Button {
			AsyncTextInput* dialog;
			void onAction(const ActionEvent& e) override {
				dialog->getParent()->requestDelete();
			}
		};
		AsyncCancelButton* const cancelButton = new AsyncCancelButton;
		cancelButton->box.size.x = buttonWidth;
		cancelButton->text = "Cancel";
		cancelButton->dialog = this;
		buttonLayout->addChild(cancelButton);

		struct AsyncOkButton : Button {
			AsyncTextInput* dialog;
			TextField* textField;
			std::function<void(char*)> action;
			void onAction(const ActionEvent& e) override {
				action(strdup(textField->text.c_str()));
				dialog->getParent()->requestDelete();
			}
		};
		AsyncOkButton* const okButton = new AsyncOkButton;
		okButton->box.size.x = buttonWidth;
		okButton->text = "Ok";
		okButton->dialog = this;
		okButton->textField = textField;
		okButton->action = action;
		buttonLayout->addChild(okButton);
	}

	void step() override
	{
		OpaqueWidget::step();
		box.pos = parent->box.size.minus(box.size).div(2).round();
	}

	void draw(const DrawArgs& args) override
	{
		bndMenuBackground(args.vg, 0.0, 0.0, box.size.x, box.size.y, 0);
		Widget::draw(args);
	}
};

void textInput(const char* const message, const char* const text, std::function<void(char* newText)> action)
{
	MenuOverlay* const overlay = new MenuOverlay;
	overlay->bgColor = nvgRGBAf(0, 0, 0, 0.33);

	AsyncTextInput* const dialog = new AsyncTextInput(message, text, action);
	overlay->addChild(dialog);

	APP->scene->addChild(overlay);
}

}
