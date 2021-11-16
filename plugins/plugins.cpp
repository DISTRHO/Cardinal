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

#include "rack.hpp"
#include "plugin.hpp"

#include "DistrhoUtils.hpp"

// Cardinal (built-in)
#include "Cardinal/src/plugin.hpp"

#ifndef NOPLUGINS
// AmalgamatedHarmonics
#include "AmalgamatedHarmonics/src/AH.hpp"

// AnimatedCircuits
#include "AnimatedCircuits/src/plugin.hpp"

// AS
#define modelADSR modelASADSR
#define modelVCA modelASVCA
#include "AS/src/AS.hpp"
#undef modelADSR
#undef modelVCA

// Atelier
#include "Atelier/src/plugin.hpp"

// AudibleInstruments
#include "AudibleInstruments/src/plugin.hpp"

// Befaco
#define modelADSR modelBefacoADSR
#include "Befaco/src/plugin.hpp"
#undef modelADSR

// Bidoo
#include "Bidoo/src/plugin.hpp"

// BogaudioModules - force dark skin as default
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#define private public
#include "BogaudioModules/src/skins.hpp"
#undef private

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

// cf
#include "cf/src/plugin.hpp"

// DrumKit
#include "DrumKit/src/DrumKit.hpp"
void setupSamples();

// ESeries
#include "ESeries/src/plugin.hpp"

// Fundamental
#include "Fundamental/src/plugin.hpp"

// GrandeModular
#include "GrandeModular/src/plugin.hpp"

// ImpromptuModular
/* NOTE too much noise in original include, do this a different way
// #include "ImpromptuModular/src/ImpromptuModular.hpp"
*/
extern Model *modelBigButtonSeq;
extern Model *modelBigButtonSeq2;
extern Model *modelChordKey;
extern Model *modelChordKeyExpander;
extern Model *modelClocked;
extern Model *modelClockedExpander;
extern Model *modelClkd;
extern Model *modelCvPad;
extern Model *modelFoundry;
extern Model *modelFoundryExpander;
extern Model *modelFourView;
extern Model *modelGateSeq64;
extern Model *modelGateSeq64Expander;
extern Model *modelHotkey;
extern Model *modelPart;
extern Model *modelPhraseSeq16;
extern Model *modelPhraseSeq32;
extern Model *modelPhraseSeqExpander;
extern Model *modelProbKey;
// extern Model *modelProbKeyExpander;
extern Model *modelSemiModularSynth;
extern Model *modelTact;
extern Model *modelTact1;
extern Model *modelTactG;
extern Model *modelTwelveKey;
extern Model *modelWriteSeq32;
extern Model *modelWriteSeq64;
extern Model *modelBlankPanel;

// JW-Modules
#define modelQuantizer modelJWQuantizer
#include "JW-Modules/src/JWModules.hpp"
#undef modelQuantizer

// MindMeldModular
/* NOTE too much noise in original include, do this a different way
// #include "MindMeldModular/src/MindMeldModular.hpp"
*/
extern Model *modelMixMasterJr;
extern Model *modelAuxExpanderJr;
extern Model *modelMixMaster;
extern Model *modelAuxExpander;
extern Model *modelMeld;
extern Model *modelUnmeld;
extern Model *modelEqMaster;
extern Model *modelEqExpander;
extern Model *modelBassMaster;
extern Model *modelBassMasterJr;
extern Model *modelShapeMaster;

// mscHack
/* NOTE too much noise in original include, do this a different way
// #include "mscHack/src/mscHack.hpp"
*/
extern Model *modelCompressor;
extern Model *modelSynthDrums;
extern Model *modelSEQ_6x32x16;
extern Model *modelMasterClockx8;
extern Model *modelMasterClockx4;
extern Model *modelSEQ_Envelope_8;
extern Model *modelSeq_Triad2;
extern Model *modelARP700;
extern Model *modelMix_24_4_4;
extern Model *modelMix_16_4_4;
extern Model *modelMix_9_3_4;
extern Model *modelMix_4_0_4;
extern Model *modelASAF8;
extern Model *modelPingPong;
extern Model *modelStepDelay;
extern Model *modelOsc_3Ch;
extern Model *modelDronez;
extern Model *modelMorze;
extern Model *modelWindz;
extern Model *modelLorenz;
extern Model *modelAlienz;
extern Model *modelOSC_WaveMorph_3;
extern Model *modelMaude_221;

