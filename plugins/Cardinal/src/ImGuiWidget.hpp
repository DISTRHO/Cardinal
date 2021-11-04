/*
 * Dear ImGui for DPF, converted to VCV
 * Copyright (C) 2021 Filipe Coelho <falktx@falktx.com>
 * Copyright (C) 2021 Jean Pierre Cimalando <jp-dev@inbox.ru>
 *
 * Permission to use, copy, modify, and/or distribute this software for any purpose with
 * or without fee is hereby granted, provided that the above copyright notice and this
 * permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
 * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#pragma once

#include "plugin.hpp"

struct ImGuiWidget : OpenGlWidget {
    struct PrivateData;
    PrivateData* const imData;

    ImGuiWidget(float width, float height);
    ~ImGuiWidget() override;
    void drawFramebuffer() override;

    void onHover(const HoverEvent& e) override;
    void onButton(const ButtonEvent& e) override;
};
