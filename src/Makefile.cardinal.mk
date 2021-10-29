#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

# Must have NAME defined

# --------------------------------------------------------------
# Build config

PREFIX  ?= /usr/local
DESTDIR ?=
SYSDEPS ?= false

# --------------------------------------------------------------
# Import base definitions

USE_NANOVG_FBO = true
include ../../dpf/Makefile.base.mk

# --------------------------------------------------------------
# Files to build (DPF stuff)

FILES_DSP  = CardinalPlugin.cpp
FILES_DSP += common.cpp

ifeq ($(HEADLESS),true)
FILES_DSP += RemoteNanoVG.cpp
FILES_DSP += RemoteWindow.cpp
else
FILES_UI  = CardinalUI.cpp
FILES_UI += MenuBar.cpp
FILES_UI += Window.cpp
endif

# --------------------------------------------------------------
# Extra libraries to link against

EXTRA_LIBS  = ../../plugins/plugins.a
EXTRA_LIBS += ../rack.a

ifneq ($(SYSDEPS),true)
EXTRA_LIBS += ../Rack/dep/lib/libjansson.a
EXTRA_LIBS += ../Rack/dep/lib/libsamplerate.a
EXTRA_LIBS += ../Rack/dep/lib/libspeexdsp.a
ifeq ($(WINDOWS),true)
EXTRA_LIBS += ../Rack/dep/lib/libarchive_static.a
else
EXTRA_LIBS += ../Rack/dep/lib/libarchive.a
endif
EXTRA_LIBS += ../Rack/dep/lib/libzstd.a
endif

EXTRA_DEPENDENCIES = $(EXTRA_LIBS)

# --------------------------------------------------------------
# Do some magic

USE_NANOVG_FBO = true
USE_VST2_BUNDLE = true
include ../../dpf/Makefile.plugins.mk

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
BASE_FLAGS += -I..
BASE_FLAGS += -I../../dpf/dgl/src/nanovg
BASE_FLAGS += -I../../include
BASE_FLAGS += -I../../include/neon-compat
BASE_FLAGS += -I../Rack/include
ifeq ($(SYSDEPS),true)
BASE_FLAGS += $(shell pkg-config --cflags jansson libarchive samplerate speexdsp)
else
BASE_FLAGS += -I../Rack/dep/include
endif
BASE_FLAGS += -I../Rack/dep/glfw/include
BASE_FLAGS += -I../Rack/dep/nanosvg/src
BASE_FLAGS += -I../Rack/dep/oui-blendish
BASE_FLAGS += -pthread

ifeq ($(WINDOWS),true)
BASE_FLAGS += -D_USE_MATH_DEFINES
BASE_FLAGS += -DWIN32_LEAN_AND_MEAN
BASE_FLAGS += -I../../include/mingw-compat
BASE_FLAGS += -I../../include/mingw-std-threads
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
# needed by VCVRack
EXTRA_LIBS += -ldbghelp -lshlwapi
# needed by JW-Modules
EXTRA_LIBS += -lws2_32 -lwinmm
endif

ifeq ($(SYSDEPS),true)
EXTRA_LIBS += $(shell pkg-config --libs jansson libarchive samplerate speexdsp)
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
SOURCE_DIR = Z:$(subst /,\\,$(abspath $(CURDIR)/..))
else
SOURCE_DIR = $(abspath $(CURDIR)/..)
endif

ifneq ($(SYSDEPS),true)
BUILD_CXX_FLAGS += -DCARDINAL_PLUGIN_SOURCE_DIR='"$(SOURCE_DIR)"'
endif

BUILD_CXX_FLAGS += -DCARDINAL_PLUGIN_PREFIX='"$(PREFIX)"'

# --------------------------------------------------------------
# Enable all possible plugin types

all: jack lv2 vst2 vst3 resources

# --------------------------------------------------------------

ifeq ($(NAME),Cardinal)

CORE_RESOURCES = $(subst ../Rack/res/,,$(wildcard ../Rack/res/*)) template.vcv

PLUGIN_RESOURCES += $(CORE_RESOURCES:%=$(TARGET_DIR)/Cardinal.lv2/resources/%)
ifeq ($(MACOS),true)
PLUGIN_RESOURCES += $(CORE_RESOURCES:%=$(TARGET_DIR)/Cardinal.vst/Contents/Resources/%)
else
PLUGIN_RESOURCES += $(CORE_RESOURCES:%=$(TARGET_DIR)/Cardinal.vst/resources/%)
endif
PLUGIN_RESOURCES += $(CORE_RESOURCES:%=$(TARGET_DIR)/Cardinal.vst3/Contents/Resources/%)

else

PLUGIN_RESOURCES += $(TARGET_DIR)/$(NAME).lv2/resources
ifeq ($(MACOS),true)
PLUGIN_RESOURCES += $(TARGET_DIR)/$(NAME).vst/Contents/Resources
else
PLUGIN_RESOURCES += $(TARGET_DIR)/$(NAME).vst/resources
endif
PLUGIN_RESOURCES += $(TARGET_DIR)/$(NAME).vst3/Contents/Resources

endif

# --------------------------------------------------------------

resources: $(PLUGIN_RESOURCES)

ifneq ($(NAME),Cardinal)
lv2: resources
vst2: resources
vst3: resources
$(TARGET_DIR)/$(NAME).%: $(TARGET_DIR)/Cardinal.%
	-@mkdir -p "$(shell dirname $@)"
	ln -sf $(abspath $<) $@
endif

$(TARGET_DIR)/Cardinal.%/template.vcv: ../template.vcv
	-@mkdir -p "$(shell dirname $@)"
	ln -sf $(abspath $<) $@

$(TARGET_DIR)/Cardinal.lv2/resources/%: ../Rack/res/%
	-@mkdir -p "$(shell dirname $@)"
	ln -sf $(abspath $<) $@

$(TARGET_DIR)/Cardinal.vst/resources/%: ../Rack/res/%
	-@mkdir -p "$(shell dirname $@)"
	ln -sf $(abspath $<) $@

$(TARGET_DIR)/Cardinal.vst/Contents/Resources/%: ../Rack/res/%
	-@mkdir -p "$(shell dirname $@)"
	ln -sf $(abspath $<) $@

$(TARGET_DIR)/Cardinal.vst3/Contents/Resources/%: ../Rack/res/%
	-@mkdir -p "$(shell dirname $@)"
	ln -sf $(abspath $<) $@

# --------------------------------------------------------------
