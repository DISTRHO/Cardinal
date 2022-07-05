#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

# also set in:
# src/CardinalCommon.cpp `CARDINAL_VERSION`
# src/CardinalPlugin.cpp `getVersion`
VERSION = 22.06

# --------------------------------------------------------------
# Import base definitions

include dpf/Makefile.base.mk

# --------------------------------------------------------------
# Build targets

all: cardinal carla deps dgl plugins gen resources

# --------------------------------------------------------------
# Build config

PREFIX  ?= /usr/local
DESTDIR ?=

ifeq ($(BSD),true)
SYSDEPS ?= true
else
SYSDEPS ?= false
endif

ifeq ($(LINUX),true)
VST3_SUPPORTED = true
else ifeq ($(MACOS),true)
VST3_SUPPORTED = true
else ifeq ($(WINDOWS),true)
VST3_SUPPORTED = true
endif

# --------------------------------------------------------------
# Carla config

CARLA_EXTRA_ARGS = \
	CARLA_BACKEND_NAMESPACE=Cardinal \
	DGL_NAMESPACE=CardinalDGL \
	HAVE_FFMPEG=false \
	HAVE_FLUIDSYNTH=false \
	HAVE_PROJECTM=false \
	HAVE_ZYN_DEPS=false \
	HAVE_ZYN_UI_DEPS=false

ifneq ($(DEBUG),true)
CARLA_EXTRA_ARGS += EXTERNAL_PLUGINS=true
endif

# --------------------------------------------------------------
# DGL config

DGL_EXTRA_ARGS = \
	DISTRHO_NAMESPACE=CardinalDISTRHO \
	DGL_NAMESPACE=CardinalDGL \
	NVG_DISABLE_SKIPPING_WHITESPACE=true \
	NVG_FONT_TEXTURE_FLAGS=NVG_IMAGE_NEAREST \
	USE_NANOVG_FBO=true \
	WINDOWS_ICON_ID=401

# --------------------------------------------------------------
# Check for required system-wide dependencies

ifeq ($(SYSDEPS),true)

ifneq ($(shell pkg-config --exists jansson && echo true),true)
$(error jansson dependency not installed/available)
endif
ifneq ($(shell pkg-config --exists libarchive && echo true),true)
$(error libarchive dependency not installed/available)
endif
ifneq ($(shell pkg-config --exists samplerate && echo true),true)
$(error samplerate dependency not installed/available)
endif
ifneq ($(shell pkg-config --exists speexdsp && echo true),true)
$(error speexdsp dependency not installed/available)
endif

endif

ifeq ($(HEADLESS),true)

ifneq ($(shell pkg-config --exists liblo && echo true),true)
$(error liblo dependency not installed/available)
endif

endif

# --------------------------------------------------------------
# Check for X11+OpenGL dependencies (unless headless build)

ifneq ($(HAIKU_OR_MACOS_OR_WASM_OR_WINDOWS),true)
ifneq ($(HEADLESS),true)

ifneq ($(HAVE_OPENGL),true)
$(error OpenGL dependency not installed/available)
endif
ifneq ($(HAVE_X11),true)
$(error X11 dependency not installed/available)
endif
ifneq ($(HAVE_XCURSOR),true)
$(warning Xcursor dependency not installed/available)
endif
ifneq ($(HAVE_XEXT),true)
$(warning Xext dependency not installed/available)
endif
ifneq ($(HAVE_XRANDR),true)
$(warning Xrandr dependency not installed/available)
endif

else

CARLA_EXTRA_ARGS += HAVE_OPENGL=false
CARLA_EXTRA_ARGS += HAVE_X11=false
CARLA_EXTRA_ARGS += HAVE_XCURSOR=false
CARLA_EXTRA_ARGS += HAVE_XEXT=false
CARLA_EXTRA_ARGS += HAVE_XRANDR=false

endif
endif

# --------------------------------------------------------------
# Check for optional system-wide dependencies

ifeq ($(shell pkg-config --exists fftw3f && echo true),true)
HAVE_FFTW3F = true
else
$(warning fftw3f dependency not installed/available)
endif

# --------------------------------------------------------------
# MOD builds

EXTRA_MOD_FLAGS  = -I../include/single-precision -fsingle-precision-constant

ifeq ($(MODDUO),true)
EXTRA_MOD_FLAGS += -mno-unaligned-access
endif
ifeq ($(WITH_LTO),true)
EXTRA_MOD_FLAGS += -ffat-lto-objects
endif

