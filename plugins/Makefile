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
# Files to build

PLUGIN_FILES = plugins.cpp

# --------------------------------------------------------------
# MindMeld

PLUGIN_FILES += $(wildcard MindMeldModular/src/*.cpp)

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
# AS

PLUGIN_FILES += $(filter-out AS/src/AS.cpp,$(wildcard AS/src/*.cpp))
PLUGIN_FILES += AS/freeverb/revmodel.cpp

# modules/types which are present in other plugins
AS_CUSTOM = ADSR LabelDisplayWidget LowFrequencyOscillator VCA allpass comb revmodel

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
# Befaco

PLUGIN_FILES += $(filter-out Befaco/src/plugin.cpp,$(wildcard Befaco/src/*.cpp))
PLUGIN_BINARIES += Befaco/src/SpringReverbIR.pcm

# --------------------------------------------------------------
# Bidoo

PLUGIN_FILES += $(filter-out Bidoo/src/plugin.cpp Bidoo/src/ANTN.cpp,$(wildcard Bidoo/src/*.cpp))
PLUGIN_FILES += $(filter-out Bidoo/src/dep/waves.cpp,$(wildcard Bidoo/src/dep/*.cpp))
PLUGIN_FILES += $(wildcard Bidoo/src/dep/filters/*.cpp)
PLUGIN_FILES += $(wildcard Bidoo/src/dep/freeverb/*.cpp)
PLUGIN_FILES += $(wildcard Bidoo/src/dep/lodepng/*.cpp)
PLUGIN_FILES += Bidoo-workaround/waves.cpp

# modules/types which are present in other plugins
BIDOO_CUSTOM = LadderFilter
BIDOO_CUSTOM_PER_FILE = channel channel filterType
# randomize

# --------------------------------------------------------------
# BogaudioModules

PLUGIN_FILES += $(filter-out BogaudioModules/src/bogaudio.cpp,$(wildcard BogaudioModules/src/*.cpp))
PLUGIN_FILES += $(wildcard BogaudioModules/src/dsp/*.cpp)
PLUGIN_FILES += $(wildcard BogaudioModules/src/dsp/filters/*.cpp)

# modules/types which are present in other plugins
BOGAUDIO_CUSTOM = ADSR BlueNoiseGenerator LFO Noise VCA VCO VCF
BOGAUDIO_CUSTOM_PER_FILE = ARQuantity AttackMenuItem ReleaseMenuItem

# --------------------------------------------------------------
# Cardinal (built-in)

PLUGIN_FILES += $(wildcard Cardinal/src/*.cpp)

# --------------------------------------------------------------
# cf

PLUGIN_FILES += $(filter-out cf/src/plugin.cpp,$(wildcard cf/src/*.cpp))

# --------------------------------------------------------------
# ESeries

PLUGIN_FILES += ESeries/src/E340.cpp

# --------------------------------------------------------------
# Fundamental

PLUGIN_FILES += $(filter-out Fundamental/src/plugin.cpp Fundamental/src/VCA.cpp,$(wildcard Fundamental/src/*.cpp))
PLUGIN_FILES += Fundamental-workaround/VCA.cpp

# --------------------------------------------------------------
# GrandeModular

PLUGIN_FILES += $(filter-out GrandeModular/src/plugin.cpp,$(wildcard GrandeModular/src/*.cpp))

# --------------------------------------------------------------
# ImpromptuModular

PLUGIN_FILES += $(wildcard ImpromptuModular/src/*.cpp)
PLUGIN_FILES += $(wildcard ImpromptuModular/src/comp/*.cpp)

# modules/types which are present in other plugins
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
JW_CUSTOM = Quantizer

# --------------------------------------------------------------
# rackwindows

PLUGIN_FILES += $(filter-out rackwindows/src/plugin.cpp,$(wildcard rackwindows/src/*.cpp))

# --------------------------------------------------------------
# ValleyAudio (does not build against v2)

# PLUGIN_FILES += $(wildcard ValleyAudio/src/*.cpp)

# --------------------------------------------------------------
# ZetaCarinaeModules

PLUGIN_FILES += $(filter-out ZetaCarinaeModules/src/plugin.cpp,$(wildcard ZetaCarinaeModules/src/*.cpp))

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

BUILD_C_FLAGS += -std=gnu11

BASE_FLAGS += -fno-finite-math-only
BASE_FLAGS += -I../dpf/dgl/src/nanovg
BASE_FLAGS += -I../dpf/distrho

BASE_FLAGS += -I../include
BASE_FLAGS += -I../include/neon-compat

BASE_FLAGS += -I../src
BASE_FLAGS += -I../src/Rack/include
BASE_FLAGS += -I../src/Rack/include/dsp
# # BASE_FLAGS += -I../src/Rack/dep/filesystem/include
# # BASE_FLAGS += -I../src/Rack/dep/fuzzysearchdatabase/src
BASE_FLAGS += -I../src/Rack/dep/glfw/include
BASE_FLAGS += -I../src/Rack/dep/nanosvg/src
BASE_FLAGS += -I../src/Rack/dep/osdialog
BASE_FLAGS += -I../src/Rack/dep/oui-blendish
BASE_FLAGS += -I../src/Rack/dep/pffft
BASE_FLAGS += -pthread

ifneq ($(SYSDEPS),true)
BASE_FLAGS += -I../src/Rack/dep/include
endif

ifeq ($(WINDOWS),true)
BASE_FLAGS += -D_USE_MATH_DEFINES
BASE_FLAGS += -DWIN32_LEAN_AND_MEAN
BASE_FLAGS += -I../include/mingw-compat
BASE_FLAGS += -I../include/mingw-std-threads
endif

ifeq ($(HEADLESS),true)
BASE_FLAGS += -DHEADLESS
endif

ifeq ($(WITH_LTO),true)
BASE_FLAGS += -fno-strict-aliasing -flto
endif

# --------------------------------------------------------------
# FIXME lots of warnings from VCV side

BASE_FLAGS += -Wno-unused-parameter
BASE_FLAGS += -Wno-unused-variable

# also lots of plugins not updated to v2 yet
BASE_FLAGS += -Wno-deprecated-declarations

# --------------------------------------------------------------
# Build targets

TARGET = plugins.a

all: $(TARGET)

clean:
	rm -f $(TARGET)
	rm -rf $(BUILD_DIR)

# --------------------------------------------------------------

PLUGIN_LIST       = $(subst /plugin.json,,$(wildcard */plugin.json))
PLUGIN_RESOURCES  = ../bin/Cardinal.lv2/resources/PluginManifests/Core.json
PLUGIN_RESOURCES += $(PLUGIN_LIST:%=../bin/Cardinal.lv2/resources/%)
PLUGIN_RESOURCES += $(PLUGIN_LIST:%=../bin/Cardinal.lv2/resources/PluginManifests/%.json)
ifeq ($(MACOS),true)
PLUGIN_RESOURCES += ../bin/Cardinal.vst/Contents/Resources/PluginManifests/Core.json
PLUGIN_RESOURCES += $(PLUGIN_LIST:%=../bin/Cardinal.vst/Contents/Resources/%)
PLUGIN_RESOURCES += $(PLUGIN_LIST:%=../bin/Cardinal.vst/Contents/Resources/PluginManifests/%.json)
else
PLUGIN_RESOURCES += ../bin/Cardinal.vst/resources/PluginManifests/Core.json
PLUGIN_RESOURCES += $(PLUGIN_LIST:%=../bin/Cardinal.vst/resources/%)
PLUGIN_RESOURCES += $(PLUGIN_LIST:%=../bin/Cardinal.vst/resources/PluginManifests/%.json)
endif
PLUGIN_RESOURCES += ../bin/Cardinal.vst3/Contents/Resources/PluginManifests/Core.json
PLUGIN_RESOURCES += $(PLUGIN_LIST:%=../bin/Cardinal.vst3/Contents/Resources/%)
PLUGIN_RESOURCES += $(PLUGIN_LIST:%=../bin/Cardinal.vst3/Contents/Resources/PluginManifests/%.json)

