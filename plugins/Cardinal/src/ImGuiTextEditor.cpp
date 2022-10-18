/*
 * Syntax highlighting text editor (for ImGui in DPF)
 * Copyright (C) 2021-2022 Filipe Coelho <falktx@falktx.com>
 * Copyright (c) 2017 BalazsJako
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "ImGuiTextEditor.hpp"
#include "DearImGuiColorTextEditor/TextEditor.h"

#include <fstream>

// --------------------------------------------------------------------------------------------------------------------

struct ImGuiTextEditor::PrivateData {
    TextEditor editor;
    std::string file;
    bool darkMode = true;
};

// --------------------------------------------------------------------------------------------------------------------

ImGuiTextEditor::ImGuiTextEditor()
    : ImGuiWidget(),
      pData(new PrivateData())
{
    setUseMonospaceFont();
}

ImGuiTextEditor::~ImGuiTextEditor()
{
    delete pData;
}

// --------------------------------------------------------------------------------------------------------------------

bool ImGuiTextEditor::setFile(const std::string& file)
{
    std::ifstream f(file);

    if (! f.good())
    {
        pData->file.clear();
        return false;
    }

    pData->file = file;
    pData->editor.SetText(std::string((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>()));
    return true;
}

void ImGuiTextEditor::setFileWithKnownText(const std::string& file, const std::string& text)
{
    pData->file = file;
    pData->editor.SetText(text);
}

std::string ImGuiTextEditor::getFile() const
{
    return pData->file;
}

void ImGuiTextEditor::setLanguageDefinition(const std::string& lang)
{
    pData->editor.SetColorizerEnable(true);

    if (lang == "AngelScript")
        return pData->editor.SetLanguageDefinition(TextEditor::LanguageDefinition::AngelScript());
    if (lang == "C")
        return pData->editor.SetLanguageDefinition(TextEditor::LanguageDefinition::C());
    if (lang == "C++")
        return pData->editor.SetLanguageDefinition(TextEditor::LanguageDefinition::CPlusPlus());
    if (lang == "GLSL")
        return pData->editor.SetLanguageDefinition(TextEditor::LanguageDefinition::GLSL());
    if (lang == "HLSL")
        return pData->editor.SetLanguageDefinition(TextEditor::LanguageDefinition::HLSL());
    if (lang == "Lua")
        return pData->editor.SetLanguageDefinition(TextEditor::LanguageDefinition::Lua());
    if (lang == "SQL")
        return pData->editor.SetLanguageDefinition(TextEditor::LanguageDefinition::SQL());

    pData->editor.SetColorizerEnable(false);
    pData->editor.SetLanguageDefinition(TextEditor::LanguageDefinition());
}

// --------------------------------------------------------------------------------------------------------------------

void ImGuiTextEditor::setText(const std::string& text)
{
    pData->file.clear();
    pData->editor.SetText(text);
}

std::string ImGuiTextEditor::getText() const
{
    return pData->editor.GetText();
}

void ImGuiTextEditor::setTextLines(const std::vector<std::string>& lines)
{
    pData->file.clear();
    pData->editor.SetTextLines(lines);
}

std::vector<std::string> ImGuiTextEditor::getTextLines() const
{
    return pData->editor.GetTextLines();
}

std::string ImGuiTextEditor::getSelectedText() const
{
    return pData->editor.GetSelectedText();
}

std::string ImGuiTextEditor::getCurrentLineText()const
{
    return pData->editor.GetCurrentLineText();
}

bool ImGuiTextEditor::hasSelection() const
{
    return pData->editor.HasSelection();
}

void ImGuiTextEditor::selectAll()
{
    pData->editor.SelectAll();
}

void ImGuiTextEditor::copy()
{
    pData->editor.Copy();
}

void ImGuiTextEditor::cut()
{
    pData->editor.Cut();
}

void ImGuiTextEditor::paste()
{
    pData->editor.Paste();
}

bool ImGuiTextEditor::canUndo() const
{
    return pData->editor.CanUndo();
}

bool ImGuiTextEditor::canRedo() const
{
    return pData->editor.CanRedo();
}

void ImGuiTextEditor::undo()
{
    pData->editor.Undo();
}

void ImGuiTextEditor::redo()
{
    pData->editor.Redo();
}

// --------------------------------------------------------------------------------------------------------------------

void ImGuiTextEditor::drawImGui()
{
    const float scaleFactor = getScaleFactor();

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(box.size.x * scaleFactor, box.size.y * scaleFactor));

    const int pflags = ImGuiWindowFlags_NoSavedSettings
                     | ImGuiWindowFlags_NoCollapse
                     | ImGuiWindowFlags_NoResize
                     | ImGuiWindowFlags_NoTitleBar
                     | ImGuiWindowFlags_AlwaysAutoResize;

    if (ImGui::Begin("TextEdit", nullptr, pflags))
    {
        TextEditor& editor(pData->editor);

        const TextEditor::Coordinates cpos = editor.GetCursorPosition();

        ImGui::Text("%6d/%-6d %6d lines | %s | %s | %s",
                    cpos.mLine + 1, cpos.mColumn + 1,
                    editor.GetTotalLines(),
                    editor.IsOverwrite() ? "Ovr" : "Ins",
                    editor.GetLanguageDefinition().mName.c_str(),
                    pData->file.c_str());

        editor.Render("TextEditor");
    }

    ImGui::End();
}

void ImGuiTextEditor::onButton(const ButtonEvent& e)
{
    // if mouse press is over the top status bar, do nothing so editor can be moved in the Rack
    if (e.action == GLFW_PRESS && e.pos.y < 25)
        return;

    ImGuiWidget::onButton(e);
}

void ImGuiTextEditor::onHoverScroll(const HoverScrollEvent& e)
{
    // use Rack view scrolling if there is no scrollbar
    if (pData->editor.GetTotalLines() < 27)
        return;

    // if there is a scrollbar, handle the event
    ImGuiWidget::onHoverScroll(e);
}

void ImGuiTextEditor::step()
{
    if (pData->darkMode != settings::darkMode)
    {
        pData->darkMode = settings::darkMode;
        pData->editor.SetPalette(settings::darkMode ? TextEditor::GetDarkPalette()
                                                    : TextEditor::GetLightPalette());
    }

    ImGuiWidget::step();
}
