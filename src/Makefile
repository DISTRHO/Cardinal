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
	dep.cpp \
	Window.cpp

# --------------------------------------------------------------
# Import base definitions

USE_NANOVG_FBO = true
USE_RGBA = true
include ../dpf/Makefile.base.mk

# --------------------------------------------------------------
# Files to build (VCV stuff)

FILES_DSP += Rack/dep/pffft/pffft.c
FILES_DSP += Rack/dep/pffft/fftpack.c

FILES_UI += Rack/dep/oui-blendish/blendish.c

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
FILES_DSP += $(filter-out Rack/src/dep.cpp Rack/src/discord.cpp Rack/src/gamepad.cpp Rack/src/keyboard.cpp Rack/src/library.cpp Rack/src/network.cpp Rack/src/rtaudio.cpp Rack/src/rtmidi.cpp, $(wildcard Rack/src/*.cpp))
FILES_DSP += $(filter-out Rack/src/window/Window.cpp, $(wildcard Rack/src/*/*.cpp))

# FOR TESTING
# FILES_DSP += Rack/src/rtaudio.cpp

# --------------------------------------------------------------
# Extra libraries to link against

EXTRA_LIBS  = ../plugins/plugins.a
EXTRA_LIBS += Rack/dep/lib/libjansson.a
EXTRA_LIBS += Rack/dep/lib/libsamplerate.a
EXTRA_LIBS += Rack/dep/lib/libspeexdsp.a
ifeq ($(WINDOWS),true)
EXTRA_LIBS += Rack/dep/lib/libarchive_static.a
else
EXTRA_LIBS += Rack/dep/lib/libarchive.a
endif
EXTRA_LIBS += Rack/dep/lib/libzstd.a

EXTRA_DEPENDENCIES = $(EXTRA_LIBS)

# --------------------------------------------------------------
# Do some magic

DPF_PATH = ../dpf
DPF_TARGET_DIR = ../bin
include ../dpf/Makefile.plugins.mk

# --------------------------------------------------------------
# Extra flags for VCV stuff

ifeq ($(MACOS),true)
BASE_FLAGS += -DARCH_MAC
else ifeq ($(WINDOWS),true)
BASE_FLAGS += -DARCH_WIN
else
BASE_FLAGS += -DARCH_LIN
endif

# FOR TESTING
# BASE_FLAGS += -I../dpf/distrho/src/jackbridge

BASE_FLAGS += -D_APP_VERSION=Cardinal
BASE_FLAGS += -I../dpf/dgl/src/nanovg
BASE_FLAGS += -I../include
BASE_FLAGS += -I../include/neon-compat
BASE_FLAGS += -IRack/include
BASE_FLAGS += -IRack/dep/include
BASE_FLAGS += -IRack/dep/filesystem/include
BASE_FLAGS += -IRack/dep/fuzzysearchdatabase/src
BASE_FLAGS += -IRack/dep/glfw/include
BASE_FLAGS += -IRack/dep/nanosvg/src
BASE_FLAGS += -IRack/dep/osdialog
BASE_FLAGS += -IRack/dep/oui-blendish
BASE_FLAGS += -IRack/dep/pffft

BASE_FLAGS += -pthread

ifeq ($(WINDOWS),true)
BASE_FLAGS += -D_USE_MATH_DEFINES
BASE_FLAGS += -Imingw-compat
BASE_FLAGS += -Imingw-std-threads
endif

BUILD_C_FLAGS += -std=gnu11

# --------------------------------------------------------------
# FIXME lots of warnings from VCV side

BASE_FLAGS += -Wno-unused-parameter
BASE_FLAGS += -Wno-unused-variable

# --------------------------------------------------------------
# extra linker flags

ifneq ($(HAIKU_OR_MACOS_OR_WINDOWS),true)
LINK_FLAGS += -ldl
endif

ifeq ($(MACOS),true)
LINK_FLAGS += -framework IOKit
else ifeq ($(WINDOWS),true)
LINK_FLAGS += -ldbghelp -lshlwapi
endif

# --------------------------------------------------------------
# temporary macro just to get the ball rolling

ifeq ($(EXE_WRAPPER),wine)
SOURCE_DIR = Z:$(subst /,\\,$(CURDIR))
else
SOURCE_DIR = $(CURDIR)
endif

BUILD_CXX_FLAGS += -DCARDINAL_PLUGIN_SOURCE_DIR='"$(SOURCE_DIR)"'

# --------------------------------------------------------------
# Enable all possible plugin types

all: jack lv2 vst2 vst3

# --------------------------------------------------------------