resources: $(PLUGIN_RESOURCES)

../bin/Cardinal.%/Core.json: Core.json
	-@mkdir -p "$(shell dirname $@)"
	ln -sf $(abspath $<) $@

../bin/Cardinal.lv2/resources/%: %/res
	-@mkdir -p "$(shell dirname $@)"
	ln -sf $(abspath $<) $@

../bin/Cardinal.vst/resources/%: %/res
	-@mkdir -p "$(shell dirname $@)"
	ln -sf $(abspath $<) $@

../bin/Cardinal.vst/Contents/Resources/%: %/res
	-@mkdir -p "$(shell dirname $@)"
	ln -sf $(abspath $<) $@

../bin/Cardinal.vst3/Contents/Resources/%: %/res
	-@mkdir -p "$(shell dirname $@)"
	ln -sf $(abspath $<) $@

../bin/Cardinal.lv2/resources/PluginManifests/%.json: %/plugin.json
	-@mkdir -p "$(shell dirname $@)"
	ln -sf $(abspath $<) $@

../bin/Cardinal.vst/resources/PluginManifests/%.json: %/plugin.json
	-@mkdir -p "$(shell dirname $@)"
	ln -sf $(abspath $<) $@

../bin/Cardinal.vst/Contents/Resources/PluginManifests/%.json: %/plugin.json
	-@mkdir -p "$(shell dirname $@)"
	ln -sf $(abspath $<) $@

