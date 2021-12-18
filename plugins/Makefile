#!/usr/bin/make -f
# Makefile for Cardinal #
# --------------------- #
# Created by falkTX
#

# --------------------------------------------------------------
# Build config

PREFIX  ?= /usr/local
DESTDIR ?=
SYSDEPS ?= false

# --------------------------------------------------------------
# Import base definitions

USE_NANOVG_FBO = true
include ../dpf/Makefile.base.mk

# --------------------------------------------------------------
# List of drwav functions, used in several modules

DRWAV  = drwav
DRWAV += drwav__on_seek
DRWAV += drwav__on_read
DRWAV += drwav__read_and_close_f32
DRWAV += drwav__read_and_close_s16
DRWAV += drwav__read_and_close_s32
DRWAV += drwav_alaw_to_f32
DRWAV += drwav_alaw_to_s16
DRWAV += drwav_alaw_to_s16
DRWAV += drwav_alaw_to_s32
DRWAV += drwav_close
DRWAV += drwav_close
DRWAV += drwav_container
DRWAV += drwav_data_chunk_size_riff
DRWAV += drwav_data_chunk_size_w64
DRWAV += drwav_data_format
DRWAV += drwav_f32_to_s16
DRWAV += drwav_f32_to_s32
DRWAV += drwav_f64_to_f32
DRWAV += drwav_f64_to_s16
DRWAV += drwav_f64_to_s16
DRWAV += drwav_f64_to_s32
DRWAV += drwav_fopen
DRWAV += drwav_free
DRWAV += drwav_init
DRWAV += drwav_init_ex
DRWAV += drwav_init_file
DRWAV += drwav_init_file_ex
DRWAV += drwav_init_file_write
DRWAV += drwav_init_file_write
DRWAV += drwav_init_file_write__internal
DRWAV += drwav_init_file_write__internal
DRWAV += drwav_init_file_write_sequential
DRWAV += drwav_init_file_write_sequential
DRWAV += drwav_init_memory
DRWAV += drwav_init_memory_ex
DRWAV += drwav_init_memory_write
DRWAV += drwav_init_memory_write
DRWAV += drwav_init_memory_write__internal
DRWAV += drwav_init_memory_write__internal
DRWAV += drwav_init_memory_write_sequential
DRWAV += drwav_init_write
DRWAV += drwav_init_write
DRWAV += drwav_init_write__internal
DRWAV += drwav_init_write_sequential
DRWAV += drwav_mulaw_to_f32
DRWAV += drwav_mulaw_to_s16
DRWAV += drwav_mulaw_to_s16
DRWAV += drwav_mulaw_to_s32
DRWAV += drwav_open
DRWAV += drwav_open_and_read_f32
DRWAV += drwav_open_and_read_file_f32
DRWAV += drwav_open_and_read_file_s16
DRWAV += drwav_open_and_read_file_s32
DRWAV += drwav_open_and_read_memory_f32
DRWAV += drwav_open_and_read_memory_s16
DRWAV += drwav_open_and_read_memory_s32
DRWAV += drwav_open_and_read_pcm_frames_f32
DRWAV += drwav_open_and_read_pcm_frames_s16
DRWAV += drwav_open_and_read_pcm_frames_s32
DRWAV += drwav_open_and_read_s16
DRWAV += drwav_open_and_read_s32
DRWAV += drwav_open_ex
DRWAV += drwav_open_file
DRWAV += drwav_open_file_and_read_f32
DRWAV += drwav_open_file_and_read_pcm_frames_f32
DRWAV += drwav_open_file_and_read_pcm_frames_s16
DRWAV += drwav_open_file_and_read_pcm_frames_s32
DRWAV += drwav_open_file_and_read_s16
DRWAV += drwav_open_file_and_read_s32
DRWAV += drwav_open_file_ex
DRWAV += drwav_open_file_write
DRWAV += drwav_open_file_write
DRWAV += drwav_open_file_write__internal
DRWAV += drwav_open_file_write__internal
DRWAV += drwav_open_file_write_sequential
DRWAV += drwav_open_file_write_sequential
DRWAV += drwav_open_memory
DRWAV += drwav_open_memory_and_read_f32
DRWAV += drwav_open_memory_and_read_pcm_frames_f32
DRWAV += drwav_open_memory_and_read_pcm_frames_s16
DRWAV += drwav_open_memory_and_read_pcm_frames_s32
DRWAV += drwav_open_memory_and_read_s16
DRWAV += drwav_open_memory_and_read_s32
DRWAV += drwav_open_memory_ex
DRWAV += drwav_open_memory_write
DRWAV += drwav_open_memory_write
DRWAV += drwav_open_memory_write__internal
DRWAV += drwav_open_memory_write__internal
DRWAV += drwav_open_memory_write_sequential
DRWAV += drwav_open_write
DRWAV += drwav_open_write
DRWAV += drwav_open_write__internal
DRWAV += drwav_open_write_sequential
DRWAV += drwav_read
DRWAV += drwav_read_f32
DRWAV += drwav_read_f32__alaw
DRWAV += drwav_read_f32__alaw
DRWAV += drwav_read_f32__ieee
DRWAV += drwav_read_f32__ieee
DRWAV += drwav_read_f32__ima
DRWAV += drwav_read_f32__ima
DRWAV += drwav_read_f32__msadpcm
DRWAV += drwav_read_f32__msadpcm
DRWAV += drwav_read_f32__mulaw
DRWAV += drwav_read_f32__mulaw
DRWAV += drwav_read_f32__pcm
DRWAV += drwav_read_f32__pcm
DRWAV += drwav_read_pcm_frames
DRWAV += drwav_read_pcm_frames_f32
DRWAV += drwav_read_pcm_frames_s16
DRWAV += drwav_read_pcm_frames_s32
DRWAV += drwav_read_raw
DRWAV += drwav_read_s16
DRWAV += drwav_read_s16__alaw
DRWAV += drwav_read_s16__ieee
DRWAV += drwav_read_s16__ima
DRWAV += drwav_read_s16__msadpcm
DRWAV += drwav_read_s16__mulaw
DRWAV += drwav_read_s16__pcm
DRWAV += drwav_read_s32
DRWAV += drwav_read_s32__alaw
DRWAV += drwav_read_s32__alaw
DRWAV += drwav_read_s32__ieee
DRWAV += drwav_read_s32__ieee
DRWAV += drwav_read_s32__ima
DRWAV += drwav_read_s32__ima
DRWAV += drwav_read_s32__msadpcm
DRWAV += drwav_read_s32__msadpcm
DRWAV += drwav_read_s32__mulaw
DRWAV += drwav_read_s32__mulaw
DRWAV += drwav_read_s32__pcm
DRWAV += drwav_read_s32__pcm
DRWAV += drwav_riff_chunk_size_riff
DRWAV += drwav_riff_chunk_size_w64
DRWAV += drwav_s16_to_f32
DRWAV += drwav_s16_to_s32
DRWAV += drwav_s24_to_f32
DRWAV += drwav_s24_to_s16
DRWAV += drwav_s24_to_s16
DRWAV += drwav_s24_to_s32
DRWAV += drwav_s32_to_f32
DRWAV += drwav_s32_to_s16
DRWAV += drwav_s32_to_s16
DRWAV += drwav_seek_to_pcm_frame
DRWAV += drwav_seek_to_sample
DRWAV += drwav_seek_to_sample
DRWAV += drwav_u8_to_f32
DRWAV += drwav_u8_to_s16
DRWAV += drwav_u8_to_s16
DRWAV += drwav_u8_to_s32
DRWAV += drwav_uninit
DRWAV += drwav_write
DRWAV += drwav_write
DRWAV += drwav_write_pcm_frames
DRWAV += drwav_write_raw

