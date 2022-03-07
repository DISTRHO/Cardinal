#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

# Must have NAME defined

ifeq ($(NAME),Cardinal)
CARDINAL_VARIANT = main
else ifeq ($(NAME),CardinalFX)
CARDINAL_VARIANT = fx
else ifeq ($(NAME),CardinalSynth)
CARDINAL_VARIANT = synth
endif

# --------------------------------------------------------------
# Carla stuff

ifneq ($(STATIC_BUILD),true)

ifneq ($(DEBUG),true)
EXTERNAL_PLUGINS = true
endif

CWD = ../../carla/source
include $(CWD)/Makefile.deps.mk

CARLA_BUILD_DIR = ../../carla/build
ifeq ($(DEBUG),true)
CARLA_BUILD_TYPE = Debug
else
CARLA_BUILD_TYPE = Release
endif

CARLA_EXTRA_LIBS  = $(CARLA_BUILD_DIR)/plugin/$(CARLA_BUILD_TYPE)/carla-host-plugin.cpp.o
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

endif # STATIC_BUILD

# --------------------------------------------------------------
# Import base definitions

USE_NANOVG_FBO = true
include ../../dpf/Makefile.base.mk

# --------------------------------------------------------------
# Build config

PREFIX  ?= /usr/local

ifeq ($(BSD),true)
SYSDEPS ?= true
else
SYSDEPS ?= false
endif

ifeq ($(SYSDEPS),true)
DEP_LIB_PATH = $(abspath ../../deps/sysroot/lib)
else
DEP_LIB_PATH = $(abspath ../Rack/dep/lib)
endif

# --------------------------------------------------------------
# Files to build (DPF stuff)

FILES_DSP  = CardinalPlugin.cpp
FILES_DSP += CardinalCommon.cpp
FILES_DSP += common.cpp

ifeq ($(HEADLESS),true)
FILES_DSP += RemoteNanoVG.cpp
FILES_DSP += RemoteWindow.cpp
else
FILES_UI  = CardinalUI.cpp
FILES_UI += Window.cpp
endif

# --------------------------------------------------------------
# Extra libraries to link against

RACK_EXTRA_LIBS  = ../../plugins/plugins.a
RACK_EXTRA_LIBS += ../rack.a
RACK_EXTRA_LIBS += $(DEP_LIB_PATH)/libquickjs.a

ifneq ($(SYSDEPS),true)
RACK_EXTRA_LIBS += $(DEP_LIB_PATH)/libjansson.a
RACK_EXTRA_LIBS += $(DEP_LIB_PATH)/libsamplerate.a
RACK_EXTRA_LIBS += $(DEP_LIB_PATH)/libspeexdsp.a
ifeq ($(WINDOWS),true)
RACK_EXTRA_LIBS += $(DEP_LIB_PATH)/libarchive_static.a
else
RACK_EXTRA_LIBS += $(DEP_LIB_PATH)/libarchive.a
endif
RACK_EXTRA_LIBS += $(DEP_LIB_PATH)/libzstd.a
endif

# --------------------------------------------------------------

EXTRA_DEPENDENCIES = $(RACK_EXTRA_LIBS) $(CARLA_EXTRA_LIBS)
EXTRA_LIBS = $(RACK_EXTRA_LIBS) $(CARLA_EXTRA_LIBS) $(STATIC_CARLA_PLUGIN_LIBS)

# --------------------------------------------------------------
# Do some magic

NVG_DISABLE_SKIPPING_WHITESPACE = true
NVG_FONT_TEXTURE_FLAGS = NVG_IMAGE_NEAREST
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

ifeq ($(HEADLESS),true)
BASE_FLAGS += -DHEADLESS
endif

ifeq ($(MOD_BUILD),true)
BASE_FLAGS += -DDISTRHO_PLUGIN_USES_MODGUI=1 -DDISTRHO_PLUGIN_MINIMUM_BUFFER_SIZE=0xffff
endif

ifeq ($(WASM),true)
BASE_FLAGS += -DNANOVG_GLES2=1
BASE_FLAGS += -msse -msse2 -msse3 -msimd128
else ifneq ($(HAIKU),true)
BASE_FLAGS += -pthread
endif

ifeq ($(WINDOWS),true)
BASE_FLAGS += -D_USE_MATH_DEFINES
BASE_FLAGS += -DWIN32_LEAN_AND_MEAN
BASE_FLAGS += -I../../include/mingw-compat
BASE_FLAGS += -I../../include/mingw-std-threads
endif

BUILD_C_FLAGS += -std=gnu11
BUILD_C_FLAGS += -fno-finite-math-only -fno-strict-aliasing
BUILD_CXX_FLAGS += -fno-finite-math-only -fno-strict-aliasing

# Rack code is not tested for this flag, unset it
BUILD_CXX_FLAGS += -U_GLIBCXX_ASSERTIONS -Wp,-U_GLIBCXX_ASSERTIONS

