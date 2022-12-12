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
else ifeq ($(NAME),CardinalNative)
CARDINAL_VARIANT = native
else ifeq ($(NAME),CardinalSynth)
CARDINAL_VARIANT = synth
endif

# --------------------------------------------------------------
# Carla stuff

ifneq ($(STATIC_BUILD),true)

STATIC_PLUGIN_TARGET = true

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
ifneq ($(WASM),true)
CARLA_EXTRA_LIBS += $(CARLA_BUILD_DIR)/modules/$(CARLA_BUILD_TYPE)/jackbridge.min.a
endif
CARLA_EXTRA_LIBS += $(CARLA_BUILD_DIR)/modules/$(CARLA_BUILD_TYPE)/lilv.a
CARLA_EXTRA_LIBS += $(CARLA_BUILD_DIR)/modules/$(CARLA_BUILD_TYPE)/rtmempool.a
CARLA_EXTRA_LIBS += $(CARLA_BUILD_DIR)/modules/$(CARLA_BUILD_TYPE)/sfzero.a
CARLA_EXTRA_LIBS += $(CARLA_BUILD_DIR)/modules/$(CARLA_BUILD_TYPE)/water.a
CARLA_EXTRA_LIBS += $(CARLA_BUILD_DIR)/modules/$(CARLA_BUILD_TYPE)/ysfx.a
CARLA_EXTRA_LIBS += $(CARLA_BUILD_DIR)/modules/$(CARLA_BUILD_TYPE)/zita-resampler.a

endif # STATIC_BUILD

# --------------------------------------------------------------
# Import base definitions

DISTRHO_NAMESPACE = CardinalDISTRHO
DGL_NAMESPACE = CardinalDGL
NVG_DISABLE_SKIPPING_WHITESPACE = true
NVG_FONT_TEXTURE_FLAGS = NVG_IMAGE_NEAREST
USE_NANOVG_FBO = true
WASM_EXCEPTIONS = true

ifeq ($(CARDINAL_VARIANT),main)
# main variant should not use rtaudio/sdl2 fallback (it has CV ports)
SKIP_NATIVE_AUDIO_FALLBACK = true
else
# fx and synth variants should only use rtaudio/sdl2 fallbacks
FORCE_NATIVE_AUDIO_FALLBACK = true
endif

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
FILES_UI += glfw.cpp
FILES_UI += Window.cpp
endif

ifeq ($(WINDOWS),true)
FILES_UI += distrho.rc
endif

# --------------------------------------------------------------
# Extra libraries to link against

ifeq ($(NOPLUGINS),true)
RACK_EXTRA_LIBS  = ../../plugins/noplugins.a
else
RACK_EXTRA_LIBS  = ../../plugins/plugins.a
endif
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
# surgext libraries

ifneq ($(NOPLUGINS),true)
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
ifneq ($(WINDOWS),true)
RACK_EXTRA_LIBS += $(SURGE_DEP_PATH)/libs/sst/sst-plugininfra/libs/filesystem/libfilesystem.a
endif
RACK_EXTRA_LIBS += $(SURGE_DEP_PATH)/libs/sst/sst-plugininfra/libs/strnatcmp/libstrnatcmp.a
RACK_EXTRA_LIBS += $(SURGE_DEP_PATH)/libs/sst/sst-plugininfra/libs/tinyxml/libtinyxml.a
endif

# --------------------------------------------------------------

# FIXME
ifeq ($(WASM),true)
ifneq ($(STATIC_BUILD),true)
STATIC_CARLA_PLUGIN_LIBS = -lsndfile -lopus -lFLAC -lvorbisenc -lvorbis -logg -lm
endif
endif

EXTRA_DEPENDENCIES = $(RACK_EXTRA_LIBS) $(CARLA_EXTRA_LIBS)
EXTRA_LIBS = $(RACK_EXTRA_LIBS) $(CARLA_EXTRA_LIBS) $(STATIC_CARLA_PLUGIN_LIBS)

ifeq ($(shell $(PKG_CONFIG) --exists fftw3f && echo true),true)
EXTRA_DEPENDENCIES += ../../deps/aubio/libaubio.a
EXTRA_LIBS += ../../deps/aubio/libaubio.a
EXTRA_LIBS += $(shell $(PKG_CONFIG) --libs fftw3f)
endif

ifneq ($(NOPLUGINS),true)
ifeq ($(MACOS),true)
EXTRA_LIBS += -framework Accelerate
endif
endif

# --------------------------------------------------------------
# Setup resources

