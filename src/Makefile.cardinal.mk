#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

# -----------------------------------------------------------------------------
# Set variant to build

ifeq ($(NAME),)
$(error invalid usage)
endif

ifeq ($(NAME),Cardinal)
CARDINAL_VARIANT = main
else ifeq ($(NAME),CardinalMini)
CARDINAL_VARIANT = mini
else ifeq ($(NAME),CardinalFX)
CARDINAL_VARIANT = fx
else ifeq ($(NAME),CardinalNative)
CARDINAL_VARIANT = native
else ifeq ($(NAME),CardinalSynth)
CARDINAL_VARIANT = synth
endif

# --------------------------------------------------------------
# Carla stuff

ifneq ($(CARDINAL_VARIANT),mini)
ifneq ($(STATIC_BUILD),true)

STATIC_PLUGIN_TARGET = true
include ../../carla/source/Makefile.deps.mk

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
CARLA_EXTRA_LIBS += $(CARLA_BUILD_DIR)/modules/$(CARLA_BUILD_TYPE)/water.a
CARLA_EXTRA_LIBS += $(CARLA_BUILD_DIR)/modules/$(CARLA_BUILD_TYPE)/ysfx.a
CARLA_EXTRA_LIBS += $(CARLA_BUILD_DIR)/modules/$(CARLA_BUILD_TYPE)/zita-resampler.a

endif # STATIC_BUILD
endif # CARDINAL_VARIANT mini

# --------------------------------------------------------------
# Import base definitions

ifeq ($(CARDINAL_VARIANT),main)
# main variant should not use rtaudio/sdl2 fallback (it has CV ports)
SKIP_NATIVE_AUDIO_FALLBACK = true
else ifneq ($(CARDINAL_VARIANT),mini)
# other variants should only use rtaudio/sdl2 fallbacks
FORCE_NATIVE_AUDIO_FALLBACK = true
endif

BUILDING_RACK = true
ROOT = ../..
include $(ROOT)/Makefile.base.mk

# --------------------------------------------------------------
# Build config

PREFIX  ?= /usr/local

DEP_LIB_PATH = $(RACK_DEP_PATH)/lib

# --------------------------------------------------------------
# Files to build (DPF stuff)

FILES_DSP  = CardinalPlugin.cpp
FILES_DSP += CardinalCommon.cpp
FILES_DSP += CardinalRemote.cpp
FILES_DSP += common.cpp

ifeq ($(DSP_UI_SPLIT),true)
FILES_DSP += RemoteNanoVG.cpp
FILES_DSP += RemoteWindow.cpp
else ifeq ($(HEADLESS),true)
FILES_DSP += RemoteNanoVG.cpp
FILES_DSP += RemoteWindow.cpp
else
FILES_UI  = CardinalUI.cpp
FILES_UI += glfw.cpp
FILES_UI += MenuBar.cpp
FILES_UI += Window.cpp
endif

ifeq ($(WINDOWS),true)
FILES_UI += distrho.rc
endif

ifneq ($(HAIKU_OR_MACOS_OR_WASM_OR_WINDOWS),true)
FILES_UI += CardinalX11WindowIcon.cpp
endif

# --------------------------------------------------------------
# Rack and plugin libs

ifeq ($(DSP_UI_SPLIT),true)
TARGET_SUFFIX = -headless
else ifeq ($(HEADLESS),true)
TARGET_SUFFIX = -headless
endif

ifeq ($(CARDINAL_VARIANT),mini)
RACK_EXTRA_LIBS  = ../../plugins/plugins-mini$(TARGET_SUFFIX).a
else
RACK_EXTRA_LIBS  = ../../plugins/plugins$(TARGET_SUFFIX).a
endif

ifeq ($(CARDINAL_VARIANT),mini)
RACK_EXTRA_LIBS += ../rack$(TARGET_SUFFIX).a
else
RACK_EXTRA_LIBS += ../rack$(TARGET_SUFFIX).a
endif

# --------------------------------------------------------------
# surgext libraries

