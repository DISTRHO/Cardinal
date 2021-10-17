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

#include <plugin.hpp>

#include "DistrhoUtils.hpp"

// AnimatedCircuits
#include "AnimatedCircuits/src/plugin.hpp"

// AudibleInstruments
#include "AudibleInstruments/src/plugin.hpp"

// Befaco
#include "Befaco/src/plugin.hpp"

// BogaudioModules
#define modelADSR modelBogaudioADSR
#define modelLFO modelBogaudioLFO
#define modelNoise modelBogaudioNoise
#define modelVCA modelBogaudioVCA
#define modelVCF modelBogaudioVCF
#define modelVCO modelBogaudioVCO
#include "BogaudioModules/src/AD.hpp"
#include "BogaudioModules/src/Additator.hpp"
#include "BogaudioModules/src/AddrSeqX.hpp"
#include "BogaudioModules/src/ADSR.hpp"
#include "BogaudioModules/src/AMRM.hpp"
#include "BogaudioModules/src/Analyzer.hpp"
#include "BogaudioModules/src/AnalyzerXL.hpp"
#include "BogaudioModules/src/Assign.hpp"
#include "BogaudioModules/src/ASR.hpp"
#include "BogaudioModules/src/Arp.hpp"
#include "BogaudioModules/src/Blank3.hpp"
#include "BogaudioModules/src/Blank6.hpp"
#include "BogaudioModules/src/Bool.hpp"
#include "BogaudioModules/src/Chirp.hpp"
#include "BogaudioModules/src/Clpr.hpp"
#include "BogaudioModules/src/Cmp.hpp"
#include "BogaudioModules/src/CmpDist.hpp"
#include "BogaudioModules/src/CVD.hpp"
#include "BogaudioModules/src/DADSRH.hpp"
#include "BogaudioModules/src/DADSRHPlus.hpp"
#include "BogaudioModules/src/Detune.hpp"
#include "BogaudioModules/src/DGate.hpp"
#include "BogaudioModules/src/Edge.hpp"
#include "BogaudioModules/src/EightFO.hpp"
#include "BogaudioModules/src/EightOne.hpp"
#include "BogaudioModules/src/EQ.hpp"
#include "BogaudioModules/src/EQS.hpp"
#include "BogaudioModules/src/FFB.hpp"
#include "BogaudioModules/src/FlipFlop.hpp"
#include "BogaudioModules/src/FMOp.hpp"
#include "BogaudioModules/src/Follow.hpp"
#include "BogaudioModules/src/FourFO.hpp"
#include "BogaudioModules/src/FourMan.hpp"
#include "BogaudioModules/src/Inv.hpp"
#include "BogaudioModules/src/LFO.hpp"
#include "BogaudioModules/src/Lgsw.hpp"
#include "BogaudioModules/src/LLFO.hpp"
#include "BogaudioModules/src/LLPG.hpp"
#include "BogaudioModules/src/Lmtr.hpp"
#include "BogaudioModules/src/LPG.hpp"
#include "BogaudioModules/src/LVCF.hpp"
#include "BogaudioModules/src/LVCO.hpp"
#include "BogaudioModules/src/Manual.hpp"
#include "BogaudioModules/src/Matrix18.hpp"
#include "BogaudioModules/src/Matrix44.hpp"
#include "BogaudioModules/src/Matrix81.hpp"
#include "BogaudioModules/src/Matrix88.hpp"
#include "BogaudioModules/src/Matrix88Cv.hpp"
#include "BogaudioModules/src/Matrix88M.hpp"
#include "BogaudioModules/src/MegaGate.hpp"
#include "BogaudioModules/src/Mix1.hpp"
#include "BogaudioModules/src/Mix2.hpp"
#include "BogaudioModules/src/Mix4.hpp"
#include "BogaudioModules/src/Mix4x.hpp"
#include "BogaudioModules/src/Mix8x.hpp"
#include "BogaudioModules/src/Mono.hpp"
#include "BogaudioModules/src/Mult.hpp"
#include "BogaudioModules/src/Mumix.hpp"
#include "BogaudioModules/src/Mute8.hpp"
#include "BogaudioModules/src/Noise.hpp"
#include "BogaudioModules/src/Nsgt.hpp"
#include "BogaudioModules/src/Offset.hpp"
#include "BogaudioModules/src/OneEight.hpp"
#include "BogaudioModules/src/Pan.hpp"
#include "BogaudioModules/src/PEQ.hpp"
#include "BogaudioModules/src/PEQ6.hpp"
#include "BogaudioModules/src/PEQ6XF.hpp"
#include "BogaudioModules/src/PEQ14.hpp"
#include "BogaudioModules/src/PEQ14XF.hpp"
#include "BogaudioModules/src/Pgmr.hpp"
#include "BogaudioModules/src/PgmrX.hpp"
#include "BogaudioModules/src/PolyCon16.hpp"
#include "BogaudioModules/src/PolyCon8.hpp"
#include "BogaudioModules/src/PolyMult.hpp"
#include "BogaudioModules/src/PolyOff16.hpp"
#include "BogaudioModules/src/PolyOff8.hpp"
#include "BogaudioModules/src/Pressor.hpp"
#include "BogaudioModules/src/Pulse.hpp"
#include "BogaudioModules/src/Ranalyzer.hpp"
#include "BogaudioModules/src/Reftone.hpp"
#include "BogaudioModules/src/RGate.hpp"
#include "BogaudioModules/src/SampleHold.hpp"
#include "BogaudioModules/src/Shaper.hpp"
#include "BogaudioModules/src/ShaperPlus.hpp"
#include "BogaudioModules/src/Sine.hpp"
#include "BogaudioModules/src/Slew.hpp"
#include "BogaudioModules/src/Stack.hpp"
#include "BogaudioModules/src/Sums.hpp"
#include "BogaudioModules/src/Switch.hpp"
#include "BogaudioModules/src/Switch1616.hpp"
#include "BogaudioModules/src/Switch18.hpp"
#include "BogaudioModules/src/Switch44.hpp"
#include "BogaudioModules/src/Switch81.hpp"
#include "BogaudioModules/src/Switch88.hpp"
#include "BogaudioModules/src/UMix.hpp"
#include "BogaudioModules/src/Unison.hpp"
#include "BogaudioModules/src/VCA.hpp"
#include "BogaudioModules/src/VCAmp.hpp"
#include "BogaudioModules/src/VCF.hpp"
#include "BogaudioModules/src/VCM.hpp"
#include "BogaudioModules/src/VCO.hpp"
#include "BogaudioModules/src/Velo.hpp"
#include "BogaudioModules/src/Vish.hpp"
#include "BogaudioModules/src/VU.hpp"
#include "BogaudioModules/src/Walk.hpp"
#include "BogaudioModules/src/Walk2.hpp"
#include "BogaudioModules/src/XCO.hpp"
#include "BogaudioModules/src/XFade.hpp"
#undef modelADSR
#undef modelLFO
#undef modelNoise
#undef modelVCA
#undef modelVCF
#undef modelVCO