CORE_RESOURCES  = patches
CORE_RESOURCES += $(subst ../Rack/res/,,$(wildcard ../Rack/res/ComponentLibrary/*.svg ../Rack/res/fonts/*.ttf))

LV2_RESOURCES   = $(CORE_RESOURCES:%=$(TARGET_DIR)/$(NAME).lv2/resources/%)
VST3_RESOURCES  = $(CORE_RESOURCES:%=$(TARGET_DIR)/$(NAME).vst3/Contents/Resources/%)

ifeq ($(MACOS),true)
CLAP_RESOURCES = $(CORE_RESOURCES:%=$(TARGET_DIR)/$(NAME).clap/Contents/Resources/%)
else
CLAP_RESOURCES = $(CORE_RESOURCES:%=$(TARGET_DIR)/Cardinal.clap/resources/%)
endif

# Install modgui resources if MOD build
ifeq ($(MOD_BUILD),true)
LV2_RESOURCES += $(TARGET_DIR)/$(NAME).lv2/Plateau_Reverb.ttl
LV2_RESOURCES += $(TARGET_DIR)/$(NAME).lv2/modgui.ttl
LV2_RESOURCES += $(TARGET_DIR)/$(NAME).lv2/modgui/documentation.pdf
LV2_RESOURCES += $(TARGET_DIR)/$(NAME).lv2/modgui
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
WASM_RESOURCES  = $(LV2_RESOURCES)

ifneq ($(STATIC_BUILD),true)
WASM_RESOURCES += $(CURDIR)/lv2/fomp.lv2/manifest.ttl
endif

EXTRA_DEPENDENCIES += $(WASM_RESOURCES)
endif

# --------------------------------------------------------------
# Do some magic

USE_VST2_BUNDLE = true
USE_CLAP_BUNDLE = true
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
BASE_FLAGS += -I../../include/simd-compat
BASE_FLAGS += -I../Rack/include
ifeq ($(SYSDEPS),true)
BASE_FLAGS += -DCARDINAL_SYSDEPS
BASE_FLAGS += $(shell $(PKG_CONFIG) --cflags jansson libarchive samplerate speexdsp)
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

ifneq ($(WASM),true)
ifneq ($(HAIKU),true)
BASE_FLAGS += -pthread
endif
endif

ifeq ($(WINDOWS),true)
BASE_FLAGS += -D_USE_MATH_DEFINES
BASE_FLAGS += -DWIN32_LEAN_AND_MEAN
BASE_FLAGS += -D_WIN32_WINNT=0x0600
BASE_FLAGS += -I../../include/mingw-compat
BASE_FLAGS += -I../../include/mingw-std-threads
endif

ifeq ($(USE_GLES2),true)
BASE_FLAGS += -DNANOVG_GLES2_FORCED
else ifeq ($(USE_GLES3),true)
BASE_FLAGS += -DNANOVG_GLES3_FORCED
endif

BUILD_C_FLAGS += -std=gnu11
BUILD_C_FLAGS += -fno-finite-math-only -fno-strict-aliasing
BUILD_CXX_FLAGS += -fno-finite-math-only -fno-strict-aliasing

ifneq ($(MACOS),true)
BUILD_CXX_FLAGS += -faligned-new -Wno-abi
endif

# Rack code is not tested for this flag, unset it
BUILD_CXX_FLAGS += -U_GLIBCXX_ASSERTIONS -Wp,-U_GLIBCXX_ASSERTIONS

# Ignore bad behaviour from Rack API
BUILD_CXX_FLAGS += -Wno-format-security

# --------------------------------------------------------------
# FIXME lots of warnings from VCV side

BASE_FLAGS += -Wno-unused-parameter
BASE_FLAGS += -Wno-unused-variable

# --------------------------------------------------------------
# extra linker flags

ifeq ($(WASM),true)
ifneq ($(STATIC_BUILD),true)
LINK_FLAGS += --use-preload-plugins
LINK_FLAGS += --preload-file=./jsfx
LINK_FLAGS += --preload-file=./lv2
endif
LINK_FLAGS += --preload-file=../../bin/CardinalNative.lv2/resources@/resources
ifneq ($(NOPLUGINS),true)
SYMLINKED_DIRS_RESOURCES  =
# find . -type l | grep -v svg | grep -v ttf | grep -v art | grep -v json | grep -v png | grep -v otf | sort
SYMLINKED_DIRS_RESOURCES += BaconPlugs/res/midi/chopin
SYMLINKED_DIRS_RESOURCES += BaconPlugs/res/midi/debussy
SYMLINKED_DIRS_RESOURCES += BaconPlugs/res/midi/goldberg
SYMLINKED_DIRS_RESOURCES += cf/playeroscs
SYMLINKED_DIRS_RESOURCES += DrumKit/res/samples
SYMLINKED_DIRS_RESOURCES += Fundamental/presets
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
LINK_FLAGS += $(foreach d,$(SYMLINKED_DIRS_RESOURCES),--preload-file=../../bin/CardinalNative.lv2/resources/$(d)@/resources/$(d))
endif
LINK_FLAGS += -sALLOW_MEMORY_GROWTH
LINK_FLAGS += -sINITIAL_MEMORY=64Mb
LINK_FLAGS += -sLZ4=1
LINK_FLAGS += --shell-file=../emscripten/shell.html
LINK_FLAGS += -O3
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
EXTRA_LIBS += $(shell $(PKG_CONFIG) --libs jansson libarchive samplerate speexdsp)
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
TARGETS = lv2 vst3 clap
ifeq ($(HAVE_JACK),true)
TARGETS += jack
endif
else ifeq ($(CARDINAL_VARIANT),native)
TARGETS = jack
else
TARGETS = lv2 vst2 vst3 clap static
endif

all: $(TARGETS)
lv2: $(LV2_RESOURCES)
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
$(CURDIR)/lv2/fomp.lv2/manifest.ttl: $(TARGET_DIR)/$(NAME).lv2/resources/PluginManifests/Cardinal.json
	wget -O - https://falktx.com/data/wasm-things-2022-08-15.tar.gz | tar xz -C $(CURDIR)
	# FIXME we are past 50Mb compressed assets! :(
	rm -rf $(CURDIR)/lv2/mda.lv2
	touch $@
endif

# --------------------------------------------------------------
# Extra rules for Windows icon

ifeq ($(WINDOWS),true)
WINDRES ?= $(subst gcc,windres,$(CC))

JACK_LIBS += -Wl,-subsystem,windows

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
