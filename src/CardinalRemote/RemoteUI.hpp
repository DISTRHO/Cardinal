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

#include "NanoVG.hpp"
#include "PluginContext.hpp"

#include <app/common.hpp>

class CardinalRemoteUI : public NanoTopLevelWidget
{
    rack::math::Vec lastMousePos;

public:
    explicit CardinalRemoteUI(Window& window, const std::string& templatePath);
    ~CardinalRemoteUI() override;
    
protected:
    void onNanoDisplay() override;
    bool onMouse(const MouseEvent& ev) override;
    bool onMotion(const MotionEvent& ev) override;
    bool onScroll(const ScrollEvent& ev) override;
    bool onCharacterInput(const CharacterInputEvent& ev) override;
    bool onKeyboard(const KeyboardEvent& ev) override;
    void onResize(const ResizeEvent& ev) override;
    
    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CardinalRemoteUI)
};
