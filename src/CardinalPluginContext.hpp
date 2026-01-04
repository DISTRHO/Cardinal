/*
 * DISTRHO Cardinal Plugin
 * Copyright (C) 2021-2026 Filipe Coelho <falktx@falktx.com>
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
#endif

#include "plugincontext.hpp"

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------------------------------------

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

   #ifdef HAVE_LIBLO
    virtual bool startRemoteServer(const char* port) = 0;
    virtual void stopRemoteServer() = 0;
    virtual void stepRemoteServer() = 0;
   #endif

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

    CardinalBaseUI()
        : UI(),
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
