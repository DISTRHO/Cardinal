/*
 * Syntax highlighting text editor (for ImGui in DPF)
 * Copyright (C) 2021 Filipe Coelho <falktx@falktx.com>
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

// --------------------------------------------------------------------------------------------------------------------

struct ImGuiTextEditor::PrivateData {
    ImGuiTextEditor* const self;
    TextEditor editor;
    std::string file;

    explicit PrivateData(ImGuiTextEditor* const s)
        : self(s)
    {
        // https://github.com/BalazsJako/ColorTextEditorDemo/blob/master/main.cpp

        editor.SetLanguageDefinition(TextEditor::LanguageDefinition::CPlusPlus());
        editor.SetText(""
        "// Welcome to a real text editor inside Cardinal\n"
        "\n"
        "#define I_AM_A_MACRO\n"
        "\n"
        "int and_i_am_a_variable;\n"
        "\n"
        "/* look ma, a comment! */\n"
        "int such_highlight_much_wow() { return 1337; }\n"
        );
        editor.SetCursorPosition(TextEditor::Coordinates(8, 0));
    }

    DISTRHO_DECLARE_NON_COPYABLE(PrivateData)
};

// --------------------------------------------------------------------------------------------------------------------

ImGuiTextEditor::ImGuiTextEditor()
    : ImGuiWidget(),
      pData(new PrivateData(this)) {}

ImGuiTextEditor::~ImGuiTextEditor()
{
    delete pData;
}

// --------------------------------------------------------------------------------------------------------------------

void ImGuiTextEditor::setText(const std::string& text)
{
    pData->editor.SetText(text);
}

std::string ImGuiTextEditor::getText() const
{
    return pData->editor.GetText();
}

void ImGuiTextEditor::setTextLines(const std::vector<std::string>& lines)
{
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

// --------------------------------------------------------------------------------------------------------------------

void ImGuiTextEditor::drawImGui()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(box.size.x, box.size.y));

    if (ImGui::Begin("TextEdit", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize))
    {
        TextEditor& editor(pData->editor);

        const TextEditor::Coordinates cpos = editor.GetCursorPosition();

        ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s",
                    cpos.mLine + 1, cpos.mColumn + 1,
                    editor.GetTotalLines(),
                    editor.IsOverwrite() ? "Ovr" : "Ins",
                    editor.CanUndo() ? "*" : " ",
                    editor.GetLanguageDefinition().mName.c_str(),
                    pData->file.c_str());

        editor.Render("TextEditor");
    }

    ImGui::End();
}
