/*
 * DISTRHO Cardinal Plugin
 * Copyright (C) 2021-2024 Filipe Coelho <falktx@falktx.com>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "rack.hpp"
#include "plugin.hpp"

#include "DistrhoUtils.hpp"

// Cardinal (built-in)
#include "Cardinal/src/plugin.hpp"

// Fundamental (always enabled)
#include "Fundamental/src/plugin.hpp"

// ZamAudio (always enabled) - TODO
// #include "ZamAudio/src/plugin.hpp"

// 21kHz
#include "21kHz/src/21kHz.hpp"

// 8Mode
#include "8Mode/src/8mode.hpp"

// Aaron Static
#include "AaronStatic/src/plugin.hpp"

// Admiral
/* NOTE too much noise in original include, do this a different way
// #include "admiral/src/plugin.hpp"
*/
extern Model* modelWatches;
extern Model* modelShifts;
extern Model* modelTables;
extern Model* modelDivisions;

// Alef's Bits
#define modelSteps modelalefsbitsSteps
#define modelLogic modelalefsbitsLogic
#include "alefsbits/src/plugin.hpp"
#undef modelSteps
#undef modelLogic

// Algoritmarte
#include "Algoritmarte/src/plugin.hpp"

// AmalgamatedHarmonics
#include "AmalgamatedHarmonics/src/AH.hpp"

// AnimatedCircuits
#include "AnimatedCircuits/src/plugin.hpp"

// ArableInstruments
#define modelClouds modelArableClouds
#include "ArableInstruments/src/ArableInstruments.hpp"
#undef modelClouds

// Aria
/* NOTE too much noise in original include, do this a different way
// #include "AriaModules/src/plugin.hpp"
*/
#define modelBlank modelAriaBlank
extern Model* modelSplort;
extern Model* modelSmerge;
extern Model* modelSpleet;
extern Model* modelSwerge;
extern Model* modelSplirge;
// extern Model* modelSrot;
extern Model* modelQqqq;
extern Model* modelQuack;
extern Model* modelQ;
extern Model* modelQuale;
extern Model* modelDarius;
extern Model* modelSolomon4;
extern Model* modelSolomon8;
extern Model* modelSolomon16;
extern Model* modelPsychopump;
extern Model* modelPokies4;
extern Model* modelGrabby;
extern Model* modelRotatoes4;
extern Model* modelUndular;
extern Model* modelBlank;
#undef modelBlank

// AS
#define modelADSR modelASADSR
#define modelVCA modelASVCA
#define modelWaveShaper modelASWaveShaper
#define LedLight ASLedLight
#define YellowRedLight ASYellowRedLight
#include "AS/src/AS.hpp"
#undef modelADSR
#undef modelVCA
#undef modelWaveShaper
#undef LedLight
#undef YellowRedLight

// AudibleInstruments
#include "AudibleInstruments/src/plugin.hpp"

// Autinn
/* NOTE too much noise in original include, do this a different way
// #include "Autinn/src/Autinn.hpp"
*/
#define modelChord modelAutinnChord
#define modelVibrato modelAutinnVibrato
extern Model* modelJette;
extern Model* modelFlora;
extern Model* modelOxcart;
extern Model* modelDeadband;
extern Model* modelDigi;
extern Model* modelFlopper;
extern Model* modelAmp;
extern Model* modelDC;
extern Model* modelSjip;
extern Model* modelBass;
extern Model* modelSquare;
extern Model* modelSaw;
extern Model* modelBoomerang;
extern Model* modelVibrato;
extern Model* modelVectorDriver; //deprecated
extern Model* modelCVConverter;
extern Model* modelZod;
extern Model* modelTriBand;
extern Model* modelMixer6;
extern Model* modelNon;
extern Model* modelFil;
extern Model* modelNap;
extern Model* modelMelody;
extern Model* modelChord;
#undef modelChord
#undef modelVibrato

// Axioma
#include "Axioma/src/plugin.hpp"

// BaconPlugs
#define INCLUDE_COMPONENTS_HPP
#include "BaconPlugs/src/BaconPlugs.hpp"
#undef INCLUDE_COMPONENTS_HPP
#undef SCREW_WIDTH
#undef RACK_HEIGHT

// Befaco
#define modelADSR modelBefacoADSR
#define modelMixer modelBefacoMixer
#define modelBurst modelBefacoBurst
#include "Befaco/src/plugin.hpp"
#undef modelADSR
#undef modelMixer
#undef modelBurst

// Bidoo
#include "Bidoo/src/plugin.hpp"

// Biset
/* NOTE too much noise in original include, do this a different way
// #include "Biset/src/plugin.hpp"
*/
#define modelBlank modelBisetBlank
#define modelTree modelBisetTree
extern Model* modelTracker;
extern Model* modelTrackerSynth;
extern Model* modelTrackerDrum;
extern Model* modelTrackerClock;
extern Model* modelTrackerPhase;
extern Model* modelTrackerQuant;
extern Model* modelTrackerState;
extern Model* modelTrackerControl;
extern Model* modelRegex;
extern Model* modelRegexCondensed;
extern Model* modelRegexExp;
extern Model* modelTree;
extern Model* modelTreeSeed;
extern Model* modelGbu;
extern Model* modelPkm;
extern Model* modelIgc;
extern Model* modelOmega3;
extern Model* modelOmega6;
extern Model* modelSegfault;
extern Model* modelBlank;
#undef modelBlank
#undef modelTree

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
#define modelADSR modelBogaudioADSR
#define modelLFO modelBogaudioLFO
#define modelNoise modelBogaudioNoise
#define modelVCA modelBogaudioVCA
#define modelVCF modelBogaudioVCF
#define modelVCO modelBogaudioVCO
extern Model* modelVCO;
extern Model* modelLVCO;
extern Model* modelSine;
extern Model* modelPulse;
extern Model* modelXCO;
extern Model* modelAdditator;
extern Model* modelFMOp;
extern Model* modelChirp;
extern Model* modelLFO;
extern Model* modelLLFO;
extern Model* modelFourFO;
extern Model* modelEightFO;
extern Model* modelVCF;
extern Model* modelLVCF;
extern Model* modelFFB;
extern Model* modelEQ;
extern Model* modelEQS;
extern Model* modelLPG;
extern Model* modelLLPG;
extern Model* modelMegaGate;
extern Model* modelPEQ;
extern Model* modelPEQ6;
extern Model* modelPEQ6XF;
extern Model* modelPEQ14;
extern Model* modelPEQ14XF;
extern Model* modelDADSRH;
extern Model* modelDADSRHPlus;
extern Model* modelShaper;
extern Model* modelShaperPlus;
extern Model* modelAD;
extern Model* modelASR;
extern Model* modelADSR;
extern Model* modelVish;
extern Model* modelFollow;
extern Model* modelDGate;
extern Model* modelRGate;
extern Model* modelEdge;
extern Model* modelNoise;
extern Model* modelSampleHold;
extern Model* modelWalk2;
extern Model* modelWalk;
extern Model* modelMix8;
extern Model* modelMix8x;
extern Model* modelMix4;
extern Model* modelMix4x;
extern Model* modelMix2;
extern Model* modelMix1;
extern Model* modelVCM;
extern Model* modelMute8;
extern Model* modelPan;
extern Model* modelXFade;
extern Model* modelVCA;
extern Model* modelVCAmp;
extern Model* modelVelo;
extern Model* modelUMix;
extern Model* modelMumix;
extern Model* modelMatrix81;
extern Model* modelMatrix18;
extern Model* modelMatrix44;
extern Model* modelMatrix44Cvm;
extern Model* modelMatrix88;
extern Model* modelMatrix88Cv;
extern Model* modelMatrix88M;
extern Model* modelSwitch81;
extern Model* modelSwitch18;
extern Model* modelSwitch44;
extern Model* modelSwitch88;
extern Model* modelSwitch1616;
extern Model* modelAMRM;
extern Model* modelPressor;
extern Model* modelClpr;
extern Model* modelLmtr;
extern Model* modelNsgt;
extern Model* modelCmpDist;
extern Model* modelOneEight;
extern Model* modelEightOne;
extern Model* modelAddrSeq;
extern Model* modelAddrSeqX;
extern Model* modelPgmr;
extern Model* modelPgmrX;
extern Model* modelVU;
extern Model* modelAnalyzer;
extern Model* modelAnalyzerXL;
extern Model* modelRanalyzer;
extern Model* modelDetune;
extern Model* modelStack;
extern Model* modelReftone;
extern Model* modelMono;
extern Model* modelArp;
extern Model* modelAssign;
extern Model* modelUnison;
extern Model* modelPolyCon8;
extern Model* modelPolyCon16;
extern Model* modelPolyOff8;
extern Model* modelPolyOff16;
extern Model* modelPolyMult;
extern Model* modelBool;
extern Model* modelCmp;
extern Model* modelCVD;
extern Model* modelFlipFlop;
extern Model* modelInv;
extern Model* modelManual;
extern Model* modelFourMan;
extern Model* modelMult;
extern Model* modelOffset;
extern Model* modelSlew;
extern Model* modelSums;
extern Model* modelSwitch;
extern Model* modelLgsw;
extern Model* modelBlank3;
extern Model* modelBlank6;
#ifdef EXPERIMENTAL
extern Model* modelLag;
extern Model* modelPEQ14XR;
extern Model* modelPEQ14XV;
#endif
#ifdef TEST
extern Model* modelTest;
extern Model* modelTest2;
extern Model* modelTestExpanderBase;
extern Model* modelTestExpanderExtension;
extern Model* modelTestGl;
extern Model* modelTestVCF;
#endif
#undef modelADSR
#undef modelLFO
#undef modelNoise
#undef modelVCA
#undef modelVCF
#undef modelVCO

// CatroModulo
#include "CatroModulo/src/CatroModulo.hpp"

// cf
#include "cf/src/plugin.hpp"

// CVfunk
#define modelSteps modelCVfunkSteps
#include "CVfunk/src/plugin.hpp"
#undef modelSteps

// ChowDSP
#include "ChowDSP/src/plugin.hpp"
#define init initChowDSP
#include "ChowDSP/src/plugin.cpp"
#undef init

// Computerscare
#include "Computerscare/src/Computerscare.hpp"

// dBiz
#define DarkDefaultItem dBizDarkDefaultItem
#define OrangeLight dBizOrangeLight
#define darkPanelID dBizdarkPanelID
#define lightPanelID dBizlightPanelID
#define modelChord modeldBizChord
#define modelDivider modeldBizDivider
#define modelFourSeq modeldBizFourSeq
#define modelVCA4 modeldBizVCA4
#include "dBiz/src/plugin.hpp"
#undef DarkDefaultItem
#undef OrangeLight
#undef darkPanelID
#undef lightPanelID
#undef modelChord
#undef modelDivider
#undef modelFourSeq
#undef modelVCA4

// DHEModules
// NOTE very unique way of handling init, needs special handling
namespace dhe {
namespace blossom { void init(Plugin*); }
namespace buttons { void init(Plugin*); }
namespace cubic { void init(Plugin*); }
namespace curve_sequencer { void init(Plugin*); }
namespace envelope { void init(Plugin*); }
namespace func { void init(Plugin*); }
namespace fuzzy_logic { void init(Plugin*); }
namespace gator { void init(Plugin*); }
namespace ranger { void init(Plugin*); }
namespace scannibal { void init(Plugin*); }
namespace sequencizer { void init(Plugin*); }
namespace swave { void init(Plugin*); }
namespace tapers { void init(Plugin*); }
namespace truth { void init(Plugin*); }
namespace xycloid { void init(Plugin*); }
}