// rackwindows
#include "rackwindows/src/plugin.hpp"

// ValleyAudio
#include "ValleyAudio/src/Valley.hpp"

// ZetaCarinaeModules
#include "ZetaCarinaeModules/src/plugin.hpp"

#endif // NOPLUGINS

// stuff that reads config files, we dont want that
int loadConsoleType() { return 0; }
int loadDirectOutMode() { return 0; }
bool loadDarkAsDefault() { return true; }
bool loadQuality() { return false; }
void saveConsoleType(int) {}
void saveDarkAsDefault(bool) {}
void saveDirectOutMode(bool) {}
void saveHighQualityAsDefault(bool) {}

// plugin instances
Plugin* pluginInstance__Cardinal;
#ifndef NOPLUGINS
Plugin* pluginInstance__AmalgamatedHarmonics;
Plugin* pluginInstance__AnimatedCircuits;
Plugin* pluginInstance__AS;
Plugin* pluginInstance__Atelier;
Plugin* pluginInstance__AudibleInstruments;
Plugin* pluginInstance__Befaco;
Plugin* pluginInstance__Bidoo;
Plugin* pluginInstance__BogaudioModules;
Plugin* pluginInstance__cf;
extern Plugin* pluginInstance__DrumKit;
Plugin* pluginInstance__ESeries;
Plugin* pluginInstance__Fundamental;
Plugin* pluginInstance__GrandeModular;
extern Plugin* pluginInstance__ImpromptuModular;
Plugin* pluginInstance__JW;
extern Plugin* pluginInstance__MindMeld;
extern Plugin* pluginInstance__mscHack;
Plugin* pluginInstance__rackwindows;
Plugin* pluginInstance__ValleyAudio;
Plugin* pluginInstance__ZetaCarinaeModules;
#endif // NOPLUGINS

namespace rack {

namespace asset {
std::string pluginManifest(const std::string& dirname);
std::string pluginPath(const std::string& dirname);
}

// core plugins
namespace core {
extern Model* modelAudio2;
extern Model* modelAudio8;
extern Model* modelAudio16;
extern Model* modelMIDI_CV;
extern Model* modelMIDICC_CV;
extern Model* modelMIDI_Gate;
extern Model* modelMIDIMap;
extern Model* modelCV_MIDI;
extern Model* modelCV_MIDICC;
extern Model* modelGate_MIDI;
extern Model* modelBlank;
extern Model* modelNotes;
}

// regular plugins
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

    const StaticPluginLoader spl(p, "Core");
    if (spl.ok())
    {
        p->addModel(rack::core::modelAudio2);
        p->addModel(rack::core::modelAudio8);
        p->addModel(rack::core::modelAudio16);
        p->addModel(rack::core::modelMIDI_CV);
        p->addModel(rack::core::modelMIDICC_CV);
        p->addModel(rack::core::modelMIDI_Gate);
        p->addModel(rack::core::modelMIDIMap);
        p->addModel(rack::core::modelCV_MIDI);
        p->addModel(rack::core::modelCV_MIDICC);
        p->addModel(rack::core::modelGate_MIDI);
        p->addModel(rack::core::modelBlank);
        p->addModel(rack::core::modelNotes);
    }
}

static void initStatic__Cardinal()
{
    Plugin* const p = new Plugin;
    pluginInstance__Cardinal = p;

    const StaticPluginLoader spl(p, "Cardinal");
    if (spl.ok())
    {
        p->addModel(modelCarla);
        p->addModel(modelCardinalBlank);
        p->addModel(modelGlBars);
        p->addModel(modelHostCV);
        p->addModel(modelHostParameters);
        p->addModel(modelHostTime);
        p->addModel(modelIldaeil);
    }
}

