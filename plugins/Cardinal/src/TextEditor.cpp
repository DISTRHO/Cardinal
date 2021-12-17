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

#include "plugincontext.hpp"

#ifndef HEADLESS
# include "ImGuiTextEditor.hpp"
# include "extra/FileBrowserDialog.hpp"
#endif

#include <fstream>

// --------------------------------------------------------------------------------------------------------------------

struct TextEditorModule : Module {
    enum ParamIds {
        NUM_PARAMS
    };
    enum InputIds {
        NUM_INPUTS
    };
    enum OutputIds {
        NUM_OUTPUTS
    };
    enum LightIds {
        NUM_LIGHTS
    };

    TextEditorModule()
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    }

    ~TextEditorModule() override
    {
    }

    void process(const ProcessArgs&) override
    {
    }

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TextEditorModule)
};

// --------------------------------------------------------------------------------------------------------------------

#ifndef HEADLESS
struct TextEditorWidget : ImGuiTextEditor
{
    TextEditorWidget() : ImGuiTextEditor() {}

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TextEditorWidget)
};

// --------------------------------------------------------------------------------------------------------------------

struct TextEditorLoadFileItem : MenuItem {
    TextEditorWidget* widget = nullptr;

    void onAction(const event::Action &e) override
    {
        WeakPtr<TextEditorWidget> widget = this->widget;
        async_dialog_filebrowser(false, nullptr, "Load File", [widget](char* path)
        {
            if (path)
            {
                if (widget)
                {
                    std::ifstream f(path);
                    if (f.good())
                    {
                        const std::string str((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
                        widget->setText(str);
                    }
                }
                free(path);
            }
        });
    }
};

// --------------------------------------------------------------------------------------------------------------------

struct TextEditorModuleWidget : ModuleWidget {
    TextEditorWidget* textEditorWidget = nullptr;

    TextEditorModuleWidget(TextEditorModule* const module)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Ildaeil.svg")));

        if (module != nullptr)
        {
            textEditorWidget = new TextEditorWidget();
            textEditorWidget->box.pos = Vec(2 * RACK_GRID_WIDTH, 0);
            textEditorWidget->box.size = Vec(box.size.x - 2 * RACK_GRID_WIDTH, box.size.y);
            addChild(textEditorWidget);
        }

        addChild(createWidget<ScrewBlack>(Vec(0, 0)));
        addChild(createWidget<ScrewBlack>(Vec(0, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    }

    void appendContextMenu(Menu *menu) override {
        menu->addChild(new MenuEntry);

        TextEditorLoadFileItem* loadFileItem = new TextEditorLoadFileItem;
        loadFileItem->text = "Load File";
        loadFileItem->widget = textEditorWidget;
        menu->addChild(loadFileItem);
    }

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TextEditorModuleWidget)
};
#else
typedef ModuleWidget TextEditorModuleWidget;
#endif

// --------------------------------------------------------------------------------------------------------------------

Model* modelTextEditor = createModel<TextEditorModule, TextEditorModuleWidget>("TextEditor");

// --------------------------------------------------------------------------------------------------------------------
