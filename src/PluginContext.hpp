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

#pragma once

#include <audio.hpp>
#include <context.hpp>
#include <midi.hpp>

#ifdef NDEBUG
# undef DEBUG
#endif

#include "DistrhoPlugin.hpp"
#include "extra/Mutex.hpp"

#ifndef HEADLESS
# include "DistrhoUI.hpp"
# include "extra/FileBrowserDialog.hpp"
#endif

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------------------------------------

static constexpr const uint kModuleParameters = 24;

// -----------------------------------------------------------------------------------------------------------

struct CardinalPluginContext : rack::Context {
    uint32_t bufferSize;
    double sampleRate;
    float parameters[kModuleParameters];
    bool playing, reset, bbtValid, loadedHostCV;
    int32_t bar, beat, beatsPerBar, beatType;
    uint64_t frame;
    double barStartTick, beatsPerMinute;
    double tick, tickClock, ticksPerBeat, ticksPerClock, ticksPerFrame;
    uintptr_t nativeWindowId;
    uint32_t dataFrame;
    const float** dataIns;
    float** dataOuts;
    Plugin* const plugin;
#ifndef HEADLESS
    UI* ui;
#endif

    CardinalPluginContext(Plugin* const p)
        : bufferSize(p->getBufferSize()),
          sampleRate(p->getSampleRate()),
          playing(false),
          reset(false),
          bbtValid(false),
          loadedHostCV(false),
          bar(1),
          beat(1),
          beatsPerBar(4),
          beatType(4),
          frame(0),
          barStartTick(0.0),
          beatsPerMinute(120.0),
          tick(0.0),
          tickClock(0.0),
          ticksPerBeat(0.0),
          ticksPerClock(0.0),
          ticksPerFrame(0.0),
          nativeWindowId(0),
          dataFrame(0),
          dataIns(nullptr),
          dataOuts(nullptr),
          plugin(p)
#ifndef HEADLESS
        , ui(nullptr)
#endif
    {
        std::memset(parameters, 0, sizeof(parameters));
    }

#ifndef HEADLESS
    bool addIdleCallback(IdleCallback* cb) const;
    void removeIdleCallback(IdleCallback* cb) const;
#endif
};

#ifndef HEADLESS
void handleHostParameterDrag(const CardinalPluginContext* pcontext, uint index, bool started);
#endif

// -----------------------------------------------------------------------------------------------------------

struct CardinalAudioDevice;
struct CardinalMidiInputDevice;
struct CardinalMidiOutputDevice;

CardinalPluginContext* getRackContextFromPlugin(void* ptr);

class CardinalBasePlugin : public Plugin {
public:
    CardinalPluginContext* const context;

    CardinalBasePlugin(uint32_t parameterCount, uint32_t programCount, uint32_t stateCount)
        : Plugin(parameterCount, programCount, stateCount),
          context(new CardinalPluginContext(this)) {}
    ~CardinalBasePlugin() override {}
    virtual bool isActive() const noexcept = 0;
    virtual bool canAssignAudioDevice() const noexcept = 0;
    virtual bool clearAudioDevice(CardinalAudioDevice* dev) noexcept = 0;
    virtual void assignAudioDevice(CardinalAudioDevice* dev) noexcept = 0;
    virtual void assignMidiInputDevice(CardinalMidiInputDevice* dev) noexcept = 0;
    virtual void assignMidiOutputDevice(CardinalMidiOutputDevice* dev) noexcept = 0;
    virtual void clearMidiInputDevice(CardinalMidiInputDevice* dev) noexcept = 0;
    virtual void clearMidiOutputDevice(CardinalMidiOutputDevice* dev) noexcept = 0;
};

#ifndef HEADLESS
class CardinalBaseUI : public UI {
public:
    CardinalPluginContext* const context;
    bool saving;

    // for 3rd party modules
    std::function<void(char* path)> filebrowseraction;
    FileBrowserHandle filebrowserhandle;

    CardinalBaseUI(const uint width, const uint height)
        : UI(width, height),
          context(getRackContextFromPlugin(getPluginInstancePointer())),
          saving(false),
          filebrowseraction(),
          filebrowserhandle(nullptr)
    {
        context->ui = this;
    }

    ~CardinalBaseUI() override
    {
        if (filebrowserhandle != nullptr)
            fileBrowserClose(filebrowserhandle);

        context->ui = nullptr;
    }
};
#endif

// -----------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
