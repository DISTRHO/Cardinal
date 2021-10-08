#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

# --------------------------------------------------------------
# Project name, used for binaries

NAME = Cardinal

# --------------------------------------------------------------
# Files to build (DPF stuff)

FILES_DSP = \
	CardinalPlugin.cpp

FILES_UI = \
	CardinalUI.cpp \
	Window.cpp

# --------------------------------------------------------------
# Import base definitions

# UI_TYPE = external
SKIP_NANOVG = true
include ../../dpf/Makefile.base.mk

# --------------------------------------------------------------
# Files to build (VCV stuff)

FILES_DSP += Rack/dep/pffft/pffft.c
FILES_DSP += Rack/dep/pffft/fftpack.c

FILES_UI += Rack/dep/oui-blendish/blendish.c
FILES_UI += Rack/dep/nanovg/src/nanovg.c

# FIXME dont use this
FILES_UI += Rack/dep/osdialog/osdialog.c
ifeq ($(MACOS),true)
FILES_UI += Rack/dep/osdialog/osdialog_mac.m
else ifeq ($(WINDOWS),true)
FILES_UI += Rack/dep/osdialog/osdialog_win.c
else
FILES_UI += Rack/dep/osdialog/osdialog_zenity.c
endif

FILES_DSP += $(wildcard Rack/src/*.c)
FILES_DSP += $(wildcard Rack/src/*/*.c)
FILES_DSP += $(filter-out Rack/src/discord.cpp Rack/src/gamepad.cpp Rack/src/network.cpp Rack/src/rtaudio.cpp Rack/src/rtmidi.cpp, $(wildcard Rack/src/*.cpp))
FILES_DSP += $(filter-out Rack/src/window/Window.cpp, $(wildcard Rack/src/*/*.cpp))

EXTRA_LIBS += Rack/dep/lib/libjansson.a
EXTRA_LIBS += Rack/dep/lib/libspeexdsp.a
EXTRA_LIBS += Rack/dep/lib/libzstd.a

ifeq ($(WINDOWS),true)
EXTRA_LIBS += Rack/dep/lib/libarchive_static.a
EXTRA_LIBS += Rack/dep/lib/libglew32.a
else
EXTRA_LIBS += Rack/dep/lib/libarchive.a
EXTRA_LIBS += Rack/dep/lib/libGLEW.a
endif

# --------------------------------------------------------------
# Do some magic

include ../../dpf/Makefile.plugins.mk

ifeq ($(WINDOWS),true)
CMAKE := cmake -G 'Unix Makefiles' -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_RC_COMPILER=$(subst gcc,windres,$(CC))
else
CMAKE := cmake
endif

BUILD_C_FLAGS += -std=gnu11

Rack/dep/lib/%.a:
	$(MAKE) CMAKE="$(CMAKE) -DCMAKE_INSTALL_LIBDIR=lib -DCMAKE_INSTALL_PREFIX='$(abspath Rack/dep)'" -C Rack/dep lib/$*.a

Rack/dep/lib/libarchive.a: Rack/dep/lib/libzstd.a

Rack/dep/lib/libarchive_static.a: Rack/dep/lib/libzstd.a

Rack/dep/lib/libcrypto.a: Rack/dep/lib/libssl.a

# --------------------------------------------------------------
# Extra flags for VCV stuff

BASE_FLAGS += -D_APP_VERSION=2.git.0
# BASE_FLAGS += -I$(DPF_PATH)/dgl/src/nanovg
BASE_FLAGS += -IRack/include
BASE_FLAGS += -IRack/dep/include
BASE_FLAGS += -IRack/dep/filesystem/include
BASE_FLAGS += -IRack/dep/fuzzysearchdatabase/src
BASE_FLAGS += -IRack/dep/glew-2.1.0/include
BASE_FLAGS += -IRack/dep/glfw/deps
BASE_FLAGS += -IRack/dep/glfw/include
BASE_FLAGS += -IRack/dep/nanovg/src
BASE_FLAGS += -IRack/dep/nanosvg/src
BASE_FLAGS += -IRack/dep/osdialog
BASE_FLAGS += -IRack/dep/oui-blendish
BASE_FLAGS += -IRack/dep/pffft

ifeq ($(WINDOWS),true)
BASE_FLAGS      += -Imingw-compat
BUILD_CXX_FLAGS += -Imingw-std-threads
endif

ifeq ($(MACOS),true)
BASE_FLAGS += -DARCH_MAC
else ifeq ($(WINDOWS),true)
BASE_FLAGS += -DARCH_WIN
else
BASE_FLAGS += -DARCH_LIN
endif

# FIXME lots of warnings from VCV side
BASE_FLAGS += -Wno-unused-parameter
BASE_FLAGS += -Wno-unused-variable

# extra linker flags
ifneq ($(HAIKU_OR_MACOS_OR_WINDOWS),true)
LINK_FLAGS += -ldl
endif

ifeq ($(MACOS),true)
LINK_FLAGS += -framework IOKit
# LINK_FLAGS += -Wl,-all_load
endif

# LINK_FLAGS += $(OPENGL_LIBS)

# TODO needed on windows? need to check
LINK_FLAGS += -lpthread

# --------------------------------------------------------------
# Enable all possible plugin types

TARGETS = jack lv2 vst2 vst3

all: $(TARGETS)

# --------------------------------------------------------------
