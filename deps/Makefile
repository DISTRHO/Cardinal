#!/usr/bin/make -f
# Makefile for Cardinal #
# --------------------- #
# Created by falkTX
#

# --------------------------------------------------------------
# Import base definitions

DISTRHO_NAMESPACE = CardinalDISTRHO
DGL_NAMESPACE = CardinalDGL
USE_NANOVG_FBO = true
WASM_EXCEPTIONS = true
include ../dpf/Makefile.base.mk

# --------------------------------------------------------------
# Build config

ifeq ($(BSD),true)
SYSDEPS ?= true
else
SYSDEPS ?= false
endif

ifeq ($(SYSDEPS),true)
DEP_PATH = $(abspath sysroot)
else
DEP_PATH = $(abspath ../src/Rack/dep)
endif

# --------------------------------------------------------------
# custom build flags

BASE_FLAGS += -I$(abspath ../include)
BASE_FLAGS += -I$(abspath ../include/simd-compat)

ifeq ($(NOSIMD),true)
BASE_FLAGS += -DCARDINAL_NOSIMD
endif

ifeq ($(HAIKU)$(WASM),true)
BASE_FLAGS += -I$(abspath ../include/linux-compat)
endif

ifneq ($(SYSDEPS),true)
BASE_FLAGS += -DZSTDLIB_VISIBILITY=
endif

ifneq ($(HAIKU),true)
ifneq ($(WASM),true)
BASE_FLAGS += -pthread
endif
endif

ifeq ($(WINDOWS),true)
BASE_FLAGS += -D_USE_MATH_DEFINES
BASE_FLAGS += -DWIN32_LEAN_AND_MEAN
BASE_FLAGS += -D_WIN32_WINNT=0x0600
BASE_FLAGS += -I$(abspath ../include/mingw-compat)
BASE_FLAGS += -I$(abspath ../include/mingw-std-threads)
endif

BUILD_C_FLAGS += -fno-finite-math-only -fno-strict-aliasing
BUILD_CXX_FLAGS += -fno-finite-math-only -fno-strict-aliasing

# Rack code is not tested for this flag, unset it
BUILD_CXX_FLAGS += -U_GLIBCXX_ASSERTIONS -Wp,-U_GLIBCXX_ASSERTIONS

# --------------------------------------------------------------
# override VCV arch.mk stuff so we can build more architectures

ifeq ($(CPU_ARM32),true)
ARCH_NAME = arm
MACHINE = i686-the-rack
else ifeq ($(CPU_ARM64),true)
ARCH_NAME = arm64
MACHINE = x86_64-the-rack
else ifeq ($(CPU_RISCV64),true)
ARCH_NAME = riscv64
MACHINE = x86_64-the-rack
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
# Set up env to pass to cmake and configure

ENV = env
ENV += AR=$(AR)
ENV += CC=$(CC)
ENV += CXX=$(CXX)
ENV += CFLAGS='$(BUILD_C_FLAGS)'
ENV += CXXFLAGS='$(BUILD_CXX_FLAGS)'
ENV += LDFLAGS='$(LINK_FLAGS)'

# --------------------------------------------------------------
# Fix up cmake

SPACE =
SPACE +=

CMAKE  = cmake
CMAKE += -DCMAKE_INSTALL_LIBDIR=lib
CMAKE += -DCMAKE_INSTALL_PREFIX='$(DEP_PATH)'
CMAKE += -DBUILD_SHARED_LIBS=OFF

# make sure debug/release matches
ifeq ($(DEBUG),true)
CMAKE += -DCMAKE_BUILD_TYPE=Debug
else
CMAKE += -DCMAKE_BUILD_TYPE=Release
endif

# make sure macOS target matches ours
ifeq ($(MACOS),true)
ifneq (,$(findstring -arch$(SPACE),$(CXXFLAGS)))
CMAKE += -DCMAKE_OSX_ARCHITECTURES='$(subst $(SPACE),;,$(subst -arch=,,$(filter -arch=%,$(subst -arch$(SPACE),-arch=,$(CXXFLAGS)))))'
else ifeq ($(CIBUILD),true)
$(error CI build requires -arch flag on macOS)
endif
ifneq (,$(findstring -mmacosx-version-min=,$(CXXFLAGS)))
export MACOSX_DEPLOYMENT_TARGET = $(subst -mmacosx-version-min=,,$(filter -mmacosx-version-min=%,$(CXXFLAGS)))
CMAKE += -DCMAKE_OSX_DEPLOYMENT_TARGET=$(MACOSX_DEPLOYMENT_TARGET)
else ifeq ($(CIBUILD),true)
$(error CI build requires -mmacosx-version-min flag on macOS)
endif
CMAKE += -DCMAKE_OSX_SYSROOT="macosx"
endif