// DrumKit
#include "DrumKit/src/DrumKit.hpp"
void setupSamples();

// EnigmaCurry
#define modelPulse modelEnigmaCurryPulse
#include "EnigmaCurry/src/plugin.hpp"
#undef modelPulse

// ESeries
#include "ESeries/src/plugin.hpp"

// ExpertSleepers-Encoders
#include "ExpertSleepers-Encoders/src/Encoders.hpp"

// Extratone
#include "Extratone/src/plugin.hpp"

// FehlerFabrik
#include "FehlerFabrik/src/plugin.hpp"

// forsitan modulare
#include "forsitan-modulare/src/forsitan.hpp"

// GlueTheGiant
#include "GlueTheGiant/src/plugin.hpp"
bool audition_mixer = false;
bool audition_depot = false;
int gtg_default_theme = 1;
int loadGtgPluginDefault(const char*, int) { return 1; }
void saveGtgPluginDefault(const char*, int) {}

// GoodSheperd
#include "GoodSheperd/src/plugin.hpp"

// GrandeModular
#include "GrandeModular/src/plugin.hpp"

// H4N4 Modules
#include "h4n4-modules/src/plugin.hpp"

// Hampton Harmonics
#define modelArp modelHamptonHarmonicsArp
#define modelProgress modelHamptonHarmonicsProgress
#include "HamptonHarmonics/src/plugin.hpp"
#undef modelProgress
#undef modelArp

// HetrickCV
#define modelASR modelHetrickCVASR
#define modelBlankPanel modelHetrickCVBlankPanel
#define modelFlipFlop modelHetrickCVFlipFlop
#define modelMidSide modelHetrickCVMidSide
#define modelMinMax modelHetrickCVMinMax
#define PanelBaseWidget HetrickCVPanelBaseWidget
#define InverterWidget HetrickCVInverterWidget
extern Model* modelTwoToFour;
extern Model* modelAnalogToDigital;
extern Model* modelASR;
extern Model* modelBinaryGate;
extern Model* modelBinaryNoise;
extern Model* modelBitshift;
extern Model* modelBlankPanel;
extern Model* modelBoolean3;
extern Model* modelChaos1Op;
extern Model* modelChaos2Op;
extern Model* modelChaos3Op;
extern Model* modelChaoticAttractors;
extern Model* modelClockedNoise;
extern Model* modelComparator;
extern Model* modelContrast;
extern Model* modelCrackle;
extern Model* modelDataCompander;
extern Model* modelDelta;
extern Model* modelDigitalToAnalog;
extern Model* modelDust;
extern Model* modelExponent;
extern Model* modelFBSineChaos;
extern Model* modelFlipFlop;
extern Model* modelFlipPan;
extern Model* modelGateDelay;
extern Model* modelGateJunction;
extern Model* modelGateJunctionExp;
extern Model* modelGingerbread;
extern Model* modelLogicCombine;
extern Model* modelMidSide;
extern Model* modelMinMax;
extern Model* modelPhaseDrivenSequencer;
extern Model* modelPhaseDrivenSequencer32;
extern Model* modelPhasorAnalyzer;
extern Model* modelPhasorBurstGen;
extern Model* modelPhasorDivMult;
extern Model* modelPhasorEuclidean;
extern Model* modelPhasorGates;
extern Model* modelPhasorGates32;
extern Model* modelPhasorGates64;
extern Model* modelPhasorGen;
extern Model* modelPhasorGeometry;
extern Model* modelPhasorHumanizer;
extern Model* modelPhasorMixer;
extern Model* modelPhasorOctature;
extern Model* modelPhasorQuadrature;
extern Model* modelPhasorRandom;
extern Model* modelPhasorRanger;
extern Model* modelPhasorReset;
extern Model* modelPhasorRhythmGroup;
extern Model* modelPhasorShape;
extern Model* modelPhasorShift;
extern Model* modelPhasorStutter;
extern Model* modelPhasorSubstepShape;
extern Model* modelPhasorSwing;
extern Model* modelPhasorTimetable;
extern Model* modelPhasorToClock;
extern Model* modelPhasorToLFO;
extern Model* modelPhasorToWaveforms;
extern Model* modelProbability;
extern Model* modelRandomGates;
extern Model* modelRotator;
extern Model* modelRungler;
extern Model* modelScanner;
extern Model* modelVectorMix;
extern Model* modelWaveshape;
extern Model* modelXYToPolar;
#undef modelASR
#undef modelBlankPanel
#undef modelFlipFlop
#undef modelMidSide
#undef modelMinMax
#undef PanelBaseWidget
#undef InverterWidget

// ImpromptuModular
/* NOTE too much noise in original include, do this a different way
// #include "ImpromptuModular/src/ImpromptuModular.hpp"
*/
extern Model* modelAdaptiveQuantizer;
extern Model* modelBigButtonSeq;
extern Model* modelBigButtonSeq2;
extern Model* modelChordKey;
extern Model* modelChordKeyExpander;
extern Model* modelClocked;
extern Model* modelClockedExpander;
extern Model* modelClkd;
extern Model* modelCvPad;
extern Model* modelFoundry;
extern Model* modelFoundryExpander;
extern Model* modelFourView;
extern Model* modelGateSeq64;
extern Model* modelGateSeq64Expander;
extern Model* modelHotkey;
extern Model* modelPart;
extern Model* modelPhraseSeq16;
extern Model* modelPhraseSeq32;
extern Model* modelPhraseSeqExpander;
extern Model* modelProbKey;
extern Model* modelSemiModularSynth;
extern Model* modelSygen;
extern Model* modelTact;
extern Model* modelTact1;
extern Model* modelTactG;
extern Model* modelTwelveKey;
extern Model* modelVariations;
extern Model* modelWriteSeq32;
extern Model* modelWriteSeq64;
extern Model* modelBlankPanel;

// ihtsyn
#include "ihtsyn/src/plugin.hpp"

// JW-Modules
#define modelQuantizer modelJWQuantizer
#include "JW-Modules/src/JWModules.hpp"
#undef modelQuantizer

// kocmoc
#include "kocmoc/src/plugin.hpp"

// LifeFormModular
/* NOTE too much noise in original include, do this a different way
// #include "LifeFormModular/src/plugin.hpp"
*/
extern Model* modelTimeDiktat;
extern Model* modelSequenceModeler;
extern Model* modelPitchDiktat;
extern Model* modelPitchIntegrator;
extern Model* modelBurstIntegrator;
extern Model* modelQuadModulator;
extern Model* modelImpulseControl;
extern Model* modelQuadSteppedOffset;
extern Model* modelPercussiveVibration;
extern Model* modelQuadUtility;
extern Model* modelAdditiveVibration;
extern Model* modelComplexOsc;
extern Model* modelDriftgen;

// LittleUtils
#include "LittleUtils/src/plugin.hpp"

// Lilac Loop
/* NOTE too much noise in original include, do this a different way
// #include "LilacLoop/src/plugin.hpp"
*/
extern Model* modelLooperOne;
extern Model* modelLooperTwo;

// LomasModules
#include "LomasModules/src/plugin.hpp"
#undef DR_WAV_IMPLEMENTATION

// LyraeModules
/* NOTE too much noise in original include, do this a different way
// #include "LyraeModules/src/plugin.hpp"
*/
#define modelDelta modelLyraeDelta
extern Model* modelSulafat;
extern Model* modelGamma;
extern Model* modelDelta;
extern Model* modelVega;
extern Model* modelBD383238;
extern Model* modelZeta;
#undef modelDelta

// Meander
extern int panelTheme;
#include "Meander/src/plugin.hpp"

// MindMeldModular
/* NOTE too much noise in original include, do this a different way
// #include "MindMeldModular/src/MindMeldModular.hpp"
*/
extern Model *modelPatchMaster;
extern Model *modelPatchMasterBlank;
extern Model *modelRouteMasterMono5to1;
extern Model *modelRouteMasterStereo5to1;
extern Model *modelRouteMasterMono1to5;
extern Model *modelRouteMasterStereo1to5;
extern Model *modelMasterChannel;
extern Model *modelMeld;
extern Model *modelUnmeld;
extern Model *modelMSMelder;
extern Model *modelEqMaster;
extern Model *modelEqExpander;
extern Model *modelBassMaster;
extern Model *modelBassMasterJr;
extern Model *modelShapeMaster;
extern Model *modelMixMasterJr;
extern Model *modelAuxExpanderJr;
extern Model *modelMixMaster;
extern Model *modelAuxExpander;

// ML_modules
/* NOTE too much noise in original include, do this a different way
// #include "ML_modules/src/ML_modules.hpp"
*/
#define modelQuantizer modelMLQuantizer
#define modelSH8 modelMLSH8
extern Model* modelQuantizer;
extern Model* modelQuantum;
extern Model* modelTrigBuf;
extern Model* modelSeqSwitch;
extern Model* modelSeqSwitch2;
extern Model* modelShiftRegister;
extern Model* modelShiftRegister2;
extern Model* modelFreeVerb;
extern Model* modelSum8;
extern Model* modelSum8mk2;
extern Model* modelSum8mk3;
extern Model* modelSH8;
extern Model* modelConstants;
extern Model* modelCounter;
extern Model* modelTrigDelay;
extern Model* modelBPMdetect;
extern Model* modelVoltMeter;
extern Model* modelOctaFlop;
extern Model* modelOctaTrig;
extern Model* modelOctaSwitch;
extern Model* modelTrigSwitch;
extern Model* modelTrigSwitch2;
extern Model* modelTrigSwitch3;
extern Model* modelTrigSwitch3_2;
extern Model* modelOctaPlus;
extern Model* modelOctaTimes;
extern Model* modelCloner;
extern Model* modelPolySplitter;
extern Model* modelArpeggiator;
#undef modelQuantizer
#undef modelSH8

// MockbaModular
#define modelBlank modelMockbaModularBlank
#define modelComparator modelMockbaModularComparator
#include "MockbaModular/src/plugin.hpp"
#undef modelBlank
#undef modelComparator
#include "MockbaModular/src/MockbaModular.hpp"
#undef min
#undef max
#define saveBack ignoreMockbaModular1
#define loadBack ignoreMockbaModular2
#include "MockbaModular/src/MockbaModular.cpp"
#undef saveBack
#undef loadBack
std::string loadBack(int) { return "res/Empty_gray.svg"; }

// Mog
#include "Mog/src/plugin.hpp"

// mscHack
/* NOTE too much noise in original include, do this a different way
// #include "mscHack/src/mscHack.hpp"
*/
extern Model* modelCompressor;
extern Model* modelSynthDrums;
extern Model* modelSEQ_6x32x16;
extern Model* modelMasterClockx8;
extern Model* modelMasterClockx4;
extern Model* modelSEQ_Envelope_8;
extern Model* modelSeq_Triad2;
extern Model* modelARP700;
extern Model* modelMix_24_4_4;
extern Model* modelMix_16_4_4;
extern Model* modelMix_9_3_4;
extern Model* modelMix_4_0_4;
extern Model* modelASAF8;
extern Model* modelPingPong;
extern Model* modelStepDelay;
extern Model* modelOsc_3Ch;
extern Model* modelDronez;
extern Model* modelMorze;
extern Model* modelWindz;
extern Model* modelLorenz;
extern Model* modelAlienz;
extern Model* modelOSC_WaveMorph_3;
extern Model* modelMaude_221;

