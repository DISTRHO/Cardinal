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
# Carla stuff

CWD = ../../carla/source
include $(CWD)/Makefile.deps.mk

CARLA_BUILD_DIR = ../../carla/build
ifeq ($(DEBUG),true)
CARLA_BUILD_TYPE = Debug
else
CARLA_BUILD_TYPE = Release
endif

CARLA_EXTRA_LIBS  = $(CARLA_BUILD_DIR)/plugin/$(CARLA_BUILD_TYPE)/carla-host-plugin.cpp.o
# ifneq ($(MACOS),true)
# CARLA_EXTRA_LIBS += -Wl,--start-group -Wl,--whole-archive
# endif
CARLA_EXTRA_LIBS += $(CARLA_BUILD_DIR)/modules/$(CARLA_BUILD_TYPE)/carla_engine_plugin.a
CARLA_EXTRA_LIBS += $(CARLA_BUILD_DIR)/modules/$(CARLA_BUILD_TYPE)/carla_plugin.a
CARLA_EXTRA_LIBS += $(CARLA_BUILD_DIR)/modules/$(CARLA_BUILD_TYPE)/native-plugins.a
CARLA_EXTRA_LIBS += $(CARLA_BUILD_DIR)/modules/$(CARLA_BUILD_TYPE)/audio_decoder.a
CARLA_EXTRA_LIBS += $(CARLA_BUILD_DIR)/modules/$(CARLA_BUILD_TYPE)/jackbridge.min.a
CARLA_EXTRA_LIBS += $(CARLA_BUILD_DIR)/modules/$(CARLA_BUILD_TYPE)/lilv.a
CARLA_EXTRA_LIBS += $(CARLA_BUILD_DIR)/modules/$(CARLA_BUILD_TYPE)/rtmempool.a
CARLA_EXTRA_LIBS += $(CARLA_BUILD_DIR)/modules/$(CARLA_BUILD_TYPE)/sfzero.a
CARLA_EXTRA_LIBS += $(CARLA_BUILD_DIR)/modules/$(CARLA_BUILD_TYPE)/water.a
CARLA_EXTRA_LIBS += $(CARLA_BUILD_DIR)/modules/$(CARLA_BUILD_TYPE)/zita-resampler.a

ifeq ($(MACOS),true)
ifeq ($(USING_JUCE),true)
CARLA_EXTRA_LIBS += $(CARLA_BUILD_DIR)/modules/$(CARLA_BUILD_TYPE)/juce_audio_basics.a
CARLA_EXTRA_LIBS += $(CARLA_BUILD_DIR)/modules/$(CARLA_BUILD_TYPE)/juce_audio_processors.a
CARLA_EXTRA_LIBS += $(CARLA_BUILD_DIR)/modules/$(CARLA_BUILD_TYPE)/juce_core.a
CARLA_EXTRA_LIBS += $(CARLA_BUILD_DIR)/modules/$(CARLA_BUILD_TYPE)/juce_data_structures.a
CARLA_EXTRA_LIBS += $(CARLA_BUILD_DIR)/modules/$(CARLA_BUILD_TYPE)/juce_events.a
CARLA_EXTRA_LIBS += $(CARLA_BUILD_DIR)/modules/$(CARLA_BUILD_TYPE)/juce_graphics.a
CARLA_EXTRA_LIBS += $(CARLA_BUILD_DIR)/modules/$(CARLA_BUILD_TYPE)/juce_gui_basics.a
ifeq ($(USING_JUCE_GUI_EXTRA),true)
CARLA_EXTRA_LIBS += $(CARLA_BUILD_DIR)/modules/$(CARLA_BUILD_TYPE)/juce_gui_extra.a
endif
endif
endif

# ifneq ($(MACOS),true)
# CARLA_EXTRA_LIBS += -Wl,--no-whole-archive -Wl,--end-group
# endif

# FIXME patch fluidsynth package
ifeq ($(WINDOWS),true)
STATIC_CARLA_PLUGIN_LIBS += -ldsound -lwinmm
endif

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

RACK_EXTRA_LIBS  = ../../plugins/plugins.a
RACK_EXTRA_LIBS += ../rack.a
RACK_EXTRA_LIBS += ../Rack/dep/lib/libquickjs.a

ifneq ($(SYSDEPS),true)
RACK_EXTRA_LIBS += ../Rack/dep/lib/libjansson.a
RACK_EXTRA_LIBS += ../Rack/dep/lib/libsamplerate.a
RACK_EXTRA_LIBS += ../Rack/dep/lib/libspeexdsp.a
ifeq ($(WINDOWS),true)
RACK_EXTRA_LIBS += ../Rack/dep/lib/libarchive_static.a
else
RACK_EXTRA_LIBS += ../Rack/dep/lib/libarchive.a
endif
RACK_EXTRA_LIBS += ../Rack/dep/lib/libzstd.a
endif