# --------------------------------------------------------------
# Files to build

PLUGIN_FILES = plugins.cpp

# --------------------------------------------------------------
# Cardinal (built-in)

PLUGIN_FILES += Cardinal/src/Blank.cpp
PLUGIN_FILES += Cardinal/src/Carla.cpp
PLUGIN_FILES += Cardinal/src/glBars.cpp
PLUGIN_FILES += Cardinal/src/HostCV.cpp
PLUGIN_FILES += Cardinal/src/HostParameters.cpp
PLUGIN_FILES += Cardinal/src/HostTime.cpp
PLUGIN_FILES += Cardinal/src/Ildaeil.cpp
PLUGIN_FILES += Cardinal/src/TextEditor.cpp

ifneq ($(HEADLESS),true)
PLUGIN_FILES += Cardinal/src/ImGuiWidget.cpp
PLUGIN_FILES += Cardinal/src/ImGuiTextEditor.cpp
PLUGIN_FILES += $(wildcard Cardinal/src/DearImGui/*.cpp)
PLUGIN_FILES += $(wildcard Cardinal/src/DearImGuiColorTextEditor/*.cpp)
endif

ifneq ($(NOPLUGINS),true)
# --------------------------------------------------------------
# 21kHz

PLUGIN_FILES += $(filter-out 21kHz/src/21kHz.cpp,$(wildcard 21kHz/src/*.cpp))

# --------------------------------------------------------------
# AmalgamatedHarmonics

PLUGIN_FILES += $(filter-out AmalgamatedHarmonics/src/AH.cpp,$(wildcard AmalgamatedHarmonics/src/*.cpp))

# modules/types which are present in other plugins
AMALGAMATEDHARMONICS_CUSTOM = BasePinkNoiseGenerator EvenVCO LowFrequencyOscillator Pattern PinkNoiseGenerator RedNoiseGenerator
AMALGAMATEDHARMONICS_CUSTOM += bogaudio

# --------------------------------------------------------------
# AnimatedCircuits

PLUGIN_FILES += $(wildcard AnimatedCircuits/src/Folding/*.cpp)

# --------------------------------------------------------------
# Aria

PLUGIN_FILES += $(filter-out AriaModules/src/Arcane.cpp AriaModules/src/plugin.cpp,$(wildcard AriaModules/src/*.cpp))

# modules/types which are present in other plugins
ARIA_CUSTOM = Blank

# --------------------------------------------------------------
# AS

PLUGIN_FILES += $(filter-out AS/src/AS.cpp,$(wildcard AS/src/*.cpp))
PLUGIN_FILES += AS/freeverb/revmodel.cpp

# modules/types which are present in other plugins
AS_CUSTOM = ADSR LabelDisplayWidget LowFrequencyOscillator VCA YellowRedLight allpass comb revmodel
AS_CUSTOM_PER_FILE = NumberDisplayWidget

# --------------------------------------------------------------
# Atelier

PLUGIN_FILES += Atelier/src/Palette.cpp
PLUGIN_FILES += Atelier/eurorack/stmlib/dsp/atan.cc
PLUGIN_FILES += Atelier/eurorack/stmlib/dsp/units.cc
PLUGIN_FILES += Atelier/eurorack/stmlib/utils/random.cc
PLUGIN_FILES += $(wildcard Atelier/eurorack/plaits/dsp/*.cc)
PLUGIN_FILES += $(wildcard Atelier/eurorack/plaits/dsp/engine/*.cc)
PLUGIN_FILES += $(wildcard Atelier/eurorack/plaits/dsp/speech/*.cc)
PLUGIN_FILES += $(wildcard Atelier/eurorack/plaits/dsp/physical_modelling/*.cc)
PLUGIN_FILES += Atelier/eurorack/plaits/resources.cc

# modules/types which are present in other plugins
ATELIER_CUSTOM = plaits stmlib

# --------------------------------------------------------------
# AudibleInstruments

PLUGIN_FILES += $(filter-out AudibleInstruments/src/plugin.cpp,$(wildcard AudibleInstruments/src/*.cpp))

PLUGIN_FILES += AudibleInstruments/eurorack/stmlib/utils/random.cc
PLUGIN_FILES += AudibleInstruments/eurorack/stmlib/dsp/atan.cc
PLUGIN_FILES += AudibleInstruments/eurorack/stmlib/dsp/units.cc

PLUGIN_FILES += AudibleInstruments/eurorack/braids/macro_oscillator.cc
PLUGIN_FILES += AudibleInstruments/eurorack/braids/analog_oscillator.cc
PLUGIN_FILES += AudibleInstruments/eurorack/braids/digital_oscillator.cc
PLUGIN_FILES += AudibleInstruments/eurorack/braids/quantizer.cc
PLUGIN_FILES += AudibleInstruments/eurorack/braids/resources.cc

PLUGIN_FILES += $(wildcard AudibleInstruments/eurorack/plaits/dsp/*.cc)
PLUGIN_FILES += $(wildcard AudibleInstruments/eurorack/plaits/dsp/engine/*.cc)
PLUGIN_FILES += $(wildcard AudibleInstruments/eurorack/plaits/dsp/speech/*.cc)
PLUGIN_FILES += $(wildcard AudibleInstruments/eurorack/plaits/dsp/physical_modelling/*.cc)
PLUGIN_FILES += AudibleInstruments/eurorack/plaits/resources.cc

PLUGIN_FILES += AudibleInstruments/eurorack/clouds/dsp/correlator.cc
PLUGIN_FILES += AudibleInstruments/eurorack/clouds/dsp/granular_processor.cc
PLUGIN_FILES += AudibleInstruments/eurorack/clouds/dsp/mu_law.cc
PLUGIN_FILES += AudibleInstruments/eurorack/clouds/dsp/pvoc/frame_transformation.cc
PLUGIN_FILES += AudibleInstruments/eurorack/clouds/dsp/pvoc/phase_vocoder.cc
PLUGIN_FILES += AudibleInstruments/eurorack/clouds/dsp/pvoc/stft.cc
PLUGIN_FILES += AudibleInstruments/eurorack/clouds/resources.cc

PLUGIN_FILES += AudibleInstruments/eurorack/elements/dsp/exciter.cc
PLUGIN_FILES += AudibleInstruments/eurorack/elements/dsp/ominous_voice.cc
PLUGIN_FILES += AudibleInstruments/eurorack/elements/dsp/resonator.cc
PLUGIN_FILES += AudibleInstruments/eurorack/elements/dsp/tube.cc
PLUGIN_FILES += AudibleInstruments/eurorack/elements/dsp/multistage_envelope.cc
PLUGIN_FILES += AudibleInstruments/eurorack/elements/dsp/part.cc
PLUGIN_FILES += AudibleInstruments/eurorack/elements/dsp/string.cc
PLUGIN_FILES += AudibleInstruments/eurorack/elements/dsp/voice.cc
PLUGIN_FILES += AudibleInstruments/eurorack/elements/resources.cc

PLUGIN_FILES += AudibleInstruments/eurorack/rings/dsp/fm_voice.cc
PLUGIN_FILES += AudibleInstruments/eurorack/rings/dsp/part.cc
PLUGIN_FILES += AudibleInstruments/eurorack/rings/dsp/string_synth_part.cc
PLUGIN_FILES += AudibleInstruments/eurorack/rings/dsp/string.cc
PLUGIN_FILES += AudibleInstruments/eurorack/rings/dsp/resonator.cc
PLUGIN_FILES += AudibleInstruments/eurorack/rings/resources.cc

PLUGIN_FILES += AudibleInstruments/eurorack/tides/generator.cc
PLUGIN_FILES += AudibleInstruments/eurorack/tides/resources.cc

PLUGIN_FILES += AudibleInstruments/eurorack/tides2/poly_slope_generator.cc
PLUGIN_FILES += AudibleInstruments/eurorack/tides2/ramp_extractor.cc
PLUGIN_FILES += AudibleInstruments/eurorack/tides2/resources.cc

PLUGIN_FILES += AudibleInstruments/eurorack/warps/dsp/modulator.cc
PLUGIN_FILES += AudibleInstruments/eurorack/warps/dsp/oscillator.cc
PLUGIN_FILES += AudibleInstruments/eurorack/warps/dsp/vocoder.cc
PLUGIN_FILES += AudibleInstruments/eurorack/warps/dsp/filter_bank.cc
PLUGIN_FILES += AudibleInstruments/eurorack/warps/resources.cc

PLUGIN_FILES += AudibleInstruments/eurorack/frames/keyframer.cc
PLUGIN_FILES += AudibleInstruments/eurorack/frames/resources.cc
PLUGIN_FILES += AudibleInstruments/eurorack/frames/poly_lfo.cc

PLUGIN_FILES += AudibleInstruments/eurorack/peaks/processors.cc
PLUGIN_FILES += AudibleInstruments/eurorack/peaks/resources.cc
PLUGIN_FILES += AudibleInstruments/eurorack/peaks/drums/bass_drum.cc
PLUGIN_FILES += AudibleInstruments/eurorack/peaks/drums/fm_drum.cc
PLUGIN_FILES += AudibleInstruments/eurorack/peaks/drums/high_hat.cc
PLUGIN_FILES += AudibleInstruments/eurorack/peaks/drums/snare_drum.cc
PLUGIN_FILES += AudibleInstruments/eurorack/peaks/modulations/lfo.cc
PLUGIN_FILES += AudibleInstruments/eurorack/peaks/modulations/multistage_envelope.cc
PLUGIN_FILES += AudibleInstruments/eurorack/peaks/pulse_processor/pulse_shaper.cc
PLUGIN_FILES += AudibleInstruments/eurorack/peaks/pulse_processor/pulse_randomizer.cc
PLUGIN_FILES += AudibleInstruments/eurorack/peaks/number_station/number_station.cc

PLUGIN_FILES += AudibleInstruments/eurorack/stages/segment_generator.cc
PLUGIN_FILES += AudibleInstruments/eurorack/stages/ramp_extractor.cc
PLUGIN_FILES += AudibleInstruments/eurorack/stages/resources.cc

PLUGIN_FILES += AudibleInstruments/eurorack/stmlib/utils/random.cc
PLUGIN_FILES += AudibleInstruments/eurorack/stmlib/dsp/atan.cc
PLUGIN_FILES += AudibleInstruments/eurorack/stmlib/dsp/units.cc
PLUGIN_FILES += AudibleInstruments/eurorack/marbles/random/t_generator.cc
PLUGIN_FILES += AudibleInstruments/eurorack/marbles/random/x_y_generator.cc
PLUGIN_FILES += AudibleInstruments/eurorack/marbles/random/output_channel.cc
PLUGIN_FILES += AudibleInstruments/eurorack/marbles/random/lag_processor.cc
PLUGIN_FILES += AudibleInstruments/eurorack/marbles/random/quantizer.cc
PLUGIN_FILES += AudibleInstruments/eurorack/marbles/ramp/ramp_extractor.cc
PLUGIN_FILES += AudibleInstruments/eurorack/marbles/resources.cc

PLUGIN_FILES += AudibleInstruments/eurorack/streams/resources.cc
PLUGIN_FILES += AudibleInstruments/eurorack/streams/processor.cc
PLUGIN_FILES += AudibleInstruments/eurorack/streams/follower.cc
PLUGIN_FILES += AudibleInstruments/eurorack/streams/lorenz_generator.cc
PLUGIN_FILES += AudibleInstruments/eurorack/streams/envelope.cc
PLUGIN_FILES += AudibleInstruments/eurorack/streams/svf.cc
PLUGIN_FILES += AudibleInstruments/eurorack/streams/vactrol.cc
PLUGIN_FILES += AudibleInstruments/eurorack/streams/compressor.cc

# --------------------------------------------------------------
# BaconPlugs

PLUGIN_FILES += $(filter-out BaconPlugs/src/BaconPlugs.cpp,$(wildcard BaconPlugs/src/*.cpp))
PLUGIN_FILES += $(wildcard BaconPlugs/libs/midifile/src/*.cpp)
PLUGIN_FILES += $(wildcard BaconPlugs/libs/open303-code/Source/DSPCode/*.cpp)

# --------------------------------------------------------------
# Befaco

PLUGIN_FILES += $(filter-out Befaco/src/plugin.cpp,$(wildcard Befaco/src/*.cpp))
PLUGIN_BINARIES += Befaco/src/SpringReverbIR.pcm

# modules/types which are present in other plugins
BEFACO_CUSTOM = ADSR Mixer

# --------------------------------------------------------------
# Bidoo

PLUGIN_FILES += $(filter-out Bidoo/src/plugin.cpp Bidoo/src/ANTN.cpp,$(wildcard Bidoo/src/*.cpp))
PLUGIN_FILES += $(wildcard Bidoo/src/dep/*.cpp)
PLUGIN_FILES += $(wildcard Bidoo/src/dep/filters/*.cpp)
PLUGIN_FILES += $(wildcard Bidoo/src/dep/freeverb/*.cpp)
PLUGIN_FILES += $(wildcard Bidoo/src/dep/lodepng/*.cpp)
PLUGIN_FILES += $(filter-out Bidoo/src/dep/resampler/main.cpp,$(wildcard Bidoo/src/dep/resampler/*.cpp))

# modules/types which are present in other plugins
BIDOO_CUSTOM = ChannelDisplay LadderFilter $(DRWAV)
BIDOO_CUSTOM_PER_FILE = channel channel filterType

# --------------------------------------------------------------
# BogaudioModules

PLUGIN_FILES += $(filter-out BogaudioModules/src/bogaudio.cpp,$(wildcard BogaudioModules/src/*.cpp))
PLUGIN_FILES += $(wildcard BogaudioModules/src/dsp/*.cpp)
PLUGIN_FILES += $(wildcard BogaudioModules/src/dsp/filters/*.cpp)

# modules/types which are present in other plugins
BOGAUDIO_CUSTOM = ADSR BlueNoiseGenerator LFO Noise VCA VCO VCF
BOGAUDIO_CUSTOM_PER_FILE = ARQuantity AttackMenuItem ReleaseMenuItem

# --------------------------------------------------------------
# ChowDSP

# Credit module crashes on save, see https://github.com/DISTRHO/Cardinal/issues/98
PLUGIN_FILES += $(filter-out ChowDSP/src/Credit.cpp,$(wildcard ChowDSP/src/*/*.cpp))
PLUGIN_FILES += $(wildcard ChowDSP/src/*/*/*.cpp)
PLUGIN_FILES += $(wildcard ChowDSP/lib/r8lib/*.cpp)

# --------------------------------------------------------------
# cf

PLUGIN_FILES += $(filter-out cf/src/plugin.cpp,$(wildcard cf/src/*.cpp))

# --------------------------------------------------------------
# DrumKit

PLUGIN_FILES += $(wildcard DrumKit/src/*.cpp)
PLUGIN_FILES += $(wildcard DrumKit/src/controller/*.cpp)
PLUGIN_FILES += $(wildcard DrumKit/src/view/*.cpp)
PLUGIN_FILES += $(wildcard DrumKit/src/model/*.cpp)
PLUGIN_FILES += $(wildcard DrumKit/deps/*.cpp)
PLUGIN_FILES += $(wildcard DrumKit/deps/SynthDevKit/src/*.cpp)

DRUMKIT_CUSTOM = ADSR Envelope LowFrequencyOscillator

# --------------------------------------------------------------
# ESeries

PLUGIN_FILES += ESeries/src/E340.cpp

# --------------------------------------------------------------
# Extratone

PLUGIN_FILES += $(filter-out Extratone/src/plugin.cpp,$(wildcard Extratone/src/*.cpp))

# --------------------------------------------------------------
# FehlerFabrik

PLUGIN_FILES += $(filter-out FehlerFabrik/src/plugin.cpp,$(wildcard FehlerFabrik/src/*.cpp))

FEHLERFABRIK_CUSTOM = Operator Sequencer SlewLimiter

# --------------------------------------------------------------
# Fundamental

PLUGIN_FILES += $(filter-out Fundamental/src/plugin.cpp,$(wildcard Fundamental/src/*.cpp))
PLUGIN_FILES += Fundamental/src/dr_wav.c

# modules/types which are present in other plugins
FUNDAMENTAL_CUSTOM = $(DRWAV)

# --------------------------------------------------------------
# GlueTheGiant

PLUGIN_FILES += $(filter-out GlueTheGiant/src/plugin.cpp,$(wildcard GlueTheGiant/src/*.cpp))

# --------------------------------------------------------------
# GrandeModular

PLUGIN_FILES += $(filter-out GrandeModular/src/plugin.cpp,$(wildcard GrandeModular/src/*.cpp))

# --------------------------------------------------------------
# HetrickCV

PLUGIN_FILES += $(wildcard HetrickCV/src/*.cpp)
PLUGIN_FILES += $(wildcard HetrickCV/src/DSP/*.cpp)
PLUGIN_FILES += $(wildcard HetrickCV/Gamma/src/arr.cpp)
PLUGIN_FILES += $(wildcard HetrickCV/Gamma/src/Domain.cpp)
PLUGIN_FILES += $(wildcard HetrickCV/Gamma/src/scl.cpp)

# modules/types which are present in other plugins
HETRICKCV_CUSTOM = ASR BlankPanel FlipFlop MidSide MinMax

# --------------------------------------------------------------
# ImpromptuModular

PLUGIN_FILES += $(wildcard ImpromptuModular/src/*.cpp)
PLUGIN_FILES += $(filter-out ImpromptuModular/src/comp/PanelTheme.cpp,$(wildcard ImpromptuModular/src/comp/*.cpp))
PLUGIN_FILES += ImpromptuModularDark/PanelTheme.cpp

# modules/types which are present in other plugins
IMPROMPTUMODULAR_CUSTOM = RefreshCounter
IMPROMPTUMODULAR_CUSTOM_PER_FILE = Clock stepClock

# --------------------------------------------------------------
# JW-Modules

PLUGIN_FILES += $(filter-out JW-Modules/src/JWModules.cpp,$(wildcard JW-Modules/src/*.cpp))
PLUGIN_FILES += $(wildcard JW-Modules/lib/oscpack/ip/*.cpp)
PLUGIN_FILES += $(wildcard JW-Modules/lib/oscpack/osc/*.cpp)
ifeq ($(WINDOWS),true)
PLUGIN_FILES += $(wildcard JW-Modules/lib/oscpack/ip/win32/*.cpp)
else
PLUGIN_FILES += $(wildcard JW-Modules/lib/oscpack/ip/posix/*.cpp)
endif

# modules/types which are present in other plugins
JW_CUSTOM = PlayHead Quantizer

# --------------------------------------------------------------
# LifeFormModular

PLUGIN_FILES += $(filter-out LifeFormModular/src/plugin.cpp,$(wildcard LifeFormModular/src/*.cpp))

# modules/types which are present in other plugins
LIFEFORMMODULAR_CUSTOM = IO MS __ct_base __ct_comp

# --------------------------------------------------------------
# LittleUtils

PLUGIN_FILES += $(filter-out LittleUtils/src/plugin.cpp,$(wildcard LittleUtils/src/*.cpp))

# modules/types which are present in other plugins
LITTLEUTILS_CUSTOM = MsDisplayWidget

# --------------------------------------------------------------
# LomasModules

PLUGIN_FILES += $(filter-out LomasModules/src/plugin.cpp,$(wildcard LomasModules/src/*.cpp))

# modules/types which are present in other plugins
LOMAS_CUSTOM = $(DRWAV)

# --------------------------------------------------------------
# LyraeModules

PLUGIN_FILES += $(filter-out LyraeModules/src/plugin.cpp,$(wildcard LyraeModules/src/*.cpp))

# modules/types which are present in other plugins
LYRAE_CUSTOM = Delta

# --------------------------------------------------------------
# MindMeld

PLUGIN_FILES += $(wildcard MindMeldModular/src/*.cpp)
PLUGIN_FILES += $(wildcard MindMeldModular/src/dsp/*.cpp)
PLUGIN_FILES += $(wildcard MindMeldModular/src/comp/*.cpp)
PLUGIN_FILES += $(wildcard MindMeldModular/src/EqMaster/*.cpp)
PLUGIN_FILES += $(wildcard MindMeldModular/src/MixMaster/*.cpp)
PLUGIN_FILES += $(wildcard MindMeldModular/src/ShapeMaster/*.cpp)
PLUGIN_FILES += $(wildcard MindMeldModular/src/Utilities/*.cpp)

# modules/types which are present in other plugins
MINDMELD_CUSTOM = printNote

# --------------------------------------------------------------
# Mog

PLUGIN_FILES += Mog/src/Network.cpp
PLUGIN_FILES += Mog/src/Nexus.cpp

# --------------------------------------------------------------
# mscHack

PLUGIN_FILES += $(wildcard mscHack/src/*.cpp)

# modules/types which are present in other plugins
MSCHACK_CUSTOM_PER_FILE = MAIN_SYNC_CLOCK FILTER_STRUCT FILTER_PARAM_STRUCT OSC_PARAM_STRUCT PHRASE_CHANGE_STRUCT

# --------------------------------------------------------------
# Prism

PLUGIN_FILES += $(filter-out Prism/src/plugin.cpp, $(wildcard Prism/src/*.cpp))
PLUGIN_FILES += $(wildcard Prism/src/scales/*.cpp)

# modules/types which are present in other plugins
PRISM_CUSTOM = bogaudio Scale

# --------------------------------------------------------------
# rackwindows

PLUGIN_FILES += $(filter-out rackwindows/src/plugin.cpp,$(wildcard rackwindows/src/*.cpp))

# --------------------------------------------------------------
# repelzen

PLUGIN_FILES += $(filter-out repelzen/src/repelzen.cpp,$(wildcard repelzen/src/*.cpp))

# modules/types which are present in other plugins
REPELZEN_CUSTOM = Blank Mixer Werner tanh_pade

# --------------------------------------------------------------
# sonusmodular

PLUGIN_FILES += $(filter-out sonusmodular/src/sonusmodular,$(wildcard sonusmodular/src/*.cpp))

# --------------------------------------------------------------
# ValleyAudio

PLUGIN_FILES += $(filter-out ValleyAudio/src/Valley.cpp,$(wildcard ValleyAudio/src/*.cpp))
PLUGIN_FILES += $(wildcard ValleyAudio/src/*/*.cpp)
PLUGIN_FILES += $(wildcard ValleyAudio/src/*/*/*.cpp)