MOD_WORKDIR ?= $(HOME)/mod-workdir
MOD_ENVIRONMENT = \
	AR=${1}/host/usr/bin/${2}-gcc-ar \
	CC=${1}/host/usr/bin/${2}-gcc \
	CPP=${1}/host/usr/bin/${2}-cpp \
	CXX=${1}/host/usr/bin/${2}-g++ \
	LD=${1}/host/usr/bin/${2}-ld \
	PKG_CONFIG=${1}/host/usr/bin/pkg-config \
	STRIP=${1}/host/usr/bin/${2}-strip \
	CFLAGS="-I${1}/staging/usr/include $(EXTRA_MOD_FLAGS)" \
	CPPFLAGS= \
	CXXFLAGS="-I${1}/staging/usr/include $(EXTRA_MOD_FLAGS) -Wno-attributes" \
	LDFLAGS="-L${1}/staging/usr/lib $(EXTRA_MOD_FLAGS)" \
	EXE_WRAPPER="qemu-${3}-static -L ${1}/target" \
	HEADLESS=true \
	MOD_BUILD=true \
	NOOPT=true \
	STATIC_BUILD=true

modduo:
	$(MAKE) $(call MOD_ENVIRONMENT,$(MOD_WORKDIR)/modduo-static,arm-mod-linux-gnueabihf.static,arm)

modduox:
	$(MAKE) $(call MOD_ENVIRONMENT,$(MOD_WORKDIR)/modduox-static,aarch64-mod-linux-gnueabi.static,aarch64)

moddwarf:
	$(MAKE) $(call MOD_ENVIRONMENT,$(MOD_WORKDIR)/moddwarf,aarch64-mod-linux-gnu,aarch64)

