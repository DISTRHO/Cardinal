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

#include "plugincontext.hpp"

#ifndef HEADLESS
# include "ImGuiTextEditor.hpp"
# include "extra/FileBrowserDialog.hpp"
#endif

#include <fstream>

// --------------------------------------------------------------------------------------------------------------------

// defaults (artwork based on content from https://www.asciiart.eu/animals/cats)
#define DEFAULT_LANG "None"
#define DEFAULT_TEXT ""                                     \
"                                                   \n"     \
"        ^ ^        ,_,                             \n"     \
"       (O,O)      (.,.)                            \n"     \
"       (   )      (   )                            \n"     \
"--------\"-\"---dwb--\"-\"---dwb-                      \n" \
"                                                   \n"     \
"                                                   \n"     \
"                                                   \n"     \
"                                                   \n"     \
"                                                   \n"     \
"                                                   \n"     \
"                                                   \n"     \
"      /^--^\\     /^--^\\     /^--^\\                 \n"  \
"      \\____/     \\____/     \\____/                 \n"  \
"     /      \\   /      \\   /      \\                \n"  \
"    |        | |        | |        |               \n"     \
"     \\__  __/   \\__  __/   \\__  __/                \n"  \
"|^|^|^|^\\ \\^|^|^|^/ /^|^|^|^|^\\ \\^|^|^|^|^|^|^|^|^|\n" \
"| | | | |\\ \\| | |/ /| | | | | |\\ \\| | | | | | | | |\n" \
"#########/ /#####\\ \\###########/ /#################\n"   \
"| | | | |\\/ | | | \\/| | | | | |\\/ | | | | | | | | |\n"  \
"|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|\n"
#define DEFAULT_WIDTH 30

struct TextEditorModule : Module {
    std::string file;
    std::string lang = DEFAULT_LANG;
    std::string text = DEFAULT_TEXT;
    int width = DEFAULT_WIDTH;
#ifndef HEADLESS
    WeakPtr<ImGuiTextEditor> widgetPtr;
#endif

    json_t* dataToJson() override
    {
        json_t* const rootJ = json_object();
        DISTRHO_SAFE_ASSERT_RETURN(rootJ != nullptr, nullptr);

#ifndef HEADLESS
        if (ImGuiTextEditor* const widget = widgetPtr)
            text = widget->getText();
#endif

        json_object_set_new(rootJ, "filepath", json_string(file.c_str()));
        json_object_set_new(rootJ, "lang", json_string(lang.c_str()));
        json_object_set_new(rootJ, "etext", json_string(text.c_str()));
        json_object_set_new(rootJ, "width", json_integer(width));
        return rootJ;
    }