PLUGIN_BINARIES += ValleyAudio/src/ADD_BANK1.bin
PLUGIN_BINARIES += ValleyAudio/src/ADD_BANK2.bin
PLUGIN_BINARIES += ValleyAudio/src/ADD_BANK3.bin
PLUGIN_BINARIES += ValleyAudio/src/ADD_BANK4.bin
PLUGIN_BINARIES += ValleyAudio/src/ADD_SAW.bin
PLUGIN_BINARIES += ValleyAudio/src/ADD_SINE.bin
PLUGIN_BINARIES += ValleyAudio/src/ADD_SQR.bin
PLUGIN_BINARIES += ValleyAudio/src/ALTOSAX.bin
PLUGIN_BINARIES += ValleyAudio/src/AM_HARM.bin
PLUGIN_BINARIES += ValleyAudio/src/BASIC.bin
PLUGIN_BINARIES += ValleyAudio/src/BI_PULSE.bin
PLUGIN_BINARIES += ValleyAudio/src/BITCRUSH1.bin
PLUGIN_BINARIES += ValleyAudio/src/BITCRUSH2.bin
PLUGIN_BINARIES += ValleyAudio/src/CELLO_1.bin
PLUGIN_BINARIES += ValleyAudio/src/CELLO_2.bin
PLUGIN_BINARIES += ValleyAudio/src/CHIP_1.bin
PLUGIN_BINARIES += ValleyAudio/src/CHIP_2.bin
PLUGIN_BINARIES += ValleyAudio/src/CHIRP.bin
PLUGIN_BINARIES += ValleyAudio/src/DISTORT.bin
PLUGIN_BINARIES += ValleyAudio/src/EBASS.bin
PLUGIN_BINARIES += ValleyAudio/src/FM1.bin
PLUGIN_BINARIES += ValleyAudio/src/FM2.bin
PLUGIN_BINARIES += ValleyAudio/src/FM3.bin
PLUGIN_BINARIES += ValleyAudio/src/FM4.bin
PLUGIN_BINARIES += ValleyAudio/src/FOLD_SINE.bin
PLUGIN_BINARIES += ValleyAudio/src/GMTRY_1.bin
PLUGIN_BINARIES += ValleyAudio/src/GMTRY_2.bin
PLUGIN_BINARIES += ValleyAudio/src/GMTRY_3.bin
PLUGIN_BINARIES += ValleyAudio/src/GRIT.bin
PLUGIN_BINARIES += ValleyAudio/src/LINEAR.bin
PLUGIN_BINARIES += ValleyAudio/src/OBOE.bin
PLUGIN_BINARIES += ValleyAudio/src/OPAL.bin
PLUGIN_BINARIES += ValleyAudio/src/OVERTONE1.bin
PLUGIN_BINARIES += ValleyAudio/src/OVERTONE2.bin
PLUGIN_BINARIES += ValleyAudio/src/PIANO.bin
PLUGIN_BINARIES += ValleyAudio/src/PLAITS_1.bin
PLUGIN_BINARIES += ValleyAudio/src/PLAITS_2.bin
PLUGIN_BINARIES += ValleyAudio/src/PLAITS_3.bin
PLUGIN_BINARIES += ValleyAudio/src/PLUCK.bin
PLUGIN_BINARIES += ValleyAudio/src/PWM.bin
PLUGIN_BINARIES += ValleyAudio/src/REED.bin
PLUGIN_BINARIES += ValleyAudio/src/RESO_SAW.bin
PLUGIN_BINARIES += ValleyAudio/src/RESO_SQR.bin
PLUGIN_BINARIES += ValleyAudio/src/SAW_GAP1.bin
PLUGIN_BINARIES += ValleyAudio/src/SAW_GAP2.bin
PLUGIN_BINARIES += ValleyAudio/src/SAW_PHASE.bin
PLUGIN_BINARIES += ValleyAudio/src/SINE_HARM.bin
PLUGIN_BINARIES += ValleyAudio/src/SWEEPHARM.bin
PLUGIN_BINARIES += ValleyAudio/src/SYMMETRY.bin
PLUGIN_BINARIES += ValleyAudio/src/TEE_EKS.bin
PLUGIN_BINARIES += ValleyAudio/src/THEREMIN.bin
PLUGIN_BINARIES += ValleyAudio/src/TWO_OP_RAND.bin
PLUGIN_BINARIES += ValleyAudio/src/TWO_OPFM1.bin
PLUGIN_BINARIES += ValleyAudio/src/TWO_OPFM2.bin
PLUGIN_BINARIES += ValleyAudio/src/VIDEOGAME.bin
PLUGIN_BINARIES += ValleyAudio/src/VIOLIN.bin
PLUGIN_BINARIES += ValleyAudio/src/VOICE_1.bin
PLUGIN_BINARIES += ValleyAudio/src/VOICE_2.bin
PLUGIN_BINARIES += ValleyAudio/src/VOICE_3.bin
PLUGIN_BINARIES += ValleyAudio/src/VOICE_4.bin
PLUGIN_BINARIES += ValleyAudio/src/VOICE_5.bin
PLUGIN_BINARIES += ValleyAudio/src/VOICE_6.bin
PLUGIN_BINARIES += ValleyAudio/src/VOX_MACH.bin
PLUGIN_BINARIES += ValleyAudio/src/XFADE.bin