../bin/Cardinal.vst3/Contents/Resources/PluginManifests/%.json: %/plugin.json
	-@mkdir -p "$(shell dirname $@)"
	ln -sf $(abspath $<) $@

# --------------------------------------------------------------
# Build commands

PLUGIN_OBJS  = $(PLUGIN_FILES:%=$(BUILD_DIR)/%.o)
PLUGIN_OBJS += $(PLUGIN_BINARIES:%=$(BUILD_DIR)/%.bin.o)

.PRECIOUS: $(PLUGIN_BINARIES:%=$(BUILD_DIR)/%.bin)

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
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -IBogaudioModules/lib -IBogaudioModules/src/dsp -c -o $@

$(BUILD_DIR)/AmalgamatedHarmonics/%.cpp.o: AmalgamatedHarmonics/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -DpluginInstance=pluginInstance__AmalgamatedHarmonics $(foreach m,$(AMALGAMATEDHARMONICS_CUSTOM),$(call custom_module_names,$(m),AmalgamatedHarmonics)) -c -o $@

$(BUILD_DIR)/AnimatedCircuits/%.cpp.o: AnimatedCircuits/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -DpluginInstance=pluginInstance__AnimatedCircuits $(foreach m,$(ANIMATEDCIRCUITS_CUSTOM),$(call custom_module_names,$(m),AnimatedCircuits)) -c -o $@

$(BUILD_DIR)/AS/%.cpp.o: AS/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -DpluginInstance=pluginInstance__AS $(foreach m,$(AS_CUSTOM),$(call custom_module_names,$(m),AS)) -c -o $@

$(BUILD_DIR)/AudibleInstruments/%.cc.o: AudibleInstruments/%.cc
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -DpluginInstance=pluginInstance__AudibleInstruments $(foreach m,$(AUDIBLEINSTRUMENTS_CUSTOM),$(call custom_module_names,$(m),AudibleInstruments)) -DTEST -IAudibleInstruments/eurorack -Wno-unused-local-typedefs -c -o $@

$(BUILD_DIR)/AudibleInstruments/%.cpp.o: AudibleInstruments/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -DpluginInstance=pluginInstance__AudibleInstruments $(foreach m,$(AUDIBLEINSTRUMENTS_CUSTOM),$(call custom_module_names,$(m),AudibleInstruments)) -DTEST -IAudibleInstruments/eurorack -Wno-unused-local-typedefs -c -o $@

$(BUILD_DIR)/Befaco/%.cpp.o: Befaco/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -DpluginInstance=pluginInstance__Befaco $(foreach m,$(BEFACO_CUSTOM),$(call custom_module_names,$(m),Befaco)) -c -o $@

$(BUILD_DIR)/Bidoo/%.cpp.o: Bidoo/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(BIDOO_CUSTOM),$(call custom_module_names,$(m),Bidoo)) \
		$(foreach m,$(BIDOO_CUSTOM_PER_FILE),$(call custom_per_file_names,$(m),Bidoo_$(shell basename $*))) \
		-DpluginInstance=pluginInstance__Bidoo \
		-IBidoo/src/dep/gverb/include

$(BUILD_DIR)/Bidoo-workaround/%.cpp.o: Bidoo-workaround/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(BIDOO_CUSTOM),$(call custom_module_names,$(m),Bidoo)) \
		$(foreach m,$(BIDOO_CUSTOM_PER_FILE),$(call custom_per_file_names,$(m),Bidoo_$(shell basename $*))) \
		-DpluginInstance=pluginInstance__Bidoo \
		-IBidoo/src/dep/gverb/include

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

$(BUILD_DIR)/Cardinal/%.cpp.o: Cardinal/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -DpluginInstance=pluginInstance__Cardinal -c -o $@

$(BUILD_DIR)/cf/src/%.cpp.o: cf/src/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(CF_CUSTOM),$(call custom_module_names,$(m),cf)) \
		-DpluginInstance=pluginInstance__cf

$(BUILD_DIR)/ESeries/%.cpp.o: ESeries/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -DpluginInstance=pluginInstance__ESeries $(foreach m,$(ESERIES_CUSTOM),$(call custom_module_names,$(m),ESeries)) -c -o $@

$(BUILD_DIR)/Fundamental/%.cpp.o: Fundamental/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -DpluginInstance=pluginInstance__Fundamental $(foreach m,$(FUNDAMENTAL_CUSTOM),$(call custom_module_names,$(m),Fundamental)) -c -o $@