# fix emar and emranlib usage
ifeq ($(WASM),true)
CMAKE_AR     = $(shell which $(AR))
CMAKE_RANLIB = $(shell which $(RANLIB))
CMAKE += -DCMAKE_AR=$(CMAKE_AR)
CMAKE += -DCMAKE_C_COMPILER_AR=$(CMAKE_AR)
CMAKE += -DCMAKE_CXX_COMPILER_AR=$(CMAKE_AR)
CMAKE += -DCMAKE_RANLIB=$(CMAKE_RANLIB)
CMAKE += -DCMAKE_C_COMPILER_RANLIB=$(CMAKE_RANLIB)
CMAKE += -DCMAKE_CXX_COMPILER_RANLIB=$(CMAKE_RANLIB)
ifeq ($(shell uname -s),Darwin)
CMAKE += -DCMAKE_CROSSCOMPILING=ON
CMAKE += -DCMAKE_SYSTEM_NAME=Generic
endif
CMAKE += -DCMAKE_SKIP_COMPATIBILITY_TESTS=1
CMAKE += -DCMAKE_SIZEOF_CHAR=1
CMAKE += -DCMAKE_SIZEOF_UNSIGNED_SHORT=2
CMAKE += -DCMAKE_SIZEOF_SHORT=2
CMAKE += -DCMAKE_SIZEOF_INT=4
CMAKE += -DCMAKE_SIZEOF_UNSIGNED_LONG=4
CMAKE += -DCMAKE_SIZEOF_UNSIGNED_INT=4
CMAKE += -DCMAKE_SIZEOF_LONG=4
CMAKE += -DCMAKE_SIZEOF_VOID_P=4
CMAKE += -DCMAKE_SIZEOF_FLOAT=4
CMAKE += -DCMAKE_SIZEOF_DOUBLE=8
CMAKE += -DCMAKE_C_SIZEOF_DATA_PTR=4
CMAKE += -DCMAKE_CXX_SIZEOF_DATA_PTR=4
CMAKE += -DCMAKE_HAVE_LIMITS_H=1
CMAKE += -DCMAKE_HAVE_UNISTD_H=1
CMAKE += -DCMAKE_HAVE_PTHREAD_H=1
CMAKE += -DCMAKE_HAVE_SYS_PRCTL_H=1
CMAKE += -DCMAKE_WORDS_BIGENDIAN=0
CMAKE += -DCMAKE_DL_LIBS=''
CMAKE += -DCMAKE_C_BYTE_ORDER=LITTLE_ENDIAN
CMAKE += -DCMAKE_CXX_BYTE_ORDER=LITTLE_ENDIAN
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
DEP_MAKE += AR=$(AR)
DEP_MAKE += CC=$(CC)
DEP_MAKE += CXX=$(CXX)
DEP_MAKE += CFLAGS="$(BUILD_C_FLAGS)"
DEP_MAKE += CXXFLAGS="$(BUILD_CXX_FLAGS)"
DEP_MAKE += LDFLAGS="$(LINK_FLAGS)"
DEP_MAKE += DEP_FLAGS="$(BASE_FLAGS)"
DEP_MAKE += DEP_MAC_SDK_FLAGS=
DEP_MAKE += MACHINE=$(MACHINE)$(MACHINE_SUFFIX)
DEP_MAKE += VERBOSE=1

ifeq ($(shell uname -s),Darwin)
ifeq ($(CIBUILD),true)
DEP_MAKE += SHA256SUM="shasum5.30 -a 256"
else
DEP_MAKE += SHA256SUM="shasum5.30 -a 256"
endif
endif

DEP_MAKE2  = $(DEP_MAKE)
DEP_MAKE2 += CMAKE="$(ENV) $(CMAKE)"
DEP_MAKE2 += CONFIGURE="$(ENV) $(CONFIGURE)"

# --------------------------------------------------------------
# Rack internal dependencies target

$(DEP_PATH)/lib/%.a:
	$(DEP_MAKE2) -C $(DEP_PATH) lib/$*.a

$(DEP_PATH)/jansson-2.12:
	$(DEP_MAKE2) -C $(DEP_PATH) jansson-2.12

