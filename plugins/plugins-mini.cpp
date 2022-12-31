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

#include "rack.hpp"
#include "plugin.hpp"

#include "DistrhoUtils.hpp"

// Cardinal (built-in)
#include "Cardinal/src/plugin.hpp"

// Fundamental
#include "Fundamental/src/plugin.hpp"

// Aria
extern Model* modelSpleet;
extern Model* modelSwerge;

// AudibleInstruments
#include "AudibleInstruments/src/plugin.hpp"

// BogaudioModules - integrate theme/skin support
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#define private public
#include "BogaudioModules/src/skins.hpp"
#undef private

// BogaudioModules
extern Model* modelBogaudioLFO;
extern Model* modelBogaudioNoise;
extern Model* modelBogaudioVCA;
extern Model* modelBogaudioVCF;
extern Model* modelBogaudioVCO;
extern Model* modelOffset;
extern Model* modelSampleHold;
extern Model* modelSwitch;
extern Model* modelSwitch18;
extern Model* modelUnison;

// ValleyAudio
#include "ValleyAudio/src/Valley.hpp"

// known terminal modules
std::vector<Model*> hostTerminalModels;

// plugin instances
Plugin* pluginInstance__Cardinal;
Plugin* pluginInstance__Fundamental;
Plugin* pluginInstance__Aria;
Plugin* pluginInstance__AudibleInstruments;
Plugin* pluginInstance__BogaudioModules;
Plugin* pluginInstance__ValleyAudio;

namespace rack {

namespace asset {
std::string pluginManifest(const std::string& dirname);
std::string pluginPath(const std::string& dirname);
}

namespace plugin {

struct StaticPluginLoader {
    Plugin* const plugin;
    FILE* file;
    json_t* rootJ;

    StaticPluginLoader(Plugin* const p, const char* const name)
        : plugin(p),
          file(nullptr),
          rootJ(nullptr)
    {
#ifdef DEBUG
        DEBUG("Loading plugin module %s", name);
#endif

        p->path = asset::pluginPath(name);

        const std::string manifestFilename = asset::pluginManifest(name);

        if ((file = std::fopen(manifestFilename.c_str(), "r")) == nullptr)
        {
            d_stderr2("Manifest file %s does not exist", manifestFilename.c_str());
            return;
        }

        json_error_t error;
        if ((rootJ = json_loadf(file, 0, &error)) == nullptr)
        {
            d_stderr2("JSON parsing error at %s %d:%d %s", manifestFilename.c_str(), error.line, error.column, error.text);
            return;
        }

        // force ABI, we use static plugins so this doesnt matter as long as it builds
        json_t* const version = json_string((APP_VERSION_MAJOR + ".0").c_str());
        json_object_set(rootJ, "version", version);
        json_decref(version);

        // Load manifest
        p->fromJson(rootJ);

        // Reject plugin if slug already exists
        if (Plugin* const existingPlugin = getPlugin(p->slug))
            throw Exception("Plugin %s is already loaded, not attempting to load it again", p->slug.c_str());
    }

    ~StaticPluginLoader()
    {
        if (rootJ != nullptr)
        {
            // Load modules manifest
            json_t* const modulesJ = json_object_get(rootJ, "modules");
            plugin->modulesFromJson(modulesJ);

            json_decref(rootJ);
            plugins.push_back(plugin);
        }

        if (file != nullptr)
            std::fclose(file);
    }

    bool ok() const noexcept
    {
        return rootJ != nullptr;
    }

