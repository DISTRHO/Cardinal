#!/usr/bin/make -f
# Makefile for Cardinal #
# --------------------- #
# Created by falkTX
#

DEP_PATH = $(abspath ../src/Rack/dep)

# --------------------------------------------------------------
# Import base definitions

USE_NANOVG_FBO = true
USE_RGBA = true
include ../dpf/Makefile.base.mk

# --------------------------------------------------------------
# override VCV arch.mk stuff so we can build more architectures

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

# NOTE speex fails to build when neon is enabled
CONFIGURE = ./configure --prefix="$(DEP_PATH)" --host=$(TARGET_MACHINE) --enable-neon=no

# --------------------------------------------------------------
# Fix up cmake

SPACE =
SPACE +=

CMAKE = cmake

ifneq (,$(findstring -arch$(SPACE),$(CXXFLAGS)))
CMAKE += -DCMAKE_OSX_ARCHITECTURES='$(subst $(SPACE),;,$(subst -arch=,,$(filter -arch=%,$(subst -arch$(SPACE),-arch=,$(CXXFLAGS)))))'
endif
ifneq (,$(findstring -mmacosx-version-min=,$(CXXFLAGS)))
CMAKE += -DCMAKE_OSX_DEPLOYMENT_TARGET=$(subst -mmacosx-version-min=,,$(filter -mmacosx-version-min=%,$(CXXFLAGS)))
endif

ifeq ($(WINDOWS),true)
CMAKE += -G 'Unix Makefiles'
CMAKE += -DCMAKE_RC_COMPILER=$(subst gcc,windres,$(CC))
CMAKE += -DCMAKE_SYSTEM_NAME=Windows
endif

ifeq ($(DEBUG),true)
CMAKE += -DCMAKE_BUILD_TYPE=Debug
else
CMAKE += -DCMAKE_BUILD_TYPE=Release
endif

CMAKE += -DCMAKE_INSTALL_LIBDIR=lib -DCMAKE_INSTALL_PREFIX='$(DEP_PATH)'

# --------------------------------------------------------------
# VCV internal dependencies target

$(DEP_PATH)/lib/%.a:
	$(MAKE) \
		ARCH_NAME=$(ARCH_NAME) \
		CFLAGS="$(BUILD_C_FLAGS)" \
		CXXFLAGS="$(BUILD_CXX_FLAGS)" \
		LDFLAGS="$(LINK_FLAGS)" \
		CMAKE="$(CMAKE)" \
		CONFIGURE="$(CONFIGURE)" \
		DEP_FLAGS="$(BASE_FLAGS)" \
		DEP_MAC_SDK_FLAGS= \
		MACHINE=$(MACHINE)$(MACHINE_SUFFIX) \
		-C $(DEP_PATH) lib/$*.a

$(DEP_PATH)/lib/libarchive.a: $(DEP_PATH)/lib/libzstd.a

$(DEP_PATH)/lib/libarchive_static.a: $(DEP_PATH)/lib/libzstd.a

ifeq ($(MACOS),true)
# zstd cmake is borked, see https://github.com/facebook/zstd/issues/1401
$(DEP_PATH)/lib/libzstd.a: $(DEP_PATH)/zstd-1.4.5/.stamp-patched

$(DEP_PATH)/zstd-1.4.5/.stamp-patched:
	$(MAKE) -C $(DEP_PATH) zstd-1.4.5
	sed -i -e "56,66d" $(DEP_PATH)/zstd-1.4.5/build/cmake/CMakeModules/AddZstdCompilationFlags.cmake
	touch $@
endif

# libsamplerate fails to link its tests in some systems
$(DEP_PATH)/libsamplerate-0.1.9:
	$(MAKE) -C $(DEP_PATH) libsamplerate-0.1.9

$(DEP_PATH)/lib/libsamplerate.a: $(DEP_PATH)/libsamplerate-0.1.9
	cd $(DEP_PATH)/libsamplerate-0.1.9 && $(CONFIGURE) --disable-fftw --disable-sndfile
	touch $(DEP_PATH)/libsamplerate-0.1.9/tests/termination_test
	touch $(DEP_PATH)/libsamplerate-0.1.9/tests/simple_test
	touch $(DEP_PATH)/libsamplerate-0.1.9/tests/misc_test
	touch $(DEP_PATH)/libsamplerate-0.1.9/tests/callback_test
	touch $(DEP_PATH)/libsamplerate-0.1.9/tests/reset_test
	touch $(DEP_PATH)/libsamplerate-0.1.9/tests/varispeed_test
	touch $(DEP_PATH)/libsamplerate-0.1.9/tests/float_short_test
	touch $(DEP_PATH)/libsamplerate-0.1.9/tests/multi_channel_test
	touch $(DEP_PATH)/libsamplerate-0.1.9/tests/callback_hang_test
	touch $(DEP_PATH)/libsamplerate-0.1.9/tests/src-evaluate
	touch $(DEP_PATH)/libsamplerate-0.1.9/tests/throughput_test
	touch $(DEP_PATH)/libsamplerate-0.1.9/tests/multichan_throughput_test
	touch $(DEP_PATH)/libsamplerate-0.1.9/tests/downsample_test
	$(MAKE) -C $(DEP_PATH)/libsamplerate-0.1.9
	$(MAKE) -C $(DEP_PATH)/libsamplerate-0.1.9 install

# --------------------------------------------------------------
# Build targets

TARGETS += $(DEP_PATH)/lib/libjansson.a
TARGETS += $(DEP_PATH)/lib/libsamplerate.a
TARGETS += $(DEP_PATH)/lib/libspeexdsp.a

ifeq ($(WINDOWS),true)
TARGETS += $(DEP_PATH)/lib/libarchive_static.a
else
TARGETS += $(DEP_PATH)/lib/libarchive.a
endif

TARGETS += $(DEP_PATH)/lib/libzstd.a

all: $(TARGETS)

clean:
	rm -f $(TARGETS)
	rm -rf $(DEP_PATH)/bin
	rm -rf $(DEP_PATH)/include
	rm -rf $(DEP_PATH)/lib
	rm -rf $(DEP_PATH)/share
	rm -rf $(DEP_PATH)/jansson-2.12
	rm -rf $(DEP_PATH)/libarchive-3.4.3
	rm -rf $(DEP_PATH)/libsamplerate-0.1.9
	rm -rf $(DEP_PATH)/speexdsp-SpeexDSP-1.2rc3
	rm -rf $(DEP_PATH)/zstd-1.4.5

# --------------------------------------------------------------
