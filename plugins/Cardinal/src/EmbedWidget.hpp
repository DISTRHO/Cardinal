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

#pragma once

#include "plugin.hpp"

struct EmbedWidget : Widget {
    struct PrivateData;
    PrivateData* const pData;

    EmbedWidget(Vec size);
    ~EmbedWidget() override;

    void embedIntoRack(uintptr_t nativeWindowId);
    void removeFromRack();

    void show();
    void hide();

    uintptr_t getNativeWindowId() const;

private:
    void draw(const DrawArgs&) override;
    void step() override;

    Rect getAbsoluteRect();
};