    void removeModule(const char* const slugToRemove) const noexcept
    {
        json_t* const modules = json_object_get(rootJ, "modules");
        DISTRHO_SAFE_ASSERT_RETURN(modules != nullptr,);

        size_t i;
        json_t* v;
        json_array_foreach(modules, i, v)
        {
            if (json_t* const slug = json_object_get(v, "slug"))
            {
                if (const char* const value = json_string_value(slug))
                {
                    if (std::strcmp(value, slugToRemove) == 0)
                    {
                        json_array_remove(modules, i);
                        break;
                    }
                }
            }
        }
    }
};

static void initStatic__Cardinal()
{
    Plugin* const p = new Plugin;
    pluginInstance__Cardinal = p;

    const StaticPluginLoader spl(p, "Cardinal");
    if (spl.ok())
    {
        p->addModel(modelHostAudio2);
        p->addModel(modelHostCV);
        p->addModel(modelHostMIDI);
        p->addModel(modelHostMIDICC);
        p->addModel(modelHostMIDIGate);
        p->addModel(modelHostMIDIMap);
        p->addModel(modelHostParameters);
        p->addModel(modelHostParametersMap);
        p->addModel(modelHostTime);
        p->addModel(modelTextEditor);
        /* TODO
       #ifdef HAVE_FFTW3F
        p->addModel(modelAudioToCVPitch);
       #else
        */
        spl.removeModule("AudioToCVPitch");
        /*
       #endif
        */
        spl.removeModule("AudioFile");
        spl.removeModule("Blank");
        spl.removeModule("Carla");
        spl.removeModule("ExpanderInputMIDI");
        spl.removeModule("ExpanderOutputMIDI");
        spl.removeModule("HostAudio8");
        spl.removeModule("Ildaeil");
        spl.removeModule("MPV");
        spl.removeModule("SassyScope");
        spl.removeModule("glBars");

        hostTerminalModels = {
            modelHostAudio2,
            modelHostCV,
            modelHostMIDI,
            modelHostMIDICC,
            modelHostMIDIGate,
            modelHostMIDIMap,
            modelHostParameters,
            modelHostParametersMap,
            modelHostTime,
        };
    }
}

static void initStatic__Fundamental()
{
    Plugin* const p = new Plugin;
    pluginInstance__Fundamental = p;

    const StaticPluginLoader spl(p, "Fundamental");
    if (spl.ok())
    {
        p->addModel(modelADSR);
        p->addModel(modelLFO);
        p->addModel(modelMerge);
        p->addModel(modelNoise);
        p->addModel(modelQuantizer);
        p->addModel(modelRandom);
        p->addModel(modelScope);
        p->addModel(modelSplit);
        p->addModel(modelVCA_1);
        p->addModel(modelVCF);
        p->addModel(modelVCMixer);
        p->addModel(modelVCO);
        spl.removeModule("8vert");
        spl.removeModule("Delay");
        spl.removeModule("LFO2");
        spl.removeModule("MidSide");
        spl.removeModule("Mixer");
        spl.removeModule("Mutes");
        spl.removeModule("Octave");
        spl.removeModule("Pulses");
        spl.removeModule("SEQ3");
        spl.removeModule("SequentialSwitch1");
        spl.removeModule("SequentialSwitch2");
        spl.removeModule("Sum");
        spl.removeModule("VCA");
        spl.removeModule("VCO2");
    }
}

static void initStatic__Aria()
{
    Plugin* const p = new Plugin;
    pluginInstance__Aria = p;

    const StaticPluginLoader spl(p, "AriaModules");
    if (spl.ok())
    {
        p->addModel(modelSpleet);
        p->addModel(modelSwerge);

        spl.removeModule("Aleister");
        spl.removeModule("Arcane");
        spl.removeModule("Atout");
        spl.removeModule("Blank");
        spl.removeModule("Darius");
        spl.removeModule("Grabby");
        spl.removeModule("Pokies4");
        spl.removeModule("Psychopump");
        spl.removeModule("Q");
        spl.removeModule("Qqqq");
        spl.removeModule("Quack");
        spl.removeModule("Quale");
        spl.removeModule("Rotatoes4");
        spl.removeModule("Smerge");
        spl.removeModule("Solomon16");
        spl.removeModule("Solomon4");
        spl.removeModule("Solomon8");
        spl.removeModule("Splirge");
        spl.removeModule("Splort");
        spl.removeModule("Undular");

    }
}

static void initStatic__AudibleInstruments()
{
    Plugin* const p = new Plugin;
    pluginInstance__AudibleInstruments = p;

    const StaticPluginLoader spl(p, "AudibleInstruments");
    if (spl.ok())
    {
        p->addModel(modelBraids);
        p->addModel(modelPlaits);

        spl.removeModule("Blinds");
        spl.removeModule("Branches");
        spl.removeModule("Clouds");
        spl.removeModule("Elements");
        spl.removeModule("Frames");
        spl.removeModule("Kinks");
        spl.removeModule("Links");
        spl.removeModule("Marbles");
        spl.removeModule("Rings");
        spl.removeModule("Ripples");
        spl.removeModule("Shades");
        spl.removeModule("Shelves");
        spl.removeModule("Stages");
        spl.removeModule("Streams");
        spl.removeModule("Tides");
        spl.removeModule("Tides2");
        spl.removeModule("Veils");
        spl.removeModule("Warps");
    }
}

static void initStatic__BogaudioModules()
{
    Plugin* const p = new Plugin;
    pluginInstance__BogaudioModules = p;

    const StaticPluginLoader spl(p, "BogaudioModules");
    if (spl.ok())
    {
        // Make sure to use match Cardinal theme
        Skins& skins(Skins::skins());
        skins._default = settings::darkMode ? "dark" : "light";

        p->addModel(modelBogaudioLFO);
        p->addModel(modelBogaudioNoise);
        p->addModel(modelBogaudioVCA);
        p->addModel(modelBogaudioVCF);
        p->addModel(modelBogaudioVCO);
        p->addModel(modelOffset);
        p->addModel(modelSampleHold);
        p->addModel(modelSwitch);
        p->addModel(modelSwitch18);
        p->addModel(modelUnison);

        // cat plugins/BogaudioModules/plugin.json  | jq -r .modules[].slug - | sort
        spl.removeModule("Bogaudio-AD");
        spl.removeModule("Bogaudio-Additator");
        spl.removeModule("Bogaudio-AddrSeq");
        spl.removeModule("Bogaudio-AddrSeqX");
        spl.removeModule("Bogaudio-ADSR");
        spl.removeModule("Bogaudio-AMRM");
        spl.removeModule("Bogaudio-Analyzer");
        spl.removeModule("Bogaudio-AnalyzerXL");
        spl.removeModule("Bogaudio-Arp");
        spl.removeModule("Bogaudio-ASR");
        spl.removeModule("Bogaudio-Assign");
        spl.removeModule("Bogaudio-Blank3");
        spl.removeModule("Bogaudio-Blank6");
        spl.removeModule("Bogaudio-Bool");
        spl.removeModule("Bogaudio-Chirp");
        spl.removeModule("Bogaudio-Clpr");
        spl.removeModule("Bogaudio-Cmp");
        spl.removeModule("Bogaudio-CmpDist");
        spl.removeModule("Bogaudio-CVD");
        spl.removeModule("Bogaudio-DADSRH");
        spl.removeModule("Bogaudio-DADSRHPlus");
        spl.removeModule("Bogaudio-Detune");
        spl.removeModule("Bogaudio-DGate");
        spl.removeModule("Bogaudio-Edge");
        spl.removeModule("Bogaudio-EightFO");
        spl.removeModule("Bogaudio-EightOne");
        spl.removeModule("Bogaudio-EQ");
        spl.removeModule("Bogaudio-EQS");
        spl.removeModule("Bogaudio-FFB");
        spl.removeModule("Bogaudio-FlipFlop");
        spl.removeModule("Bogaudio-FMOp");
        spl.removeModule("Bogaudio-Follow");
        spl.removeModule("Bogaudio-FourFO");
        spl.removeModule("Bogaudio-FourMan");
        spl.removeModule("Bogaudio-Inv");
        spl.removeModule("Bogaudio-Lgsw");
        spl.removeModule("Bogaudio-LLFO");
        spl.removeModule("Bogaudio-LLPG");
        spl.removeModule("Bogaudio-Lmtr");
        spl.removeModule("Bogaudio-LPG");
        spl.removeModule("Bogaudio-LVCF");
        spl.removeModule("Bogaudio-LVCO");
        spl.removeModule("Bogaudio-Manual");
        spl.removeModule("Bogaudio-Matrix18");
        spl.removeModule("Bogaudio-Matrix44");
        spl.removeModule("Bogaudio-Matrix44Cvm");
        spl.removeModule("Bogaudio-Matrix81");
        spl.removeModule("Bogaudio-Matrix88");
        spl.removeModule("Bogaudio-Matrix88Cv");
        spl.removeModule("Bogaudio-Matrix88M");
        spl.removeModule("Bogaudio-MegaGate");
        spl.removeModule("Bogaudio-Mix1");
        spl.removeModule("Bogaudio-Mix2");
        spl.removeModule("Bogaudio-Mix4");
        spl.removeModule("Bogaudio-Mix4x");
        spl.removeModule("Bogaudio-Mix8");
        spl.removeModule("Bogaudio-Mix8x");
        spl.removeModule("Bogaudio-Mono");
        spl.removeModule("Bogaudio-Mult");
        spl.removeModule("Bogaudio-Mumix");
        spl.removeModule("Bogaudio-Mute8");
        spl.removeModule("Bogaudio-Nsgt");
        spl.removeModule("Bogaudio-OneEight");
        spl.removeModule("Bogaudio-Pan");
        spl.removeModule("Bogaudio-PEQ");
        spl.removeModule("Bogaudio-PEQ14");
        spl.removeModule("Bogaudio-PEQ14XF");
        spl.removeModule("Bogaudio-PEQ6");
        spl.removeModule("Bogaudio-PEQ6XF");
        spl.removeModule("Bogaudio-Pgmr");
        spl.removeModule("Bogaudio-PgmrX");
        spl.removeModule("Bogaudio-PolyCon");
        spl.removeModule("Bogaudio-PolyCon8");
        spl.removeModule("Bogaudio-PolyMult");
        spl.removeModule("Bogaudio-PolyOff16");
        spl.removeModule("Bogaudio-PolyOff8");
        spl.removeModule("Bogaudio-Pressor");
        spl.removeModule("Bogaudio-Pulse");
        spl.removeModule("Bogaudio-Ranalyzer");
        spl.removeModule("Bogaudio-Reftone");
        spl.removeModule("Bogaudio-RGate");
        spl.removeModule("Bogaudio-Shaper");
        spl.removeModule("Bogaudio-ShaperPlus");
        spl.removeModule("Bogaudio-Sine");
        spl.removeModule("Bogaudio-Slew");
        spl.removeModule("Bogaudio-Stack");
        spl.removeModule("Bogaudio-Sums");
        spl.removeModule("Bogaudio-Switch1616");
        spl.removeModule("Bogaudio-Switch44");
        spl.removeModule("Bogaudio-Switch81");
        spl.removeModule("Bogaudio-Switch88");
        spl.removeModule("Bogaudio-UMix");
        spl.removeModule("Bogaudio-VCAmp");
        spl.removeModule("Bogaudio-VCM");
        spl.removeModule("Bogaudio-Velo");
        spl.removeModule("Bogaudio-Vish");
        spl.removeModule("Bogaudio-VU");
        spl.removeModule("Bogaudio-Walk");
        spl.removeModule("Bogaudio-Walk2");
        spl.removeModule("Bogaudio-XCO");
        spl.removeModule("Bogaudio-XFade");
    }
}

static void initStatic__ValleyAudio()
{
    Plugin* const p = new Plugin;
    pluginInstance__ValleyAudio = p;

    const StaticPluginLoader spl(p, "ValleyAudio");
    if (spl.ok())
    {
        p->addModel(modelDexter);
        p->addModel(modelInterzone);

        spl.removeModule("Amalgam");
        spl.removeModule("Feline");
        spl.removeModule("Plateau");
        spl.removeModule("Terrorform");
        spl.removeModule("Topograph");
        spl.removeModule("uGraph");
    }
}

void initStaticPlugins()
{
    initStatic__Cardinal();
    initStatic__Fundamental();
    initStatic__Aria();
    initStatic__AudibleInstruments();
    initStatic__BogaudioModules();
    initStatic__ValleyAudio();
}

void destroyStaticPlugins()
{
    for (Plugin* p : plugins)
        delete p;
    plugins.clear();
}

void updateStaticPluginsDarkMode()
{
    const bool darkMode = settings::darkMode;
    // bogaudio
    {
        Skins& skins(Skins::skins());
        skins._default = darkMode ? "dark" : "light";

        std::lock_guard<std::mutex> lock(skins._defaultSkinListenersLock);
        for (auto listener : skins._defaultSkinListeners) {
            listener->defaultSkinChanged(skins._default);
        }
    }
}

}
}
