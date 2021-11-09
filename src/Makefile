#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
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
# Build setup

BUILD_DIR = ../build/rack

ifeq ($(MACOS),true)
BASE_FLAGS += -DARCH_MAC
else ifeq ($(WINDOWS),true)
BASE_FLAGS += -DARCH_WIN
else
BASE_FLAGS += -DARCH_LIN
endif

BASE_FLAGS += -I../dpf/dgl/src/nanovg
BASE_FLAGS += -I../dpf/distrho
BASE_FLAGS += -I../include
BASE_FLAGS += -I../include/neon-compat
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
BASE_FLAGS += -pthread

ifeq ($(WASM),true)
BASE_FLAGS += -DNANOVG_GLES2=1
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
BASE_FLAGS += -fno-strict-aliasing -flto -ffat-lto-objects
endif

BUILD_C_FLAGS += -std=gnu11
BUILD_C_FLAGS += -fno-finite-math-only
BUILD_CXX_FLAGS += -fno-finite-math-only

# use our custom function to filter out VCV trademarks
BUILD_CXX_FLAGS += -DnsvgParseFromFile=nsvgParseFromFileCardinal

# --------------------------------------------------------------
# Rack files to build

RACK_FILES += AsyncDialog.cpp
RACK_FILES += override/Engine.cpp
RACK_FILES += override/asset.cpp
RACK_FILES += override/context.cpp
RACK_FILES += override/dep.cpp
RACK_FILES += override/library.cpp
RACK_FILES += override/network.cpp
RACK_FILES += override/osdialog.cpp

RACK_FILES += Rack/dep/pffft/pffft.c
RACK_FILES += Rack/dep/pffft/fftpack.c
RACK_FILES += Rack/dep/osdialog/osdialog.c
RACK_FILES += Rack/dep/oui-blendish/blendish.c

IGNORED_FILES  = Rack/src/asset.cpp
IGNORED_FILES += Rack/src/common.cpp
IGNORED_FILES += Rack/src/context.cpp
IGNORED_FILES += Rack/src/dep.cpp
IGNORED_FILES += Rack/src/discord.cpp
IGNORED_FILES += Rack/src/gamepad.cpp
IGNORED_FILES += Rack/src/keyboard.cpp
IGNORED_FILES += Rack/src/library.cpp
IGNORED_FILES += Rack/src/network.cpp
IGNORED_FILES += Rack/src/rtaudio.cpp
IGNORED_FILES += Rack/src/rtmidi.cpp
IGNORED_FILES += Rack/src/app/MenuBar.cpp
IGNORED_FILES += Rack/src/engine/Engine.cpp
IGNORED_FILES += Rack/src/window/Window.cpp

RACK_FILES += $(wildcard Rack/src/*.c)
RACK_FILES += $(wildcard Rack/src/*/*.c)
RACK_FILES += $(filter-out $(IGNORED_FILES),$(wildcard Rack/src/*.cpp))
RACK_FILES += $(filter-out $(IGNORED_FILES), $(wildcard Rack/src/*/*.cpp))

# --------------------------------------------------------------
# FIXME lots of warnings from VCV side

BASE_FLAGS += -Wno-unused-parameter
BASE_FLAGS += -Wno-unused-variable

# --------------------------------------------------------------
# Build targets

TARGET = rack.a

all: $(TARGET)
	$(MAKE) -C Cardinal
	$(MAKE) -C CardinalSynth

resources:
	$(MAKE) resources -C Cardinal
	$(MAKE) resources -C CardinalSynth

clean:
	rm -f $(TARGET)
	rm -rf $(BUILD_DIR)
	$(MAKE) clean -C Cardinal
	$(MAKE) clean -C CardinalSynth

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

# --------------------------------------------------------------

-include $(RACK_OBJS:%.o=%.d)

# --------------------------------------------------------------