publish:
	tar -C bin -cz $(subst bin/,,$(wildcard bin/*.lv2)) | base64 | curl -F 'package=@-' http://192.168.51.1/sdk/install && echo

ifneq (,$(findstring modduo-,$(MAKECMDGOALS)))
$(MAKECMDGOALS):
	$(MAKE) $(call MOD_ENVIRONMENT,$(MOD_WORKDIR)/modduo-static,arm-mod-linux-gnueabihf.static,arm) $(subst modduo-,,$(MAKECMDGOALS))
endif

ifneq (,$(findstring modduox-,$(MAKECMDGOALS)))
$(MAKECMDGOALS):
	$(MAKE) $(call MOD_ENVIRONMENT,$(MOD_WORKDIR)/modduox-static,aarch64-mod-linux-gnueabi.static,aarch64) $(subst modduox-,,$(MAKECMDGOALS))
endif

ifneq (,$(findstring moddwarf-,$(MAKECMDGOALS)))
$(MAKECMDGOALS):
	$(MAKE) $(call MOD_ENVIRONMENT,$(MOD_WORKDIR)/moddwarf,aarch64-mod-linux-gnu,aarch64) $(subst moddwarf-,,$(MAKECMDGOALS))
endif

# --------------------------------------------------------------
# Individual targets

cardinal: carla deps dgl plugins
	$(MAKE) all -C src $(CARLA_EXTRA_ARGS)

carla:
ifneq ($(STATIC_BUILD),true)
	$(MAKE) static-plugin -C carla $(CARLA_EXTRA_ARGS) \
		CAN_GENERATE_LV2_TTL=false \
		CUSTOM_DPF_PATH=$(CURDIR)/dpf \
		STATIC_PLUGIN_TARGET=true \
		USING_CUSTOM_DPF=true
endif

deps:
ifeq ($(SYSDEPS),true)
	$(MAKE) quickjs -C deps
else
	$(MAKE) all -C deps
endif
ifeq ($(HAVE_FFTW3F),true)
	$(MAKE) all -C deps/aubio
endif

dgl:
ifneq ($(HEADLESS),true)
	$(MAKE) -C dpf/dgl opengl $(DGL_EXTRA_ARGS)
endif

plugins: deps
	$(MAKE) all -C plugins

resources: cardinal
	$(MAKE) resources -C plugins

ifneq ($(CROSS_COMPILING),true)
gen: cardinal resources dpf/utils/lv2_ttl_generator
	@$(CURDIR)/dpf/utils/generate-ttl.sh

dpf/utils/lv2_ttl_generator:
	$(MAKE) -C dpf/utils/lv2-ttl-generator
else
gen:
endif

# --------------------------------------------------------------
# Packaging standalone for CI

unzipfx: deps/unzipfx/unzipfx2cat$(APP_EXT) Cardinal.zip
	cat deps/unzipfx/unzipfx2cat$(APP_EXT) Cardinal.zip > Cardinal
	chmod +x Cardinal

Cardinal.zip: bin/Cardinal bin/CardinalFX.lv2/resources
	mkdir -p build/unzipfx
	ln -sf ../../bin/Cardinal build/unzipfx/Cardinal
	ln -s ../../bin/CardinalFX.lv2/resources build/unzipfx/resources
	cd build/unzipfx && \
		zip -r -9 ../../Cardinal.zip Cardinal resources

deps/unzipfx/unzipfx2cat:
	make -C deps/unzipfx -f Makefile.linux

deps/unzipfx/unzipfx2cat.exe:
	make -C deps/unzipfx -f Makefile.win32

# --------------------------------------------------------------
# Clean step

clean:
	$(MAKE) distclean -C carla $(CARLA_EXTRA_ARGS) CAN_GENERATE_LV2_TTL=false STATIC_PLUGIN_TARGET=true
	$(MAKE) clean -C deps
	$(MAKE) clean -C deps/aubio
	$(MAKE) clean -C dpf/dgl
	$(MAKE) clean -C dpf/utils/lv2-ttl-generator
	$(MAKE) clean -C plugins
	$(MAKE) clean -C src
	rm -rf bin build

# --------------------------------------------------------------
# Install step

install:
	install -d $(DESTDIR)$(PREFIX)/bin
	install -d $(DESTDIR)$(PREFIX)/lib/lv2/Cardinal.lv2
	install -d $(DESTDIR)$(PREFIX)/lib/lv2/CardinalFX.lv2
	install -d $(DESTDIR)$(PREFIX)/lib/lv2/CardinalSynth.lv2
	install -d $(DESTDIR)$(PREFIX)/lib/vst/Cardinal.vst
ifeq ($(VST3_SUPPORTED),true)
	install -d $(DESTDIR)$(PREFIX)/lib/vst3/Cardinal.vst3/Contents
	install -d $(DESTDIR)$(PREFIX)/lib/vst3/CardinalFX.vst3/Contents
	install -d $(DESTDIR)$(PREFIX)/lib/vst3/CardinalSynth.vst3/Contents
endif
	install -d $(DESTDIR)$(PREFIX)/share/cardinal
	install -d $(DESTDIR)$(PREFIX)/share/doc/cardinal/docs

	install -m 644 bin/Cardinal.lv2/*.*      $(DESTDIR)$(PREFIX)/lib/lv2/Cardinal.lv2/
	install -m 644 bin/CardinalFX.lv2/*.*    $(DESTDIR)$(PREFIX)/lib/lv2/CardinalFX.lv2/
	install -m 644 bin/CardinalSynth.lv2/*.* $(DESTDIR)$(PREFIX)/lib/lv2/CardinalSynth.lv2/

	install -m 644 bin/Cardinal.vst/*.*      $(DESTDIR)$(PREFIX)/lib/vst/Cardinal.vst/

ifeq ($(VST3_SUPPORTED),true)
	cp -rL bin/Cardinal.vst3/Contents/*-*      $(DESTDIR)$(PREFIX)/lib/vst3/Cardinal.vst3/Contents/
	cp -rL bin/CardinalFX.vst3/Contents/*-*    $(DESTDIR)$(PREFIX)/lib/vst3/CardinalFX.vst3/Contents/
	cp -rL bin/CardinalSynth.vst3/Contents/*-* $(DESTDIR)$(PREFIX)/lib/vst3/CardinalSynth.vst3/Contents/
endif

	install -m 755 bin/Cardinal$(APP_EXT) $(DESTDIR)$(PREFIX)/bin/
	cp -rL bin/Cardinal.lv2/resources/* $(DESTDIR)$(PREFIX)/share/cardinal/

	install -m 644 README.md $(DESTDIR)$(PREFIX)/share/doc/cardinal/
	install -m 644 docs/*.md docs/*.png $(DESTDIR)$(PREFIX)/share/doc/cardinal/docs/

# --------------------------------------------------------------
# Tarball step, for releases

TAR_ARGS = \
	--exclude=".appveyor*" \
	--exclude=".ci*" \
	--exclude=".clang*" \
	--exclude=".drone*" \
	--exclude=".editor*" \
	--exclude=".git*" \
	--exclude="*.kdev*" \
	--exclude=".travis*" \
	--exclude=".vscode*" \
	--exclude="carla/source/modules/juce_*" \
	--exclude="carla/source/native-plugins/external/zynaddsubfx*" \
	--exclude="src/Rack/dep/osdialog/osdialog_*" \
	--exclude="src/Rack/icon.*" \
	--exclude=bin \
	--exclude=build \
	--exclude=jucewrapper \
	--exclude=lv2export \
	--exclude=carla/data \
	--exclude=carla/source/frontend \
	--exclude=carla/source/interposer \
	--exclude=carla/source/libjack \
	--exclude=carla/source/native-plugins/resources \
	--exclude=carla/source/rest \
	--exclude=carla/source/tests.old \
	--exclude=carla/source/theme \
	--exclude=carla/resources \
	--exclude=deps/PawPaw \
	--exclude=deps/sysroot \
	--exclude=deps/unzipfx \
	--exclude=docs/.generate-plugin-licenses.sh \
	--exclude=docs/MODDEVICES.md \
	--exclude=dpf/cmake \
	--exclude=dpf/examples \
	--exclude=dpf/lac \
	--exclude=dpf/tests \
	--exclude=plugins/.kdev_include_paths \
	--exclude=plugins/todo.txt \
	--exclude=plugins/AriaModules/res/Arcane \
	--exclude=plugins/AudibleInstruments/design \
	--exclude=plugins/BaconPlugs/res/midi/beeth \
	--exclude=plugins/BogaudioModules/res-pp \
	--exclude=plugins/BogaudioModules/res-src \
	--exclude=plugins/Cardinal/orig \
	--exclude=plugins/GrandeModular/res-src \
	--exclude=src/MOD \
	--exclude=src/Rack/adapters \
	--exclude=src/Rack/dep/filesystem/cmake \
	--exclude=src/Rack/dep/filesystem/examples \
	--exclude=src/Rack/dep/filesystem/test \
	--exclude=src/Rack/dep/glfw/CMake \
	--exclude=src/Rack/dep/glfw/deps \
	--exclude=src/Rack/dep/glfw/docs \
	--exclude=src/Rack/dep/glfw/examples \
	--exclude=src/Rack/dep/glfw/src \
	--exclude=src/Rack/dep/glfw/tests \
	--exclude=src/Rack/dep/nanosvg/example \
	--exclude=src/Rack/dep/nanovg \
	--exclude=src/Rack/dep/rtaudio \
	--exclude=src/Rack/include/audio.hpp \
	--exclude=src/Rack/include/midi.hpp \
	--exclude=src/Rack/include/engine/Port.hpp \
	--exclude=src/Rack/src/core \
	--exclude=src/Rack/src/asset.cpp \
	--exclude=src/Rack/src/audio.cpp \
	--exclude=src/Rack/src/common.cpp \
	--exclude=src/Rack/src/context.cpp \
	--exclude=src/Rack/src/dep.cpp \
	--exclude=src/Rack/src/discord.cpp \
	--exclude=src/Rack/src/gamepad.cpp \
	--exclude=src/Rack/src/keyboard.cpp \
	--exclude=src/Rack/src/library.cpp \
	--exclude=src/Rack/src/midi.cpp \
	--exclude=src/Rack/src/network.cpp \
	--exclude=src/Rack/src/plugin.cpp \
	--exclude=src/Rack/src/rtaudio.cpp \
	--exclude=src/Rack/src/rtmidi.cpp \
	--exclude=src/Rack/src/app/AudioDisplay.cpp \
	--exclude=src/Rack/src/app/MenuBar.cpp \
	--exclude=src/Rack/src/app/MidiDisplay.cpp \
	--exclude=src/Rack/src/app/Scene.cpp \
	--exclude=src/Rack/src/app/TipWindow.cpp \
	--exclude=src/Rack/src/engine/Engine.cpp \
	--exclude=src/Rack/src/plugin/Model.cpp \
	--exclude=src/Rack/src/window/Window.cpp \
	--exclude=src/Rack/res/Core \
	--exclude=src/Rack/res/icon.png \
	--transform='s,^\.\.,-.-.,' \
	--transform='s,^\.,cardinal-$(VERSION),' \
	--transform='s,^-\.-\.,..,' \

download:
	$(MAKE) download -C deps

tarball:
	$(MAKE) clean -C deps
	rm -f ../cardinal-$(VERSION).tar.xz
	tar -c --lzma $(TAR_ARGS) -f ../cardinal-$(VERSION).tar.xz .

tarball+deps: download
	rm -f ../cardinal+deps-$(VERSION).tar.xz
	tar -c --lzma $(TAR_ARGS) -f ../cardinal+deps-$(VERSION).tar.xz .

version:
	@echo $(VERSION)

# --------------------------------------------------------------

.PHONY: carla deps plugins