# --------------------------------------------------------------
# FIXME lots of warnings from VCV side

BASE_FLAGS += -Wno-unused-parameter
BASE_FLAGS += -Wno-unused-variable

# --------------------------------------------------------------
# extra linker flags

ifeq ($(HAIKU),true)
LINK_FLAGS += -lpthread
else
LINK_FLAGS += -pthread
endif

ifneq ($(HAIKU_OR_MACOS_OR_WINDOWS),true)
ifneq ($(STATIC_BUILD),true)
LINK_FLAGS += -ldl
endif
endif

ifeq ($(BSD),true)
ifeq ($(DEBUG),true)
LINK_FLAGS += -lexecinfo
endif
endif

ifeq ($(MACOS),true)
LINK_FLAGS += -framework IOKit
else ifeq ($(WINDOWS),true)
# needed by VCVRack
EXTRA_LIBS += -ldbghelp -lshlwapi -Wl,--stack,0x100000
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

ifneq ($(SYSDEPS),true)

ifeq ($(EXE_WRAPPER),wine)
SOURCE_DIR = Z:$(subst /,\\,$(abspath $(CURDIR)/..))
else
SOURCE_DIR = $(abspath $(CURDIR)/..)
endif

BUILD_CXX_FLAGS += -DCARDINAL_PLUGIN_SOURCE_DIR='"$(SOURCE_DIR)"'

endif

# --------------------------------------------------------------
# install path prefix for resource files

BUILD_CXX_FLAGS += -DCARDINAL_PLUGIN_PREFIX='"$(PREFIX)"'

# --------------------------------------------------------------
# Enable all possible plugin types and setup resources

ifeq ($(CARDINAL_VARIANT),main)
all: jack lv2 vst3
else ifeq ($(CARDINAL_VARIANT),fx)
all: lv2 vst2 vst3 static
else
all: lv2 vst2 vst3
endif

CORE_RESOURCES  = $(subst ../Rack/res/,,$(wildcard ../Rack/res/ComponentLibrary/*.svg ../Rack/res/fonts/*.ttf))
CORE_RESOURCES += template.vcv

LV2_RESOURCES   = $(CORE_RESOURCES:%=$(TARGET_DIR)/$(NAME).lv2/resources/%)
VST3_RESOURCES  = $(CORE_RESOURCES:%=$(TARGET_DIR)/$(NAME).vst3/Contents/Resources/%)

# Install modgui resources if MOD build
ifeq ($(MOD_BUILD),true)
LV2_RESOURCES += $(TARGET_DIR)/$(NAME).lv2/modgui.ttl
LV2_RESOURCES += $(TARGET_DIR)/$(NAME).lv2/modgui/documentation.pdf
LV2_RESOURCES += $(TARGET_DIR)/$(NAME).lv2/modgui
endif

# Cardinal main variant should not use rtaudio fallback (it has CV ports)
ifeq ($(CARDINAL_VARIANT),main)
jack: BUILD_CXX_FLAGS += -DDPF_JACK_STANDALONE_SKIP_RTAUDIO_FALLBACK
endif

# Cardinal main variant is not available as VST2 due to lack of CV ports
ifneq ($(CARDINAL_VARIANT),main)
ifeq ($(MACOS),true)
VST2_RESOURCES = $(CORE_RESOURCES:%=$(TARGET_DIR)/$(NAME).vst/Contents/Resources/%)
else
VST2_RESOURCES = $(CORE_RESOURCES:%=$(TARGET_DIR)/$(NAME).vst/resources/%)
endif
endif

lv2: $(LV2_RESOURCES)
vst2: $(VST2_RESOURCES)
vst3: $(VST3_RESOURCES)

# --------------------------------------------------------------

$(TARGET_DIR)/$(NAME).%/template.vcv: ../template.vcv
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

$(TARGET_DIR)/$(NAME).lv2/resources/%: ../Rack/res/%
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

ifeq ($(MOD_BUILD),true)
$(TARGET_DIR)/$(NAME).lv2/resources/%.svg: ../Rack/res/%.svg ../../deps/svg2stub.py
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)python3 ../../deps/svg2stub.py $< $@

$(TARGET_DIR)/$(NAME).lv2/mod%: ../MOD/$(NAME).lv2/mod%
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

$(TARGET_DIR)/$(NAME).lv2/modgui/documentation.pdf: ../../docs/MODDEVICES.md $(TARGET_DIR)/$(NAME).lv2/modgui
	(cd ../../docs/ && pandoc MODDEVICES.md -f markdown+implicit_figures -o $(abspath $@))
endif

$(TARGET_DIR)/$(NAME).vst/resources/%: ../Rack/res/%
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

$(TARGET_DIR)/$(NAME).vst/Contents/Resources/%: ../Rack/res/%
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

$(TARGET_DIR)/$(NAME).vst3/Contents/Resources/%: ../Rack/res/%
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

# --------------------------------------------------------------