// MSM
/* NOTE too much noise in original include, do this a different way
// #include "MSM/src/MSM.hpp"
*/
#define modelADSR modelMSMADSR
#define modelBlankPanel modelMSMBlankPanel
#define modelDelay modelMSMDelay
#define modelLFO modelMSMLFO
#define modelMult modelMSMMult
#define modelNoise modelMSMNoise
#define modelVCA modelMSMVCA
#define modelVCO modelMSMVCO
extern Model* modelVCO;
extern Model* modelBVCO;
extern Model* modelExperimentalVCO;
extern Model* modelNoise;
extern Model* modelLFO;
extern Model* modelVCA;
extern Model* modelADSR;
extern Model* modelDelay;
extern Model* modelWaveShaper;
extern Model* modelWavefolder;
extern Model* modelBitcrusher;
extern Model* modelPhaserModule;
extern Model* modelMorpher;
extern Model* modelRingMod;
extern Model* modelRandomSource;
extern Model* modelMult;
extern Model* modelCrazyMult;
extern Model* modelFade;
extern Model* modelSimpleSlider;
extern Model* modelxseq;
extern Model* modelBlankPanel;
#undef modelADSR
#undef modelBlankPanel
#undef modelDelay
#undef modelLFO
#undef modelMult
#undef modelNoise
#undef modelVCA
#undef modelVCO

// MUS-X
#include "MUS-X/src/plugin.hpp"

// myth-modules
#include "myth-modules/src/plugin.hpp"

// Nonlinear Circuits
#include "nonlinearcircuits/src/NLC.hpp"

// Orbits
#include "Orbits/src/plugin.hpp"

// ParableInstruments
#define modelClouds modelParableClouds
#include "ParableInstruments/src/ArableInstruments.hpp"
#undef modelClouds

// Path Set
#include "PathSet/src/plugin.hpp"

// PdArray
#define TextBox PdArrayTextBox
#define CustomTrimpot PdArrayCustomTrimpot
#define MsDisplayWidget PdArrayMsDisplayWidget
#define MAX_POLY_CHANNELS PDARRAY_MAX_POLY_CHANNELS
#include "PdArray/src/plugin.hpp"
#undef Textbox
#undef CustomTrimpot
#undef MsDisplayWidget
#undef MAX_POLY_CHANNELS

// PinkTrombone
#include "PinkTrombone/src/plugin.hpp"

// Prism
#include "Prism/src/plugin.hpp"

// rackwindows
#include "rackwindows/src/plugin.hpp"

// RCM
#include "rcm-modules/src/plugin.hpp"

// RebelTech
#define BefacoInputPort BefacoInputPortRebelTech
#define BefacoOutputPort BefacoOutputPortRebelTech
#include "RebelTech/src/plugin.hpp"
#undef BefacoInputPort
#undef BefacoOutputPort
ModuleTheme defaultPanelTheme = DARK_THEME;
void addThemeMenuItems(Menu*, ModuleTheme*) {}

// repelzen
#define modelBlank modelrepelzenBlank
#define modelMixer modelrepelzenMixer
#define modelWerner modelrepelzenWerner
#define tanh_pade repelzentanh_pade
#include "repelzen/src/repelzen.hpp"
#undef modelBlank
#undef modelMixer
#undef modelWerner
#undef tanh_pade

// Sapphire
#include "Sapphire/src/plugin.hpp"

// sonusmodular
#include "sonusmodular/src/sonusmodular.hpp"

// Starling Via
#define modelScanner modelStarlingViaScanner
#define Scale starlingViaScale
#define Wavetable starlingViaWavetable
#include "StarlingVia/src/starling.hpp"
#undef modelScanner
#undef Scale
#undef Wavetable

// stocaudio
#include "stocaudio/src/plugin.hpp"

// stoermelder-packone
#include "stoermelder-packone/src/plugin.hpp"
Model* modelAudioInterface64;
Model* modelMidiCat;
Model* modelMidiCatCtx;
Model* modelMidiCatMem;
Model* modelMidiKey;
Model* modelMidiMon;
Model* modelMidiPlug;
Model* modelMidiStep;
StoermelderSettings pluginSettings;
void StoermelderSettings::saveToJson() {}
void StoermelderSettings::readFromJson() {}

// surgext
#include "surgext/src/SurgeXT.h"
void surgext_rack_initialize();
void surgext_rack_update_theme();

// unless_modules
#include "unless_modules/src/unless.hpp"

// ValleyAudio
#include "ValleyAudio/src/Valley.hpp"

// Voxglitch
#define modelLooper modelVoxglitchLooper
#include "voxglitch/src/plugin.hpp"
#undef modelLooper

// WhatTheRack
#include "WhatTheRack/src/WhatTheRack.hpp"

// ZetaCarinaeModules
#include "ZetaCarinaeModules/src/plugin.hpp"

// ZZC
#define DISPLAYS_H
#define ZZC_SHARED_H
#define ZZC_WIDGETS_H
#define modelClock modelZZCClock
#include "ZZC/src/ZZC.hpp"
#undef modelClock

// known terminal modules
std::vector<Model*> hostTerminalModels;

// stuff that reads config files, we don't want that
int loadConsoleType() { return 0; }
bool loadDarkAsDefault() { return settings::preferDarkPanels; }
ModuleTheme loadDefaultTheme() { return settings::preferDarkPanels ? DARK_THEME : LIGHT_THEME; }
int loadDirectOutMode() { return 0; }
void readDefaultTheme() { defaultPanelTheme = loadDefaultTheme(); }
void saveConsoleType(int) {}
void saveDarkAsDefault(bool) {}
void saveDefaultTheme(ModuleTheme) {}
void saveDirectOutMode(bool) {}
void saveHighQualityAsDefault(bool) {}
void writeDefaultTheme() {}

// plugin instances
Plugin* pluginInstance__Cardinal;
Plugin* pluginInstance__Fundamental;
// Plugin* pluginInstance__ZamAudio;
Plugin* pluginInstance__21kHz;
Plugin* pluginInstance__8Mode;
extern Plugin* pluginInstance__AaronStatic;
Plugin* pluginInstance__admiral;
Plugin* pluginInstance__alefsbits;
Plugin* pluginInstance__Algoritmarte;
Plugin* pluginInstance__AmalgamatedHarmonics;
Plugin* pluginInstance__ArableInstruments;
Plugin* pluginInstance__AnimatedCircuits;
Plugin* pluginInstance__Aria;
Plugin* pluginInstance__AS;
Plugin* pluginInstance__AudibleInstruments;
extern Plugin* pluginInstance__Autinn;
Plugin* pluginInstance__Axioma;
Plugin* pluginInstance__Bacon;
Plugin* pluginInstance__Befaco;
Plugin* pluginInstance__Bidoo;
Plugin* pluginInstance__Biset;
Plugin* pluginInstance__BogaudioModules;
Plugin* pluginInstance__CatroModulo;
Plugin* pluginInstance__cf;
Plugin* pluginInstance__ChowDSP;
Plugin* pluginInstance__Computerscare;
Plugin* pluginInstance__CVfunk;
Plugin* pluginInstance__dBiz;
Plugin* pluginInstance__DHE;
extern Plugin* pluginInstance__DrumKit;
Plugin* pluginInstance__EnigmaCurry;
Plugin* pluginInstance__ESeries;
Plugin* pluginInstance__ExpertSleepersEncoders;
Plugin* pluginInstance__Extratone;
Plugin* pluginInstance__FehlerFabrik;
Plugin* pluginInstance__forsitan;
Plugin* pluginInstance__GlueTheGiant;
Plugin* pluginInstance__GoodSheperd;
Plugin* pluginInstance__GrandeModular;
Plugin* pluginInstance__H4N4;
Plugin* pluginInstance__HamptonHarmonics;
Plugin* pluginInstance__HetrickCV;
extern Plugin* pluginInstance__ImpromptuModular;
Plugin* pluginInstance__ihtsyn;
Plugin* pluginInstance__JW;
Plugin* pluginInstance__kocmoc;
Plugin* pluginInstance__LifeFormModular;
Plugin* pluginInstance__LilacLoop;
Plugin* pluginInstance__LittleUtils;
Plugin* pluginInstance__Lomas;
Plugin* pluginInstance__Lyrae;
Plugin* pluginInstance__Meander;
extern Plugin* pluginInstance__MindMeld;
Plugin* pluginInstance__ML;
Plugin* pluginInstance__MockbaModular;
Plugin* pluginInstance__Mog;
extern Plugin* pluginInstance__mscHack;
Plugin* pluginInstance__MSM;
Plugin* pluginInstance__MUS_X;
Plugin* pluginInstance__myth_modules;
Plugin* pluginInstance__nonlinearcircuits;
Plugin* pluginInstance__Orbits;
Plugin* pluginInstance__ParableInstruments;
Plugin* pluginInstance__PathSet;
Plugin* pluginInstance__PdArray;
Plugin* pluginInstance__PinkTrombone;
Plugin* pluginInstance__Prism;
Plugin* pluginInstance__rackwindows;
Plugin* pluginInstance__RCM;
Plugin* pluginInstance__RebelTech;
Plugin* pluginInstance__repelzen;
Plugin* pluginInstance__sapphire;
Plugin* pluginInstance__sonusmodular;
Plugin* pluginInstance__StarlingVia;
Plugin* pluginInstance__stocaudio;
extern Plugin* pluginInstance__stoermelder_p1;
Plugin* pluginInstance__surgext;
Plugin* pluginInstance__unless_modules;
Plugin* pluginInstance__ValleyAudio;
Plugin* pluginInstance__Voxglitch;
Plugin* pluginInstance__WhatTheRack;
Plugin* pluginInstance__ZetaCarinaeModules;
Plugin* pluginInstance__ZZC;

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
        p->addModel(modelAidaX);
        p->addModel(modelCardinalBlank);
        p->addModel(modelExpanderInputMIDI);
        p->addModel(modelExpanderOutputMIDI);
        p->addModel(modelHostAudio2);
        p->addModel(modelHostAudio8);
        p->addModel(modelHostCV);
        p->addModel(modelHostMIDI);
        p->addModel(modelHostMIDICC);
        p->addModel(modelHostMIDIGate);
        p->addModel(modelHostMIDIMap);
        p->addModel(modelHostParameters);
        p->addModel(modelHostParametersMap);
        p->addModel(modelHostTime);
        p->addModel(modelTextEditor);
       #ifndef DGL_USE_GLES
        p->addModel(modelGlBars);
       #else
        spl.removeModule("glBars");
       #endif
       #ifndef STATIC_BUILD
        p->addModel(modelAudioFile);
       #else
        spl.removeModule("AudioFile");
       #endif
       #if !(defined(DISTRHO_OS_WASM) || defined(STATIC_BUILD))
        p->addModel(modelCarla);
        p->addModel(modelIldaeil);
       #else
        spl.removeModule("Carla");
        spl.removeModule("Ildaeil");
       #endif
       #ifndef HEADLESS
        p->addModel(modelSassyScope);
       #else
        spl.removeModule("SassyScope");
       #endif
       #if defined(HAVE_X11) && !defined(HEADLESS) && !defined(STATIC_BUILD)
        p->addModel(modelMPV);
       #else
        spl.removeModule("MPV");
       #endif
       #ifdef HAVE_FFTW3F
        p->addModel(modelAudioToCVPitch);
       #else
        spl.removeModule("AudioToCVPitch");
       #endif

        hostTerminalModels = {
            modelHostAudio2,
            modelHostAudio8,
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
        p->addModel(model_8vert);
        p->addModel(modelADSR);
        p->addModel(modelDelay);
        p->addModel(modelLFO);
        p->addModel(modelLFO2);
        p->addModel(modelMerge);
        p->addModel(modelMidSide);
        p->addModel(modelMixer);
        p->addModel(modelMutes);
        p->addModel(modelNoise);
        p->addModel(modelOctave);
        p->addModel(modelPulses);
        p->addModel(modelQuantizer);
        p->addModel(modelRandom);
        p->addModel(modelScope);
        p->addModel(modelSEQ3);
        p->addModel(modelSequentialSwitch1);
        p->addModel(modelSequentialSwitch2);
        p->addModel(modelSplit);
        p->addModel(modelSum);
        p->addModel(modelVCA);
        p->addModel(modelVCA_1);
        p->addModel(modelVCF);
        p->addModel(modelVCMixer);
        p->addModel(modelVCO);
        p->addModel(modelVCO2);
    }
}

