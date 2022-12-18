#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

# --------------------------------------------------------------
# Import base definitions

DISTRHO_NAMESPACE = CardinalDISTRHO
DGL_NAMESPACE = CardinalDGL
USE_NANOVG_FBO = true
WASM_EXCEPTIONS = true
include ../dpf/Makefile.base.mk

# --------------------------------------------------------------
# Build config

ifeq ($(BSD),true)
SYSDEPS ?= true
else
SYSDEPS ?= false
endif

# --------------------------------------------------------------
# Build setup

BUILD_DIR = ../build/rack

ifeq ($(MACOS),true)
BASE_FLAGS += -DARCH_MAC
else ifeq ($(WINDOWS),true)
BASE_FLAGS += -DARCH_WIN
else
BASE_FLAGS += -DARCH_LIN
endif

ifeq ($(HAIKU),true)
BASE_FLAGS += -I../include/linux-compat
else ifeq ($(WASM),true)
BASE_FLAGS += -I../include/linux-compat
endif

BASE_FLAGS += -DPRIVATE=
BASE_FLAGS += -I../dpf/dgl/src/nanovg
BASE_FLAGS += -I../dpf/distrho
BASE_FLAGS += -I../include
BASE_FLAGS += -I../include/simd-compat
BASE_FLAGS += -IRack/include
ifeq ($(SYSDEPS),true)
BASE_FLAGS += -DCARDINAL_SYSDEPS
BASE_FLAGS += $(shell pkg-config --cflags jansson libarchive samplerate speexdsp)
else
BASE_FLAGS += -DZSTDLIB_VISIBILITY=
BASE_FLAGS += -IRack/dep/include
endif
BASE_FLAGS += -IRack/dep/filesystem/include
BASE_FLAGS += -IRack/dep/fuzzysearchdatabase/src
BASE_FLAGS += -IRack/dep/glfw/include
BASE_FLAGS += -IRack/dep/nanosvg/src
BASE_FLAGS += -IRack/dep/osdialog
BASE_FLAGS += -IRack/dep/oui-blendish
BASE_FLAGS += -IRack/dep/pffft

ifeq ($(DEBUG),true)
BASE_FLAGS += -UDEBUG
ifeq ($(WASM),true)
# SSE must always be enabled on wasm, even in debug builds
BASE_FLAGS += -msse -msse2 -msse3 -msimd128
endif
endif

ifeq ($(HAVE_LIBLO),true)
BASE_FLAGS += -DHAVE_LIBLO $(LIBLO_FLAGS)
endif

ifeq ($(HEADLESS),true)
BASE_FLAGS += -DHEADLESS
endif

ifeq ($(BSD),true)
BASE_FLAGS += -DCLOCK_MONOTONIC_RAW=CLOCK_MONOTONIC_PRECISE
endif

ifeq ($(HAIKU),true)
BASE_FLAGS += -DCLOCK_MONOTONIC_RAW=CLOCK_MONOTONIC
endif

ifneq ($(WASM),true)
ifneq ($(HAIKU),true)
BASE_FLAGS += -pthread
endif
endif

ifeq ($(WINDOWS),true)
BASE_FLAGS += -D_USE_MATH_DEFINES
BASE_FLAGS += -DWIN32_LEAN_AND_MEAN
BASE_FLAGS += -D_WIN32_WINNT=0x0600
BASE_FLAGS += -I../include/mingw-compat
BASE_FLAGS += -I../include/mingw-std-threads
endif

ifeq ($(USE_GLES2),true)
BASE_FLAGS += -DNANOVG_GLES2_FORCED
else ifeq ($(USE_GLES3),true)
BASE_FLAGS += -DNANOVG_GLES3_FORCED
endif

BUILD_C_FLAGS += -std=gnu11
BUILD_C_FLAGS += -fno-finite-math-only -fno-strict-aliasing
BUILD_CXX_FLAGS += -fno-finite-math-only -fno-strict-aliasing

