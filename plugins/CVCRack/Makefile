#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

# --------------------------------------------------------------
# Project name, used for binaries

NAME = CVCRack

# --------------------------------------------------------------
# Files to build (DPF stuff)

FILES_DSP = \
	CVCRackPlugin.cpp

FILES_UI = \
	CVCRackUI.cpp

# --------------------------------------------------------------
# Files to build (VCV stuff)

FILES_DSP += Rack/dep/pffft/pffft.c
FILES_DSP += Rack/dep/pffft/fftpack.c

# FILES_DSP += Rack/build/src/common.cpp

FILES_UI += Rack/dep/oui-blendish/blendish.c
FILES_UI += Rack/dep/nanovg/src/nanovg.c
FILES_UI += Rack/dep/glfw/deps/glad.c

# FIXME
FILES_UI += Rack/dep/osdialog/osdialog.c
FILES_UI += Rack/dep/osdialog/osdialog_zenity.c

FILES_DSP += $(wildcard Rack/src/*.c)
FILES_DSP += $(wildcard Rack/src/*/*.c)
# TODO filter out src/rtaudio.cpp src/rtmidi.cpp
FILES_DSP += $(wildcard Rack/src/*.cpp)
FILES_DSP += $(wildcard Rack/src/*/*.cpp)

EXTRA_LIBS  = Rack/dep/lib/libglfw3.a
EXTRA_LIBS += Rack/dep/lib/libjansson.a
EXTRA_LIBS += Rack/dep/lib/libspeexdsp.a

# ifeq ($(LINUX),true)
EXTRA_LIBS += Rack/dep/lib/libGLEW.a
# endif

# ifeq ($(MACOS),true)
# EXTRA_LIBS += Rack/dep/lib/libGLEW.a
# endif
# 
# ifeq ($(WINDOWS),true)
# EXTRA_LIBS += Rack/dep/lib/libglew32.a
# endif

# --------------------------------------------------------------

Rack/dep/lib/%.a:
	$(MAKE) -C Rack/dep lib/$*.a

# --------------------------------------------------------------
# Do some magic

include ../../dpf/Makefile.plugins.mk

# --------------------------------------------------------------
# Extra flags for VCV stuff

BASE_FLAGS += -D_APP_VERSION=2.git.0
BASE_FLAGS += -IRack/include
BASE_FLAGS += -IRack/dep/include
BASE_FLAGS += -IRack/dep/filesystem/include
BASE_FLAGS += -IRack/dep/fuzzysearchdatabase/src
BASE_FLAGS += -IRack/dep/glfw/deps
BASE_FLAGS += -IRack/dep/nanovg/src
BASE_FLAGS += -IRack/dep/nanosvg/src
BASE_FLAGS += -IRack/dep/osdialog
BASE_FLAGS += -IRack/dep/oui-blendish
BASE_FLAGS += -IRack/dep/pffft

ifeq ($(MACOS),true)
BASE_FLAGS += -DARCH_MAC
else ifeq ($(WINDOWS),true)
BASE_FLAGS += -DARCH_WIN
else
BASE_FLAGS += -DARCH_LIN
endif

# FIXME
BASE_FLAGS += -Wno-unused-parameter
BASE_FLAGS += -Wno-unused-variable

LINK_FLAGS += -lpthread -ldl

# --------------------------------------------------------------
# Enable all possible plugin types

TARGETS = jack lv2 vst2 vst3

all: $(TARGETS)

# --------------------------------------------------------------
