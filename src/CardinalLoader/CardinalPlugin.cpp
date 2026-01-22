/*
 * DISTRHO Cardinal Plugin
 * Copyright (C) 2021-2026 Filipe Coelho <falktx@falktx.com>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <library.hpp>
#include <midi.hpp>
#include <patch.hpp>
#include <plugin.hpp>
#include <settings.hpp>
#include <system.hpp>

#include <app/Scene.hpp>
#include <engine/Engine.hpp>

#ifdef NDEBUG
# undef DEBUG
#endif

#if defined(HAVE_LIBLO) && defined(HEADLESS)
# include <lo/lo.h>
# include "extra/Thread.hpp"
#endif

#include <cfloat>
#include <list>

#include "CardinalCommon.hpp"
#include "DistrhoPluginUtils.hpp"
#include "CardinalPluginContext.hpp"
#include "extra/ScopedDenormalDisable.hpp"
#include "extra/ScopedSafeLocale.hpp"

#ifdef DISTRHO_OS_WASM
# include <emscripten/emscripten.h>
#else
# include "extra/SharedResourcePointer.hpp"
#endif

namespace rack {
namespace app {
rack::widget::Widget* createMenuBar() { return new rack::widget::Widget; }
}
namespace engine {
void Engine_setAboutToClose(Engine*);
}
}

START_NAMESPACE_DISTRHO

template<typename T>
static inline
bool d_isDiffHigherThanLimit(const T& v1, const T& v2, const T& limit)
{
    return v1 != v2 ? (v1 > v2 ? v1 - v2 : v2 - v1) > limit : false;
}

// -----------------------------------------------------------------------------------------------------------

struct ScopedContext {
    ScopedContext(const CardinalBasePlugin* const plugin)
    {
        rack::contextSet(plugin->context);
    }

    ~ScopedContext()
    {
        rack::contextSet(nullptr);
    }
};

// -----------------------------------------------------------------------------------------------------------

class CardinalPlugin : public CardinalBasePlugin
{
   #ifdef DISTRHO_OS_WASM
    ScopedPointer<Initializer> fInitializer;
   #else
    SharedResourcePointer<Initializer> fInitializer;
   #endif

   #if DISTRHO_PLUGIN_NUM_INPUTS != 0
    /* If host audio ins == outs we can get issues for inplace processing.
     * So allocate a float array that will serve as safe copy for those cases.
     */
    float** fAudioBufferCopy;
   #endif

    std::string fAutosavePath;
    uint64_t fNextExpectedFrame;

    // bypass handling
    bool fWasBypassed;
    MidiEvent bypassMidiEvents[16];

    static constexpr const uint16_t fNumActiveInputs = DISTRHO_PLUGIN_NUM_INPUTS;
    static constexpr const uint16_t fNumActiveOutputs = DISTRHO_PLUGIN_NUM_OUTPUTS;

public:
    CardinalPlugin()
        : CardinalBasePlugin(kCardinalParameterCount, 0, kCardinalStateCount),
         #ifdef DISTRHO_OS_WASM
          fInitializer(new Initializer(this, static_cast<const CardinalBaseUI*>(nullptr))),
         #else
          fInitializer(this, static_cast<const CardinalBaseUI*>(nullptr)),
         #endif
         #if DISTRHO_PLUGIN_NUM_INPUTS != 0
          fAudioBufferCopy(nullptr),
         #endif
          fNextExpectedFrame(0),
          fWasBypassed(false)
    {
        // check if first time loading a real instance
        if (!fInitializer->shouldSaveSettings && !isDummyInstance())
            fInitializer->loadSettings(true);

        // create unique temporary path for this instance
        try {
            char uidBuf[24];
            const std::string tmp = rack::system::getTempDirectory();

            for (int i=1;; ++i)
            {
                std::snprintf(uidBuf, sizeof(uidBuf), "Cardinal.%04d", i);
                const std::string trypath = rack::system::join(tmp, uidBuf);

                if (! rack::system::exists(trypath))
                {
                    if (rack::system::createDirectories(trypath))
                        fAutosavePath = trypath;
                    break;
                }
            }
        } DISTRHO_SAFE_EXCEPTION("create unique temporary path");

        // initialize midi events used when entering bypassed state
        std::memset(bypassMidiEvents, 0, sizeof(bypassMidiEvents));

        for (uint8_t i=0; i<16; ++i)
        {
            bypassMidiEvents[i].size = 3;
            bypassMidiEvents[i].data[0] = 0xB0 + i;
            bypassMidiEvents[i].data[1] = 0x7B;
        }

        const float sampleRate = getSampleRate();
        rack::settings::sampleRate = sampleRate;

        context->bufferSize = getBufferSize();
        context->sampleRate = sampleRate;

        const ScopedContext sc(this);

        context->engine = new rack::engine::Engine;
        context->engine->setSampleRate(sampleRate);

        context->history = new rack::history::State;
        context->patch = new rack::patch::Manager;
        context->patch->autosavePath = fAutosavePath;
        context->patch->templatePath = fInitializer->templatePath;
        context->patch->factoryTemplatePath = fInitializer->factoryTemplatePath;

        context->event = new rack::widget::EventState;
        context->scene = new rack::app::Scene;
        context->event->rootWidget = context->scene;

        if (! isDummyInstance())
            context->window = new rack::window::Window;

        context->patch->clear();
        context->scene->rackScroll->reset();
    }

    ~CardinalPlugin() override
    {
        {
            const ScopedContext sc(this);
            context->patch->clear();

            Engine_setAboutToClose(context->engine);
            delete context;
        }

        if (! fAutosavePath.empty())
            rack::system::removeRecursively(fAutosavePath);
    }

    CardinalPluginContext* getRackContext() const noexcept
    {
        return context;
    }

