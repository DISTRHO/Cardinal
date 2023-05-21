#!/usr/bin/make -f
# Makefile for Cardinal #
# --------------------- #
# Created by falkTX
#

ifeq ($(ROOT),)
$(error invalid usage)
endif

ifeq ($(NOSIMD),true)
ifneq (,$(findstring -msse,$(CXXFLAGS)))
$(error NOSIMD build requested but -msse compiler flag is present in CXXFLAGS)
endif
endif

# -----------------------------------------------------------------------------
# Import base definitions

export DISTRHO_NAMESPACE = CardinalDISTRHO
export DGL_NAMESPACE = CardinalDGL
export NVG_DISABLE_SKIPPING_WHITESPACE = true
export NVG_FONT_TEXTURE_FLAGS = NVG_IMAGE_NEAREST
export USE_NANOVG_FBO = true
export WASM_EXCEPTIONS = true
export WINDOWS_ICON_ID = 401
include $(ROOT)/dpf/Makefile.base.mk

DGL_EXTRA_ARGS = \
	DISTRHO_NAMESPACE=$(DISTRHO_NAMESPACE) \
	DGL_NAMESPACE=$(DGL_NAMESPACE) \
	NVG_DISABLE_SKIPPING_WHITESPACE=$(NVG_DISABLE_SKIPPING_WHITESPACE) \
	NVG_FONT_TEXTURE_FLAGS=$(NVG_FONT_TEXTURE_FLAGS) \
	USE_NANOVG_FBO=$(USE_NANOVG_FBO) \
	WASM_EXCEPTIONS=$(WASM_EXCEPTIONS) \
	WINDOWS_ICON_ID=$(WINDOWS_ICON_ID)

# -----------------------------------------------------------------------------
# Build config

ifeq ($(BSD),true)
SYSDEPS ?= true
else
SYSDEPS ?= false
endif

ifeq ($(SYSDEPS),true)
RACK_DEP_PATH = $(abspath $(ROOT)/deps/sysroot)
else
RACK_DEP_PATH = $(abspath $(ROOT)/src/Rack/dep)
endif

# -----------------------------------------------------------------------------
# Custom build flags

BASE_FLAGS += -I$(abspath $(ROOT)/include)
BASE_FLAGS += -I$(abspath $(ROOT)/include/simd-compat)
BASE_FLAGS += -I$(RACK_DEP_PATH)/include

ifeq ($(MOD_BUILD),true)
BASE_FLAGS += -DSIMDE_ENABLE_OPENMP -fopenmp
LINK_FLAGS += -fopenmp
endif

ifeq ($(NOSIMD),true)
BASE_FLAGS += -DCARDINAL_NOSIMD
endif

ifeq ($(SYSDEPS),true)
BASE_FLAGS += -DCARDINAL_SYSDEPS
BASE_FLAGS += $(shell $(PKG_CONFIG) --cflags jansson libarchive samplerate speexdsp)
else
BASE_FLAGS += -DZSTDLIB_VISIBILITY=
endif

ifeq ($(BSD)$(HAIKU),true)
BASE_FLAGS += -DCLOCK_MONOTONIC_RAW=CLOCK_MONOTONIC
endif

ifeq ($(HAIKU)$(WASM),true)
BASE_FLAGS += -I$(abspath $(ROOT)/include/linux-compat)
else
BASE_FLAGS += -pthread
endif

ifeq ($(WINDOWS),true)
BASE_FLAGS += -D_USE_MATH_DEFINES
BASE_FLAGS += -DWIN32_LEAN_AND_MEAN
BASE_FLAGS += -D_WIN32_WINNT=0x0600
BASE_FLAGS += -I$(abspath $(ROOT)/include/mingw-compat)
endif

# make sure these flags always end up last
BUILD_C_FLAGS += -fno-finite-math-only -fno-strict-aliasing
BUILD_CXX_FLAGS += -fno-finite-math-only -fno-strict-aliasing

# -----------------------------------------------------------------------------
# simde flags