ifneq ($(MACOS),true)
BUILD_CXX_FLAGS += -faligned-new -Wno-abi
ifeq ($(MOD_BUILD),true)
BUILD_CXX_FLAGS += -fabi-version=12
endif
endif

# use our custom function to invert some colors
BUILD_CXX_FLAGS += -DnsvgDelete=nsvgDeleteCardinal
BUILD_CXX_FLAGS += -DnsvgParseFromFile=nsvgParseFromFileCardinal

# Rack code is not tested for this flag, unset it
BUILD_CXX_FLAGS += -U_GLIBCXX_ASSERTIONS -Wp,-U_GLIBCXX_ASSERTIONS

# Ignore bad behaviour from Rack API
BUILD_CXX_FLAGS += -Wno-format-security

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
RACK_FILES += override/MenuBar.cpp
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

TARGET = rack.a

ifneq ($(MACOS),true)
CARDINAL_FX_ARGS = VST2_FILENAME=Cardinal.vst/CardinalFX$(LIB_EXT) CLAP_FILENAME=Cardinal.clap/CardinalFX.clap
CARDINAL_SYNTH_ARGS = VST2_FILENAME=Cardinal.vst/CardinalSynth$(LIB_EXT) CLAP_FILENAME=Cardinal.clap/CardinalSynth.clap
endif

all: $(TARGET)
ifeq ($(MOD_BUILD),true)
	$(MAKE) -C Cardinal lv2
	$(MAKE) -C CardinalFX lv2
else ifeq ($(WASM),true)
	$(MAKE) -C CardinalNative
else
	$(MAKE) -C Cardinal
	$(MAKE) -C CardinalNative
	$(MAKE) -C CardinalFX $(CARDINAL_FX_ARGS)
	$(MAKE) -C CardinalSynth $(CARDINAL_SYNTH_ARGS)
endif

jack: $(TARGET)
	$(MAKE) jack -C Cardinal

native: $(TARGET)
	$(MAKE) jack -C CardinalNative

lv2: $(TARGET)
	$(MAKE) lv2 -C Cardinal
	$(MAKE) lv2 -C CardinalFX $(CARDINAL_FX_ARGS)
	$(MAKE) lv2 -C CardinalSynth $(CARDINAL_SYNTH_ARGS)

vst2: $(TARGET)
	$(MAKE) vst2 -C CardinalFX $(CARDINAL_FX_ARGS)
	$(MAKE) vst2 -C CardinalSynth $(CARDINAL_SYNTH_ARGS)

vst3: $(TARGET)
	$(MAKE) vst3 -C Cardinal
	$(MAKE) vst3 -C CardinalFX $(CARDINAL_FX_ARGS)
	$(MAKE) vst3 -C CardinalSynth $(CARDINAL_SYNTH_ARGS)

clap: $(TARGET)
	$(MAKE) clap -C CardinalFX $(CARDINAL_FX_ARGS)
	$(MAKE) clap -C CardinalSynth $(CARDINAL_SYNTH_ARGS)

clean:
	rm -f $(TARGET)
	rm -rf $(BUILD_DIR)
	$(MAKE) clean -C Cardinal
	$(MAKE) clean -C CardinalFX $(CARDINAL_FX_ARGS)
	$(MAKE) clean -C CardinalSynth $(CARDINAL_SYNTH_ARGS)

# --------------------------------------------------------------
# Build commands

RACK_OBJS = $(RACK_FILES:%=$(BUILD_DIR)/%.o)

$(TARGET): $(RACK_OBJS)
	@echo "Creating $@"
	$(SILENT)rm -f $@
	$(SILENT)$(AR) crs $@ $^

$(BUILD_DIR)/%.c.o: %.c
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CC) $< $(BUILD_C_FLAGS) -c -o $@

$(BUILD_DIR)/%.cpp.o: %.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@

$(BUILD_DIR)/emscripten/WasmUtils.cpp.o: BUILD_CXX_FLAGS += -fno-exceptions

# --------------------------------------------------------------

-include $(RACK_OBJS:%.o=%.d)

# --------------------------------------------------------------
