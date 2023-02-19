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

#include "CardinalRemote.hpp"
#include "DistrhoPlugin.hpp"

#if CARDINAL_VARIANT_MINI || !defined(HEADLESS)
# include "WindowParameters.hpp"
#else
# define kWindowParameterCount 0
#endif

#ifndef HEADLESS
# include "DistrhoUI.hpp"
#else
# include "Base.hpp"
START_NAMESPACE_DGL
class TopLevelWidget;
template <class BaseWidget> class NanoBaseWidget;
typedef NanoBaseWidget<TopLevelWidget> NanoTopLevelWidget;
END_NAMESPACE_DGL
#endif

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------------------------------------

static constexpr const uint kModuleParameterCount = 24;

enum CardinalVariant {
    kCardinalVariantMain,
    kCardinalVariantMini,
    kCardinalVariantFX,
    kCardinalVariantNative,
    kCardinalVariantSynth,
};

enum CardinalParameters {
    kCardinalParameterCountAtModules = kModuleParameterCount,
    kCardinalParameterBypass = kCardinalParameterCountAtModules,
  #if CARDINAL_VARIANT_MINI || !defined(HEADLESS)
    kCardinalParameterStartWindow,
    kCardinalParameterCountAtWindow = kCardinalParameterStartWindow + kWindowParameterCount,
   #if CARDINAL_VARIANT_MINI
    kCardinalParameterStartMini = kCardinalParameterCountAtWindow,
    kCardinalParameterStartMiniBuffers = kCardinalParameterStartMini,
    kCardinalParameterMiniAudioIn1 = kCardinalParameterStartMiniBuffers,
    kCardinalParameterMiniAudioIn2,
    kCardinalParameterMiniCVIn1,
    kCardinalParameterMiniCVIn2,
    kCardinalParameterMiniCVIn3,
    kCardinalParameterMiniCVIn4,
    kCardinalParameterMiniCVIn5,
    kCardinalParameterCountAtMiniBuffers,
    kCardinalParameterStartMiniTime = kCardinalParameterCountAtMiniBuffers,
    kCardinalParameterMiniTimeFlags = kCardinalParameterStartMiniTime,
    kCardinalParameterMiniTimeBar,
    kCardinalParameterMiniTimeBeat,
    kCardinalParameterMiniTimeBeatsPerBar,
    kCardinalParameterMiniTimeBeatType,
    kCardinalParameterMiniTimeFrame,
    kCardinalParameterMiniTimeBarStartTick,
    kCardinalParameterMiniTimeBeatsPerMinute,
    kCardinalParameterMiniTimeTick,
    kCardinalParameterMiniTimeTicksPerBeat,
    kCardinalParameterCountAtMiniTime,
    kCardinalParameterCountAtMini = kCardinalParameterCountAtMiniTime,
    kCardinalParameterCount = kCardinalParameterCountAtMini
   #else
    kCardinalParameterCount = kCardinalParameterCountAtWindow
   #endif
  #else
    kCardinalParameterCount
  #endif
};

enum CardinalStates {
    kCardinalStatePatch,
    kCardinalStateScreenshot,
    kCardinalStateComment,
   #if CARDINAL_VARIANT_MINI || !defined(HEADLESS)
    kCardinalStateModuleInfos,
    kCardinalStateWindowSize,
   #endif
   #if CARDINAL_VARIANT_MINI
    kCardinalStateParamChange,
   #endif
    kCardinalStateCount
};