// Fundamental
#include "Fundamental/src/plugin.hpp"

// GrandeModular
#include "GrandeModular/src/plugin.hpp"

// ZetaCarinaeModules
#include "ZetaCarinaeModules/src/plugin.hpp"

Plugin* pluginInstance__AnimatedCircuits;
Plugin* pluginInstance__AudibleInstruments;
Plugin* pluginInstance__Befaco;
Plugin* pluginInstance__BogaudioModules;
Plugin* pluginInstance__Fundamental;
Plugin* pluginInstance__GrandeModular;
Plugin* pluginInstance__ZetaCarinaeModules;

namespace rack {

// core plugins
namespace core {
extern Model* modelAudioInterface2;
extern Model* modelMIDI_CV;
extern Model* modelMIDI_CC;
extern Model* modelMIDI_Gate;
extern Model* modelMIDI_Map;
extern Model* modelCV_MIDI;
extern Model* modelCV_CC;
extern Model* modelCV_Gate;
extern Model* modelBlank;
extern Model* modelNotes;
}

// regular plugins
namespace plugin {

struct StaticPluginLoader {
    Plugin* const plugin;
    FILE* file;
    json_t* rootJ;

    // core
    StaticPluginLoader(Plugin* const p)
        : plugin(p),
          file(nullptr),
          rootJ(nullptr)
    {
        p->path = system::join(CARDINAL_PLUGINS_DIR, "Core.json");

        if ((file = std::fopen(p->path.c_str(), "r")) == nullptr)
        {
            d_stderr2("Manifest file %s does not exist", p->path.c_str());
            return;
        }

        json_error_t error;
        if ((rootJ = json_loadf(file, 0, &error)) == nullptr)
        {
            d_stderr2("JSON parsing error at %s %d:%d %s", p->path.c_str(), error.line, error.column, error.text);
            return;
        }

        // force ABI, we use static plugins so this doesnt matter as long as it builds
        json_t* const version = json_string((APP_VERSION_MAJOR + ".0").c_str());
        json_object_set(rootJ, "version", version);
        json_decref(version);
    }

