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
include ../../dpf/Makefile.base.mk

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
FILES_DSP += $(filter-out Rack/src/dep.cpp Rack/src/discord.cpp Rack/src/gamepad.cpp Rack/src/network.cpp Rack/src/rtaudio.cpp Rack/src/rtmidi.cpp, $(wildcard Rack/src/*.cpp))
FILES_DSP += $(filter-out Rack/src/window/Window.cpp, $(wildcard Rack/src/*/*.cpp))

EXTRA_LIBS += Rack/dep/lib/libjansson.a
EXTRA_LIBS += Rack/dep/lib/libspeexdsp.a

ifeq ($(WINDOWS),true)
EXTRA_LIBS += Rack/dep/lib/libarchive_static.a
else
EXTRA_LIBS += Rack/dep/lib/libarchive.a
endif

EXTRA_LIBS += Rack/dep/lib/libzstd.a

# --------------------------------------------------------------
# Do some magic

include ../../dpf/Makefile.plugins.mk

# --------------------------------------------------------------
# override VCV arch.mk stuff so we can build more architectures

DEP_PATH = $(abspath Rack/dep)

ifeq ($(CPU_ARM),true)
ARCH_NAME = arm
MACHINE = i686-bring-forth-the-rack
else ifeq ($(CPU_ARM64),true)
ARCH_NAME = arm64
MACHINE = x86_64-bring-forth-the-rack
else ifeq ($(CPU_AARCH64),true)
ARCH_NAME = aarch64
MACHINE = x86_64-bring-forth-the-rack
else
MACHINE = $(TARGET_MACHINE)
endif

ifneq ($(MACOS_OR_WINDOWS),true)
MACHINE_SUFFIX = -linux
endif

CONFIGURE = ./configure --prefix="$(DEP_PATH)" --host=$(TARGET_MACHINE)

# --------------------------------------------------------------
# Fix up cmake for windows cross-compilation

ifeq ($(WINDOWS),true)
CMAKE = cmake -G 'Unix Makefiles' -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_RC_COMPILER=$(subst gcc,windres,$(CC))
else
CMAKE = cmake
endif

CMAKE += -DCMAKE_INSTALL_LIBDIR=lib -DCMAKE_INSTALL_PREFIX='$(DEP_PATH)'

# --------------------------------------------------------------
# VCV internal dependencies target

Rack/dep/lib/%.a:
	$(MAKE) ARCH_NAME=$(ARCH_NAME) CMAKE="$(CMAKE)" CONFIGURE="$(CONFIGURE)" MACHINE=$(MACHINE)$(MACHINE_SUFFIX) -C Rack/dep lib/$*.a

Rack/dep/lib/libarchive.a: Rack/dep/lib/libzstd.a

Rack/dep/lib/libarchive_static.a: Rack/dep/lib/libzstd.a

# --------------------------------------------------------------
# Extra flags for VCV stuff

ifeq ($(MACOS),true)
BASE_FLAGS += -DARCH_MAC
else ifeq ($(WINDOWS),true)
BASE_FLAGS += -DARCH_WIN
else
BASE_FLAGS += -DARCH_LIN
endif

BUILD_C_FLAGS += -std=gnu11

# temporary macro just to get the ball rolling
ifeq ($(EXE_WRAPPER),wine)
SOURCE_DIR = Z:$(subst /,\\,$(CURDIR))
else
SOURCE_DIR = $(CURDIR)
endif
BUILD_CXX_FLAGS += -DCARDINAL_PLUGIN_SOURCE_DIR='"$(SOURCE_DIR)"'

BASE_FLAGS += -D_APP_VERSION=Cardinal
BASE_FLAGS += -I$(DPF_PATH)/dgl/src/nanovg
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
BASE_FLAGS      += -Imingw-compat
BUILD_CXX_FLAGS += -Imingw-std-threads
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
else ifeq ($(WINDOWS),true)
LINK_FLAGS += -ldbghelp -lshlwapi
endif

# --------------------------------------------------------------
# Enable all possible plugin types

TARGETS = jack lv2 vst2 vst3

all: $(TARGETS)

# --------------------------------------------------------------
