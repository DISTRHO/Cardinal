#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

BUILDING_RACK = true
ROOT = ..
include $(ROOT)/Makefile.base.mk

# --------------------------------------------------------------
# Build setup

ifeq ($(HEADLESS),true)
BUILD_DIR = ../build-headless/rack
else
BUILD_DIR = ../build/rack
endif

BASE_FLAGS += -DPRIVATE=
BASE_FLAGS += -I$(abspath $(ROOT)/src/Rack/dep/osdialog)

ifeq ($(HAVE_LIBLO),true)
BASE_FLAGS += -DHAVE_LIBLO $(LIBLO_FLAGS)
endif

# use our custom function to invert some colors
BUILD_CXX_FLAGS += -DnsvgDelete=nsvgDeleteCardinal
BUILD_CXX_FLAGS += -DnsvgParseFromFile=nsvgParseFromFileCardinal

# --------------------------------------------------------------
# Rack files to build

RACK_FILES += AsyncDialog.cpp
RACK_FILES += CardinalModuleWidget.cpp
RACK_FILES += custom/asset.cpp
RACK_FILES += custom/dep.cpp
RACK_FILES += custom/library.cpp
RACK_FILES += custom/network.cpp
RACK_FILES += custom/osdialog.cpp
RACK_FILES += override/blendish.c
RACK_FILES += override/context.cpp
RACK_FILES += override/minblep.cpp
RACK_FILES += override/plugin.cpp
RACK_FILES += override/Engine.cpp
RACK_FILES += override/Model.cpp
RACK_FILES += override/ModuleWidget.cpp
RACK_FILES += override/OpenGlWidget.cpp
RACK_FILES += override/Scene.cpp

RACK_FILES += Rack/dep/pffft/pffft.c
RACK_FILES += Rack/dep/pffft/fftpack.c
RACK_FILES += Rack/dep/osdialog/osdialog.c

IGNORED_FILES  = Rack/src/asset.cpp
IGNORED_FILES += Rack/src/audio.cpp
IGNORED_FILES += Rack/src/common.cpp
IGNORED_FILES += Rack/src/context.cpp
IGNORED_FILES += Rack/src/dep.cpp
IGNORED_FILES += Rack/src/discord.cpp
IGNORED_FILES += Rack/src/gamepad.cpp
IGNORED_FILES += Rack/src/keyboard.cpp
IGNORED_FILES += Rack/src/library.cpp
IGNORED_FILES += Rack/src/midi.cpp
IGNORED_FILES += Rack/src/midiloopback.cpp
IGNORED_FILES += Rack/src/network.cpp
IGNORED_FILES += Rack/src/plugin.cpp
IGNORED_FILES += Rack/src/rtaudio.cpp
IGNORED_FILES += Rack/src/rtmidi.cpp
IGNORED_FILES += Rack/src/app/AudioDisplay.cpp
IGNORED_FILES += Rack/src/app/MenuBar.cpp
IGNORED_FILES += Rack/src/app/MidiDisplay.cpp
IGNORED_FILES += Rack/src/app/ModuleWidget.cpp
IGNORED_FILES += Rack/src/app/Scene.cpp
IGNORED_FILES += Rack/src/app/TipWindow.cpp
IGNORED_FILES += Rack/src/dsp/minblep.cpp
IGNORED_FILES += Rack/src/engine/Engine.cpp
IGNORED_FILES += Rack/src/plugin/Model.cpp
IGNORED_FILES += Rack/src/widget/OpenGlWidget.cpp
IGNORED_FILES += Rack/src/window/Window.cpp
IGNORED_FILES += $(wildcard Rack/src/core/*.cpp)

RACK_FILES += Rack/dep/tinyexpr/tinyexpr.c
RACK_FILES += $(wildcard Rack/src/*.c)
RACK_FILES += $(wildcard Rack/src/*/*.c)
RACK_FILES += $(filter-out $(IGNORED_FILES),$(wildcard Rack/src/*.cpp))
RACK_FILES += $(filter-out $(IGNORED_FILES), $(wildcard Rack/src/*/*.cpp))

ifeq ($(WASM),true)
RACK_FILES += emscripten/WasmUtils.cpp
endif

# --------------------------------------------------------------
# lots of warnings from VCV side

BASE_FLAGS += -Wno-unused-but-set-variable
BASE_FLAGS += -Wno-unused-parameter
BASE_FLAGS += -Wno-unused-result
BASE_FLAGS += -Wno-unused-variable

ifeq ($(MACOS),true)
BASE_FLAGS += -Wno-unknown-warning-option
endif

# --------------------------------------------------------------
# Build targets

