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
#endif

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------------------------------------

static constexpr const uint kModuleParameters = 24;

enum CardinalVariant {
    kCardinalVariantMain,
    kCardinalVariantFX,
    kCardinalVariantSynth,
};

// -----------------------------------------------------------------------------------------------------------

struct CardinalPluginContext : rack::Context {
    uint32_t bufferSize, processCounter;
    double sampleRate;
    float parameters[kModuleParameters];
    CardinalVariant variant;
    bool bypassed, playing, reset, bbtValid;
    int32_t bar, beat, beatsPerBar, beatType;
    uint64_t frame;
    double barStartTick, beatsPerMinute;
    double tick, tickClock, ticksPerBeat, ticksPerClock, ticksPerFrame;
    uintptr_t nativeWindowId;
    const float* const* dataIns;
    float** dataOuts;
    const MidiEvent* midiEvents;
    uint32_t midiEventCount;
    Plugin* const plugin;
#ifndef HEADLESS
    UI* ui;
#endif

    CardinalPluginContext(Plugin* const p)
        : bufferSize(p->getBufferSize()),
          processCounter(0),
          sampleRate(p->getSampleRate()),
         #if CARDINAL_VARIANT_MAIN
          variant(kCardinalVariantMain),
         #elif CARDINAL_VARIANT_FX
          variant(kCardinalVariantFX),
         #elif CARDINAL_VARIANT_SYNTH
          variant(kCardinalVariantSynth),
         #else
          #error cardinal variant not set
         #endif
          bypassed(false),
          playing(false),
          reset(false),
          bbtValid(false),
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
          dataIns(nullptr),
          dataOuts(nullptr),
          midiEvents(nullptr),
          midiEventCount(0),
          plugin(p)
#ifndef HEADLESS
        , ui(nullptr)
#endif
    {
        std::memset(parameters, 0, sizeof(parameters));
    }

    void writeMidiMessage(const rack::midi::Message& message, uint8_t channel);

#ifndef HEADLESS
    bool addIdleCallback(IdleCallback* cb) const;
    void removeIdleCallback(IdleCallback* cb) const;
#endif
};

#ifndef HEADLESS
void handleHostParameterDrag(const CardinalPluginContext* pcontext, uint index, bool started);
#endif

// -----------------------------------------------------------------------------------------------------------

CardinalPluginContext* getRackContextFromPlugin(void* ptr);

class CardinalBasePlugin : public Plugin {
public:
    CardinalPluginContext* const context;

    CardinalBasePlugin(uint32_t parameterCount, uint32_t programCount, uint32_t stateCount)
        : Plugin(parameterCount, programCount, stateCount),
          context(new CardinalPluginContext(this)) {}
    ~CardinalBasePlugin() override {}
};

#ifndef HEADLESS
struct WasmPatchStorageLoadingDialog;

class CardinalBaseUI : public UI {
public:
    CardinalPluginContext* const context;
    bool saving;
    bool savingUncompressed;

   #ifdef DISTRHO_OS_WASM
    WasmPatchStorageLoadingDialog* psDialog;
   #endif

    // for 3rd party modules
    std::function<void(char* path)> filebrowseraction;
    FileBrowserHandle filebrowserhandle;

    CardinalBaseUI(const uint width, const uint height)
        : UI(width, height),
          context(getRackContextFromPlugin(getPluginInstancePointer())),
          saving(false),
          savingUncompressed(false),
         #ifdef DISTRHO_OS_WASM
          psDialog(nullptr),
         #endif
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