/*
static void initStatic__ZamAudio()
{
    Plugin* const p = new Plugin;
    pluginInstance__ZamAudio = p;

    const StaticPluginLoader spl(p, "ZamAudio");
    if (spl.ok())
    {
        p->addModel(modelZamComp);
    }
}
*/

static void initStatic__21kHz()
{
    Plugin* const p = new Plugin;
    pluginInstance__21kHz = p;

    const StaticPluginLoader spl(p, "21kHz");
    if (spl.ok())
    {
        p->addModel(modelPalmLoop);
        p->addModel(modelD_Inf);
        p->addModel(modelTachyonEntangler);
    }
}

static void initStatic__8Mode()
{
    Plugin* const p = new Plugin;
    pluginInstance__8Mode = p;

    const StaticPluginLoader spl(p, "8Mode");
    if (spl.ok())
    {
        p->addModel(modelsoftSN);
    }
}

static void initStatic__AaronStatic()
{
    Plugin* const p = new Plugin;
    pluginInstance__AaronStatic = p;

    const StaticPluginLoader spl(p, "AaronStatic");
    if (spl.ok())
    {
        p->addModel(modelChordCV);
        p->addModel(modelScaleCV);
        p->addModel(modelRandomNoteCV);
        p->addModel(modelDiatonicCV);
    }
}

static void initStatic__admiral()
{
    Plugin* const p = new Plugin;
    pluginInstance__admiral = p;

    const StaticPluginLoader spl(p, "admiral");
    if (spl.ok())
    {
        p->addModel(modelWatches);
        p->addModel(modelShifts);
        p->addModel(modelTables);
        p->addModel(modelDivisions);
    }
}

static void initStatic__alefsbits()
{
    Plugin* const p = new Plugin;
    pluginInstance__alefsbits = p;

    const StaticPluginLoader spl(p, "alefsbits");
    if (spl.ok())
    {
#define modelSteps modelalefsbitsSteps
#define modelLogic modelalefsbitsLogic
        p->addModel(modelSimplexandhold);
        p->addModel(modelBlank6hp);
        p->addModel(modelPolyrand);
        p->addModel(modelNoize);
        p->addModel(modelSteps);
        p->addModel(modelFibb);
        p->addModel(modelOctsclr);
        p->addModel(modelShift);
        p->addModel(modelMlt);
        p->addModel(modelMath);
        p->addModel(modelLogic);
        p->addModel(modelProbablynot);
#undef modelSteps
#undef modelLogic
    }
}

static void initStatic__Algoritmarte()
{
    Plugin* const p = new Plugin;
    pluginInstance__Algoritmarte = p;

    const StaticPluginLoader spl(p, "Algoritmarte");
    if (spl.ok())
    {
        p->addModel(modelClockkky);
        p->addModel(modelPlanetz);
        p->addModel(modelMusiFrog);
        p->addModel(modelZefiro);
        p->addModel(modelHoldMeTight);
        p->addModel(modelCyclicCA);
        p->addModel(modelMusiMath);
    }
}

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
        p->addModel(model_AC_LFold);
    }
}

static void initStatic__ArableInstruments()
{
    Plugin* const p = new Plugin;
    pluginInstance__ArableInstruments = p;

    const StaticPluginLoader spl(p, "ArableInstruments");
    if (spl.ok())
    {
#define modelClouds modelArableClouds
        p->addModel(modelClouds);
#undef modelClouds
    }
}