$(BUILD_DIR)/Fundamental-workaround/%.cpp.o: Fundamental-workaround/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -DpluginInstance=pluginInstance__Fundamental $(foreach m,$(FUNDAMENTAL_CUSTOM),$(call custom_module_names,$(m),Fundamental)) -c -o $@

$(BUILD_DIR)/GrandeModular/%.cpp.o: GrandeModular/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -DpluginInstance=pluginInstance__GrandeModular $(foreach m,$(GRANDEMODULAR_CUSTOM),$(call custom_module_names,$(m),GrandeModular)) -c -o $@

$(BUILD_DIR)/ImpromptuModular/%.cpp.o: ImpromptuModular/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(IMPROMPTUMODULAR_CUSTOM),$(call custom_module_names,$(m),ImpromptuModular)) \
		$(foreach m,$(IMPROMPTUMODULAR_CUSTOM_PER_FILE),$(call custom_per_file_names,$(m),ImpromptuModular_$(shell basename $*))) \
		-Dinit=init__ImpromptuModular \
		-DpluginInstance=pluginInstance__ImpromptuModular

$(BUILD_DIR)/ImpromptuModular/src/Foundr%.cpp.o: ImpromptuModular/src/Foundr%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(IMPROMPTUMODULAR_CUSTOM),$(call custom_module_names,$(m),ImpromptuModular)) \
		-DpluginInstance=pluginInstance__ImpromptuModular \
		-DStepAttributes=StepAttributesKernel

$(BUILD_DIR)/ImpromptuModular/src/comp/Components.cpp.o: ImpromptuModular/src/comp/Components.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(IMPROMPTUMODULAR_CUSTOM),$(call custom_module_names,$(m),ImpromptuModular)) \
		-DpluginInstance=pluginInstance__ImpromptuModular \
		-DsaveDarkAsDefault=ImpromptuModularIgnore1 \
		-DloadDarkAsDefault=ImpromptuModularIgonre2

$(BUILD_DIR)/JW-Modules/%.cpp.o: JW-Modules/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(JW_CUSTOM),$(call custom_module_names,$(m),JW)) \
		-DpluginInstance=pluginInstance__JW \
		-IJW-Modules/src \
		-IJW-Modules/lib/oscpack

$(BUILD_DIR)/MindMeldModular/%.cpp.o: MindMeldModular/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(MINDMELDMODULAR_CUSTOM),$(call custom_module_names,$(m),MindMeldModular)) \
		$(foreach m,$(MINDMELDMODULAR_CUSTOM_PER_FILE),$(call custom_per_file_names,$(m),MindMeldModular_$(shell basename $*))) \
		-Dinit=init__MindMeldModular \
		-DpluginInstance=pluginInstance__MindMeldModular

$(BUILD_DIR)/MindMeldModular/src/Foundr%.cpp.o: MindMeldModular/src/Foundr%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(MINDMELDMODULAR_CUSTOM),$(call custom_module_names,$(m),MindMeldModular)) \
		-DpluginInstance=pluginInstance__MindMeldModular \
		-DStepAttributes=StepAttributesKernel

$(BUILD_DIR)/MindMeldModular/src/comp/Components.cpp.o: MindMeldModular/src/comp/Components.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@ \
		$(foreach m,$(MINDMELDMODULAR_CUSTOM),$(call custom_module_names,$(m),MindMeldModular)) \
		-DpluginInstance=pluginInstance__MindMeldModular \
		-DsaveDarkAsDefault=MindMeldModularIgnore1 \
		-DloadDarkAsDefault=MindMeldModularIgonre2

$(BUILD_DIR)/rackwindows/%.cpp.o: rackwindows/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -DpluginInstance=pluginInstance__rackwindows $(foreach m,$(RACKWINDOWS_CUSTOM),$(call custom_module_names,$(m),rackwindows)) -c -o $@

$(BUILD_DIR)/ValleyAudio/%.cpp.o: ValleyAudio/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -DpluginInstance=pluginInstance__ValleyAudio $(foreach m,$(VALLEYAUDIO_CUSTOM),$(call custom_module_names,$(m),ValleyAudio)) -c -o $@

$(BUILD_DIR)/ZetaCarinaeModules/%.cpp.o: ZetaCarinaeModules/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -DpluginInstance=pluginInstance__ZetaCarinaeModules $(foreach m,$(ZETACARINAE_CUSTOM),$(call custom_module_names,$(m),ZetaCarinae)) -c -o $@

# --------------------------------------------------------------

-include $(PLUGIN_OBJS:%.o=%.d)

# --------------------------------------------------------------