# modules/types which are present in other plugins
VALLEYAUDIO_CUSTOM = $(DRWAV) DigitalDisplay
VALLEYAUDIO_CUSTOM_PER_FILE = TempoKnob

# --------------------------------------------------------------
# ZetaCarinaeModules

PLUGIN_FILES += $(filter-out ZetaCarinaeModules/src/plugin.cpp,$(wildcard ZetaCarinaeModules/src/*.cpp))

# --------------------------------------------------------------
# ZZC

PLUGIN_FILES += $(filter-out ZZC/src/ZZC.cpp,$(wildcard ZZC/src/*.cpp))
PLUGIN_FILES += ZZC/src/dsp/Wavetable.cpp
PLUGIN_FILES += ZZC/src/filetypes/WavSupport.cpp

# modules/types which are present in other plugins
ZZC_CUSTOM = Clock LowFrequencyOscillator

# --------------------------------------------------------------

endif # !NOPLUGINS

# --------------------------------------------------------------
# Build setup

BUILD_DIR = ../build/plugins

ifeq ($(MACOS),true)
BASE_FLAGS += -DARCH_MAC
else ifeq ($(WINDOWS),true)
BASE_FLAGS += -DARCH_WIN
else
BASE_FLAGS += -DARCH_LIN
endif

BASE_FLAGS += -DBUILDING_PLUGIN_MODULES
BASE_FLAGS += -fno-finite-math-only
BASE_FLAGS += -I../dpf/dgl/src/nanovg
BASE_FLAGS += -I../dpf/distrho

BASE_FLAGS += -I../include
BASE_FLAGS += -I../include/neon-compat

ifeq ($(SYSDEPS),true)
BASE_FLAGS += -DCARDINAL_SYSDEPS
else
BASE_FLAGS += -DZSTDLIB_VISIBILITY=
endif

BASE_FLAGS += -I../src
BASE_FLAGS += -I../src/Rack/include
BASE_FLAGS += -I../src/Rack/include/dsp
BASE_FLAGS += -I../src/Rack/dep/include
# # BASE_FLAGS += -I../src/Rack/dep/filesystem/include
# # BASE_FLAGS += -I../src/Rack/dep/fuzzysearchdatabase/src
BASE_FLAGS += -I../src/Rack/dep/glfw/include
BASE_FLAGS += -I../src/Rack/dep/nanosvg/src
BASE_FLAGS += -I../src/Rack/dep/osdialog
BASE_FLAGS += -I../src/Rack/dep/oui-blendish
BASE_FLAGS += -I../src/Rack/dep/pffft

ifeq ($(DEBUG),true)
BASE_FLAGS += -UDEBUG
endif

ifeq ($(HEADLESS),true)
BASE_FLAGS += -DHEADLESS
endif

ifeq ($(WASM),true)
BASE_FLAGS += -DNANOVG_GLES2=1
BASE_FLAGS += -msse -msse2 -msse3 -msimd128
else
BASE_FLAGS += -pthread
endif

ifeq ($(WINDOWS),true)
BASE_FLAGS += -D_USE_MATH_DEFINES
BASE_FLAGS += -DWIN32_LEAN_AND_MEAN
BASE_FLAGS += -I../include/mingw-compat
BASE_FLAGS += -I../include/mingw-std-threads
endif

ifeq ($(NOPLUGINS),true)
BASE_FLAGS += -DNOPLUGINS
endif

BUILD_C_FLAGS += -std=gnu11
BUILD_C_FLAGS += -fno-finite-math-only
BUILD_CXX_FLAGS += -fno-finite-math-only

# --------------------------------------------------------------
# lots of warnings from VCV side

BASE_FLAGS += -Wno-unused-parameter
BASE_FLAGS += -Wno-unused-variable

# --------------------------------------------------------------
# also from plugins

BASE_FLAGS += -Wno-deprecated-declarations

ifeq ($(MACOS),true)
BASE_FLAGS += -Wno-unknown-warning-option
endif

# --------------------------------------------------------------
# Build targets

TARGET = plugins.a

all: $(TARGET)

clean:
	rm -f $(TARGET)
	rm -rf $(BUILD_DIR)

# --------------------------------------------------------------

ifeq ($(NOPLUGINS),true)
PLUGIN_LIST = Cardinal
else
PLUGIN_LIST = $(subst /plugin.json,,$(wildcard */plugin.json))
endif