SURGE_DEP_PATH = $(abspath ../../deps/surge-build)
RACK_EXTRA_LIBS += $(SURGE_DEP_PATH)/src/common/libsurge-common.a
RACK_EXTRA_LIBS += $(SURGE_DEP_PATH)/src/common/libjuce_dsp_rack_sub.a
RACK_EXTRA_LIBS += $(SURGE_DEP_PATH)/libs/airwindows/libairwindows.a
RACK_EXTRA_LIBS += $(SURGE_DEP_PATH)/libs/eurorack/libeurorack.a
ifeq ($(DEBUG),true)
RACK_EXTRA_LIBS += $(SURGE_DEP_PATH)/libs/fmt/libfmtd.a
else
RACK_EXTRA_LIBS += $(SURGE_DEP_PATH)/libs/fmt/libfmt.a
endif
RACK_EXTRA_LIBS += $(SURGE_DEP_PATH)/libs/sqlite-3.23.3/libsqlite.a
RACK_EXTRA_LIBS += $(SURGE_DEP_PATH)/libs/sst/sst-plugininfra/libsst-plugininfra.a
ifneq ($(MACOS)$(WINDOWS),true)
RACK_EXTRA_LIBS += $(SURGE_DEP_PATH)/libs/sst/sst-plugininfra/libs/filesystem/libfilesystem.a
endif
RACK_EXTRA_LIBS += $(SURGE_DEP_PATH)/libs/sst/sst-plugininfra/libs/strnatcmp/libstrnatcmp.a
RACK_EXTRA_LIBS += $(SURGE_DEP_PATH)/libs/sst/sst-plugininfra/libs/tinyxml/libtinyxml.a

# --------------------------------------------------------------
# Extra libraries to link against

ifneq ($(CARDINAL_VARIANT),mini)
RACK_EXTRA_LIBS += $(DEP_LIB_PATH)/libquickjs.a
endif

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

# FIXME
ifeq ($(CARDINAL_VARIANT)$(WASM),nativetrue)
ifneq ($(OLD_PATH),)
STATIC_CARLA_PLUGIN_LIBS = -lsndfile -lopus -lFLAC -lvorbisenc -lvorbis -logg -lm
endif
endif

EXTRA_DSP_DEPENDENCIES = $(RACK_EXTRA_LIBS) $(CARLA_EXTRA_LIBS)
EXTRA_DSP_LIBS = $(RACK_EXTRA_LIBS) $(CARLA_EXTRA_LIBS) $(STATIC_CARLA_PLUGIN_LIBS)

ifneq ($(CARDINAL_VARIANT),mini)
ifeq ($(shell $(PKG_CONFIG) --exists fftw3f && echo true),true)
EXTRA_DSP_DEPENDENCIES += ../../deps/aubio/libaubio.a
EXTRA_DSP_LIBS += ../../deps/aubio/libaubio.a
EXTRA_DSP_LIBS += $(filter-out -lpthread,$(shell $(PKG_CONFIG) --libs fftw3f))
endif
endif

ifeq ($(MACOS),true)
EXTRA_DSP_LIBS += -framework Accelerate -framework AppKit
else ifeq ($(WINDOWS),true)
EXTRA_DSP_LIBS += -lole32 -lshlwapi -luuid -lversion
endif

# --------------------------------------------------------------
# Setup resources

