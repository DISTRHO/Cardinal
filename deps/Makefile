#!/usr/bin/make -f
# Makefile for Cardinal #
# --------------------- #
# Created by falkTX
#

DEP_PATH = $(abspath ../src/Rack/dep)

# --------------------------------------------------------------
# Import base definitions

include ../dpf/Makefile.base.mk

# --------------------------------------------------------------
# custom build flags

BASE_FLAGS += -DZSTDLIB_VISIBILITY=
BASE_FLAGS += -I../include
BASE_FLAGS += -I../include/neon-compat

ifeq ($(HEADLESS),true)
ifeq ($(WITH_LTO),true)
BASE_FLAGS += -ffat-lto-objects
endif
endif

ifeq ($(WASM),true)
BASE_FLAGS += -msse -msse2 -msse3 -msimd128
else
BASE_FLAGS += -pthread
endif

ifeq ($(WINDOWS),true)
BASE_FLAGS += -D_USE_MATH_DEFINES
BASE_FLAGS += -DWIN32_LEAN_AND_MEAN
BASE_FLAGS += -I../include/mingw-compat
BASE_FLAGS += -I../include/mingw-std-threads
endif

BUILD_C_FLAGS += -fno-finite-math-only
BUILD_CXX_FLAGS += -fno-finite-math-only

# --------------------------------------------------------------
# override VCV arch.mk stuff so we can build more architectures

ifeq ($(CPU_AARCH64),true)
ARCH_NAME = aarch64
MACHINE = x86_64-the-rack
else ifeq ($(CPU_ARM64),true)
ARCH_NAME = arm64
MACHINE = x86_64-the-rack
else ifeq ($(CPU_ARM),true)
ARCH_NAME = arm
MACHINE = i686-the-rack
else ifeq ($(WASM),true)
MACHINE = i686-wasm
else
MACHINE = $(TARGET_MACHINE)
endif

ifeq ($(MACOS),true)
MACHINE_SUFFIX = -darwin
else ifeq ($(WINDOWS),true)
MACHINE_SUFFIX = -mingw32
else
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
DEP_MAKE += CC=$(CC)
DEP_MAKE += CXX=$(CXX)
DEP_MAKE += CFLAGS="$(BUILD_C_FLAGS)"
DEP_MAKE += CXXFLAGS="$(BUILD_CXX_FLAGS)"
DEP_MAKE += LDFLAGS="$(LINK_FLAGS)"
DEP_MAKE += CMAKE="$(CMAKE)"
DEP_MAKE += CONFIGURE="$(CONFIGURE)"
DEP_MAKE += DEP_FLAGS="$(BASE_FLAGS)"
DEP_MAKE += DEP_MAC_SDK_FLAGS=
DEP_MAKE += MACHINE=$(MACHINE)$(MACHINE_SUFFIX)

ifeq ($(MACOS),true)
DEP_MAKE += SHA256SUM="shasum5.28 -a 256"
endif

# --------------------------------------------------------------
# Rack internal dependencies target

$(DEP_PATH)/lib/%.a:
	$(DEP_MAKE) -C $(DEP_PATH) lib/$*.a

# libarchive: skip shared lib and ensure libzstd is enabled
$(DEP_PATH)/lib/libarchive.a: $(DEP_PATH)/lib/libzstd.a $(DEP_PATH)/libarchive-3.4.3/.stamp-patched

$(DEP_PATH)/lib/libarchive_static.a: $(DEP_PATH)/lib/libzstd.a $(DEP_PATH)/libarchive-3.4.3/.stamp-patched

$(DEP_PATH)/libarchive-3.4.3/.stamp-patched:
	$(DEP_MAKE) -C $(DEP_PATH) libarchive-3.4.3
	sed -i -e "618,625d" $(DEP_PATH)/libarchive-3.4.3/CMakeLists.txt
	awk 'NR==616{print "  SET(HAVE_LIBZSTD 1)"}1' $(DEP_PATH)/libarchive-3.4.3/CMakeLists.txt > $(DEP_PATH)/libarchive-3.4.3/CMakeLists.txt2
	mv $(DEP_PATH)/libarchive-3.4.3/CMakeLists.txt2 $(DEP_PATH)/libarchive-3.4.3/CMakeLists.txt
	sed -i -e "238,243d" $(DEP_PATH)/libarchive-3.4.3/libarchive/CMakeLists.txt
	sed -i -e "s/TARGETS archive archive_static/TARGETS archive_static/" $(DEP_PATH)/libarchive-3.4.3/libarchive/CMakeLists.txt
	touch $@