PLUGIN_RESOURCES  = ../bin/CardinalFX.lv2/resources/PluginManifests/Core.json
PLUGIN_RESOURCES += $(PLUGIN_LIST:%=../bin/CardinalFX.lv2/resources/%)
PLUGIN_RESOURCES += $(PLUGIN_LIST:%=../bin/CardinalFX.lv2/resources/PluginManifests/%.json)
ifeq ($(MACOS),true)
PLUGIN_RESOURCES += ../bin/CardinalFX.vst/Contents/Resources/PluginManifests/Core.json
PLUGIN_RESOURCES += $(PLUGIN_LIST:%=../bin/CardinalFX.vst/Contents/Resources/%)
PLUGIN_RESOURCES += $(PLUGIN_LIST:%=../bin/CardinalFX.vst/Contents/Resources/PluginManifests/%.json)
else
PLUGIN_RESOURCES += ../bin/CardinalFX.vst/resources/PluginManifests/Core.json
PLUGIN_RESOURCES += $(PLUGIN_LIST:%=../bin/CardinalFX.vst/resources/%)
PLUGIN_RESOURCES += $(PLUGIN_LIST:%=../bin/CardinalFX.vst/resources/PluginManifests/%.json)
endif
PLUGIN_RESOURCES += ../bin/CardinalFX.vst3/Contents/Resources/PluginManifests/Core.json
PLUGIN_RESOURCES += $(PLUGIN_LIST:%=../bin/CardinalFX.vst3/Contents/Resources/%)
PLUGIN_RESOURCES += $(PLUGIN_LIST:%=../bin/CardinalFX.vst3/Contents/Resources/PluginManifests/%.json)

