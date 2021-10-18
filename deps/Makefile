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

# --------------------------------------------------------------
# Fix up cmake

SPACE =
SPACE +=

CMAKE = cmake

CMAKE += -DCMAKE_INSTALL_LIBDIR=lib
CMAKE += -DCMAKE_INSTALL_PREFIX='$(DEP_PATH)'
CMAKE += -DBUILD_SHARED_LIBS=OFF

# make sure macOS target matches ours
ifneq (,$(findstring -arch$(SPACE),$(CXXFLAGS)))
CMAKE += -DCMAKE_OSX_ARCHITECTURES='$(subst $(SPACE),;,$(subst -arch=,,$(filter -arch=%,$(subst -arch$(SPACE),-arch=,$(CXXFLAGS)))))'
endif
ifneq (,$(findstring -mmacosx-version-min=,$(CXXFLAGS)))
CMAKE += -DCMAKE_OSX_DEPLOYMENT_TARGET=$(subst -mmacosx-version-min=,,$(filter -mmacosx-version-min=%,$(CXXFLAGS)))
endif

# make sure debug/release matches
ifeq ($(DEBUG),true)
CMAKE += -DCMAKE_BUILD_TYPE=Debug
else
CMAKE += -DCMAKE_BUILD_TYPE=Release
endif

# fix cross-compilation for windows
ifeq ($(WINDOWS),true)
CMAKE += -G 'Unix Makefiles'
CMAKE += -DCMAKE_RC_COMPILER=$(subst gcc,windres,$(CC))
CMAKE += -DCMAKE_SYSTEM_NAME=Windows
endif

# --------------------------------------------------------------
# Fix up configure

CONFIGURE  = ./configure
CONFIGURE += --prefix="$(DEP_PATH)"
CONFIGURE += --host=$(TARGET_MACHINE)
CONFIGURE += --enable-static
CONFIGURE += --disable-shared
# NOTE libsamplerate wants to link against alsa, so we disable that
CONFIGURE += --disable-alsa
# NOTE speex fails to build when neon is enabled, so we disable that
CONFIGURE += --disable-neon
# NOTE libsamplerate fails with invalid host, so we force ac_cv_host
CONFIGURE += ac_cv_host=$(TARGET_MACHINE)

# --------------------------------------------------------------
# Fix up make

DEP_MAKE  = $(MAKE)
DEP_MAKE += ARCH_NAME=$(ARCH_NAME)
DEP_MAKE += CFLAGS="$(BUILD_C_FLAGS)"
DEP_MAKE += CXXFLAGS="$(BUILD_CXX_FLAGS)"
DEP_MAKE += LDFLAGS="$(LINK_FLAGS)"
DEP_MAKE += CMAKE="$(CMAKE)"
DEP_MAKE += CONFIGURE="$(CONFIGURE)"
DEP_MAKE += DEP_FLAGS="$(BASE_FLAGS)"
DEP_MAKE += DEP_MAC_SDK_FLAGS=
DEP_MAKE += MACHINE=$(MACHINE)$(MACHINE_SUFFIX)

# --------------------------------------------------------------
# VCV internal dependencies target

$(DEP_PATH)/lib/%.a:
	$(DEP_MAKE) -C $(DEP_PATH) lib/$*.a

# skip libarchive shared lib
$(DEP_PATH)/lib/libarchive.a: $(DEP_PATH)/lib/libzstd.a $(DEP_PATH)/libarchive-3.4.3/.stamp-patched

$(DEP_PATH)/lib/libarchive_static.a: $(DEP_PATH)/lib/libzstd.a $(DEP_PATH)/libarchive-3.4.3/.stamp-patched

$(DEP_PATH)/libarchive-3.4.3/.stamp-patched:
	$(DEP_MAKE) -C $(DEP_PATH) libarchive-3.4.3
	sed -i -e "238,243d" $(DEP_PATH)/libarchive-3.4.3/libarchive/CMakeLists.txt
	sed -i -e "s/TARGETS archive archive_static/TARGETS archive_static/" $(DEP_PATH)/libarchive-3.4.3/libarchive/CMakeLists.txt
	touch $@

ifeq ($(MACOS),true)
# zstd cmake is borked, see https://github.com/facebook/zstd/issues/1401
$(DEP_PATH)/lib/libzstd.a: $(DEP_PATH)/zstd-1.4.5/.stamp-patched

$(DEP_PATH)/zstd-1.4.5/.stamp-patched:
	$(DEP_MAKE) -C $(DEP_PATH) zstd-1.4.5
	sed -i -e "56,66d" $(DEP_PATH)/zstd-1.4.5/build/cmake/CMakeModules/AddZstdCompilationFlags.cmake
	touch $@
endif

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