BASE_FLAGS += -I$(abspath $(ROOT)/src/Rack/dep/simde)
BASE_FLAGS += -DSIMDE_ACCURACY_PREFERENCE=0
BASE_FLAGS += -DSIMDE_FAST_CONVERSION_RANGE
BASE_FLAGS += -DSIMDE_FAST_MATH
BASE_FLAGS += -DSIMDE_FAST_NANS
BASE_FLAGS += -DSIMDE_FAST_ROUND_MODE
BASE_FLAGS += -DSIMDE_FAST_ROUND_TIES

# unwanted
BASE_FLAGS += -DSIMDE_X86_SSE4_1_H
BASE_FLAGS += -DSIMDE_X86_SSE4_2_H

# -----------------------------------------------------------------------------
# Rack build flags

ifeq ($(BUILDING_RACK),true)

# Rack code is not tested for this flag, unset it
BUILD_CXX_FLAGS += -U_GLIBCXX_ASSERTIONS -Wp,-U_GLIBCXX_ASSERTIONS

# Ignore bad behaviour from Rack API
BUILD_CXX_FLAGS += -Wno-format-security

# Ignore warnings from simde
ifeq ($(MOD_BUILD),true)
BUILD_CXX_FLAGS += -Wno-overflow
endif

# lots of warnings from VCV side
BASE_FLAGS += -Wno-unused-parameter
BASE_FLAGS += -Wno-unused-variable

ifeq ($(CPU_ARM_OR_ARM64)$(CPU_RISCV64),true)
BASE_FLAGS += -Wno-attributes
endif

ifeq ($(MACOS),true)
BASE_FLAGS += -DARCH_MAC
else ifeq ($(WINDOWS),true)
BASE_FLAGS += -DARCH_WIN
else
BASE_FLAGS += -DARCH_LIN
endif

ifeq ($(DEBUG),true)
BASE_FLAGS += -UDEBUG
endif

ifeq ($(HEADLESS),true)
BASE_FLAGS += -DHEADLESS
endif

ifeq ($(USE_GLES2),true)
BASE_FLAGS += -DNANOVG_GLES2_FORCED
else ifeq ($(USE_GLES3),true)
BASE_FLAGS += -DNANOVG_GLES3_FORCED
endif

# needed for enabling SSE in pffft
ifeq ($(CPU_I386),true)
ifneq ($(NOSIMD),true)
BASE_FLAGS += -Di386
endif
endif

# SIMD must always be enabled, even in debug builds
ifneq ($(NOSIMD),true)
ifeq ($(DEBUG),true)

ifeq ($(WASM),true)
BASE_FLAGS += -msse -msse2 -msse3 -msimd128
else ifeq ($(CPU_ARM32),true)
BASE_FLAGS += -mfpu=neon-vfpv4 -mfloat-abi=hard
else ifeq ($(CPU_I386_OR_X86_64),true)
BASE_FLAGS += -msse -msse2 -mfpmath=sse
endif

endif
endif

BASE_FLAGS += -I$(abspath $(ROOT)/dpf/dgl/src/nanovg)
BASE_FLAGS += -I$(abspath $(ROOT)/dpf/distrho)

BASE_FLAGS += -I$(abspath $(ROOT)/src)
BASE_FLAGS += -I$(abspath $(ROOT)/src/Rack/include)
BASE_FLAGS += -I$(abspath $(ROOT)/src/Rack/include/dsp)
BASE_FLAGS += -I$(abspath $(ROOT)/src/Rack/dep/filesystem/include)
BASE_FLAGS += -I$(abspath $(ROOT)/src/Rack/dep/fuzzysearchdatabase/src)
BASE_FLAGS += -I$(abspath $(ROOT)/src/Rack/dep/glfw/include)
BASE_FLAGS += -I$(abspath $(ROOT)/src/Rack/dep/nanosvg/src)
BASE_FLAGS += -I$(abspath $(ROOT)/src/Rack/dep/oui-blendish)
BASE_FLAGS += -I$(abspath $(ROOT)/src/Rack/dep/pffft)
BASE_FLAGS += -I$(abspath $(ROOT)/src/Rack/dep/tinyexpr)

BUILD_C_FLAGS += -std=gnu11

ifneq ($(MACOS),true)
BUILD_CXX_FLAGS += -faligned-new -Wno-abi
ifeq ($(MOD_BUILD),true)
BUILD_CXX_FLAGS += -std=gnu++17
endif
endif

endif

# -----------------------------------------------------------------------------