resources: $(PLUGIN_RESOURCES)

../bin/CardinalFX.%/Core.json: Core.json
	-@mkdir -p "$(shell dirname $@)"
	ln -sf $(abspath $<) $@

../bin/CardinalFX.lv2/resources/%: %/res
	-@mkdir -p "$(shell dirname $@)"
	ln -sf $(abspath $<) $@

../bin/CardinalFX.vst/resources/%: %/res
	-@mkdir -p "$(shell dirname $@)"
	ln -sf $(abspath $<) $@

../bin/CardinalFX.vst/Contents/Resources/%: %/res
	-@mkdir -p "$(shell dirname $@)"
	ln -sf $(abspath $<) $@

../bin/CardinalFX.vst3/Contents/Resources/%: %/res
	-@mkdir -p "$(shell dirname $@)"
	ln -sf $(abspath $<) $@

../bin/CardinalFX.lv2/resources/PluginManifests/%.json: %/plugin.json
	-@mkdir -p "$(shell dirname $@)"
	ln -sf $(abspath $<) $@

../bin/CardinalFX.vst/resources/PluginManifests/%.json: %/plugin.json
	-@mkdir -p "$(shell dirname $@)"
	ln -sf $(abspath $<) $@

../bin/CardinalFX.vst/Contents/Resources/PluginManifests/%.json: %/plugin.json
	-@mkdir -p "$(shell dirname $@)"
	ln -sf $(abspath $<) $@

../bin/CardinalFX.vst3/Contents/Resources/PluginManifests/%.json: %/plugin.json
	-@mkdir -p "$(shell dirname $@)"
	ln -sf $(abspath $<) $@

# --------------------------------------------------------------
# Build commands

PLUGIN_OBJS  = $(PLUGIN_FILES:%=$(BUILD_DIR)/%.o)
PLUGIN_OBJS += $(PLUGIN_BINARIES:%=$(BUILD_DIR)/%.bin.o)

.PRECIOUS: $(PLUGIN_BINARIES:%=$(BUILD_DIR)/%.bin.c)

# function for custom module names macro
custom_module_names = -D${1}=${2}${1} -Dmodel${1}=model${2}${1} -D${1}Widget=${2}${1}Widget
custom_per_file_names = -D${1}=${2}_${1}

$(TARGET): $(PLUGIN_OBJS)
	@echo "Creating $@"
	$(SILENT)rm -f $@
	$(SILENT)$(AR) crs $@ $^

$(BUILD_DIR)/%.bin.c: % res2c.py
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Generating $*.bin.c"
	$(SILENT)python3 ./res2c.py $< > $@

$(BUILD_DIR)/%.bin.o: $(BUILD_DIR)/%.bin.c
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $*.bin"
	$(SILENT)$(CC) $< $(BUILD_C_FLAGS) -c -o $@

$(BUILD_DIR)/plugins.cpp.o: plugins.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@

$(BUILD_DIR)/Cardinal/%.cpp.o: Cardinal/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		-DpluginInstance=pluginInstance__Cardinal \
		-Dstbrp_context=stbrp_context_cardinal \
		-Dstbrp_coord=stbrp_coord_cardinal \
		-Dstbtt_fontinfo=stbtt_fontinfo_cardinal \
		-Dstbrp_node=stbrp_node_cardinal \
		-Dstbrp_rect=stbrp_rect_cardinal \
		-DREAL_BUILD \
		-DSTATIC_PLUGIN_TARGET \
		-I../carla/source/backend \
		-I../carla/source/includes \
		-I../carla/source/modules \
		-I../carla/source/utils