protected:
   /* --------------------------------------------------------------------------------------------------------
    * Information */

    const char* getLabel() const override
    {
        return DISTRHO_PLUGIN_LABEL;
    }

    const char* getDescription() const override
    {
        return ""
        "Cardinal is a free and open-source virtual modular synthesizer plugin.\n"
        "It is based on the popular VCV Rack but with a focus on being a fully self-contained plugin version.\n"
        "It is not an official VCV project, and it is not affiliated with it in any way.\n"
        "\n"
        "Cardinal contains Rack, some 3rd-party modules and a few internal utilities.\n"
        "It does not load external modules and does not connect to the official Rack library/store.\n";
    }

    const char* getMaker() const override
    {
        return "DISTRHO";
    }

    const char* getHomePage() const override
    {
        return "https://github.com/DISTRHO/Cardinal";
    }

    const char* getLicense() const override
    {
        return "GPLv3+";
    }

    uint32_t getVersion() const override
    {
        return d_version(0, 26, 1);
    }

    int64_t getUniqueId() const override
    {
        return d_cconst('d', 'C', 'd', 'C');
    }

   /* --------------------------------------------------------------------------------------------------------
    * Init */

    void initParameter(const uint32_t index, Parameter& parameter) override
    {
        if (index < kCardinalParameterCountAtModules)
        {
            parameter.name = "Parameter ";
            parameter.name += String(index + 1);
            parameter.symbol = "param_";
            parameter.symbol += String(index + 1);
            parameter.unit = "v";
            parameter.hints = kParameterIsAutomatable;
            parameter.ranges.def = 0.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 10.0f;
            return;
        }

        if (index == kCardinalParameterBypass)
        {
            parameter.initDesignation(kParameterDesignationBypass);
            return;
        }
    }

    void initState(const uint32_t index, State& state) override
    {
        switch (index)
        {
        case kCardinalStatePatch:
            state.hints = kStateIsFilenamePath;
            state.key = "patch";
            state.label = "Patch";
            break;
        }
    }

   /* --------------------------------------------------------------------------------------------------------
    * Internal data */

    float getParameterValue(uint32_t index) const override
    {
        // host mapped parameters
        if (index < kCardinalParameterCountAtModules)
            return context->parameters[index];

        // bypass
        if (index == kCardinalParameterBypass)
            return context->bypassed ? 1.0f : 0.0f;

        return 0.0f;
    }

    void setParameterValue(uint32_t index, float value) override
    {
        // host mapped parameters
        if (index < kCardinalParameterCountAtModules)
        {
            context->parameters[index] = value;
            return;
        }

        // bypass
        if (index == kCardinalParameterBypass)
        {
            context->bypassed = value > 0.5f;
            return;
        }
    }

    void setState(const char* const key, const char* const value) override
    {
        if (std::strcmp(key, "param") == 0)
        {
            long long moduleId = 0;
            int paramId = 0;
            float paramValue = 0.f;
            {
                const ScopedSafeLocale cssl;
                std::sscanf(value, "%lld:%d:%f", &moduleId, &paramId, &paramValue);
            }

            rack::engine::Module* const module = context->engine->getModule(moduleId);
            DISTRHO_SAFE_ASSERT_RETURN(module != nullptr,);

            context->engine->setParamValue(module, paramId, paramValue);
            return;
        }

        if (std::strcmp(key, "patch") == 0)
        {
            const ScopedContext sc(this);

            try {
                context->patch->load(value);
            } catch (rack::Exception& e) {
                d_stderr(e.what());
                return;
            } DISTRHO_SAFE_EXCEPTION("setState patch load");
            return;
        }
    }

   /* --------------------------------------------------------------------------------------------------------
    * Process */

    void activate() override
    {
        context->bufferSize = getBufferSize();

       #if DISTRHO_PLUGIN_NUM_INPUTS != 0
        fAudioBufferCopy = new float*[DISTRHO_PLUGIN_NUM_INPUTS];
        for (int i=0; i<DISTRHO_PLUGIN_NUM_INPUTS; ++i)
        {
            fAudioBufferCopy[i] = new float[context->bufferSize];
            std::memset(fAudioBufferCopy[i], 0, sizeof(float) * context->bufferSize);
        }
       #endif

        fNextExpectedFrame = 0;
    }

    void deactivate() override
    {
       #if DISTRHO_PLUGIN_NUM_INPUTS != 0
        if (fAudioBufferCopy != nullptr)
        {
            for (int i=0; i<DISTRHO_PLUGIN_NUM_INPUTS; ++i)
                delete[] fAudioBufferCopy[i];
            delete[] fAudioBufferCopy;
            fAudioBufferCopy = nullptr;
        }
       #endif
    }

    void run(const float** const inputs, float** const outputs, const uint32_t frames,
             const MidiEvent* const midiEvents, const uint32_t midiEventCount) override
    {
        const ScopedDenormalDisable sdd;

        rack::contextSet(context);

        const bool bypassed = context->bypassed;

        {
            const TimePosition& timePos(getTimePosition());

            bool reset = timePos.playing && (timePos.frame == 0 || d_isDiffHigherThanLimit(fNextExpectedFrame, timePos.frame, (uint64_t)2));

            // ignore hosts which cannot supply time frame position
            if (context->playing == timePos.playing && timePos.frame == 0 && context->frame == 0)
                reset = false;

            context->playing = timePos.playing;
            context->bbtValid = timePos.bbt.valid;
            context->frame = timePos.frame;

            if (timePos.bbt.valid)
            {
                const double samplesPerTick = 60.0 * getSampleRate()
                                            / timePos.bbt.beatsPerMinute
                                            / timePos.bbt.ticksPerBeat;
                context->bar = timePos.bbt.bar;
                context->beat = timePos.bbt.beat;
                context->beatsPerBar = timePos.bbt.beatsPerBar;
                context->beatType = timePos.bbt.beatType;
                context->barStartTick = timePos.bbt.barStartTick;
                context->beatsPerMinute = timePos.bbt.beatsPerMinute;
                context->tick = timePos.bbt.tick;
                context->ticksPerBeat = timePos.bbt.ticksPerBeat;
                context->ticksPerClock = timePos.bbt.ticksPerBeat / timePos.bbt.beatType;
                context->ticksPerFrame = 1.0 / samplesPerTick;
                context->tickClock = std::fmod(timePos.bbt.tick, context->ticksPerClock);
            }

            context->reset = reset;
            fNextExpectedFrame = timePos.playing ? timePos.frame + frames : 0;
        }

        // separate buffers, use them
        if (inputs != outputs && (inputs == nullptr || inputs[0] != outputs[0]))
        {
            context->dataIns = inputs;
            context->dataOuts = outputs;
        }
        // inline processing, use a safe copy
        else
        {
           #if DISTRHO_PLUGIN_NUM_INPUTS != 0
            for (int i=0; i<fNumActiveInputs; ++i)
            {
               #if CARDINAL_VARIANT_MAIN || CARDINAL_VARIANT_MINI
                // can be null on main and mini variants
                if (inputs[i] != nullptr)
               #endif
                    std::memcpy(fAudioBufferCopy[i], inputs[i], sizeof(float)*frames);
            }
            context->dataIns = fAudioBufferCopy;
           #else
            context->dataIns = nullptr;
           #endif
            context->dataOuts = outputs;
        }

        for (int i=0; i<fNumActiveOutputs; ++i)
        {
           #if CARDINAL_VARIANT_MAIN || CARDINAL_VARIANT_MINI
            // can be null on main and mini variants
            if (outputs[i] != nullptr)
           #endif
                std::memset(outputs[i], 0, sizeof(float)*frames);
        }

        if (bypassed)
        {
            if (fWasBypassed != bypassed)
            {
                context->midiEvents = bypassMidiEvents;
                context->midiEventCount = 16;
            }
            else
            {
                context->midiEvents = nullptr;
                context->midiEventCount = 0;
            }
        }
        else
        {
            context->midiEvents = midiEvents;
            context->midiEventCount = midiEventCount;
        }

        ++context->processCounter;
        context->engine->stepBlock(frames);

        fWasBypassed = bypassed;
    }

    void sampleRateChanged(const double newSampleRate) override
    {
        rack::contextSet(context);
        rack::settings::sampleRate = newSampleRate;
        context->sampleRate = newSampleRate;
        context->engine->setSampleRate(newSampleRate);
    }

    // -------------------------------------------------------------------------------------------------------

private:
   /**
      Set our plugin class as non-copyable and add a leak detector just in case.
    */
    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CardinalPlugin)
};

CardinalPluginContext* getRackContextFromPlugin(void* const ptr)
{
    return static_cast<CardinalPlugin*>(ptr)->getRackContext();
}

/* ------------------------------------------------------------------------------------------------------------
 * Plugin entry point, called by DPF to create a new plugin instance. */

Plugin* createPlugin()
{
    return new CardinalPlugin();
}

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