# libarchive: skip shared lib and ensure libzstd is enabled
$(DEP_PATH)/lib/libarchive.a: $(DEP_PATH)/lib/libzstd.a $(DEP_PATH)/libarchive-3.4.3/.stamp-patched

$(DEP_PATH)/lib/libarchive_static.a: $(DEP_PATH)/lib/libzstd.a $(DEP_PATH)/libarchive-3.4.3/.stamp-patched

$(DEP_PATH)/libarchive-3.4.3/.stamp-patched:
	$(DEP_MAKE2) -C $(DEP_PATH) libarchive-3.4.3
	sed -i -e "618,625d" $(DEP_PATH)/libarchive-3.4.3/CMakeLists.txt
	awk 'NR==616{print "  SET(HAVE_LIBZSTD 1)"}1' $(DEP_PATH)/libarchive-3.4.3/CMakeLists.txt > $(DEP_PATH)/libarchive-3.4.3/CMakeLists.txt2
	mv $(DEP_PATH)/libarchive-3.4.3/CMakeLists.txt2 $(DEP_PATH)/libarchive-3.4.3/CMakeLists.txt
	sed -i -e "238,243d" $(DEP_PATH)/libarchive-3.4.3/libarchive/CMakeLists.txt
	sed -i -e "s/TARGETS archive archive_static/TARGETS archive_static/" $(DEP_PATH)/libarchive-3.4.3/libarchive/CMakeLists.txt
	touch $@

# libsamplerate: skip tests, fails to build in some systems and are not needed or wanted anyway
$(DEP_PATH)/lib/libsamplerate.a: $(DEP_PATH)/libsamplerate-0.1.9/.stamp-patched

$(DEP_PATH)/libsamplerate-0.1.9/.stamp-patched:
	$(DEP_MAKE2) -C $(DEP_PATH) libsamplerate-0.1.9
	sed -i -e "s/src doc examples tests/src/" $(DEP_PATH)/libsamplerate-0.1.9/Makefile.in
	touch $@

# libspeexdsp: hide symbols
$(DEP_PATH)/lib/libspeexdsp.a: $(DEP_PATH)/speexdsp-SpeexDSP-1.2rc3/.stamp-patched

$(DEP_PATH)/speexdsp-SpeexDSP-1.2rc3/.stamp-patched:
	$(DEP_MAKE2) -C $(DEP_PATH) speexdsp-SpeexDSP-1.2rc3 \
		WGET="wget -c http://downloads.xiph.org/releases/speex/speexdsp-1.2rc3.tar.gz && mv speexdsp-1.2rc3.tar.gz speexdsp-SpeexDSP-1.2rc3.tgz #" \
		SHA256SUM="true" \
		UNTAR="mkdir -p speexdsp-SpeexDSP-1.2rc3 && tar -x --strip-components=1 --directory=$(DEP_PATH)/speexdsp-SpeexDSP-1.2rc3 -f"
	sed -i -e "s/#pragma GCC visibility push/#error we dont want this/" $(DEP_PATH)/speexdsp-SpeexDSP-1.2rc3/configure
	touch $@

# custom zstd build for only building static libs
$(DEP_PATH)/lib/libzstd.a: $(DEP_PATH)/zstd-1.4.5/.stamp-patched
	cd $(DEP_PATH)/zstd-1.4.5/build/cmake && $(CMAKE) -DZSTD_BUILD_STATIC=ON -DZSTD_BUILD_PROGRAMS=OFF -DZSTD_BUILD_SHARED=OFF -DZSTD_MULTITHREAD_SUPPORT=OFF .
	$(DEP_MAKE2) -C $(DEP_PATH)/zstd-1.4.5/build/cmake
	$(DEP_MAKE2) -C $(DEP_PATH)/zstd-1.4.5/build/cmake install

# zstd cmake is borked, see https://github.com/facebook/zstd/issues/1401
# zstd also fails to build on old systems, patch that too
$(DEP_PATH)/zstd-1.4.5/.stamp-patched:
	$(DEP_MAKE2) -C $(DEP_PATH) zstd-1.4.5
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
QUICKJS_MAKE_FLAGS += CONFIG_LTO=
endif

ifeq ($(WINDOWS),true)
QUICKJS_MAKE_FLAGS += CONFIG_WIN32=y
else ifeq ($(MACOS),true)
QUICKJS_MAKE_FLAGS += CONFIG_DARWIN=y
endif