ifeq ($(HEADLESS),true)
TARGET_SUFFIX = -headless
endif

TARGETS = rack$(TARGET_SUFFIX).a rack-headless.a

ifneq ($(MACOS),true)
CARDINAL_FX_ARGS = VST2_FILENAME=Cardinal.vst/CardinalFX$(LIB_EXT) CLAP_FILENAME=Cardinal.clap/CardinalFX.clap
CARDINAL_SYNTH_ARGS = VST2_FILENAME=Cardinal.vst/CardinalSynth$(LIB_EXT) CLAP_FILENAME=Cardinal.clap/CardinalSynth.clap
endif

all: $(TARGETS)
ifeq ($(MOD_BUILD),true)
	$(MAKE) -C Cardinal lv2
	$(MAKE) -C CardinalFX lv2
	$(MAKE) -C CardinalMiniSep lv2_sep
else ifeq ($(WASM),true)
	$(MAKE) -C CardinalNative
	$(MAKE) -C CardinalMini
else
	$(MAKE) -C Cardinal
	$(MAKE) -C CardinalMini
	$(MAKE) -C CardinalMiniSep
	$(MAKE) -C CardinalNative
	$(MAKE) -C CardinalFX $(CARDINAL_FX_ARGS)
	$(MAKE) -C CardinalSynth $(CARDINAL_SYNTH_ARGS)
endif

jack: $(TARGETS)
	$(MAKE) jack -C Cardinal

native: $(TARGETS)
	$(MAKE) jack -C CardinalNative

mini: $(TARGETS)
	$(MAKE) jack -C CardinalMini
	$(MAKE) lv2_sep -C CardinalMiniSep

au: $(TARGETS)
	$(MAKE) au -C CardinalFX $(CARDINAL_SYNTH_ARGS)
	$(MAKE) au -C CardinalSynth $(CARDINAL_SYNTH_ARGS)

mapi: $(TARGETS)
	$(MAKE) mapi -C CardinalFX $(CARDINAL_FX_ARGS)

lv2: $(TARGETS)
	$(MAKE) lv2 -C Cardinal
	$(MAKE) lv2 -C CardinalFX $(CARDINAL_FX_ARGS)
	$(MAKE) lv2 -C CardinalSynth $(CARDINAL_SYNTH_ARGS)
	$(MAKE) lv2_sep -C CardinalMiniSep

vst2: $(TARGETS)
	$(MAKE) vst2 -C CardinalFX $(CARDINAL_FX_ARGS)
	$(MAKE) vst2 -C CardinalSynth $(CARDINAL_SYNTH_ARGS)

vst2fx: $(TARGETS)
	$(MAKE) vst2 -C CardinalFX $(CARDINAL_FX_ARGS)

vst3: $(TARGETS)
	$(MAKE) vst3 -C Cardinal
	$(MAKE) vst3 -C CardinalFX $(CARDINAL_FX_ARGS)
	$(MAKE) vst3 -C CardinalSynth $(CARDINAL_SYNTH_ARGS)

clap: $(TARGETS)
	$(MAKE) clap -C CardinalFX $(CARDINAL_FX_ARGS)
	$(MAKE) clap -C CardinalSynth $(CARDINAL_SYNTH_ARGS)

clean:
	rm -f *.a
	rm -rf $(BUILD_DIR)
	$(MAKE) clean -C Cardinal
	$(MAKE) clean -C CardinalFX $(CARDINAL_FX_ARGS)
	$(MAKE) clean -C CardinalSynth $(CARDINAL_SYNTH_ARGS)

# --------------------------------------------------------------
# Build commands

RACK_OBJS = $(RACK_FILES:%=$(BUILD_DIR)/%.o)

rack$(TARGET_SUFFIX).a: $(RACK_OBJS)
	@echo "Creating $@"
	$(SILENT)rm -f $@
	$(SILENT)$(AR) crs $@ $^

ifneq ($(HEADLESS),true)
rack-headless.a:
	$(MAKE) HEADLESS=true $@
endif

$(BUILD_DIR)/%.c.o: %.c
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CC) $< $(BUILD_C_FLAGS) -c -o $@

$(BUILD_DIR)/%.cpp.o: %.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@

$(BUILD_DIR)/emscripten/WasmUtils.cpp.o: BUILD_CXX_FLAGS += -fno-exceptions

$(BUILD_DIR)/Rack/dep/tinyexpr/tinyexpr.c.o: BUILD_C_FLAGS += -DTE_POW_FROM_RIGHT -DTE_NAT_LOG

# --------------------------------------------------------------

-include $(RACK_OBJS:%.o=%.d)

# --------------------------------------------------------------