#ifndef NOPLUGINS
static void initStatic__AmalgamatedHarmonics()
{
    Plugin* const p = new Plugin;
    pluginInstance__AmalgamatedHarmonics = p;

    const StaticPluginLoader spl(p, "AmalgamatedHarmonics");
    if (spl.ok())
    {
        p->addModel(modelArp31);
        p->addModel(modelArp32);
        p->addModel(modelBombe);
        p->addModel(modelChord);
        p->addModel(modelCircle);
        p->addModel(modelGalaxy);
        p->addModel(modelGenerative);
        p->addModel(modelImp);
        p->addModel(modelImperfect2);
        p->addModel(modelProgress2);
        p->addModel(modelRuckus);
        p->addModel(modelScaleQuantizer2);
        p->addModel(modelSLN);
        p->addModel(modelMuxDeMux);
        p->addModel(modelPolyProbe);
        p->addModel(modelPolyScope);
        p->addModel(modelPolyUtils);
        p->addModel(modelPolyVolt);
        p->addModel(modelScaleQuantizer);
        p->addModel(modelArpeggiator2);
        p->addModel(modelProgress);
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

static void initStatic__AS()
{
    Plugin* const p = new Plugin;
    pluginInstance__AS = p;

    const StaticPluginLoader spl(p, "AS");
    if (spl.ok())
    {
#define modelADSR modelASADSR
#define modelVCA modelASVCA
        //OSCILLATORS
        p->addModel(modelSineOsc);
        p->addModel(modelSawOsc);

        //TOOLS
        p->addModel(modelADSR);
        p->addModel(modelVCA);
        p->addModel(modelQuadVCA);
        p->addModel(modelTriLFO);
        p->addModel(modelAtNuVrTr);
        p->addModel(modelBPMClock);
        p->addModel(modelSEQ16);
        p->addModel(modelMixer2ch);
        p->addModel(modelMixer4ch);
        p->addModel(modelMixer8ch);
        p->addModel(modelMonoVUmeter);
        p->addModel(modelStereoVUmeter);
        p->addModel(modelMultiple2_5);
        p->addModel(modelMerge2_5);
        p->addModel(modelSteps);
        p->addModel(modelLaunchGate);
        p->addModel(modelKillGate);
        p->addModel(modelFlow);
        p->addModel(modelSignalDelay);
        p->addModel(modelTriggersMKI);
        p->addModel(modelTriggersMKII);
        p->addModel(modelTriggersMKIII);
        p->addModel(modelBPMCalc);
        p->addModel(modelBPMCalc2);
        p->addModel(modelCv2T);
        p->addModel(modelZeroCV2T);
        p->addModel(modelReScale);

        //EFFECTS
        p->addModel(modelDelayPlusFx);
        p->addModel(modelDelayPlusStereoFx);
        p->addModel(modelPhaserFx);
        p->addModel(modelReverbFx);
        p->addModel(modelReverbStereoFx);
        p->addModel(modelSuperDriveFx);
        p->addModel(modelSuperDriveStereoFx);
        p->addModel(modelTremoloFx);
        p->addModel(modelTremoloStereoFx);
        p->addModel(modelWaveShaper);
        p->addModel(modelWaveShaperStereo);

        //BLANK PANELS
        p->addModel(modelBlankPanel4);
        p->addModel(modelBlankPanel6);
        p->addModel(modelBlankPanel8);
        p->addModel(modelBlankPanelSpecial);
#undef modelADSR
#undef modelVCA
    }
}

static void initStatic__Atelier()
{
    Plugin* const p = new Plugin;
    pluginInstance__Atelier = p;

    const StaticPluginLoader spl(p, "Atelier");
    if (spl.ok())
    {
        p->addModel(modelPalette);
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
#define modelADSR modelBefacoADSR
        p->addModel(modelEvenVCO);
        p->addModel(modelRampage);
        p->addModel(modelABC);
        p->addModel(modelSpringReverb);
        p->addModel(modelMixer);
        p->addModel(modelSlewLimiter);
        p->addModel(modelDualAtenuverter);
        p->addModel(modelPercall);
        p->addModel(modelHexmixVCA);
        p->addModel(modelChoppingKinky);
        p->addModel(modelKickall);
        p->addModel(modelSamplingModulator);
        p->addModel(modelMorphader);
        p->addModel(modelADSR);
        p->addModel(modelSTMix);
        p->addModel(modelMuxlicer);
        p->addModel(modelMex);
#undef modelADSR
    }
}

static void initStatic__Bidoo()
{
    Plugin* const p = new Plugin;
    pluginInstance__Bidoo = p;

    const StaticPluginLoader spl(p, "Bidoo");
    if (spl.ok())
    {
        p->addModel(modelTOCANTE);
        p->addModel(modelLATE);
        p->addModel(modelDIKTAT);
        p->addModel(modelDTROY);
        p->addModel(modelBORDL);
        p->addModel(modelZOUMAI);
        p->addModel(modelMU);
        p->addModel(modelCHUTE);
        p->addModel(modelLOURDE);
        p->addModel(modelACNE);
        p->addModel(modelMS);
        p->addModel(modelDUKE);
        p->addModel(modelMOIRE);
        p->addModel(modelPILOT);
        p->addModel(modelHUITRE);
        p->addModel(modelOUAIVE);
        p->addModel(modelPOUPRE);
        p->addModel(modelMAGMA);
        p->addModel(modelOAI);
        p->addModel(modelCANARD);
        p->addModel(modelEMILE);
        p->addModel(modelFORK);
        p->addModel(modelTIARE);
        p->addModel(modelLIMONADE);
        p->addModel(modelLIMBO);
        p->addModel(modelPERCO);
        p->addModel(modelBAFIS);
        p->addModel(modelBAR);
        p->addModel(modelMINIBAR);
        p->addModel(modelZINC);
        p->addModel(modelFREIN);
        p->addModel(modelHCTIP);
        p->addModel(modelDFUZE);
        p->addModel(modelREI);
        p->addModel(modelRABBIT);
        p->addModel(modelBISTROT);
        p->addModel(modelSIGMA);
        p->addModel(modelFLAME);
        p->addModel(modelVOID);

        // NOTE disabled in Cardinal due to curl usage
        // p->addModel(modelANTN);

        // intentionally remove known bad plugin
        if (json_t* const modules = json_object_get(spl.rootJ, "modules"))
        {
            size_t i;
            json_t* v;
            json_array_foreach(modules, i, v)
            {
                if (json_t* const slug = json_object_get(v, "slug"))
                {
                    if (const char* const value = json_string_value(slug))
                    {
                        if (std::strcmp(value, "antN") == 0)
                        {
                            json_array_remove(modules, i);
                            break;
                        }
                    }
                }
            }
        }
    }
}

static void initStatic__BogaudioModules()
{
    Plugin* const p = new Plugin;
    pluginInstance__BogaudioModules = p;

    const StaticPluginLoader spl(p, "BogaudioModules");
    if (spl.ok())
    {
        // Make sure to use dark theme as default
        Skins& skins(Skins::skins());
        skins._default = "dark";
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

static void initStatic__cf()
{
    Plugin* const p = new Plugin;
    pluginInstance__cf = p;

    const StaticPluginLoader spl(p, "cf");
    if (spl.ok())
    {
        p->addModel(modelMETRO);
        p->addModel(modelEACH);
        p->addModel(modeltrSEQ);
        p->addModel(modelLEDSEQ);
        p->addModel(modelL3DS3Q);
        p->addModel(modelSLIDERSEQ);
        p->addModel(modelPLAYER);
        p->addModel(modelPLAY);
        p->addModel(modelMONO);
        p->addModel(modelSTEREO);
        p->addModel(modelSUB);
        p->addModel(modelMASTER);
        p->addModel(modelVARIABLE);
        p->addModel(modelALGEBRA);
        p->addModel(modelFUNKTION);
        p->addModel(modelCHOKE);
        p->addModel(modelFOUR);
        p->addModel(modelSTEPS);
        p->addModel(modelPEAK);
        p->addModel(modelCUTS);
        p->addModel(modelBUFFER);
        p->addModel(modelDISTO);
        p->addModel(modelCUBE);
        p->addModel(modelPATCH);
        p->addModel(modelLABEL);
        p->addModel(modelDAVE);
    }
}

static void initStatic__DrumKit()
{
    Plugin* const p = new Plugin;
    pluginInstance__DrumKit = p;

    const StaticPluginLoader spl(p, "DrumKit");
    if (spl.ok())
    {
        setupSamples();
        p->addModel(modelBD9);
        p->addModel(modelSnare);
        p->addModel(modelClosedHH);
        p->addModel(modelOpenHH);
        p->addModel(modelDMX);
        p->addModel(modelCR78);
        p->addModel(modelSBD);
        p->addModel(modelGnome);
        p->addModel(modelSequencer);
        p->addModel(modelTomi);
        p->addModel(modelBaronial);
        p->addModel(modelMarionette);
    }
}

static void initStatic__ESeries()
{
    Plugin* const p = new Plugin;
    pluginInstance__ESeries = p;

    const StaticPluginLoader spl(p, "ESeries");
    if (spl.ok())
    {
        p->addModel(modelE340);
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

static void initStatic__ImpromptuModular()
{
    Plugin* const p = new Plugin;
    pluginInstance__ImpromptuModular = p;

    const StaticPluginLoader spl(p, "ImpromptuModular");
    if (spl.ok())
    {
        p->addModel(modelBigButtonSeq);
        p->addModel(modelBigButtonSeq2);
        p->addModel(modelChordKey);
        p->addModel(modelChordKeyExpander);
        p->addModel(modelClocked);
        p->addModel(modelClockedExpander);
        p->addModel(modelClkd);
        p->addModel(modelCvPad);
        p->addModel(modelFoundry);
        p->addModel(modelFoundryExpander);
        p->addModel(modelFourView);
        p->addModel(modelGateSeq64);
        p->addModel(modelGateSeq64Expander);
        p->addModel(modelHotkey);
        p->addModel(modelPart);
        p->addModel(modelPhraseSeq16);
        p->addModel(modelPhraseSeq32);
        p->addModel(modelPhraseSeqExpander);
        p->addModel(modelProbKey);
        // p->addModel(modelProbKeyExpander);
        p->addModel(modelSemiModularSynth);
        p->addModel(modelTact);
        p->addModel(modelTact1);
        p->addModel(modelTactG);
        p->addModel(modelTwelveKey);
        p->addModel(modelWriteSeq32);
        p->addModel(modelWriteSeq64);
        p->addModel(modelBlankPanel);
    }
}

static void initStatic__JW()
{
    Plugin* const p = new Plugin;
    pluginInstance__JW = p;

    const StaticPluginLoader spl(p, "JW-Modules");
    if (spl.ok())
    {
#define modelQuantizer modelJWQuantizer
        p->addModel(modelAdd5);
        p->addModel(modelBouncyBalls);
        p->addModel(modelCat);
        p->addModel(modelTree);
        p->addModel(modelFullScope);
        p->addModel(modelGridSeq);
        p->addModel(modelEightSeq);
        p->addModel(modelDivSeq);
        p->addModel(modelMinMax);
        p->addModel(modelNoteSeq);
        p->addModel(modelNoteSeqFu);
        p->addModel(modelNoteSeq16);
        p->addModel(modelTrigs);
        p->addModel(modelOnePattern);
        p->addModel(modelPatterns);
        p->addModel(modelQuantizer);
        p->addModel(modelSimpleClock);
        p->addModel(modelStr1ker);
        p->addModel(modelD1v1de);
        p->addModel(modelPres1t);
        p->addModel(modelThingThing);
        p->addModel(modelWavHead);
        p->addModel(modelXYPad);
        p->addModel(modelBlankPanel1hp);
        p->addModel(modelBlankPanelSmall);
        p->addModel(modelBlankPanelMedium);
        p->addModel(modelBlankPanelLarge);
        p->addModel(modelCoolBreeze);
        p->addModel(modelPete);
#undef modelQuantizer
    }
}

static void initStatic__MindMeld()
{
    Plugin* const p = new Plugin;
    pluginInstance__MindMeld = p;

    const StaticPluginLoader spl(p, "MindMeldModular");
    if (spl.ok())
    {
        p->addModel(modelMixMasterJr);
        p->addModel(modelAuxExpanderJr);
        p->addModel(modelMixMaster);
        p->addModel(modelAuxExpander);
        p->addModel(modelMeld);
        p->addModel(modelUnmeld);
        p->addModel(modelEqMaster);
        p->addModel(modelEqExpander);
        p->addModel(modelBassMaster);
        p->addModel(modelBassMasterJr);
        p->addModel(modelShapeMaster);
    }
}

static void initStatic__mscHack()
{
    Plugin* const p = new Plugin;
    pluginInstance__mscHack = p;

    const StaticPluginLoader spl(p, "mscHack");
    if (spl.ok())
    {
        p->addModel(modelCompressor);
        p->addModel(modelSynthDrums);
        p->addModel(modelSEQ_6x32x16);
        p->addModel(modelMasterClockx4);
        //p->addModel(modelMasterClockx8);  
        p->addModel(modelSEQ_Envelope_8);
        p->addModel(modelSeq_Triad2);
        p->addModel(modelARP700);
        p->addModel(modelMix_4_0_4);
        p->addModel(modelMix_9_3_4);
        p->addModel(modelMix_16_4_4);
        p->addModel(modelMix_24_4_4);
        p->addModel(modelASAF8);
        p->addModel(modelPingPong);
        p->addModel(modelStepDelay);
        p->addModel(modelOsc_3Ch);
        p->addModel(modelDronez);
        p->addModel(modelMorze);
        p->addModel(modelWindz);
        p->addModel(modelLorenz);
        p->addModel(modelAlienz);
        p->addModel(modelOSC_WaveMorph_3);
        p->addModel(modelMaude_221);
    }
}

static void initStatic__rackwindows()
{
    Plugin* const p = new Plugin;
    pluginInstance__rackwindows = p;

    const StaticPluginLoader spl(p, "rackwindows");
    if (spl.ok())
    {
        // p->addModel(modelAcceleration);
        p->addModel(modelBitshiftgain);
        p->addModel(modelCapacitor);
        p->addModel(modelCapacitor_stereo);
        p->addModel(modelChorus);
        p->addModel(modelConsole);
        p->addModel(modelConsole_mm);
        p->addModel(modelDistance);
        p->addModel(modelGolem);
        p->addModel(modelHolt);
        p->addModel(modelHombre);
        p->addModel(modelInterstage);
        p->addModel(modelMonitoring);
        p->addModel(modelMv);
        p->addModel(modelRasp);
        p->addModel(modelReseq);
        p->addModel(modelTape);
        p->addModel(modelTremolo);
        p->addModel(modelVibrato);
    }
}

static void initStatic__ValleyAudio()
{
    Plugin* const p = new Plugin;
    pluginInstance__ValleyAudio = p;

    const StaticPluginLoader spl(p, "ValleyAudio");
    if (spl.ok())
    {
        p->addModel(modelTopograph);
        p->addModel(modelUGraph);
        p->addModel(modelDexter);
        p->addModel(modelPlateau);
        p->addModel(modelInterzone);
        p->addModel(modelAmalgam);
        p->addModel(modelFeline);
        p->addModel(modelTerrorform);
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
#endif // NOPLUGINS

void initStaticPlugins()
{
    initStatic__Core();
    initStatic__Cardinal();
#ifndef NOPLUGINS
    initStatic__AmalgamatedHarmonics();
    initStatic__AnimatedCircuits();
    initStatic__AS();
    initStatic__Atelier();
    initStatic__AudibleInstruments();
    initStatic__Befaco();
    initStatic__Bidoo();
    initStatic__BogaudioModules();
    initStatic__cf();
    initStatic__DrumKit();
    initStatic__ESeries();
    initStatic__Fundamental();
    initStatic__GrandeModular();
    initStatic__ImpromptuModular();
    initStatic__JW();
    initStatic__MindMeld();
    initStatic__mscHack();
    initStatic__rackwindows();
    initStatic__ValleyAudio();
    initStatic__ZetaCarinaeModules();
#endif // NOPLUGINS
}

void destroyStaticPlugins()
{
    for (Plugin* p : plugins)
        delete p;
    plugins.clear();
}

}
}