# --------------------------------------------------------------

EXTRA_DEPENDENCIES = $(RACK_EXTRA_LIBS) $(CARLA_EXTRA_LIBS)
EXTRA_LIBS = $(RACK_EXTRA_LIBS) $(CARLA_EXTRA_LIBS) $(STATIC_CARLA_PLUGIN_LIBS)

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

BASE_FLAGS += -DPRIVATE=

BASE_FLAGS += -I..
BASE_FLAGS += -I../../dpf/dgl/src/nanovg
BASE_FLAGS += -I../../include
BASE_FLAGS += -I../../include/neon-compat
BASE_FLAGS += -I../Rack/include
ifeq ($(SYSDEPS),true)
BASE_FLAGS += -DCARDINAL_SYSDEPS
BASE_FLAGS += $(shell pkg-config --cflags jansson libarchive samplerate speexdsp)
else
BASE_FLAGS += -DZSTDLIB_VISIBILITY=
BASE_FLAGS += -I../Rack/dep/include
endif
BASE_FLAGS += -I../Rack/dep/glfw/include
BASE_FLAGS += -I../Rack/dep/nanosvg/src
BASE_FLAGS += -I../Rack/dep/oui-blendish
BASE_FLAGS += -pthread

ifeq ($(HEADLESS),true)
BASE_FLAGS += -DHEADLESS
endif

ifeq ($(WASM),true)
BASE_FLAGS += -DNANOVG_GLES2=1
BASE_FLAGS += -msse -msse2 -msse3 -msimd128
else
BASE_FLAGS += -pthread
endif

ifeq ($(WINDOWS),true)
BASE_FLAGS += -D_USE_MATH_DEFINES
BASE_FLAGS += -DWIN32_LEAN_AND_MEAN
BASE_FLAGS += -I../../include/mingw-compat
BASE_FLAGS += -I../../include/mingw-std-threads
endif

BUILD_C_FLAGS += -std=gnu11
BUILD_C_FLAGS += -fno-finite-math-only
BUILD_CXX_FLAGS += -fno-finite-math-only

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
# false positive
LINK_FLAGS += -Wno-alloc-size-larger-than
ifneq ($(SYSDEPS),true)
# triggered by jansson
LINK_FLAGS += -Wno-stringop-overflow
endif
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
# Enable all possible plugin types and setup resources

ifeq ($(NAME),CardinalFX)

all: jack vst2 lv2 shared resources

CORE_RESOURCES = $(filter-out icon.png,$(subst ../Rack/res/,,$(wildcard ../Rack/res/*))) template.vcv

PLUGIN_RESOURCES += $(CORE_RESOURCES:%=$(TARGET_DIR)/CardinalFX.lv2/resources/%)
ifeq ($(MACOS),true)
PLUGIN_RESOURCES += $(CORE_RESOURCES:%=$(TARGET_DIR)/CardinalFX.vst/Contents/Resources/%)
else
PLUGIN_RESOURCES += $(CORE_RESOURCES:%=$(TARGET_DIR)/CardinalFX.vst/resources/%)
endif
PLUGIN_RESOURCES += $(CORE_RESOURCES:%=$(TARGET_DIR)/CardinalFX.vst3/Contents/Resources/%)

else # CardinalFX

ifeq ($(NAME),Cardinal)
all: jack lv2 vst3 resources
else
all: jack lv2 vst2 vst3 shared resources
endif

PLUGIN_RESOURCES += $(TARGET_DIR)/$(NAME).lv2/resources
PLUGIN_RESOURCES += $(TARGET_DIR)/$(NAME).vst3/Contents/Resources

# Cardinal (full) is not available as VST2 due to lack of CV ports
ifneq ($(NAME),Cardinal)
ifeq ($(MACOS),true)
PLUGIN_RESOURCES += $(TARGET_DIR)/$(NAME).vst/Contents/Resources
else
PLUGIN_RESOURCES += $(TARGET_DIR)/$(NAME).vst/resources
endif
endif

endif # CardinalFX

# --------------------------------------------------------------

resources: $(PLUGIN_RESOURCES)

ifneq ($(NAME),CardinalFX)
lv2: resources
vst2: resources
vst3: resources
$(TARGET_DIR)/$(NAME).%: $(TARGET_DIR)/CardinalFX.%
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@
endif

$(TARGET_DIR)/CardinalFX.%/template.vcv: ../template.vcv
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

$(TARGET_DIR)/CardinalFX.lv2/resources/%: ../Rack/res/%
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

$(TARGET_DIR)/CardinalFX.vst/resources/%: ../Rack/res/%
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

$(TARGET_DIR)/CardinalFX.vst/Contents/Resources/%: ../Rack/res/%
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

$(TARGET_DIR)/CardinalFX.vst3/Contents/Resources/%: ../Rack/res/%
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

# --------------------------------------------------------------