$(BUILD_DIR)/21kHz/%.cpp.o: 21kHz/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(21KHZ_CUSTOM),$(call custom_module_names,$(m),21kHz)) \
		-DpluginInstance=pluginInstance__21kHz

$(BUILD_DIR)/AmalgamatedHarmonics/%.cpp.o: AmalgamatedHarmonics/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(AMALGAMATEDHARMONICS_CUSTOM),$(call custom_module_names,$(m),AmalgamatedHarmonics)) \
		-DpluginInstance=pluginInstance__AmalgamatedHarmonics

$(BUILD_DIR)/AnimatedCircuits/%.cpp.o: AnimatedCircuits/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(ANIMATEDCIRCUITS_CUSTOM),$(call custom_module_names,$(m),AnimatedCircuits)) \
		-DpluginInstance=pluginInstance__AnimatedCircuits

$(BUILD_DIR)/AriaModules/%.cpp.o: AriaModules/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(ARIA_CUSTOM),$(call custom_module_names,$(m),Aria)) \
		-DpluginInstance=pluginInstance__Aria \
		-Wno-cast-function-type

$(BUILD_DIR)/AS/%.cpp.o: AS/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(AS_CUSTOM),$(call custom_module_names,$(m),AS)) \
		$(foreach m,$(AS_CUSTOM_PER_FILE),$(call custom_per_file_names,$(m),AS_$(shell basename $*))) \
		-DpluginInstance=pluginInstance__AS \
		-Wno-unused-private-field

$(BUILD_DIR)/Atelier/%.o: Atelier/%
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(ATELIER_CUSTOM),$(call custom_module_names,$(m),Atelier)) \
		-DpluginInstance=pluginInstance__Atelier \
		-DTEST \
		-IAtelier/eurorack \
		-Wno-sign-compare \
		-Wno-unused-local-typedefs

$(BUILD_DIR)/AudibleInstruments/%.o: AudibleInstruments/%
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(AUDIBLEINSTRUMENTS_CUSTOM),$(call custom_module_names,$(m),AudibleInstruments)) \
		-DpluginInstance=pluginInstance__AudibleInstruments \
		-DTEST \
		-IAudibleInstruments/eurorack \
		-Wno-class-memaccess \
		-Wno-unused-local-typedefs

$(BUILD_DIR)/BaconPlugs/%.cpp.o: BaconPlugs/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(BACON_CUSTOM),$(call custom_module_names,$(m),BaconPlugs)) \
		-DpluginInstance=pluginInstance__Bacon \
		-DDARK_BACON \
		-IBaconPlugs/libs/midifile/include \
		-IBaconPlugs/libs/open303-code/Source/DSPCode \
		-Wno-array-bounds \
		-Wno-strict-aliasing

$(BUILD_DIR)/Befaco/%.cpp.o: Befaco/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(BEFACO_CUSTOM),$(call custom_module_names,$(m),Befaco)) \
		-DpluginInstance=pluginInstance__Befaco

$(BUILD_DIR)/Bidoo/%.cpp.o: Bidoo/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(BIDOO_CUSTOM),$(call custom_module_names,$(m),Bidoo)) \
		$(foreach m,$(BIDOO_CUSTOM_PER_FILE),$(call custom_per_file_names,$(m),Bidoo_$(shell basename $*))) \
		-DpluginInstance=pluginInstance__Bidoo \
		-IBidoo/src/dep/gverb/include \
		-Wno-ignored-qualifiers \
		-Wno-sign-compare \
		-Wno-unused-function

$(BUILD_DIR)/BogaudioModules/src/follower_base.cpp.o: BogaudioModules/src/follower_base.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(BOGAUDIO_CUSTOM),$(call custom_module_names,$(m),Bogaudio)) \
		-DpluginInstance=pluginInstance__BogaudioModules \
		-DefGainMaxDecibels=efGainMaxDecibelsDebug \
		-DRACK_SIMD=1 \
		-IBogaudioModules/lib \
		-IBogaudioModules/src/dsp

$(BUILD_DIR)/BogaudioModules/%.cpp.o: BogaudioModules/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(BOGAUDIO_CUSTOM),$(call custom_module_names,$(m),Bogaudio)) \
		$(foreach m,$(BOGAUDIO_CUSTOM_PER_FILE),$(call custom_per_file_names,$(m),Bogaudio_$(shell basename $*))) \
		-DpluginInstance=pluginInstance__BogaudioModules \
		-DRACK_SIMD=1 \
		-IBogaudioModules/lib \
		-IBogaudioModules/src/dsp

$(BUILD_DIR)/cf/src/%.cpp.o: cf/src/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(CF_CUSTOM),$(call custom_module_names,$(m),cf)) \
		-DpluginInstance=pluginInstance__cf \
		-Wno-misleading-indentation

$(BUILD_DIR)/ChowDSP/%.cpp.o: ChowDSP/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(CHOWDSP_CUSTOM),$(call custom_module_names,$(m),ChowDSP)) \
		-DpluginInstance=pluginInstance__ChowDSP \
		-DUSE_EIGEN \
		-IChowDSP/lib \
		-IChowDSP/lib/chowdsp_utils/DSP/WDF \
		-Wno-deprecated-copy

$(BUILD_DIR)/DrumKit/%.cpp.o: DrumKit/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(DRUMKIT_CUSTOM),$(call custom_module_names,$(m),DrumKit)) \
		-DpluginInstance=pluginInstance__DrumKit \
		-Dinit=init__DrumKit \
		-Wno-sign-compare

$(BUILD_DIR)/ESeries/%.cpp.o: ESeries/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(ESERIES_CUSTOM),$(call custom_module_names,$(m),ESeries)) \
		-DpluginInstance=pluginInstance__ESeries

$(BUILD_DIR)/Extratone/src/%.cpp.o: Extratone/src/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(EXTRATONE_CUSTOM),$(call custom_module_names,$(m),Extratone)) \
		-DpluginInstance=pluginInstance__Extratone

$(BUILD_DIR)/FehlerFabrik/%.cpp.o: FehlerFabrik/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(FEHLERFABRIK_CUSTOM),$(call custom_module_names,$(m),FehlerFabrik)) \
		-DpluginInstance=pluginInstance__FehlerFabrik

$(BUILD_DIR)/Fundamental/%.c.o: Fundamental/%.c
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CC) $< $(BUILD_C_FLAGS) -c -o $@ \
		$(foreach m,$(FUNDAMENTAL_CUSTOM),$(call custom_module_names,$(m),Fundamental)) \
		-DpluginInstance=pluginInstance__Fundamental

$(BUILD_DIR)/Fundamental/%.cpp.o: Fundamental/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(FUNDAMENTAL_CUSTOM),$(call custom_module_names,$(m),Fundamental)) \
		-DpluginInstance=pluginInstance__Fundamental

$(BUILD_DIR)/GlueTheGiant/%.cpp.o: GlueTheGiant/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(GLUETHEGIANT_CUSTOM),$(call custom_module_names,$(m),GlueTheGiant)) \
		-DpluginInstance=pluginInstance__GlueTheGiant

$(BUILD_DIR)/GlueTheGiant/src/gtgComponents.cpp.o: GlueTheGiant/src/gtgComponents.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(GLUETHEGIANT_CUSTOM),$(call custom_module_names,$(m),GlueTheGiant)) \
		-DpluginInstance=pluginInstance__GlueTheGiant \
		-DloadGtgPluginDefault=ignoredGlueTheGiant1 \
		-DsaveGtgPluginDefault=ignoredGlueTheGiant2

