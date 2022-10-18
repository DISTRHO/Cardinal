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

#include "RemoteUI.hpp"

// #include <asset.hpp>
// #include <random.hpp>
#include <patch.hpp>
#include <settings.hpp>
#include <system.hpp>

#include <app/Scene.hpp>
#include <engine/Engine.hpp>

CardinalRemoteUI::CardinalRemoteUI(Window& window, const std::string& templatePath)
    : NanoTopLevelWidget(window),
      context(nullptr)
{
    // create unique temporary path for this instance
    try {
        char uidBuf[24];
        const std::string tmp = rack::system::getTempDirectory();

        for (int i=1;; ++i)
        {
            std::snprintf(uidBuf, sizeof(uidBuf), "CardinalRemote.%04d", i);
            const std::string trypath = rack::system::join(tmp, uidBuf);

            if (! rack::system::exists(trypath))
            {
                if (rack::system::createDirectories(trypath))
                    autosavePath = trypath;
                break;
            }
        }
    } DISTRHO_SAFE_EXCEPTION("create unique temporary path");

    rack::contextSet(&context);

    context.bufferSize = 512;
    rack::settings::sampleRate = context.sampleRate = 48000;

    context.engine = new rack::engine::Engine;
    context.engine->setSampleRate(context.sampleRate);

    context.history = new rack::history::State;
    context.patch = new rack::patch::Manager;
    context.patch->autosavePath = autosavePath;
    context.patch->templatePath = templatePath;

    context.event = new rack::widget::EventState;
    context.scene = new rack::app::Scene;
    context.event->rootWidget = context.scene;
    context.window = new rack::window::Window;

    context.patch->loadTemplate();
    context.scene->rackScroll->reset();

    context.nativeWindowId = getWindow().getNativeWindowHandle();
}

CardinalRemoteUI::~CardinalRemoteUI()
{
    rack::contextSet(&context);

    context.nativeWindowId = 0;
    context.patch->clear();

    if (! autosavePath.empty())
        rack::system::removeRecursively(autosavePath);
}

void CardinalRemoteUI::onNanoDisplay()
{
    rack::contextSet(&context);
    context.window->step();
}