static void initStatic__Aria()
{
    Plugin* const p = new Plugin;
    pluginInstance__Aria = p;

    const StaticPluginLoader spl(p, "AriaModules");
    if (spl.ok())
    {
#define modelBlank modelAriaBlank
        p->addModel(modelSplort);
        p->addModel(modelSmerge);
        p->addModel(modelSpleet);
        p->addModel(modelSwerge);
        p->addModel(modelSplirge);
        p->addModel(modelQqqq);
        p->addModel(modelQuack);
        p->addModel(modelQ);
        p->addModel(modelQuale);
        p->addModel(modelDarius);
        p->addModel(modelSolomon4);
        p->addModel(modelSolomon8);
        p->addModel(modelSolomon16);
        p->addModel(modelPsychopump);
        p->addModel(modelPokies4);
        p->addModel(modelGrabby);
        p->addModel(modelRotatoes4);
        p->addModel(modelUndular);
        p->addModel(modelBlank);
#undef modelBlank
        // NOTE disabled in Cardinal due to online requirement
        spl.removeModule("Arcane");
        spl.removeModule("Atout");
        spl.removeModule("Aleister");
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
#define modelWaveShaper modelASWaveShaper
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
#undef modelWaveShaper
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

static void initStatic__Autinn()
{
    Plugin* const p = new Plugin;
    pluginInstance__Autinn = p;

    const StaticPluginLoader spl(p, "Autinn");
    if (spl.ok())
    {
#define modelChord modelAutinnChord
#define modelVibrato modelAutinnVibrato
        p->addModel(modelAmp);
        p->addModel(modelDeadband);
        p->addModel(modelBass);
        p->addModel(modelCVConverter);
        p->addModel(modelDC);
        p->addModel(modelDigi);
        p->addModel(modelFlopper);
        p->addModel(modelFlora);
        p->addModel(modelJette);
        p->addModel(modelBoomerang);
        p->addModel(modelOxcart);
        p->addModel(modelSaw);
        p->addModel(modelSjip);
        p->addModel(modelSquare);
        p->addModel(modelVibrato);
        p->addModel(modelVectorDriver);
        p->addModel(modelZod);
        p->addModel(modelTriBand);
        p->addModel(modelMixer6);
        p->addModel(modelNon);
        p->addModel(modelFil);
        p->addModel(modelNap);
        p->addModel(modelMelody);
        p->addModel(modelChord);
#undef modelChord
#undef modelVibrato
    }
}

static void initStatic__Axioma()
{
    Plugin* const p = new Plugin;
    pluginInstance__Axioma = p;

    const StaticPluginLoader spl(p, "Axioma");
    if (spl.ok())
    {
        p->addModel(modelTheBifurcator);
        p->addModel(modelTesseract);
        p->addModel(modelIkeda);
        p->addModel(modelRhodonea);
    }
}

static void initStatic__Bacon()
{
    Plugin* const p = new Plugin;
    pluginInstance__Bacon = p;

    const StaticPluginLoader spl(p, "BaconPlugs");
    if (spl.ok())
    {
        p->addModel(modelHarMoNee);
        p->addModel(modelGlissinator);
        p->addModel(modelPolyGnome);
        p->addModel(modelQuantEyes);
        p->addModel(modelSampleDelay);
        p->addModel(modelChipNoise);
        p->addModel(modelChipWaves);
        p->addModel(modelChipYourWave);
        p->addModel(modelOpen303);
        p->addModel(modelKarplusStrongPoly);
        p->addModel(modelALingADing);
        p->addModel(modelBitulator);
        p->addModel(modelPolyGenerator);
        p->addModel(modelLintBuddy);
        p->addModel(modelLuckyHold);
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
#define modelMixer modelBefacoMixer
#define modelBurst modelBefacoBurst
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
        p->addModel(modelNoisePlethora);
        p->addModel(modelChannelStrip);
        p->addModel(modelPonyVCO);
        p->addModel(modelMotionMTR);
        p->addModel(modelBurst);
        p->addModel(modelVoltio);
        p->addModel(modelOctaves);
#undef modelADSR
#undef modelMixer
#undef modelBurst

        // NOTE disabled in Cardinal due to MIDI usage
        spl.removeModule("MidiThingV2");
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
        p->addModel(modelZOUMAIExpander);
        p->addModel(modelENCORE);
        p->addModel(modelENCOREExpander);
        p->addModel(modelMU);
        p->addModel(modelCHUTE);
        p->addModel(modelLOURDE);
        p->addModel(modelDILEMO);
        p->addModel(modelLAMBDA);
        p->addModel(modelBANCAU);
        p->addModel(modelACNE);
        p->addModel(modelMS);
        p->addModel(modelDUKE);
        p->addModel(modelMOIRE);
        p->addModel(modelPILOT);
        p->addModel(modelHUITRE);
        p->addModel(modelOUAIVE);
        p->addModel(modelEDSAROS);
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
        p->addModel(modelSPORE);
        p->addModel(modelDFUZE);
        p->addModel(modelREI);
        p->addModel(modelRABBIT);
        p->addModel(modelBISTROT);
        p->addModel(modelSIGMA);
        p->addModel(modelFLAME);
        p->addModel(modelVOID);

        // NOTE disabled in Cardinal due to curl usage
        // p->addModel(modelANTN);
        spl.removeModule("antN");
    }
}

static void initStatic__Biset()
{
    Plugin* const p = new Plugin;
    pluginInstance__Biset = p;

    const StaticPluginLoader spl(p, "Biset");
    if (spl.ok())
    {
#define modelBlank modelBisetBlank
#define modelTree modelBisetTree
        p->addModel(modelTracker);
        p->addModel(modelTrackerSynth);
        p->addModel(modelTrackerDrum);
        p->addModel(modelTrackerClock);
        p->addModel(modelTrackerPhase);
        p->addModel(modelTrackerQuant);
        p->addModel(modelTrackerState);
        p->addModel(modelTrackerControl);

        p->addModel(modelRegex);
        p->addModel(modelRegexCondensed);
        p->addModel(modelRegexExp);

        p->addModel(modelTree);
        p->addModel(modelTreeSeed);

        p->addModel(modelGbu);
        p->addModel(modelPkm);

        p->addModel(modelIgc);
        p->addModel(modelOmega3);
        p->addModel(modelOmega6);

        p->addModel(modelSegfault);
        p->addModel(modelBlank);
#undef modelBlank
#undef modelTree
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
        skins._default = settings::preferDarkPanels ? "dark" : "light";
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

static void initStatic__CatroModulo()
{
    Plugin* const p = new Plugin;
    pluginInstance__CatroModulo = p;

    const StaticPluginLoader spl(p, "CatroModulo");
    if (spl.ok())
    {
        p->addModel(modelCM1Module);
        p->addModel(modelCM2Module);
        p->addModel(modelCM3Module);
        p->addModel(modelCM4Module);
        p->addModel(modelCM5Module);
        p->addModel(modelCM6Module);
        p->addModel(modelCM7Module);
        p->addModel(modelCM8Module);
        p->addModel(modelCM9Module);
        p->addModel(modelCM10Module);
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

static void initStatic__ChowDSP()
{
    Plugin* const p = new Plugin;
    pluginInstance__ChowDSP = p;

    const StaticPluginLoader spl(p, "ChowDSP");
    if (spl.ok())
    {
        p->addModel(modelChowTape);
        p->addModel(modelChowPhaserFeedback);
        p->addModel(modelChowPhaserMod);
        p->addModel(modelChowFDN);
        p->addModel(modelChowRNN);
        p->addModel(modelChowModal);
        p->addModel(modelChowDer);
        p->addModel(modelWarp);
        p->addModel(modelWerner);
        p->addModel(modelCredit);
        p->addModel(modelChowPulse);
        p->addModel(modelChowTapeCompression);
        p->addModel(modelChowTapeChew);
        p->addModel(modelChowTapeDegrade);
        p->addModel(modelChowTapeLoss);
        p->addModel(modelChowChorus);
    }
}

static void initStatic__Computerscare()
{
    Plugin* const p = new Plugin;
    pluginInstance__Computerscare = p;

    const StaticPluginLoader spl(p, "Computerscare");
    if (spl.ok())
    {
    	p->addModel(modelComputerscarePatchSequencer);
        p->addModel(modelComputerscareDebug);
        p->addModel(modelComputerscareLaundrySoup);
        p->addModel(modelComputerscareILoveCookies);
        p->addModel(modelComputerscareOhPeas);
        p->addModel(modelComputerscareHorseADoodleDoo);
        p->addModel(modelComputerscareKnolyPobs);
        p->addModel(modelComputerscareBolyPuttons);
        p->addModel(modelComputerscareRolyPouter);
        p->addModel(modelComputerscareSolyPequencer);
        p->addModel(modelComputerscareTolyPools);
        p->addModel(modelComputerscareMolyPatrix);
        p->addModel(modelComputerscareGolyPenerator);
        p->addModel(modelComputerscareFolyPace);
        p->addModel(modelComputerscareStolyFickPigure);
        p->addModel(modelComputerscareBlank);
        p->addModel(modelComputerscareBlankExpander);
    }
}

static void initStatic__CVfunk()
{
    Plugin* const p = new Plugin;
    pluginInstance__CVfunk = p;

    const StaticPluginLoader spl(p, "CVfunk");
    if (spl.ok())
    {
#define modelSteps modelCVfunkSteps
		p->addModel(modelSteps);
		p->addModel(modelEnvelopeArray);
		p->addModel(modelPentaSequencer);
		p->addModel(modelImpulseController);
		p->addModel(modelSignals);
		p->addModel(modelRanges);
		p->addModel(modelHexMod);
		p->addModel(modelCollatz);
		p->addModel(modelStrings);
		p->addModel(modelMagnets);
		p->addModel(modelOuros);
		p->addModel(modelPressedDuck);
		p->addModel(modelFlowerPatch);
		p->addModel(modelSyncro);
		p->addModel(modelNona);
		p->addModel(modelDecima);
		p->addModel(modelMorta);
#undef modelSteps
    }
}

static void initStatic__dBiz()
{
    Plugin* const p = new Plugin;
    pluginInstance__dBiz = p;

    const StaticPluginLoader spl(p, "dBiz");
    if (spl.ok())
    {
#define modelChord modeldBizChord
#define modelDivider modeldBizDivider
#define modelFourSeq modeldBizFourSeq
#define modelVCA4 modeldBizVCA4
        p->addModel(modelNavControl);
        p->addModel(modelBench);
        p->addModel(modelContorno);
        //p->addModel(modelContornoMK2);
        p->addModel(modelTranspose);
        p->addModel(modelUtility);
        p->addModel(modelChord);
        p->addModel(modelBene);
        p->addModel(modelBenePads);
        p->addModel(modelPerfMixer);
        p->addModel(modelDrMix);
        p->addModel(modelPerfMixer4);
        p->addModel(modelVCA4);
        p->addModel(modelVCA530);
        p->addModel(modelRemix);
        p->addModel(modelSmixer);
        p->addModel(modelVerbo);
        p->addModel(modelDVCO);
        p->addModel(modelDAOSC);
        p->addModel(modelTROSC);
        p->addModel(modelTROSCMK2);
        p->addModel(modelSuHa);
        p->addModel(modelSuHaMK2);
        p->addModel(modelFourSeq);
        p->addModel(modelDivider);
        p->addModel(modelUtil2);
        p->addModel(modelSmorph);
        p->addModel(modelBigSmorph);
        p->addModel(modelSPan);
        p->addModel(modelQuePasa);
        p->addModel(modelDualFilter);
        p->addModel(modelOrder);
        p->addModel(modelDualMatrix);
#undef modelChord
#undef modelDivider
#undef modelFourSeq
#undef modelVCA4
    }
}

static void initStatic__DHE()
{
    Plugin* const p = new Plugin;
    pluginInstance__DHE = p;

    const StaticPluginLoader spl(p, "DHE-Modules");
    if (spl.ok())
    {
        dhe::blossom::init(p);
        dhe::buttons::init(p);
        dhe::cubic::init(p);
        dhe::curve_sequencer::init(p);
        dhe::envelope::init(p);
        dhe::func::init(p);
        dhe::fuzzy_logic::init(p);
        dhe::gator::init(p);
        dhe::ranger::init(p);
        dhe::scannibal::init(p);
        dhe::sequencizer::init(p);
        dhe::swave::init(p);
        dhe::tapers::init(p);
        dhe::truth::init(p);
        dhe::xycloid::init(p);
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

static void initStatic__EnigmaCurry()
{
    Plugin* const p = new Plugin;
    pluginInstance__EnigmaCurry = p;

    const StaticPluginLoader spl(p, "EnigmaCurry");
    if (spl.ok())
    {
#define modelPulse modelEnigmaCurryPulse
      p->addModel(modelTransport);
      p->addModel(modelLatch);
      p->addModel(modelPulse);
      p->addModel(modelRange);
#undef modelPulse
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

static void initStatic__ExpertSleepersEncoders()
{
    Plugin* const p = new Plugin;
    pluginInstance__ExpertSleepersEncoders = p;

    const StaticPluginLoader spl(p, "ExpertSleepers-Encoders");
    if (spl.ok())
    {
        p->addModel(model8GT);
        p->addModel(model8CV);
        p->addModel(modelES40);
        p->addModel(modelES5);
        p->addModel(modelSMUX);
        p->addModel(modelCalibrator);
    }
}

static void initStatic__Extratone()
{
    Plugin* const p = new Plugin;
    pluginInstance__Extratone = p;

    const StaticPluginLoader spl(p, "Extratone");
    if (spl.ok())
    {
        p->addModel(modelModulo);
        p->addModel(modelMesoglea);
        p->addModel(modelMesoglea2);
        p->addModel(modelOpabinia);
        p->addModel(modelSplitterburst);
        p->addModel(modelPuzzlebox);
        p->addModel(modelDarwinism);
        // p->addModel(modelHalluciMemory);
        p->addModel(modelIchneumonid);
        p->addModel(modelMeganeura);
        p->addModel(modelPureneura);
        p->addModel(modelMesohyl);
        p->addModel(modelXtrtnBlank);
    }
}

static void initStatic__FehlerFabrik()
{
    Plugin* const p = new Plugin;
    pluginInstance__FehlerFabrik = p;

    const StaticPluginLoader spl(p, "FehlerFabrik");
    if (spl.ok())
    {
        p->addModel(modelPSIOP);
        p->addModel(modelPlanck);
        p->addModel(modelLuigi);
        p->addModel(modelAspect);
        p->addModel(modelMonte);
        p->addModel(modelArpanet);
        p->addModel(modelSigma);
        p->addModel(modelFax);
        p->addModel(modelRasoir);
        p->addModel(modelChi);
        p->addModel(modelNova);
        p->addModel(modelLilt);
        p->addModel(modelBotzinger);
    }
}

static void initStatic__forsitan()
{
    Plugin* const p = new Plugin;
    pluginInstance__forsitan = p;

    const StaticPluginLoader spl(p, "forsitan-modulare");
    if (spl.ok())
    {
        p->addModel(alea);
        p->addModel(interea);
        p->addModel(cumuli);
        p->addModel(deinde);
        p->addModel(pavo);
    }
}

static void initStatic__GlueTheGiant()
{
    Plugin* const p = new Plugin;
    pluginInstance__GlueTheGiant = p;

    const StaticPluginLoader spl(p, "GlueTheGiant");
    if (spl.ok())
    {
        p->addModel(modelGigBus);
        p->addModel(modelMiniBus);
        p->addModel(modelSchoolBus);
        p->addModel(modelMetroCityBus);
        p->addModel(modelBusDepot);
        p->addModel(modelBusRoute);
        p->addModel(modelRoad);
        p->addModel(modelEnterBus);
        p->addModel(modelExitBus);
    }
}

static void initStatic__GoodSheperd()
{
    Plugin* const p = new Plugin;
    pluginInstance__GoodSheperd = p;

    const StaticPluginLoader spl(p, "GoodSheperd");
    if (spl.ok())
    {
        p->addModel(modelHurdle);
        p->addModel(modelSEQ3st);
        p->addModel(modelStable16);
        p->addModel(modelStall);
        p->addModel(modelSwitch1);
        p->addModel(modelSeqtrol);
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
        p->addModel(modelCompare3);
        p->addModel(modelLFO3);
        p->addModel(modelLFO4);
        p->addModel(modelLogic);
        p->addModel(modelMerge8);
        p->addModel(modelMergeSplit4);
        p->addModel(modelMicrotonalChords);
        p->addModel(modelMicrotonalNotes);
        p->addModel(modelNoteMT);
        p->addModel(modelPeak);
        p->addModel(modelPolyMergeResplit);
        p->addModel(modelPolySplit);
        p->addModel(modelPush);
        p->addModel(modelQuant);
        p->addModel(modelQuantIntervals);
        p->addModel(modelQuantMT);
        p->addModel(modelSampleDelays);
        p->addModel(modelScale);
        p->addModel(modelSplit8);
        p->addModel(modelTails);
        p->addModel(modelTails4);
        p->addModel(modelVarSampleDelays);
        p->addModel(modelVCA3);
        p->addModel(modelVCA4);
    }
}

static void initStatic__H4N4()
{
    Plugin* const p = new Plugin;
    pluginInstance__H4N4 = p;

    const StaticPluginLoader spl(p, "h4n4-modules");
    if (spl.ok())
    {
        p->addModel(modelXenQnt);
    }
}

static void initStatic__HamptonHarmonics()
{
    Plugin* const p = new Plugin;
    pluginInstance__HamptonHarmonics = p;

    const StaticPluginLoader spl(p, "HamptonHarmonics");
    if (spl.ok())
    {
#define modelArp modelHamptonHarmonicsArp
#define modelProgress modelHamptonHarmonicsProgress
        p->addModel(modelArp);
        p->addModel(modelProgress);
#undef modelProgress
#undef modelArp
    }
}

static void initStatic__HetrickCV()
{
    Plugin* const p = new Plugin;
    pluginInstance__HetrickCV = p;

    const StaticPluginLoader spl(p, "HetrickCV");
    if (spl.ok())
    {
#define modelASR modelHetrickCVASR
#define modelBlankPanel modelHetrickCVBlankPanel
#define modelFlipFlop modelHetrickCVFlipFlop
#define modelMidSide modelHetrickCVMidSide
#define modelMinMax modelHetrickCVMinMax
#define PanelBaseWidget HetrickCVPanelBaseWidget
#define InverterWidget HetrickCVInverterWidget
        p->addModel(modelTwoToFour);
        p->addModel(modelAnalogToDigital);
        p->addModel(modelASR);
        p->addModel(modelBinaryGate);
        p->addModel(modelBinaryNoise);
        p->addModel(modelBitshift);
        p->addModel(modelBlankPanel);
        p->addModel(modelBoolean3);
        p->addModel(modelChaos1Op);
        p->addModel(modelChaos2Op);
        p->addModel(modelChaos3Op);
        p->addModel(modelChaoticAttractors);
        p->addModel(modelClockedNoise);
        p->addModel(modelComparator);
        p->addModel(modelContrast);
        p->addModel(modelCrackle);
        p->addModel(modelDataCompander);
        p->addModel(modelDelta);
        p->addModel(modelDigitalToAnalog);
        p->addModel(modelDust);
        p->addModel(modelExponent);
        p->addModel(modelFBSineChaos);
        p->addModel(modelFlipFlop);
        p->addModel(modelFlipPan);
        p->addModel(modelGateDelay);
        p->addModel(modelGateJunction);
        p->addModel(modelGateJunctionExp);
        p->addModel(modelGingerbread);
        p->addModel(modelLogicCombine);
        p->addModel(modelMidSide);
        p->addModel(modelMinMax);
        p->addModel(modelPhaseDrivenSequencer);
        p->addModel(modelPhaseDrivenSequencer32);
        p->addModel(modelPhasorAnalyzer);
        p->addModel(modelPhasorBurstGen);
        p->addModel(modelPhasorDivMult);
        p->addModel(modelPhasorEuclidean);
        p->addModel(modelPhasorGates);
        p->addModel(modelPhasorGates32);
        p->addModel(modelPhasorGates64);
        p->addModel(modelPhasorGen);
        p->addModel(modelPhasorGeometry);
        p->addModel(modelPhasorHumanizer);
        p->addModel(modelPhasorMixer);
        p->addModel(modelPhasorOctature);
        p->addModel(modelPhasorQuadrature);
        p->addModel(modelPhasorRandom);
        p->addModel(modelPhasorRanger);
        p->addModel(modelPhasorReset);
        p->addModel(modelPhasorRhythmGroup);
        p->addModel(modelPhasorShape);
        p->addModel(modelPhasorShift);
        p->addModel(modelPhasorStutter);
        p->addModel(modelPhasorSubstepShape);
        p->addModel(modelPhasorSwing);
        p->addModel(modelPhasorTimetable);
        p->addModel(modelPhasorToClock);
        p->addModel(modelPhasorToLFO);
        p->addModel(modelPhasorToWaveforms);
        p->addModel(modelProbability);
        p->addModel(modelRandomGates);
        p->addModel(modelRotator);
        p->addModel(modelRungler);
        p->addModel(modelScanner);
        p->addModel(modelVectorMix);
        p->addModel(modelWaveshape);
        p->addModel(modelXYToPolar);
#undef modelASR
#undef modelBlankPanel
#undef modelFlipFlop
#undef modelMidSide
#undef modelMinMax
#undef PanelBaseWidget
#undef InverterWidget
    }
}

static void initStatic__ImpromptuModular()
{
    Plugin* const p = new Plugin;
    pluginInstance__ImpromptuModular = p;

    const StaticPluginLoader spl(p, "ImpromptuModular");
    if (spl.ok())
    {
        p->addModel(modelAdaptiveQuantizer);
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
        p->addModel(modelSemiModularSynth);
        p->addModel(modelSygen);
        p->addModel(modelTact);
        p->addModel(modelTact1);
        p->addModel(modelTactG);
        p->addModel(modelTwelveKey);
        p->addModel(modelVariations);
        p->addModel(modelWriteSeq32);
        p->addModel(modelWriteSeq64);
        p->addModel(modelBlankPanel);
    }
}

static void initStatic__ihtsyn()
{
    Plugin* const p = new Plugin;
    pluginInstance__ihtsyn = p;

    const StaticPluginLoader spl(p, "ihtsyn");
    if (spl.ok())
    {
        p->addModel(modelPitchMangler);
        p->addModel(modelTwistedVerb);
        p->addModel(modelHiVerb);
        p->addModel(modelMVerb);
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
       #ifndef STATIC_BUILD
        p->addModel(modelStr1ker);
       #else
        spl.removeModule("Str1ker");
       #endif
#undef modelQuantizer
    }
}

static void initStatic__kocmoc()
{
    Plugin* const p = new Plugin;
    pluginInstance__kocmoc= p;

    const StaticPluginLoader spl(p, "kocmoc");
    if (spl.ok())
    {
        p->addModel(modelSVF_1);
        p->addModel(modelTRG);
        p->addModel(modelLADR);
        p->addModel(modelOP);
        p->addModel(modelPHASR);
        p->addModel(modelMUL);
        p->addModel(modelSKF);
        p->addModel(modelDDLY);
    }
}

static void initStatic__LifeFormModular()
{
    Plugin* const p = new Plugin;
    pluginInstance__LifeFormModular= p;

    const StaticPluginLoader spl(p, "LifeFormModular");
    if (spl.ok())
    {
        p->addModel(modelTimeDiktat);
        p->addModel(modelSequenceModeler);
        p->addModel(modelPitchDiktat);
        p->addModel(modelPitchIntegrator);
        p->addModel(modelBurstIntegrator);
        p->addModel(modelQuadModulator);
        p->addModel(modelImpulseControl);
        p->addModel(modelQuadSteppedOffset);
        p->addModel(modelPercussiveVibration);
        p->addModel(modelQuadUtility);
        p->addModel(modelAdditiveVibration);
        p->addModel(modelComplexOsc);
        p->addModel(modelDriftgen);
    }
}

static void initStatic__LilacLoop()
{
    Plugin* const p = new Plugin;
    pluginInstance__LilacLoop = p;

    const StaticPluginLoader spl(p, "LilacLoop");
    if (spl.ok())
    {
        p->addModel(modelLooperOne);
    }
}

static void initStatic__LittleUtils()
{
    Plugin* const p = new Plugin;
    pluginInstance__LittleUtils = p;

    const StaticPluginLoader spl(p, "LittleUtils");
    if (spl.ok())
    {
        p->addModel(modelButtonModule);
        p->addModel(modelPulseGenerator);
        p->addModel(modelBias_Semitone);
        p->addModel(modelMulDiv);
        p->addModel(modelTeleportInModule);
        p->addModel(modelTeleportOutModule);
    }
}

static void initStatic__Lomas()
{
    Plugin* const p = new Plugin;
    pluginInstance__Lomas = p;

    const StaticPluginLoader spl(p, "LomasModules");
    if (spl.ok())
    {
        p->addModel(modelAdvancedSampler);
        p->addModel(modelGateSequencer);
    }
}

static void initStatic__Lyrae()
{
    Plugin* const p = new Plugin;
    pluginInstance__Lyrae = p;

    const StaticPluginLoader spl(p, "LyraeModules");
    if (spl.ok())
    {
#define modelDelta modelLyraeDelta
        p->addModel(modelSulafat);
        p->addModel(modelGamma);
        p->addModel(modelDelta);
        p->addModel(modelVega);
        p->addModel(modelBD383238);
        p->addModel(modelZeta);
#undef modelDelta
    }
}

static void initStatic__Meander()
{
    Plugin* const p = new Plugin;
    pluginInstance__Meander = p;

    const StaticPluginLoader spl(p, "Meander");
    if (spl.ok())
    {
        // for dark theme
        panelTheme = 1;
        p->addModel(modelMeander);
    }
}

static void initStatic__MindMeld()
{
    Plugin* const p = new Plugin;
    pluginInstance__MindMeld = p;

    const StaticPluginLoader spl(p, "MindMeldModular");
    if (spl.ok())
    {
        p->addModel(modelPatchMaster);
        p->addModel(modelPatchMasterBlank);
        p->addModel(modelRouteMasterMono5to1);
        p->addModel(modelRouteMasterStereo5to1);
        p->addModel(modelRouteMasterMono1to5);
        p->addModel(modelRouteMasterStereo1to5);
        p->addModel(modelMasterChannel);
        p->addModel(modelMeld);
        p->addModel(modelUnmeld);
        p->addModel(modelMSMelder);
        p->addModel(modelEqMaster);
        p->addModel(modelEqExpander);
        p->addModel(modelBassMaster);
        p->addModel(modelBassMasterJr);
        p->addModel(modelShapeMaster);
        p->addModel(modelMixMasterJr);
        p->addModel(modelAuxExpanderJr);
        p->addModel(modelMixMaster);
        p->addModel(modelAuxExpander);
    }
}

static void initStatic__ML()
{
    Plugin* const p = new Plugin;
    pluginInstance__ML = p;

    const StaticPluginLoader spl(p, "ML_modules");
    if (spl.ok())
    {
#define modelQuantizer modelMLQuantizer
#define modelSH8 modelMLSH8
        p->addModel(modelQuantizer);
        p->addModel(modelQuantum);
        p->addModel(modelTrigBuf);
        p->addModel(modelSeqSwitch);
        p->addModel(modelSeqSwitch2);
        p->addModel(modelShiftRegister);
        p->addModel(modelShiftRegister2);
        p->addModel(modelFreeVerb);
        p->addModel(modelSum8);
        p->addModel(modelSum8mk2);
        p->addModel(modelSum8mk3);
        p->addModel(modelSH8);
        p->addModel(modelConstants);
        p->addModel(modelCounter);
        p->addModel(modelTrigDelay);
        p->addModel(modelBPMdetect);
        p->addModel(modelVoltMeter);
        p->addModel(modelOctaFlop);
        p->addModel(modelOctaTrig);
        p->addModel(modelOctaSwitch);
        p->addModel(modelTrigSwitch);
        p->addModel(modelTrigSwitch2);
        p->addModel(modelTrigSwitch3);
        p->addModel(modelTrigSwitch3_2);
        p->addModel(modelOctaPlus);
        p->addModel(modelOctaTimes);
        p->addModel(modelCloner);
        p->addModel(modelPolySplitter);
        p->addModel(modelArpeggiator);
#undef modelQuantizer
#undef modelSH8
    }
}

static void initStatic__MockbaModular()
{
    Plugin* const p = new Plugin;
    pluginInstance__MockbaModular = p;

    const StaticPluginLoader spl(p, "MockbaModular");
    if (spl.ok())
    {
#define modelBlank modelMockbaModularBlank
#define modelComparator modelMockbaModularComparator
        p->addModel(modelBlank);
        p->addModel(modelFeidah);
        p->addModel(modelFeidahS);
        p->addModel(modelFiltah);
        p->addModel(modelMixah);
        p->addModel(modelMixah3);
        p->addModel(modelDividah);
        p->addModel(modelCountah);
        p->addModel(modelSelectah);
        p->addModel(modelShapah);
        p->addModel(modelHoldah);
        p->addModel(modelPannah);
        p->addModel(modelReVoltah);
        p->addModel(modelCZSaw);
        p->addModel(modelCZSquare);
        p->addModel(modelCZPulse);
        p->addModel(modelCZDblSine);
        p->addModel(modelCZSawPulse);
        p->addModel(modelCZReso1);
        p->addModel(modelCZReso2);
        p->addModel(modelCZReso3);
        p->addModel(modelCZOsc);
        p->addModel(modelMaugTriangle);
        p->addModel(modelMaugShark);
        p->addModel(modelMaugSaw);
        p->addModel(modelMaugSaw2);
        p->addModel(modelMaugSquare);
        p->addModel(modelMaugSquare2);
        p->addModel(modelMaugSquare3);
        p->addModel(modelMaugOsc);
        p->addModel(modelComparator);
        p->addModel(modelDualBUFFER);
        p->addModel(modelDualNOT);
        p->addModel(modelDualOR);
        p->addModel(modelDualNOR);
        p->addModel(modelDualAND);
        p->addModel(modelDualNAND);
        p->addModel(modelDualXOR);
        p->addModel(modelDualXNOR);
        p->addModel(modelPSelectah);

        // require input files to work
        spl.removeModule("UDPClockMaster");
        spl.removeModule("UDPClockSlave");
#undef modelBlank
#undef modelComparator
    }
}

static void initStatic__Mog()
{
    Plugin* const p = new Plugin;
    pluginInstance__Mog = p;

    const StaticPluginLoader spl(p, "Mog");
    if (spl.ok())
    {
        p->addModel(modelNetwork);
        p->addModel(modelNexus);
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

static void initStatic__MSM()
{
    Plugin* const p = new Plugin;
    pluginInstance__MSM = p;

    const StaticPluginLoader spl(p, "MSM");
    if (spl.ok())
    {
#define modelADSR modelMSMADSR
#define modelBlankPanel modelMSMBlankPanel
#define modelDelay modelMSMDelay
#define modelLFO modelMSMLFO
#define modelMult modelMSMMult
#define modelNoise modelMSMNoise
#define modelVCA modelMSMVCA
#define modelVCO modelMSMVCO
        p->addModel(modelVCO);
        p->addModel(modelBVCO);
        p->addModel(modelExperimentalVCO);
        p->addModel(modelNoise);
        p->addModel(modelLFO);
        p->addModel(modelVCA);
        p->addModel(modelADSR);
        p->addModel(modelDelay);
        p->addModel(modelWaveShaper);
        p->addModel(modelWavefolder);
        p->addModel(modelBitcrusher);
        p->addModel(modelPhaserModule);
        p->addModel(modelMorpher);
        p->addModel(modelRingMod);
        p->addModel(modelRandomSource);
        p->addModel(modelMult);
        p->addModel(modelCrazyMult);
        p->addModel(modelFade);
        p->addModel(modelSimpleSlider);
        p->addModel(modelxseq);
        p->addModel(modelBlankPanel);
#undef modelADSR
#undef modelBlankPanel
#undef modelDelay
#undef modelLFO
#undef modelMult
#undef modelNoise
#undef modelVCA
#undef modelVCO
    }
}

static void initStatic__MUS_X()
{
    Plugin* const p = new Plugin;
    pluginInstance__MUS_X = p;

    const StaticPluginLoader spl(p, "MUS-X");
    if (spl.ok())
    {
    	p->addModel(musx::modelADSR);
    	p->addModel(musx::modelDelay);
    	p->addModel(musx::modelDrift);
    	p->addModel(musx::modelFilter);
    	p->addModel(musx::modelLast);
    	p->addModel(musx::modelLFO);
    	p->addModel(musx::modelModMatrix);
    	p->addModel(musx::modelOnePole);
    	p->addModel(musx::modelOnePoleLP);
    	p->addModel(musx::modelOscillators);
    	p->addModel(musx::modelSplitStack);
    	p->addModel(musx::modelSynth);
    	p->addModel(musx::modelTuner);
    }
}

static void initStatic__myth_modules()
{
    Plugin* const p = new Plugin;
    pluginInstance__myth_modules = p;

    const StaticPluginLoader spl(p, "myth-modules");
    if (spl.ok())
    {
        p->addModel(modelMavka);
        p->addModel(modelMolphar);
    }
}

static void initStatic__nonlinearcircuits()
{
    Plugin* const p = new Plugin;
    pluginInstance__nonlinearcircuits = p;

    const StaticPluginLoader spl(p, "nonlinearcircuits");
    if (spl.ok())
    {
        p->addModel(model4Seq);
        p->addModel(modelCipher);
        p->addModel(modelBOOLs);
        p->addModel(modelDivideConquer);
        p->addModel(modelDivineCMOS);
        p->addModel(modelDoubleNeuron);
        p->addModel(modelGenie);
        p->addModel(modelLetsSplosh);
        p->addModel(modelNeuron);
        p->addModel(modelNumberwang);
        p->addModel(modelRouter);
        p->addModel(modelSegue);
        p->addModel(modelSlothApathy);
        p->addModel(modelSlothInertia);
        p->addModel(modelSlothTorpor);
        p->addModel(modelSquidAxon);
        p->addModel(modelStatues);
        p->addModel(modelTripleSloth);
    }
}

static void initStatic__Orbits()
{
    Plugin* const p = new Plugin;
    pluginInstance__Orbits = p;

    const StaticPluginLoader spl(p, "Orbits");
    if (spl.ok())
    {
        p->addModel(modelRareBreeds_Orbits_Eugene);
        p->addModel(modelRareBreeds_Orbits_Polygene);
    }
}

static void initStatic__ParableInstruments()
{
    Plugin* const p = new Plugin;
    pluginInstance__ParableInstruments = p;

    const StaticPluginLoader spl(p, "ParableInstruments");
    if (spl.ok())
    {
#define modelClouds modelParableClouds
        p->addModel(modelClouds);
#undef modelClouds
    }
}

static void initStatic__PathSet()
{
    Plugin* const p = new Plugin;
    pluginInstance__PathSet = p;

    const StaticPluginLoader spl(p, "PathSet");
    if (spl.ok())
    {
        p->addModel(modelShiftyMod);
        p->addModel(modelShiftyExpander);
        p->addModel(modelIceTray);
        p->addModel(modelAstroVibe);
        p->addModel(modelGlassPane);
	    p->addModel(modelPlusPane);
        p->addModel(modelNudge);
        p->addModel(modelOneShot);
    }
}

static void initStatic__PdArray()
{
    Plugin* const p = new Plugin;
    pluginInstance__PdArray = p;

    const StaticPluginLoader spl(p, "PdArray");
    if (spl.ok())
    {
        p->addModel(modelArray);
        p->addModel(modelMiniramp);
        p->addModel(modelMinistep);
    }
}

static void initStatic__PinkTrombone()
{
    Plugin* const p = new Plugin;
    pluginInstance__PinkTrombone = p;

    const StaticPluginLoader spl(p, "PinkTrombone");
    if (spl.ok())
    {
        p->addModel(modelPinkTrombone);
    }
}

static void initStatic__Prism()
{
    Plugin* const p = new Plugin;
    pluginInstance__Prism = p;

    const StaticPluginLoader spl(p, "Prism");
    if (spl.ok())
    {
        p->addModel(modelRainbow);
        p->addModel(modelRainbowScaleExpander);
        p->addModel(modelDroplet);
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

static void initStatic__RCM()
{
    Plugin* const p = new Plugin;
    pluginInstance__RCM = p;

    const StaticPluginLoader spl(p, "rcm-modules");
    if (spl.ok())
    {
        p->addModel(modelGVerbModule);
        p->addModel(modelCV0to10Module);
        p->addModel(modelCVS0to10Module);
        p->addModel(modelCV5to5Module);
        p->addModel(modelCVMmtModule);
        p->addModel(modelCVTglModule);
        p->addModel(modelPianoRollModule);
        p->addModel(modelDuckModule);
        p->addModel(modelSEQAdapterModule);
        p->addModel(modelSyncModule);
        p->addModel(modelPolyNosModule);
    }
}

static void initStatic__RebelTech()
{
    Plugin* const p = new Plugin;
    pluginInstance__RebelTech = p;

    const StaticPluginLoader spl(p, "RebelTech");
    if (spl.ok())
    {
        p->addModel(modelStoicheia);
        p->addModel(modelTonic);
        p->addModel(modelKlasmata);
        p->addModel(modelCLK);
        p->addModel(modelLogoi);
        p->addModel(modelPhoreo);
    }
}

static void initStatic__repelzen()
{
    Plugin* const p = new Plugin;
    pluginInstance__repelzen = p;

    const StaticPluginLoader spl(p, "repelzen");
    if (spl.ok())
    {
#define modelBlank modelrepelzenBlank
#define modelMixer modelrepelzenMixer
#define modelWerner modelrepelzenWerner
        p->addModel(modelBlank);
        p->addModel(modelBurst);
        p->addModel(modelFolder);
        p->addModel(modelErwin);
        p->addModel(modelWerner);
        p->addModel(modelMixer);
#undef modelBlank
#undef modelMixer
#undef modelWerner
    }
}

static void initStatic__Sapphire()
{
    Plugin* const p = new Plugin;
    pluginInstance__sapphire = p;

    const StaticPluginLoader spl(p, "Sapphire");
    if (spl.ok())
    {
        p->addModel(modelSapphireElastika);
        p->addModel(modelSapphireFrolic);
        p->addModel(modelSapphireGalaxy);
        p->addModel(modelSapphireGlee);
        p->addModel(modelSapphireGravy);
        p->addModel(modelSapphireHiss);
        p->addModel(modelSapphireMoots);
        p->addModel(modelSapphireNucleus);
        p->addModel(modelSapphirePivot);
        p->addModel(modelSapphirePolynucleus);
        p->addModel(modelSapphirePop);
        p->addModel(modelSapphireRotini);
        p->addModel(modelSapphireSam);
        p->addModel(modelSapphireTin);
        p->addModel(modelSapphireTout);
        p->addModel(modelSapphireTricorder);
        p->addModel(modelSapphireTubeUnit);
    }
}

static void initStatic__sonusmodular()
{
    Plugin* const p = new Plugin;
    pluginInstance__sonusmodular = p;

    const StaticPluginLoader spl(p, "sonusmodular");
    if (spl.ok())
    {
        p->addModel(modelAddiction);
        p->addModel(modelBitter);
        p->addModel(modelBymidside);
        p->addModel(modelCampione);
        p->addModel(modelChainsaw);
        p->addModel(modelCtrl);
        p->addModel(modelDeathcrush);
        p->addModel(modelFraction);
        p->addModel(modelHarmony);
        p->addModel(modelLadrone);
        p->addModel(modelLuppolo);
        p->addModel(modelLuppolo3);
        p->addModel(modelMicromacro);
        p->addModel(modelMrcheb);
        p->addModel(modelMultimulti);
        p->addModel(modelNeurosc);
        p->addModel(modelOktagon);
        p->addModel(modelOsculum);
        p->addModel(modelParamath);
        p->addModel(modelPiconoise);
        p->addModel(modelPith);
        p->addModel(modelPusher);
        p->addModel(modelRingo);
        p->addModel(modelScramblase);
        p->addModel(modelTropicana);
        p->addModel(modelTwoff);
        p->addModel(modelYabp);
    }
}

static void initStatic__StarlingVia()
{
    Plugin* const p = new Plugin;
    pluginInstance__StarlingVia = p;

    const StaticPluginLoader spl(p, "StarlingVia");
    if (spl.ok())
    {
#define modelScanner modelStarlingViaScanner
        p->addModel(modelMeta);
        p->addModel(modelGateseq);
        p->addModel(modelScanner);
        p->addModel(modelSync);
        p->addModel(modelAtsr);
        p->addModel(modelOsc3);
        p->addModel(modelSync3);
        p->addModel(modelSync3XL);
        p->addModel(modelSync3XLLevels);
#undef modelScanner
    }
}

static void initStatic__stocaudio()
{
    Plugin* const p = new Plugin;
    pluginInstance__stocaudio = p;

    const StaticPluginLoader spl(p, "stocaudio");
    if (spl.ok())
    {
        p->addModel(modelPolyturing);
        p->addModel(modelPolydelay);
        p->addModel(modelSpread);
    }
}

static void initStatic__stoermelder_p1()
{
    Plugin* const p = new Plugin;
    pluginInstance__stoermelder_p1 = p;

    const StaticPluginLoader spl(p, "stoermelder-packone");
    if (spl.ok())
    {
        p->addModel(modelCVMap);
        p->addModel(modelCVMapCtx);
        p->addModel(modelCVMapMicro);
        p->addModel(modelCVPam);
        p->addModel(modelRotorA);
        p->addModel(modelReMoveLite);
        p->addModel(modelBolt);
        p->addModel(modelInfix);
        p->addModel(modelInfixMicro);
        p->addModel(modelEightFace);
        p->addModel(modelEightFaceX2);
        p->addModel(modelSipo);
        p->addModel(modelFourRounds);
        p->addModel(modelArena);
        p->addModel(modelMaze);
        p->addModel(modelHive);
        p->addModel(modelIntermix);
        p->addModel(modelIntermixGate);
        p->addModel(modelIntermixEnv);
        p->addModel(modelIntermixFade);
        p->addModel(modelSail);
        p->addModel(modelPile);
        p->addModel(modelPilePoly);
        p->addModel(modelMirror);
        p->addModel(modelAffix);
        p->addModel(modelAffixMicro);
        p->addModel(modelGrip);
        p->addModel(modelGlue);
        p->addModel(modelGoto);
        p->addModel(modelStroke);
        p->addModel(modelSpin);
        p->addModel(modelTransit);
        p->addModel(modelTransitEx);
        p->addModel(modelX4);
        p->addModel(modelMacro);
        p->addModel(modelOrbit);
        p->addModel(modelEightFaceMk2);
        p->addModel(modelEightFaceMk2Ex);
        p->addModel(modelDirt);
        p->addModel(modelMb);
        p->addModel(modelMe);
        p->addModel(modelRaw);
        p->addModel(modelStrip);
        p->addModel(modelStripBay4);
        p->addModel(modelStripPp);

        spl.removeModule("AudioInterface64");
        spl.removeModule("MidiCat");
        spl.removeModule("MidiCatEx");
        spl.removeModule("MidiCatCtx");
        spl.removeModule("MidiKey");
        spl.removeModule("MidiMon");
        spl.removeModule("MidiPlug");
        spl.removeModule("MidiStep");
    }
}

static void initStatic__surgext()
{
    Plugin* const p = new Plugin;
    pluginInstance__surgext = p;

    const StaticPluginLoader spl(p, "surgext");
    if (spl.ok())
    {
        p->addModel(modelVCOClassic);
        p->addModel(modelVCOModern);
        p->addModel(modelVCOWavetable);
        p->addModel(modelVCOWindow);
        p->addModel(modelVCOSine);
        p->addModel(modelVCOFM2);
        p->addModel(modelVCOFM3);
        p->addModel(modelVCOSHNoise);
        p->addModel(modelVCOAlias);
        p->addModel(modelVCOString);
        p->addModel(modelVCOTwist);

        // Add the ported ones
        p->addModel(modelSurgeVCF);
        p->addModel(modelSurgeDelay);
        p->addModel(modelSurgeDelayLineByFreq);
        p->addModel(modelSurgeDelayLineByFreqExpanded);
        p->addModel(modelSurgeDigitalRingMods);
        p->addModel(modelSurgeWaveshaper);
        p->addModel(modelSurgeLFO);
        p->addModel(modelSurgeMixer);
        p->addModel(modelSurgeMixerSlider);
        p->addModel(modelSurgeModMatrix);

        p->addModel(modelFXReverb);
        p->addModel(modelFXPhaser);
        p->addModel(modelFXRotarySpeaker);
        p->addModel(modelFXDistortion);
        p->addModel(modelFXFrequencyShifter);
        p->addModel(modelFXChorus);
        p->addModel(modelFXVocoder);
        p->addModel(modelFXReverb2);
        p->addModel(modelFXFlanger);
        p->addModel(modelFXRingMod);
        p->addModel(modelFXNeuron);
        p->addModel(modelFXResonator);
        p->addModel(modelFXChow);
        p->addModel(modelFXExciter);
        p->addModel(modelFXEnsemble);
        p->addModel(modelFXCombulator);
        p->addModel(modelFXNimbus);
        p->addModel(modelFXSpringReverb);
        p->addModel(modelFXTreeMonster);
        p->addModel(modelFXBonsai);

        p->addModel(modelEGxVCA);
        p->addModel(modelQuadAD);
        p->addModel(modelQuadLFO);
        p->addModel(modelUnisonHelper);
        p->addModel(modelUnisonHelperCVExpander);

        surgext_rack_initialize();
    }
}

static void initStatic__unless_modules()
{
    Plugin* const p = new Plugin;
    pluginInstance__unless_modules = p;

    const StaticPluginLoader spl(p, "unless_modules");
    if (spl.ok())
    {
        // unless_modules::init_theme();
        // theme = _less::Theme();
        p->addModel(modelPiong);
        p->addModel(modelChainkov);
        p->addModel(modelAtoms);
        p->addModel(modelCantor);
        p->addModel(modelRoom);
        p->addModel(modelSnake);
        p->addModel(modelTowers);
        p->addModel(modelPianoid);
        p->addModel(modelPremuter);
        p->addModel(modelAvoider);
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

static void initStatic__Voxglitch()
{
    Plugin* p = new Plugin;
    pluginInstance__Voxglitch = p;

    const StaticPluginLoader spl(p, "voxglitch");
    if (spl.ok())
    {
#define modelLooper modelVoxglitchLooper
        p->addModel(modelArpSeq);
        p->addModel(modelAutobreak);
        p->addModel(modelAutobreakStudio);
        p->addModel(modelByteBeat);
        p->addModel(modelDigitalProgrammer);
        p->addModel(modelDigitalSequencer);
        p->addModel(modelDigitalSequencerXP);
        p->addModel(modelDrumRandomizer);
        p->addModel(modelGlitchSequencer);
        p->addModel(modelGhosts);
        p->addModel(modelGrainEngineMK2);
        p->addModel(modelGrainEngineMK2Expander);
        p->addModel(modelGrainFx);
        p->addModel(modelGrooveBox);
        p->addModel(modelGrooveBoxExpander);
        p->addModel(modelHazumi);
        p->addModel(modelOnePoint);
        p->addModel(modelOneZero);
        p->addModel(modelLooper);
        p->addModel(modelNoteDetector);
        p->addModel(modelRepeater);
        p->addModel(modelSamplerX8);
        p->addModel(modelSampler16P);
        p->addModel(modelSatanonaut);
        p->addModel(modelWavBank);
        p->addModel(modelWavBankMC);
        p->addModel(modelXY);
#undef modelLooper
    }
}

static void initStatic__WhatTheRack()
{
    Plugin* p = new Plugin;
    pluginInstance__WhatTheRack = p;

    const StaticPluginLoader spl(p, "WhatTheRack");
    if (spl.ok())
    {
      p->addModel(modelWhatTheRack);
      p->addModel(modelWhatTheMod);
      p->addModel(modelWhatTheJack);
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

static void initStatic__ZZC()
{
    Plugin* p = new Plugin;
    pluginInstance__ZZC = p;

    const StaticPluginLoader spl(p, "ZZC");
    if (spl.ok())
    {
#define modelClock modelZZCClock
        p->addModel(modelClock);
        p->addModel(modelDivider);
        p->addModel(modelFN3);
        p->addModel(modelSCVCA);
        p->addModel(modelSH8);
        p->addModel(modelSRC);
        p->addModel(modelDiv);
        p->addModel(modelDivExp);
        p->addModel(modelPolygate);
#undef modelClock
    }
}

void initStaticPlugins()
{
    initStatic__Cardinal();
    initStatic__Fundamental();
    // initStatic__ZamAudio();
    initStatic__21kHz();
    initStatic__8Mode();
    initStatic__AaronStatic();
    initStatic__admiral();
    initStatic__alefsbits();
    initStatic__Algoritmarte();
    initStatic__AmalgamatedHarmonics();
    initStatic__AnimatedCircuits();
    initStatic__ArableInstruments();
    initStatic__Aria();
    initStatic__AS();
    initStatic__AudibleInstruments();
    initStatic__Autinn();
    initStatic__Axioma();
    initStatic__Bacon();
    initStatic__Befaco();
    initStatic__Bidoo();
    initStatic__Biset();
    initStatic__BogaudioModules();
    initStatic__CatroModulo();
    initStatic__cf();
    initStatic__ChowDSP();
    initStatic__Computerscare();
    initStatic__CVfunk();
    initStatic__dBiz();
    initStatic__DHE();
    initStatic__DrumKit();
    initStatic__EnigmaCurry();
    initStatic__ESeries();
    initStatic__ExpertSleepersEncoders();
    initStatic__Extratone();
    initStatic__FehlerFabrik();
    initStatic__forsitan();
    initStatic__GlueTheGiant();
    initStatic__GoodSheperd();
    initStatic__GrandeModular();
    initStatic__H4N4();
    initStatic__HamptonHarmonics();
    initStatic__HetrickCV();
    initStatic__ImpromptuModular();
    initStatic__ihtsyn();
    initStatic__JW();
    initStatic__kocmoc();
    initStatic__LifeFormModular();
    initStatic__LilacLoop();
    initStatic__LittleUtils();
    initStatic__Lomas();
    initStatic__Lyrae();
    initStatic__Meander();
    initStatic__MindMeld();
    initStatic__ML();
    initStatic__MockbaModular();
    initStatic__Mog();
    initStatic__mscHack();
    initStatic__MSM();
    initStatic__MUS_X();
    initStatic__myth_modules();
    initStatic__nonlinearcircuits();
    initStatic__Orbits();
    initStatic__ParableInstruments();
    initStatic__PathSet();
    initStatic__PdArray();
    initStatic__PinkTrombone();
    initStatic__Prism();
    initStatic__rackwindows();
    initStatic__RCM();
    initStatic__RebelTech();
    initStatic__repelzen();
    initStatic__Sapphire();
    initStatic__sonusmodular();
    initStatic__StarlingVia();
    initStatic__stocaudio();
    initStatic__stoermelder_p1();
    initStatic__surgext();
    initStatic__unless_modules();
    initStatic__ValleyAudio();
    initStatic__Voxglitch();
    initStatic__WhatTheRack();
    initStatic__ZetaCarinaeModules();
    initStatic__ZZC();
}

void destroyStaticPlugins()
{
    for (Plugin* p : plugins)
        delete p;
    plugins.clear();
}

void updateStaticPluginsDarkMode()
{
    const bool darkMode = settings::preferDarkPanels;
    // bogaudio
    {
        Skins& skins(Skins::skins());
        skins._default = darkMode ? "dark" : "light";

        std::lock_guard<std::mutex> lock(skins._defaultSkinListenersLock);
        for (auto listener : skins._defaultSkinListeners) {
            listener->defaultSkinChanged(skins._default);
        }
    }
    // meander
    {
        panelTheme = darkMode ? 1 : 0;
    }
    // glue the giant
    {
        gtg_default_theme = darkMode ? 1 : 0;
    }
    // surgext
    {
        surgext_rack_update_theme();
    }
}

}
}