# libsamplerate: skip tests, fails to build in some systems and are not needed or wanted anyway
$(DEP_PATH)/lib/libsamplerate.a: $(DEP_PATH)/libsamplerate-0.1.9/.stamp-patched

$(DEP_PATH)/libsamplerate-0.1.9/.stamp-patched:
	$(DEP_MAKE) -C $(DEP_PATH) libsamplerate-0.1.9
	sed -i -e "s/src doc examples tests/src/" $(DEP_PATH)/libsamplerate-0.1.9/Makefile.in
	touch $@

# libspeexdsp: hide symbols
$(DEP_PATH)/lib/libspeexdsp.a: $(DEP_PATH)/speexdsp-SpeexDSP-1.2rc3/.stamp-patched

$(DEP_PATH)/speexdsp-SpeexDSP-1.2rc3/.stamp-patched:
	$(DEP_MAKE) -C $(DEP_PATH) speexdsp-SpeexDSP-1.2rc3
	sed -i -e "s/#pragma GCC visibility push/#error we dont want this/" $(DEP_PATH)/speexdsp-SpeexDSP-1.2rc3/configure
	touch $@

# zstd cmake is borked, see https://github.com/facebook/zstd/issues/1401
# zstd also fails to build on old systems, patch that too
$(DEP_PATH)/lib/libzstd.a: $(DEP_PATH)/zstd-1.4.5/.stamp-patched

$(DEP_PATH)/zstd-1.4.5/.stamp-patched:
	$(DEP_MAKE) -C $(DEP_PATH) zstd-1.4.5
	sed -i -e "56,66d" $(DEP_PATH)/zstd-1.4.5/build/cmake/CMakeModules/AddZstdCompilationFlags.cmake
	sed -i -e "146,175d" $(DEP_PATH)/zstd-1.4.5/programs/util.c
	sed -i -e "142,144d" $(DEP_PATH)/zstd-1.4.5/programs/util.c
	touch $@

# --------------------------------------------------------------
# QuickJS target, needed for AriaModules

QUICKJS_MAKE_FLAGS  = CFLAGS="$(BUILD_C_FLAGS) -D_GNU_SOURCE -DCONFIG_VERSION='\"Cardinal\"' -w"
QUICKJS_MAKE_FLAGS += PROGS=libquickjs.a

ifeq ($(WITH_LTO),true)
QUICKJS_MAKE_FLAGS += CONFIG_LTO=y
else
QUICKJS_MAKE_FLAGS += CONFIG_LTO=n
endif

ifeq ($(WINDOWS),true)
QUICKJS_MAKE_FLAGS += CONFIG_WIN32=y
else ifeq ($(MACOS),true)
QUICKJS_MAKE_FLAGS += CONFIG_DARWIN=y
endif

$(DEP_PATH)/lib/libquickjs.a: $(DEP_PATH)/QuickJS
	$(DEP_MAKE) $(QUICKJS_MAKE_FLAGS) -C $(DEP_PATH)/QuickJS
	install -d $(DEP_PATH)/include
	install -d $(DEP_PATH)/lib
	install -m644 $(DEP_PATH)/QuickJS/libquickjs.a $@
	install -m644 $(DEP_PATH)/QuickJS/quickjs.h $(DEP_PATH)/include/quickjs.h

$(DEP_PATH)/QuickJS:
	git clone "https://github.com/JerrySievert/QuickJS.git" $(DEP_PATH)/QuickJS
	git -C $(DEP_PATH)/QuickJS checkout b70d5344013836544631c361ae20569b978176c9

# --------------------------------------------------------------
# Build targets

TARGETS += $(DEP_PATH)/lib/libjansson.a
TARGETS += $(DEP_PATH)/lib/libquickjs.a
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
	rm -f $(DEP_PATH)/*.tgz
	rm -f $(DEP_PATH)/*.tar.gz
	rm -rf $(DEP_PATH)/bin
	rm -rf $(DEP_PATH)/include
	rm -rf $(DEP_PATH)/lib
	rm -rf $(DEP_PATH)/share
	rm -rf $(DEP_PATH)/jansson-2.12
	rm -rf $(DEP_PATH)/libarchive-3.4.3
	rm -rf $(DEP_PATH)/libsamplerate-0.1.9
	rm -rf $(DEP_PATH)/speexdsp-SpeexDSP-1.2rc3
	rm -rf $(DEP_PATH)/zstd-1.4.5
	rm -rf $(DEP_PATH)/QuickJS

quickjs: $(DEP_PATH)/lib/libquickjs.a

# --------------------------------------------------------------
