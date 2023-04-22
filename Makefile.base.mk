#!/usr/bin/make -f
# Makefile for Cardinal #
# --------------------- #
# Created by falkTX
#

ifeq ($(ROOT),)
$(error invalid usage)
endif

# -----------------------------------------------------------------------------
# Import base definitions

DISTRHO_NAMESPACE = CardinalDISTRHO
DGL_NAMESPACE = CardinalDGL
NVG_DISABLE_SKIPPING_WHITESPACE = true
NVG_FONT_TEXTURE_FLAGS = NVG_IMAGE_NEAREST
USE_NANOVG_FBO = true
WASM_EXCEPTIONS = true
WINDOWS_ICON_ID = 401
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

BASE_OPTS += -fno-finite-math-only
BASE_OPTS += -fno-strict-aliasing

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
BASE_FLAGS += -Di386
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

BASE_FLAGS += -I$(abspath $(ROOT)/include/simde)

BASE_FLAGS += -I$(abspath $(ROOT)/src)
BASE_FLAGS += -I$(abspath $(ROOT)/src/Rack/include)
BASE_FLAGS += -I$(abspath $(ROOT)/src/Rack/include/dsp)
BASE_FLAGS += -I$(abspath $(ROOT)/src/Rack/dep/filesystem/include)
BASE_FLAGS += -I$(abspath $(ROOT)/src/Rack/dep/fuzzysearchdatabase/src)
BASE_FLAGS += -I$(abspath $(ROOT)/src/Rack/dep/glfw/include)
BASE_FLAGS += -I$(abspath $(ROOT)/src/Rack/dep/nanosvg/src)
BASE_FLAGS += -I$(abspath $(ROOT)/src/Rack/dep/oui-blendish)
BASE_FLAGS += -I$(abspath $(ROOT)/src/Rack/dep/pffft)

BUILD_C_FLAGS += -std=gnu11

ifneq ($(MACOS),true)
BUILD_CXX_FLAGS += -faligned-new -Wno-abi
ifeq ($(MOD_BUILD),true)
BUILD_CXX_FLAGS += -std=gnu++17
endif
endif

endif

# -----------------------------------------------------------------------------