$(DEP_PATH)/lib/libquickjs.a:
	$(DEP_MAKE) $(QUICKJS_MAKE_FLAGS) -C $(CURDIR)/QuickJS
	install -d $(DEP_PATH)/include
	install -d $(DEP_PATH)/lib
	install -m644 $(CURDIR)/QuickJS/libquickjs.a $@
	install -m644 $(CURDIR)/QuickJS/quickjs.h $(DEP_PATH)/include/quickjs.h

# --------------------------------------------------------------
# SurgeXT target

SURGE_DEP_PATH = $(abspath surge-build)
SURGE_SRC_PATH = $(abspath ../plugins/surgext/surge)
SURGE_NAMES = HysteresisProcessing Patch SolverType Tunings Wavetable clouds ghc plaits stmlib

SURGE_CXX_FLAGS  = $(filter-out -fsingle-precision-constant,$(filter-out -std=gnu++11,$(BUILD_CXX_FLAGS)))
SURGE_CXX_FLAGS += $(foreach n,$(SURGE_NAMES),-D$(n)=surgext$(n))

# fix JUCE build https://github.com/juce-framework/JUCE/issues/374
ifeq ($(CPU_I386),true)
SURGE_CXX_FLAGS += -D__sigemptyset=sigemptyset
endif

# unwanted in this build
SURGE_CXX_FLAGS += -DJUCE_DSP_ENABLE_SNAP_TO_ZERO=0

# SIMD must always be enabled, even in debug builds
ifeq ($(NOSIMD),true)
SURGE_CXX_FLAGS += -DCARDINAL_NOSIMD
else ifeq ($(DEBUG),true)
ifeq ($(WASM),true)
SURGE_CXX_FLAGS += -msse -msse2 -msse3 -msimd128
else ifeq ($(CPU_ARM32),true)
SURGE_CXX_FLAGS += -mfpu=neon-vfpv4 -mfloat-abi=hard
else ifeq ($(CPU_I386_OR_X86_64),true)
SURGE_CXX_FLAGS += -msse -msse2 -mfpmath=sse
endif
endif

# possibly use fftw?
# ifeq ($(shell $(PKG_CONFIG) --exists fftw3 fftw3f && echo true),true)
# SURGE_CXX_FLAGS += -DJUCE_DSP_USE_STATIC_FFTW=1
# endif

# JUCE_USE_CURL
SURGE_ENV = env \
	AR=$(AR) \
	CC=$(CC) \
	CXX=$(CXX) \
	CFLAGS='$(BUILD_C_FLAGS) -w' \
	CXXFLAGS='$(SURGE_CXX_FLAGS) -w' \
	LDFLAGS='$(LINK_FLAGS)'

SURGE_LIB = $(SURGE_DEP_PATH)/src/common/libsurge-common.a

$(SURGE_LIB): $(SURGE_DEP_PATH)/Makefile
	$(DEP_MAKE) -C $(SURGE_DEP_PATH) surge-common

$(SURGE_DEP_PATH)/Makefile: $(SURGE_SRC_PATH)/CMakeLists.txt
	mkdir -p $(SURGE_DEP_PATH)
	cd $(SURGE_DEP_PATH) && \
		$(SURGE_ENV) $(CMAKE) \
		-DSURGE_COMPILE_BLOCK_SIZE=8 \
		-DSURGE_SKIP_AIRWINDOWS=TRUE \
		-DSURGE_SKIP_JUCE_FOR_RACK=TRUE \
		-DSURGE_SKIP_LUA=TRUE \
		-DSURGE_SKIP_ODDSOUND_MTS=TRUE \
		$(SURGE_SRC_PATH)

# --------------------------------------------------------------
# Build targets

ifneq ($(NOPLUGINS),true)
TARGETS += $(SURGE_LIB)
endif

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
	$(DEP_MAKE) $(QUICKJS_MAKE_FLAGS) -C $(CURDIR)/QuickJS clean
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
	rm -rf $(SURGE_DEP_PATH)

download: \
	$(DEP_PATH)/jansson-2.12 \
	$(DEP_PATH)/libarchive-3.4.3/.stamp-patched \
	$(DEP_PATH)/libsamplerate-0.1.9/.stamp-patched \
	$(DEP_PATH)/speexdsp-SpeexDSP-1.2rc3/.stamp-patched \
	$(DEP_PATH)/zstd-1.4.5/.stamp-patched

quickjs: $(DEP_PATH)/lib/libquickjs.a
surge: $(SURGE_LIB)

# --------------------------------------------------------------