    // regular plugins
    StaticPluginLoader(Plugin* const p, const char* const name)
        : plugin(p),
          file(nullptr),
          rootJ(nullptr)
    {
        p->path = system::join(CARDINAL_PLUGINS_DIR, name);

        const std::string manifestFilename = system::join(p->path, "plugin.json");

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
    }

    ~StaticPluginLoader()
    {
        if (rootJ != nullptr)
        {
            plugin->fromJson(rootJ);
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
};

static void initStatic__Core()
{
    Plugin* const p = new Plugin;

    const StaticPluginLoader spl(p);
    if (spl.ok())
    {
        p->addModel(rack::core::modelAudioInterface2);
        p->addModel(rack::core::modelMIDI_CV);
        p->addModel(rack::core::modelMIDI_CC);
        p->addModel(rack::core::modelMIDI_Gate);
        p->addModel(rack::core::modelMIDI_Map);
        p->addModel(rack::core::modelCV_MIDI);
        p->addModel(rack::core::modelCV_CC);
        p->addModel(rack::core::modelCV_Gate);
        p->addModel(rack::core::modelBlank);
        p->addModel(rack::core::modelNotes);
    }
}

static void initStatic__AnimatedCircuits()
{
    Plugin* const p = new Plugin;
    pluginInstance__AnimatedCircuits = p;

    const StaticPluginLoader spl(p, "AnimatedCircuits");
    if (spl.ok())
    {
        p->addModel(model_AC_Folding);
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
        p->addModel(modelElements);
        p->addModel(modelTides);
        p->addModel(modelTides2);
        p->addModel(modelClouds);
        p->addModel(modelWarps);
        p->addModel(modelRings);
        p->addModel(modelLinks);
        p->addModel(modelKinks);
        p->addModel(modelShades);
        p->addModel(modelBranches);
        p->addModel(modelBlinds);
        p->addModel(modelVeils);
        p->addModel(modelFrames);
        p->addModel(modelMarbles);
        p->addModel(modelStages);
        p->addModel(modelRipples);
        p->addModel(modelShelves);
        p->addModel(modelStreams);
    }
}

static void initStatic__Befaco()
{
    Plugin* const p = new Plugin;
    pluginInstance__Befaco = p;

    const StaticPluginLoader spl(p, "Befaco");
    if (spl.ok())
    {
        p->addModel(modelEvenVCO);
        p->addModel(modelRampage);
        p->addModel(modelABC);
        p->addModel(modelSpringReverb);
        p->addModel(modelMixer);
        p->addModel(modelSlewLimiter);
        p->addModel(modelDualAtenuverter);
    }
}

static void initStatic__BogaudioModules()
{
    Plugin* const p = new Plugin;
    pluginInstance__BogaudioModules = p;

    const StaticPluginLoader spl(p, "BogaudioModules");
    if (spl.ok())
    {
#define modelADSR modelBogaudioADSR
#define modelLFO modelBogaudioLFO
#define modelNoise modelBogaudioNoise
#define modelVCA modelBogaudioVCA
#define modelVCF modelBogaudioVCF
#define modelVCO modelBogaudioVCO
        p->addModel(modelVCO);
        p->addModel(modelLVCO);
        p->addModel(modelSine);
        p->addModel(modelPulse);
        p->addModel(modelXCO);
        p->addModel(modelAdditator);
        p->addModel(modelFMOp);
        p->addModel(modelChirp);
        p->addModel(modelLFO);
        p->addModel(modelLLFO);
        p->addModel(modelFourFO);
        p->addModel(modelEightFO);
        p->addModel(modelVCF);
        p->addModel(modelLVCF);
        p->addModel(modelFFB);
        p->addModel(modelEQ);
        p->addModel(modelEQS);
        p->addModel(modelLPG);
        p->addModel(modelLLPG);
        p->addModel(modelMegaGate);
        p->addModel(modelPEQ);
        p->addModel(modelPEQ6);
        p->addModel(modelPEQ6XF);
        p->addModel(modelPEQ14);
        p->addModel(modelPEQ14XF);
        p->addModel(modelDADSRH);
        p->addModel(modelDADSRHPlus);
        p->addModel(modelShaper);
        p->addModel(modelShaperPlus);
        p->addModel(modelAD);
        p->addModel(modelASR);
        p->addModel(modelADSR);
        p->addModel(modelVish);
        p->addModel(modelFollow);
        p->addModel(modelDGate);
        p->addModel(modelRGate);
        p->addModel(modelEdge);
        p->addModel(modelNoise);
        p->addModel(modelSampleHold);
        p->addModel(modelWalk2);
        p->addModel(modelWalk);
        p->addModel(modelMix8);
        p->addModel(modelMix8x);
        p->addModel(modelMix4);
        p->addModel(modelMix4x);
        p->addModel(modelMix2);
        p->addModel(modelMix1);
        p->addModel(modelVCM);
        p->addModel(modelMute8);
        p->addModel(modelPan);
        p->addModel(modelXFade);
        p->addModel(modelVCA);
        p->addModel(modelVCAmp);
        p->addModel(modelVelo);
        p->addModel(modelUMix);
        p->addModel(modelMumix);
        p->addModel(modelMatrix81);
        p->addModel(modelMatrix18);
        p->addModel(modelMatrix44);
        p->addModel(modelMatrix44Cvm);
        p->addModel(modelMatrix88);
        p->addModel(modelMatrix88Cv);
        p->addModel(modelMatrix88M);
        p->addModel(modelSwitch81);
        p->addModel(modelSwitch18);
        p->addModel(modelSwitch44);
        p->addModel(modelSwitch88);
        p->addModel(modelSwitch1616);
        p->addModel(modelAMRM);
        p->addModel(modelPressor);
        p->addModel(modelClpr);
        p->addModel(modelLmtr);
        p->addModel(modelNsgt);
        p->addModel(modelCmpDist);
        p->addModel(modelOneEight);
        p->addModel(modelEightOne);
        p->addModel(modelAddrSeq);
        p->addModel(modelAddrSeqX);
        p->addModel(modelPgmr);
        p->addModel(modelPgmrX);
        p->addModel(modelVU);
        p->addModel(modelAnalyzer);
        p->addModel(modelAnalyzerXL);
        p->addModel(modelRanalyzer);
        p->addModel(modelDetune);
        p->addModel(modelStack);
        p->addModel(modelReftone);
        p->addModel(modelMono);
        p->addModel(modelArp);
        p->addModel(modelAssign);
        p->addModel(modelUnison);
        p->addModel(modelPolyCon8);
        p->addModel(modelPolyCon16);
        p->addModel(modelPolyOff8);
        p->addModel(modelPolyOff16);
        p->addModel(modelPolyMult);
        p->addModel(modelBool);
        p->addModel(modelCmp);
        p->addModel(modelCVD);
        p->addModel(modelFlipFlop);
        p->addModel(modelInv);
        p->addModel(modelManual);
        p->addModel(modelFourMan);
        p->addModel(modelMult);
        p->addModel(modelOffset);
        p->addModel(modelSlew);
        p->addModel(modelSums);
        p->addModel(modelSwitch);
        p->addModel(modelLgsw);
        p->addModel(modelBlank3);
        p->addModel(modelBlank6);
#ifdef EXPERIMENTAL
        p->addModel(modelLag);
        p->addModel(modelPEQ14XR);
        p->addModel(modelPEQ14XV);
#endif
#ifdef TEST
        p->addModel(modelTest);
        p->addModel(modelTest2);
        p->addModel(modelTestExpanderBase);
        p->addModel(modelTestExpanderExtension);
        p->addModel(modelTestGl);
        p->addModel(modelTestVCF);
#endif
#undef modelADSR
#undef modelLFO
#undef modelNoise
#undef modelVCA
#undef modelVCF
#undef modelVCO
    }
}

static void initStatic__Fundamental()
{
    Plugin* const p = new Plugin;
    pluginInstance__Fundamental = p;

    const StaticPluginLoader spl(p, "Fundamental");
    if (spl.ok())
    {
        p->addModel(modelVCO);
        p->addModel(modelVCO2);
        p->addModel(modelVCF);
        p->addModel(modelVCA_1);
        p->addModel(modelVCA);
        p->addModel(modelLFO);
        p->addModel(modelLFO2);
        p->addModel(modelDelay);
        p->addModel(modelADSR);
        p->addModel(modelVCMixer);
        p->addModel(model_8vert);
        p->addModel(modelUnity);
        p->addModel(modelMutes);
        p->addModel(modelPulses);
        p->addModel(modelScope);
        p->addModel(modelSEQ3);
        p->addModel(modelSequentialSwitch1);
        p->addModel(modelSequentialSwitch2);
        p->addModel(modelOctave);
        p->addModel(modelQuantizer);
        p->addModel(modelSplit);
        p->addModel(modelMerge);
        p->addModel(modelSum);
        p->addModel(modelViz);
        p->addModel(modelMidSide);
        p->addModel(modelNoise);
        p->addModel(modelRandom);
    }
}

static void initStatic__GrandeModular()
{
    Plugin* const p = new Plugin;
    pluginInstance__GrandeModular = p;

    const StaticPluginLoader spl(p, "GrandeModular");
    if (spl.ok())
    {
        p->addModel(modelClip);
        p->addModel(modelMergeSplit4);
        p->addModel(modelMicrotonalChords);
        p->addModel(modelMicrotonalNotes);
        p->addModel(modelNoteMT);
        p->addModel(modelPolyMergeResplit);
        p->addModel(modelQuant);
        p->addModel(modelQuantIntervals);
        p->addModel(modelQuantMT);
        p->addModel(modelSampleDelays);
        p->addModel(modelScale);
        p->addModel(modelTails);
        p->addModel(modelVarSampleDelays);
    }
}

static void initStatic__ZetaCarinaeModules()
{
    Plugin* p = new Plugin;
    pluginInstance__ZetaCarinaeModules = p;

    const StaticPluginLoader spl(p, "ZetaCarinaeModules");
    if (spl.ok())
    {
        p->addModel(modelBrownianBridge);
        p->addModel(modelOrnsteinUhlenbeck);
        p->addModel(modelIOU);
        p->addModel(modelWarbler);
        p->addModel(modelRosenchance);
        p->addModel(modelGuildensTurn);
        p->addModel(modelRosslerRustler);
        p->addModel(modelFirefly);
    }
}

void initStaticPlugins()
{
    initStatic__Core();
    initStatic__AnimatedCircuits();
    initStatic__AudibleInstruments();
    initStatic__Befaco();
    initStatic__BogaudioModules();
    initStatic__Fundamental();
    initStatic__GrandeModular();
    initStatic__ZetaCarinaeModules();
}

void destroyStaticPlugins()
{
    for (Plugin* p : plugins)
        delete p;
    plugins.clear();
}

}
}