static_assert(kCardinalParameterBypass == kModuleParameterCount, "valid parameter indexes");
#if CARDINAL_VARIANT_MINI || !defined(HEADLESS)
static_assert(kCardinalParameterStartWindow == kModuleParameterCount + 1, "valid parameter indexes");
static_assert(kCardinalParameterStartWindow == kCardinalParameterBypass + 1, "valid parameter indexes");
static_assert(kCardinalParameterCountAtWindow == kModuleParameterCount + kWindowParameterCount + 1, "valid parameter indexes");
#endif
#if CARDINAL_VARIANT_MINI
static_assert(0 == kCardinalParameterStartMini - kCardinalParameterMiniAudioIn1, "valid parameter indexes");
static_assert(kCardinalParameterStartMini == kCardinalParameterCountAtWindow, "valid parameter indexes");
static_assert(kCardinalParameterStartMini == kCardinalParameterBypass + kWindowParameterCount + 1, "valid parameter indexes");
static_assert(kCardinalParameterStartMini == kModuleParameterCount + kWindowParameterCount + 1, "valid parameter indexes");
static_assert(kCardinalParameterCountAtWindow == kModuleParameterCount + kWindowParameterCount + 1, "valid parameter indexes");
static_assert(DISTRHO_PLUGIN_NUM_INPUTS == kCardinalParameterCountAtMiniBuffers - kCardinalParameterStartMiniBuffers, "valid parameter indexes");
#endif

class UI;

// -----------------------------------------------------------------------------------------------------------

struct CardinalPluginContext : rack::Context {
    uint32_t bufferSize, processCounter;
    double sampleRate;
    float parameters[kModuleParameterCount];
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
    NanoTopLevelWidget* tlw;
    UI* ui;

    CardinalPluginContext(Plugin* const p)
        : bufferSize(p != nullptr ? p->getBufferSize() : 0),
          processCounter(0),
          sampleRate(p != nullptr ? p->getSampleRate() : 0.0),
         #if CARDINAL_VARIANT_MAIN
          variant(kCardinalVariantMain),
         #elif CARDINAL_VARIANT_MINI
          variant(kCardinalVariantMini),
         #elif CARDINAL_VARIANT_FX
          variant(kCardinalVariantFX),
         #elif CARDINAL_VARIANT_NATIVE
          variant(kCardinalVariantNative),
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
          plugin(p),
          tlw(nullptr),
          ui(nullptr)
    {
        std::memset(parameters, 0, sizeof(parameters));
    }

    void writeMidiMessage(const rack::midi::Message& message, uint8_t channel);

   #ifndef HEADLESS
    bool addIdleCallback(IdleCallback* cb) const;
    void removeIdleCallback(IdleCallback* cb) const;
   #endif
};

// -----------------------------------------------------------------------------------------------------------

#if DISTRHO_PLUGIN_WANT_DIRECT_ACCESS
CardinalPluginContext* getRackContextFromPlugin(void* ptr);
#endif

class CardinalBasePlugin : public Plugin {
public:
    CardinalPluginContext* const context;

    CardinalBasePlugin(uint32_t parameterCount, uint32_t programCount, uint32_t stateCount)
        : Plugin(parameterCount, programCount, stateCount),
          context(new CardinalPluginContext(this)) {}
    ~CardinalBasePlugin() override {}

   #ifndef HEADLESS
    friend class CardinalUI;
   #endif
};

#ifndef HEADLESS
struct WasmRemotePatchLoadingDialog;

class CardinalBaseUI : public UI {
public:
    CardinalPluginContext* const context;
    remoteUtils::RemoteDetails* remoteDetails;
    bool saving;
    bool savingUncompressed;

   #ifdef DISTRHO_OS_WASM
    WasmRemotePatchLoadingDialog* psDialog;
   #endif

    // for 3rd party modules
    std::function<void(char* path)> filebrowseraction;
    FileBrowserHandle filebrowserhandle;

    CardinalBaseUI(const uint width, const uint height)
        : UI(width, height),
         #if DISTRHO_PLUGIN_WANT_DIRECT_ACCESS
          context(getRackContextFromPlugin(getPluginInstancePointer())),
         #else
          context(new CardinalPluginContext(nullptr)),
         #endif
          remoteDetails(nullptr),
          saving(false),
          savingUncompressed(false),
         #ifdef DISTRHO_OS_WASM
          psDialog(nullptr),
         #endif
          filebrowseraction(),
          filebrowserhandle(nullptr)
    {
        context->tlw = this;
        context->ui = this;
    }

    ~CardinalBaseUI() override
    {
        remoteUtils::disconnectFromRemote(remoteDetails);

        if (filebrowserhandle != nullptr)
            fileBrowserClose(filebrowserhandle);
    }
};
#endif

// -----------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
