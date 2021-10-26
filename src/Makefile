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
# Project name, used for binaries

NAME = Cardinal

# --------------------------------------------------------------
# Files to build (DPF stuff)

FILES_DSP = \
	CardinalPlugin.cpp \
	override/asset.cpp \
	override/common.cpp \
	override/context.cpp \
	override/dep.cpp \
	override/library.cpp \
	override/network.cpp \
	override/osdialog.cpp

ifeq ($(HEADLESS),true)
FILES_DSP += \
	override/RemoteNanoVG.cpp \
	override/RemoteWindow.cpp
else
FILES_UI = \
	CardinalUI.cpp \
	override/MenuBar.cpp \
	override/Window.cpp
endif

# --------------------------------------------------------------
# Import base definitions

USE_NANOVG_FBO = true
include ../dpf/Makefile.base.mk

# --------------------------------------------------------------
# Files to build (VCV stuff)

FILES_DSP += Rack/dep/pffft/pffft.c
FILES_DSP += Rack/dep/pffft/fftpack.c
FILES_DSP += Rack/dep/osdialog/osdialog.c
FILES_DSP += Rack/dep/oui-blendish/blendish.c

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
IGNORED_FILES += Rack/src/window/Window.cpp

FILES_DSP += $(wildcard Rack/src/*.c)
FILES_DSP += $(wildcard Rack/src/*/*.c)
FILES_DSP += $(filter-out $(IGNORED_FILES),$(wildcard Rack/src/*.cpp))
FILES_DSP += $(filter-out $(IGNORED_FILES), $(wildcard Rack/src/*/*.cpp))

# --------------------------------------------------------------
# Extra libraries to link against

ifneq ($(SYSDEPS),true)
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
endif

# --------------------------------------------------------------
# Do some magic

DPF_PATH = ../dpf
DPF_BUILD_DIR = ../build
DPF_TARGET_DIR = ../bin
USE_VST2_BUNDLE = true
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

BASE_FLAGS += -fno-finite-math-only
BASE_FLAGS += -I../dpf/dgl/src/nanovg
BASE_FLAGS += -I../include
BASE_FLAGS += -I../include/neon-compat
BASE_FLAGS += -IRack/include
ifeq ($(SYSDEPS),true)
BASE_FLAGS += $(shell pkg-config --cflags jansson libarchive samplerate speexdsp)
else
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

BUILD_C_FLAGS += -std=gnu11

# --------------------------------------------------------------
# FIXME lots of warnings from VCV side

BASE_FLAGS += -Wno-unused-parameter
BASE_FLAGS += -Wno-unused-variable

# --------------------------------------------------------------
# extra linker flags

LINK_FLAGS += -pthread

ifneq ($(HAIKU_OR_MACOS_OR_WINDOWS),true)
LINK_FLAGS += -ldl
endif

ifeq ($(MACOS),true)
LINK_FLAGS += -framework IOKit
else ifeq ($(WINDOWS),true)
LINK_FLAGS += -ldbghelp -lshlwapi
# needed by JW-Modules
EXTRA_LIBS += -lws2_32 -lwinmm
endif

ifeq ($(SYSDEPS),true)
LINK_FLAGS += $(shell pkg-config --libs jansson libarchive samplerate speexdsp)
endif

ifeq ($(WITH_LTO),true)
LINK_FLAGS += -fno-strict-aliasing -flto -Werror=odr -Werror=lto-type-mismatch
endif

# --------------------------------------------------------------
# optional liblo

ifeq ($(HAVE_LIBLO),true)
BASE_FLAGS += $(LIBLO_FLAGS)
LINK_FLAGS += $(LIBLO_LIBS)
endif

# --------------------------------------------------------------
# fallback path to resource files

ifeq ($(EXE_WRAPPER),wine)
SOURCE_DIR = Z:$(subst /,\\,$(CURDIR))
else
SOURCE_DIR = $(CURDIR)
endif

ifneq ($(SYSDEPS),true)
BUILD_CXX_FLAGS += -DCARDINAL_PLUGIN_SOURCE_DIR='"$(SOURCE_DIR)"'
endif

BUILD_CXX_FLAGS += -DCARDINAL_PLUGIN_PREFIX='"$(PREFIX)"'

# --------------------------------------------------------------
# Enable all possible plugin types

all: jack lv2 vst2 vst3 resources

# --------------------------------------------------------------

CORE_RESOURCES = $(subst Rack/res/,,$(wildcard Rack/res/*)) template.vcv

PLUGIN_RESOURCES += $(CORE_RESOURCES:%=../bin/Cardinal.lv2/resources/%)
ifeq ($(MACOS),true)
PLUGIN_RESOURCES += $(CORE_RESOURCES:%=../bin/Cardinal.vst/Contents/Resources/%)
else
PLUGIN_RESOURCES += $(CORE_RESOURCES:%=../bin/Cardinal.vst/resources/%)
endif
PLUGIN_RESOURCES += $(CORE_RESOURCES:%=../bin/Cardinal.vst3/Contents/Resources/%)

resources: $(PLUGIN_RESOURCES)

../bin/Cardinal.%/template.vcv: template.vcv
	-@mkdir -p "$(shell dirname $@)"
	ln -sf $(abspath $<) $@

../bin/Cardinal.lv2/resources/%: Rack/res/%
	-@mkdir -p "$(shell dirname $@)"
	ln -sf $(abspath $<) $@

../bin/Cardinal.vst/resources/%: Rack/res/%
	-@mkdir -p "$(shell dirname $@)"
	ln -sf $(abspath $<) $@

../bin/Cardinal.vst/Contents/Resources/%: Rack/res/%
	-@mkdir -p "$(shell dirname $@)"
	ln -sf $(abspath $<) $@

../bin/Cardinal.vst3/Contents/Resources/%: Rack/res/%
	-@mkdir -p "$(shell dirname $@)"
	ln -sf $(abspath $<) $@

# --------------------------------------------------------------