CORE_RESOURCES  = $(subst ../Rack/res/,,$(wildcard ../Rack/res/ComponentLibrary/*.svg ../Rack/res/fonts/*.ttf))
# ifneq ($(CARDINAL_VARIANT),mini)
CORE_RESOURCES += patches
# endif

LV2_RESOURCES   = $(CORE_RESOURCES:%=$(TARGET_DIR)/$(NAME).lv2/resources/%)
VST3_RESOURCES  = $(CORE_RESOURCES:%=$(TARGET_DIR)/$(NAME).vst3/Contents/Resources/%)

ifeq ($(MACOS),true)
CLAP_RESOURCES = $(CORE_RESOURCES:%=$(TARGET_DIR)/$(NAME).clap/Contents/Resources/%)
else
CLAP_RESOURCES = $(CORE_RESOURCES:%=$(TARGET_DIR)/Cardinal.clap/resources/%)
endif

# Install modgui resources if MOD build
ifeq ($(MOD_BUILD),true)
ifneq ($(CARDINAL_VARIANT),mini)
LV2_RESOURCES += $(TARGET_DIR)/$(NAME).lv2/Plateau_Reverb.ttl
LV2_RESOURCES += $(TARGET_DIR)/$(NAME).lv2/modgui.ttl
LV2_RESOURCES += $(TARGET_DIR)/$(NAME).lv2/modgui/documentation.pdf
LV2_RESOURCES += $(TARGET_DIR)/$(NAME).lv2/modgui
endif
endif

ifeq ($(CARDINAL_VARIANT),mini)
LV2_RESOURCES += $(TARGET_DIR)/$(NAME).lv2/modgui/screenshot.png
LV2_RESOURCES += $(TARGET_DIR)/$(NAME).lv2/modgui/thumbnail.png
endif

# Cardinal main variant is not available as VST2 due to lack of CV ports
ifneq ($(CARDINAL_VARIANT),main)
ifeq ($(MACOS),true)
VST2_RESOURCES = $(CORE_RESOURCES:%=$(TARGET_DIR)/$(NAME).vst/Contents/Resources/%)
else
VST2_RESOURCES = $(CORE_RESOURCES:%=$(TARGET_DIR)/Cardinal.vst/resources/%)
endif
endif

ifeq ($(WASM),true)
WASM_RESOURCES  = $(TARGET_DIR)/$(NAME).html $(LV2_RESOURCES)

EXTRA_DSP_DEPENDENCIES += $(WASM_RESOURCES)
endif

# --------------------------------------------------------------
# mini variant UI

ifeq ($(DSP_UI_SPLIT),true)
ifneq ($(HEADLESS),true)
FILES_UI  = CardinalUI.cpp
FILES_UI += CardinalCommon-UI.cpp
FILES_UI += CardinalRemote.cpp
FILES_UI += CardinalX11WindowIcon.cpp
FILES_UI += common.cpp
FILES_UI += glfw.cpp
FILES_UI += MenuBar.cpp
FILES_UI += Window.cpp
EXTRA_UI_DEPENDENCIES = $(subst -headless,,$(EXTRA_DSP_DEPENDENCIES))
EXTRA_UI_LIBS += $(subst -headless,,$(EXTRA_DSP_LIBS))
endif
endif

# --------------------------------------------------------------
# Do some magic

ifeq ($(WASM),true)
APP_EXT = .js
endif

USE_VST2_BUNDLE = true
USE_CLAP_BUNDLE = true
include ../../dpf/Makefile.plugins.mk

# --------------------------------------------------------------
# Extra flags for VCV stuff

BASE_FLAGS += -DPRIVATE=

# --------------------------------------------------------------
# Extra flags for MOD and Mini stuff

ifeq ($(MOD_BUILD),true)
BASE_FLAGS += -DDISTRHO_PLUGIN_MINIMUM_BUFFER_SIZE=0xffff
BASE_FLAGS += -DDISTRHO_PLUGIN_USES_MODGUI=1
else ifeq ($(CARDINAL_VARIANT),mini)
BASE_FLAGS += -DDISTRHO_PLUGIN_MINIMUM_BUFFER_SIZE=0xffff
endif

# --------------------------------------------------------------
# extra linker flags

ifeq ($(WASM),true)

LINK_FLAGS += -O3
LINK_FLAGS += -sALLOW_MEMORY_GROWTH
LINK_FLAGS += -sINITIAL_MEMORY=64Mb
LINK_FLAGS += -sLZ4=1
LINK_FLAGS += -sSTACK_SIZE=5MB

ifeq ($(CARDINAL_VARIANT),mini)
LINK_FLAGS += --preload-file=../../bin/CardinalMini.lv2/resources@/resources
else
LINK_FLAGS += --preload-file=../../bin/CardinalNative.lv2/resources@/resources
LINK_FLAGS += --use-preload-cache
LINK_FLAGS += --use-preload-plugins
endif

ifneq ($(DSP_UI_SPLIT),true)
LINK_FLAGS += -lidbfs.js
JACK_LIBS += -sEXPORTED_RUNTIME_METHODS=IDBFS,FS,cwrap
endif

# find . -type l | grep -v svg | grep -v ttf | grep -v art | grep -v json | grep -v png | grep -v otf | sort
SYMLINKED_DIRS_RESOURCES  = Fundamental/presets
ifneq ($(CARDINAL_VARIANT),mini)
SYMLINKED_DIRS_RESOURCES += BaconPlugs/res/midi/chopin
SYMLINKED_DIRS_RESOURCES += BaconPlugs/res/midi/debussy
SYMLINKED_DIRS_RESOURCES += BaconPlugs/res/midi/goldberg
SYMLINKED_DIRS_RESOURCES += cf/playeroscs
SYMLINKED_DIRS_RESOURCES += DrumKit/res/samples
SYMLINKED_DIRS_RESOURCES += GrandeModular/presets
SYMLINKED_DIRS_RESOURCES += LyraeModules/presets
SYMLINKED_DIRS_RESOURCES += Meander/res
SYMLINKED_DIRS_RESOURCES += MindMeldModular/presets
SYMLINKED_DIRS_RESOURCES += MindMeldModular/res/ShapeMaster/CommunityPresets
SYMLINKED_DIRS_RESOURCES += MindMeldModular/res/ShapeMaster/CommunityShapes
SYMLINKED_DIRS_RESOURCES += MindMeldModular/res/ShapeMaster/MindMeldPresets
SYMLINKED_DIRS_RESOURCES += MindMeldModular/res/ShapeMaster/MindMeldShapes
SYMLINKED_DIRS_RESOURCES += Mog/res
SYMLINKED_DIRS_RESOURCES += nonlinearcircuits/res
SYMLINKED_DIRS_RESOURCES += Orbits/presets
SYMLINKED_DIRS_RESOURCES += stoermelder-packone/presets
SYMLINKED_DIRS_RESOURCES += surgext/build/surge-data/fx_presets
SYMLINKED_DIRS_RESOURCES += surgext/build/surge-data/wavetables
SYMLINKED_DIRS_RESOURCES += surgext/patches
SYMLINKED_DIRS_RESOURCES += surgext/presets
endif
LINK_FLAGS += $(foreach d,$(SYMLINKED_DIRS_RESOURCES),--preload-file=../../bin/CardinalNative.lv2/resources/$(d)@/resources/$(d))

else ifeq ($(HAIKU),true)

LINK_FLAGS += -lpthread

else

LINK_FLAGS += -pthread

endif

ifneq ($(HAIKU_OR_MACOS_OR_WINDOWS),true)
ifneq ($(STATIC_BUILD),true)
LINK_FLAGS += -ldl
endif
endif

ifeq ($(BSD)$(DEBUG),truetrue)
LINK_FLAGS += -lexecinfo
endif

ifeq ($(MACOS),true)
LINK_FLAGS += -framework IOKit
else ifeq ($(WINDOWS),true)
# needed by VCVRack
LINK_FLAGS += -Wl,--stack,0x100000
EXTRA_DSP_LIBS += -ldbghelp -lshlwapi
# needed by JW-Modules
EXTRA_DSP_LIBS += -lws2_32 -lwinmm
endif

ifeq ($(SYSDEPS),true)
EXTRA_DSP_LIBS += $(shell $(PKG_CONFIG) --libs jansson libarchive samplerate speexdsp)
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
EXTRA_DSP_LIBS += $(LIBLO_LIBS)
endif

# --------------------------------------------------------------
# fallback path to resource files

ifneq ($(CIBUILD),true)
ifneq ($(SYSDEPS),true)

ifeq ($(EXE_WRAPPER),wine)
SOURCE_DIR = Z:$(subst /,\\,$(abspath $(CURDIR)/..))
else
SOURCE_DIR = $(abspath $(CURDIR)/..)
endif

BUILD_CXX_FLAGS += -DCARDINAL_PLUGIN_SOURCE_DIR='"$(SOURCE_DIR)"'

endif
endif

# --------------------------------------------------------------
# install path prefix for resource files

BUILD_CXX_FLAGS += -DCARDINAL_PLUGIN_PREFIX='"$(PREFIX)"'

# --------------------------------------------------------------
# Enable all possible plugin types and setup resources

ifeq ($(CARDINAL_VARIANT),main)
TARGETS = jack lv2 vst3 clap
else ifeq ($(DSP_UI_SPLIT),true)
TARGETS = lv2_sep
else ifeq ($(CARDINAL_VARIANT),mini)
TARGETS = jack
else ifeq ($(CARDINAL_VARIANT),native)
TARGETS = jack
else
TARGETS = lv2 vst2 vst3 clap static
endif

all: $(TARGETS)
lv2: $(LV2_RESOURCES)
lv2_sep: $(LV2_RESOURCES)
vst2: $(VST2_RESOURCES)
vst3: $(VST3_RESOURCES)
clap: $(CLAP_RESOURCES)

# --------------------------------------------------------------
# Extra rules for macOS app bundle

$(TARGET_DIR)/Cardinal.app/Contents/Info.plist: ../../utils/macOS/Info_JACK.plist $(TARGET_DIR)/Cardinal.app/Contents/Resources/distrho.icns
	-@mkdir -p $(shell dirname $@)
	cp $< $@

$(TARGET_DIR)/CardinalNative.app/Contents/Info.plist: ../../utils/macOS/Info_Native.plist $(TARGET_DIR)/CardinalNative.app/Contents/Resources/distrho.icns
	-@mkdir -p $(shell dirname $@)
	cp $< $@

$(TARGET_DIR)/%.app/Contents/Resources/distrho.icns: ../../utils/distrho.icns
	-@mkdir -p $(shell dirname $@)
	cp $< $@

# --------------------------------------------------------------
# Extra rules for wasm resources

ifeq ($(WASM),true)
$(TARGET_DIR)/$(NAME).html: ../emscripten/$(NAME).html
	-@mkdir -p $(shell dirname $@)
	cp $< $@
endif

# --------------------------------------------------------------
# Extra rules for Windows icon

ifeq ($(WINDOWS),true)
WINDRES ?= $(subst gcc,windres,$(CC))

$(BUILD_DIR)/distrho.rc.o: ../../utils/distrho.rc ../../utils/distrho.ico
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling distrho.rc"
	$(SILENT)$(WINDRES) $< -O coff -o $@
endif

# --------------------------------------------------------------

$(TARGET_DIR)/%/patches: ../../patches
	-@mkdir -p "$(shell dirname $@)"
ifeq ($(WASM),true)
	cp -rL $< $@
else
	$(SILENT)ln -sf $(abspath $<) $@
endif

$(TARGET_DIR)/$(NAME).lv2/mod%: ../MOD/$(NAME).lv2/mod%
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

$(TARGET_DIR)/$(NAME).lv2/resources/%: ../Rack/res/%
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

ifeq ($(MOD_BUILD),true)
$(TARGET_DIR)/$(NAME).lv2/resources/%.svg: ../Rack/res/%.svg ../../deps/svg2stub.py
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)python3 ../../deps/svg2stub.py $< $@

$(TARGET_DIR)/$(NAME).lv2/%.ttl: ../MOD/$(NAME).lv2/%.ttl
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

$(TARGET_DIR)/$(NAME).lv2/modgui/documentation.pdf: ../../docs/MODDEVICES.md $(TARGET_DIR)/$(NAME).lv2/modgui
	(cd ../../docs/ && pandoc MODDEVICES.md -f markdown+implicit_figures -o $(abspath $@))
endif

$(TARGET_DIR)/Cardinal.vst/resources/%: ../Rack/res/%
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

$(TARGET_DIR)/Cardinal.clap/resources/%: ../Rack/res/%
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

$(TARGET_DIR)/$(NAME).vst/Contents/Resources/%: ../Rack/res/%
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

$(TARGET_DIR)/$(NAME).vst3/Contents/Resources/%: ../Rack/res/%
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

$(TARGET_DIR)/$(NAME).clap/Contents/Resources/%: ../Rack/res/%
	-@mkdir -p "$(shell dirname $@)"
	$(SILENT)ln -sf $(abspath $<) $@

# --------------------------------------------------------------