$(BUILD_DIR)/GrandeModular/%.cpp.o: GrandeModular/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(GRANDEMODULAR_CUSTOM),$(call custom_module_names,$(m),GrandeModular)) \
		-DpluginInstance=pluginInstance__GrandeModular \
		-Wno-missing-braces \
		-Wno-self-assign

$(BUILD_DIR)/HetrickCV/%.cpp.o: HetrickCV/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(HETRICKCV_CUSTOM),$(call custom_module_names,$(m),HetrickCV)) \
		-DpluginInstance=pluginInstance__HetrickCV \
		-DSTDIO_OVERRIDE=HetrickCV \
		-IHetrickCV/Gamma \
		-Wno-unused-but-set-variable

$(BUILD_DIR)/ImpromptuModular/src/Foundr%.cpp.o: ImpromptuModular/src/Foundr%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(IMPROMPTUMODULAR_CUSTOM),$(call custom_module_names,$(m),ImpromptuModular)) \
		-DpluginInstance=pluginInstance__ImpromptuModular \
		-DStepAttributes=StepAttributesKernel \

$(BUILD_DIR)/ImpromptuModular/src/ImpromptuModular.cpp.o: ImpromptuModular/src/ImpromptuModular.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(IMPROMPTUMODULAR_CUSTOM),$(call custom_module_names,$(m),ImpromptuModular)) \
		-DpluginInstance=pluginInstance__ImpromptuModular \
		-Dinit=init__ImpromptuModular \
		-UBUILDING_PLUGIN_MODULES

$(BUILD_DIR)/ImpromptuModular%.cpp.o: ImpromptuModular%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(IMPROMPTUMODULAR_CUSTOM),$(call custom_module_names,$(m),ImpromptuModular)) \
		$(foreach m,$(IMPROMPTUMODULAR_CUSTOM_PER_FILE),$(call custom_per_file_names,$(m),ImpromptuModular_$(shell basename $*))) \
		-DpluginInstance=pluginInstance__ImpromptuModular \
		-Wno-format-truncation

$(BUILD_DIR)/JW-Modules/src/WavHead.cpp.o: JW-Modules/src/WavHead.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(JW_CUSTOM),$(call custom_module_names,$(m),JW)) \
		-DpluginInstance=pluginInstance__JW \
		-D'nvgRGB(r,g,b)=nvgRGBblank(r,g,b)' \
		-IJW-Modules/src \
		-IJW-Modules/lib/oscpack \
		-Wno-misleading-indentation \
		-Wno-unused-but-set-variable \
		-Wno-unused-result

$(BUILD_DIR)/JW-Modules/%.cpp.o: JW-Modules/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(JW_CUSTOM),$(call custom_module_names,$(m),JW)) \
		-DpluginInstance=pluginInstance__JW \
		-IJW-Modules/src \
		-IJW-Modules/lib/oscpack \
		-Wno-misleading-indentation \
		-Wno-unused-but-set-variable \
		-Wno-unused-result

$(BUILD_DIR)/LifeFormModular/%.cpp.o: LifeFormModular/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(LIFEFORMMODULAR_CUSTOM),$(call custom_module_names,$(m),LifeFormModular)) \
		-DpluginInstance=pluginInstance__LifeFormModular

$(BUILD_DIR)/LittleUtils/%.cpp.o: LittleUtils/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(LITTLEUTILS_CUSTOM),$(call custom_module_names,$(m),LittleUtils)) \
		-DpluginInstance=pluginInstance__LittleUtils

$(BUILD_DIR)/LomasModules/%.cpp.o: LomasModules/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(LOMAS_CUSTOM),$(call custom_module_names,$(m),Lomas)) \
		-DpluginInstance=pluginInstance__Lomas

$(BUILD_DIR)/LyraeModules/%.cpp.o: LyraeModules/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(LYRAE_CUSTOM),$(call custom_module_names,$(m),Lyrae)) \
		-DpluginInstance=pluginInstance__Lyrae

$(BUILD_DIR)/MindMeldModular/MindMeldModular.cpp.o: MindMeldModular/src/MindMeldModular.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(MINDMELD_CUSTOM),$(call custom_module_names,$(m),MindMeld)) \
		-DpluginInstance=pluginInstance__MindMeld \
		-Dinit=init__MindMeld

$(BUILD_DIR)/MindMeldModular/%.cpp.o: MindMeldModular/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(MINDMELD_CUSTOM),$(call custom_module_names,$(m),MindMeld)) \
		-DpluginInstance=pluginInstance__MindMeld

$(BUILD_DIR)/Mog/%.cpp.o: Mog/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(MOG_CUSTOM),$(call custom_module_names,$(m),Mog)) \
		-DpluginInstance=pluginInstance__Mog

$(BUILD_DIR)/mscHack/%.cpp.o: mscHack/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(MSCHACK_CUSTOM),$(call custom_module_names,$(m),mscHack)) \
		$(foreach m,$(MSCHACK_CUSTOM_PER_FILE),$(call custom_per_file_names,$(m),mscHack_$(shell basename $*))) \
		-DthePlugin=pluginInstance__mscHack \
		-Dinit=init__mscHack \
		-Wno-class-memaccess \
		-Wno-format-overflow \
		-Wno-non-c-typedef-for-linkage \
		-Wno-unused-but-set-variable

$(BUILD_DIR)/Prism/%.cpp.o: Prism/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(PRISM_CUSTOM),$(call custom_module_names,$(m),Prism)) \
		$(foreach m,$(PRISM_CUSTOM_PER_FILE),$(call custom_per_file_names,$(m),Prism_$(shell basename $*))) \
		-DpluginInstance=pluginInstance__Prism \

$(BUILD_DIR)/rackwindows/%.cpp.o: rackwindows/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(RACKWINDOWS_CUSTOM),$(call custom_module_names,$(m),rackwindows)) \
		-DpluginInstance=pluginInstance__rackwindows \
		-Wno-implicit-fallthrough \
		-Wno-sign-compare

$(BUILD_DIR)/repelzen/%.cpp.o: repelzen/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(REPELZEN_CUSTOM),$(call custom_module_names,$(m),repelzen)) \
		-DpluginInstance=pluginInstance__repelzen

$(BUILD_DIR)/sonusmodular/%.cpp.o: sonusmodular/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(SONUSMODULAR_CUSTOM),$(call custom_module_names,$(m),sonusmodular)) \
		-DpluginInstance=pluginInstance__sonusmodular

$(BUILD_DIR)/ValleyAudio/%.cpp.o: ValleyAudio/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(VALLEYAUDIO_CUSTOM),$(call custom_module_names,$(m),ValleyAudio)) \
		$(foreach m,$(VALLEYAUDIO_CUSTOM_PER_FILE),$(call custom_per_file_names,$(m),ValleyAudio_$(shell basename $*))) \
		-DpluginInstance=pluginInstance__ValleyAudio \
		-DSTDIO_OVERRIDE=ValleyAudio \
		-Wno-sign-compare \
		-Wno-unused-but-set-variable

$(BUILD_DIR)/ZetaCarinaeModules/%.cpp.o: ZetaCarinaeModules/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(ZETACARINAE_CUSTOM),$(call custom_module_names,$(m),ZetaCarinae)) \
		-DpluginInstance=pluginInstance__ZetaCarinaeModules

$(BUILD_DIR)/ZZC/%.cpp.o: ZZC/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(ZZC_CUSTOM),$(call custom_module_names,$(m),ZZC)) \
		-DpluginInstance=pluginInstance__ZZC

# --------------------------------------------------------------

-include $(PLUGIN_OBJS:%.o=%.d)

# --------------------------------------------------------------
