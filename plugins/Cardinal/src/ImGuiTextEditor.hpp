/*
 * Syntax highlighting text editor (for ImGui in DPF, converted to VCV)
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

#pragma once

#include "ImGuiWidget.hpp"
#include "DearImGuiColorTextEditor/TextEditor.h"

#include <string>
#include <vector>

struct ImGuiTextEditor : ImGuiWidget
{
    struct PrivateData;
    PrivateData* const pData;

    ImGuiTextEditor();
    ~ImGuiTextEditor() override;

    bool setFile(const std::string& file);
    void setFileWithKnownText(const std::string& file, const std::string& text);
    std::string getFile() const;

    void setLanguageDefinition(const std::string& lang);

   /**
      Methods from internal TextEdit.
    */
    void setText(const std::string& text);
    std::string getText() const;

    void setTextLines(const std::vector<std::string>& lines);
    std::vector<std::string> getTextLines() const;

    std::string getSelectedText() const;
    std::string getCurrentLineText()const;

protected:
    /** @internal */
    void drawImGui() override;
};