    void dataFromJson(json_t* const rootJ) override
    {
        file.clear();

        // Rack Core Notes compatiblity
        if (json_t* const textJ = json_object_get(rootJ, "text"))
        {
            text = json_string_value(textJ);
            lang = "None";
            width = 16;
#ifndef HEADLESS
            if (ImGuiTextEditor* const widget = widgetPtr)
            {
                widget->setLanguageDefinition(lang);
                widget->setText(text);
            }
#endif
            return;
        }

        if (json_t* const widthJ = json_object_get(rootJ, "width"))
            width = json_integer_value(widthJ);

        if (json_t* const langJ = json_object_get(rootJ, "lang"))
        {
            lang = json_string_value(langJ);
#ifndef HEADLESS
            if (ImGuiTextEditor* const widget = widgetPtr)
                widget->setLanguageDefinition(lang);
#endif
        }

        if (json_t* const filepathJ = json_object_get(rootJ, "filepath"))
        {
            const char* const filepath = json_string_value(filepathJ);

            if (filepath[0] != '\0')
            {
                std::ifstream f(filepath);

                if (f.good())
                {
                    file = filepath;
                    text = std::string((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
#ifndef HEADLESS
                    if (ImGuiTextEditor* const widget = widgetPtr)
                        widget->setFileWithKnownText(file, text);
#endif
                    return;
                }
            }
        }

        if (json_t* const textJ = json_object_get(rootJ, "etext"))
        {
            text = json_string_value(textJ);
#ifndef HEADLESS
            if (ImGuiTextEditor* const widget = widgetPtr)
                widget->setText(text);
#endif
        }
    }

    bool loadFileFromMenuAction(const char* const filepath)
    {
        std::ifstream f(filepath);

        if (f.good())
        {
            file = filepath;
            text = std::string((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
            return true;
        }

        return false;
    }
};

#ifndef HEADLESS
// --------------------------------------------------------------------------------------------------------------------

struct TextEditorLangSelectItem : MenuItem {
    TextEditorModule* const module;
    ImGuiTextEditor* const widget;

    TextEditorLangSelectItem(TextEditorModule* const textEditorModule,
                             ImGuiTextEditor* const textEditorWidget,
                             const char* const lang)
        : module(textEditorModule),
          widget(textEditorWidget)
    {
        text = lang;

        if (module->lang == lang)
            rightText = CHECKMARK_STRING;
    }

    void onAction(const event::Action&) override
    {
        module->lang = text;
        widget->setLanguageDefinition(text);
    }
};

struct TextEditorLangSelectMenu : Menu {
    TextEditorLangSelectMenu(TextEditorModule* const module, ImGuiTextEditor* const widget)
    {
        addChild(new TextEditorLangSelectItem(module, widget, "None"));
        addChild(new TextEditorLangSelectItem(module, widget, "AngelScript"));
        addChild(new TextEditorLangSelectItem(module, widget, "C"));
        addChild(new TextEditorLangSelectItem(module, widget, "C++"));
        addChild(new TextEditorLangSelectItem(module, widget, "GLSL"));
        addChild(new TextEditorLangSelectItem(module, widget, "HLSL"));
        addChild(new TextEditorLangSelectItem(module, widget, "Lua"));
        addChild(new TextEditorLangSelectItem(module, widget, "SQL"));
    }
};

struct TextEditorLangSelectMenuItem : MenuItem {
    TextEditorModule* const module;
    ImGuiTextEditor* const widget;

    TextEditorLangSelectMenuItem(TextEditorModule* const textEditorModule, ImGuiTextEditor* const textEditorWidget)
        : module(textEditorModule),
          widget(textEditorWidget)
    {
        text = "Syntax Highlight";
        rightText = RIGHT_ARROW;
    }

    Menu* createChildMenu() override
    {
        return new TextEditorLangSelectMenu(module, widget);
    }
};

// --------------------------------------------------------------------------------------------------------------------

struct TextEditorLoadFileItem : MenuItem {
    TextEditorModule* const module;
    ImGuiTextEditor* const widget;

    TextEditorLoadFileItem(TextEditorModule* const textEditorModule, ImGuiTextEditor* const textEditorWidget)
        : module(textEditorModule),
          widget(textEditorWidget)
    {
        text = "Load File";
    }

    void onAction(const event::Action&) override
    {
        TextEditorModule* const module = this->module;;
        WeakPtr<ImGuiTextEditor> widget = this->widget;

        async_dialog_filebrowser(false, nullptr, text.c_str(), [module, widget](char* path)
        {
            if (path)
            {
                if (module->loadFileFromMenuAction(path))
                {
                    if (widget)
                        widget->setFileWithKnownText(module->file, module->text);
                }
                free(path);
            }
        });
    }
};

// --------------------------------------------------------------------------------------------------------------------

/**
 * Code adapted from VCVRack's Blank.cpp
 * Copyright (C) 2016-2021 VCV.
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 */

struct ModuleResizeHandle : OpaqueWidget {
    TextEditorModule* const module;
    ModuleWidget* const widget;
    const bool right;
    Vec dragPos;
    Rect originalBox;

    ModuleResizeHandle(TextEditorModule* const textEditorModule, ModuleWidget* const moduleWidget, const bool r)
        : module(textEditorModule),
          widget(moduleWidget),
          right(r)
    {
        box.size = Vec(RACK_GRID_WIDTH * 1, RACK_GRID_HEIGHT);
    }

    void onDragStart(const DragStartEvent& e) override
    {
        if (e.button != GLFW_MOUSE_BUTTON_LEFT)
            return;

        dragPos = APP->scene->rack->getMousePos();
        originalBox = widget->box;
    }

    void onDragMove(const DragMoveEvent& e) override
    {
        static const float kMinWidth = 10 * RACK_GRID_WIDTH;

        Vec newDragPos = APP->scene->rack->getMousePos();
        float deltaX = newDragPos.x - dragPos.x;

        Rect newBox = originalBox;
        Rect oldBox = widget->box;
        if (right) {
            newBox.size.x += deltaX;
            newBox.size.x = std::fmax(newBox.size.x, kMinWidth);
            newBox.size.x = std::round(newBox.size.x / RACK_GRID_WIDTH) * RACK_GRID_WIDTH;
        }
        else {
            newBox.size.x -= deltaX;
            newBox.size.x = std::fmax(newBox.size.x, kMinWidth);
            newBox.size.x = std::round(newBox.size.x / RACK_GRID_WIDTH) * RACK_GRID_WIDTH;
            newBox.pos.x = originalBox.pos.x + originalBox.size.x - newBox.size.x;
        }

        // Set box and test whether it's valid
        widget->box = newBox;
        if (!APP->scene->rack->requestModulePos(widget, newBox.pos)) {
            widget->box = oldBox;
        }
        module->width = std::round(widget->box.size.x / RACK_GRID_WIDTH);
    }

    void draw(const DrawArgs& args) override
    {
        for (float x = 5.0; x <= 10.0; x += 5.0)
        {
            nvgBeginPath(args.vg);
            const float margin = 5.0;
            nvgMoveTo(args.vg, x + 0.5, margin + 0.5);
            nvgLineTo(args.vg, x + 0.5, box.size.y - margin + 0.5);
            nvgStrokeWidth(args.vg, 1.0);
            nvgStrokeColor(args.vg, nvgRGBAf(0.5, 0.5, 0.5, 0.5));
            nvgStroke(args.vg);
        }
    }
};

// --------------------------------------------------------------------------------------------------------------------

struct TextEditorModuleWidget : ModuleWidget {
    TextEditorModule* textEditorModule = nullptr;
    ImGuiTextEditor* textEditorWidget = nullptr;
    Widget* panelBorder;
    ModuleResizeHandle* rightHandle;

    TextEditorModuleWidget(TextEditorModule* const module)
    {
        setModule(module);
        addChild(panelBorder = new PanelBorder);
        addChild(rightHandle = new ModuleResizeHandle(module, this, true));
        addChild(new ModuleResizeHandle(module, this, false));

        box.size = Vec(RACK_GRID_WIDTH * (module != nullptr ? module->width : DEFAULT_WIDTH), RACK_GRID_HEIGHT);

        textEditorModule = module;
        textEditorWidget = new ImGuiTextEditor();
        textEditorWidget->box.pos = Vec(RACK_GRID_WIDTH, 0);
        textEditorWidget->box.size = Vec(box.size.x - 2 * RACK_GRID_WIDTH, box.size.y);
        addChild(textEditorWidget);

        if (module != nullptr)
        {
            textEditorWidget->setLanguageDefinition(module->lang);
            textEditorWidget->setFileWithKnownText(module->file, module->text);
            module->widgetPtr = textEditorWidget;
        }
        else
        {
            textEditorWidget->setLanguageDefinition(DEFAULT_LANG);
            textEditorWidget->setText(DEFAULT_TEXT);
        }
    }

    void step() override
    {
        if (textEditorModule)
        {
            box.size.x = textEditorModule->width * RACK_GRID_WIDTH;
            textEditorWidget->box.size.x = (textEditorModule->width - 2) * RACK_GRID_WIDTH;
        }

        panelBorder->box.size = box.size;
        rightHandle->box.pos.x = box.size.x - rightHandle->box.size.x;

        ModuleWidget::step();
    }

    void draw(const DrawArgs& args) override
    {
        nvgBeginPath(args.vg);
        nvgRect(args.vg, 0.0, 0.0, box.size.x, box.size.y);
        nvgFillColor(args.vg, nvgRGB(0x20, 0x20, 0x20));
        nvgFill(args.vg);
        ModuleWidget::draw(args);
    }

    void appendContextMenu(Menu* const menu) override
    {
        menu->addChild(new MenuSeparator);
        menu->addChild(new TextEditorLoadFileItem(textEditorModule, textEditorWidget));
        menu->addChild(new TextEditorLangSelectMenuItem(textEditorModule, textEditorWidget));

        menu->addChild(new ui::MenuSeparator);
        menu->addChild(createMenuItem("Undo", RACK_MOD_CTRL_NAME "+Z", [=]{ textEditorWidget->undo(); },
                                      !textEditorWidget->canUndo()));
        menu->addChild(createMenuItem("Redo", RACK_MOD_CTRL_NAME "+Y", [=]{ textEditorWidget->redo(); },
                                      !textEditorWidget->canRedo()));

        menu->addChild(new ui::MenuSeparator);
        menu->addChild(createMenuItem("Cut", RACK_MOD_CTRL_NAME "+X", [=]{ textEditorWidget->cut(); },
                                      !textEditorWidget->hasSelection()));
        menu->addChild(createMenuItem("Copy", RACK_MOD_CTRL_NAME "+C", [=]{ textEditorWidget->copy(); },
                                      !textEditorWidget->hasSelection()));
        menu->addChild(createMenuItem("Paste", RACK_MOD_CTRL_NAME "+V", [=]{ textEditorWidget->paste(); }));

        menu->addChild(new ui::MenuSeparator);
        menu->addChild(createMenuItem("Select all", RACK_MOD_CTRL_NAME "+A", [=]{ textEditorWidget->selectAll(); }));
    }

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TextEditorModuleWidget)
};
#else
struct TextEditorModuleWidget : ModuleWidget {
    TextEditorModuleWidget(TextEditorModule* const module) {
        setModule(module);
    }
};
#endif

// --------------------------------------------------------------------------------------------------------------------

Model* modelTextEditor = createModel<TextEditorModule, TextEditorModuleWidget>("TextEditor");

// --------------------------------------------------------------------------------------------------------------------
