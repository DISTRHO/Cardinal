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
RACK_DEP_PATH = $(abspath $(ROOT)/dep/sysroot)
else
RACK_DEP_PATH = $(abspath $(ROOT)/src/Rack/dep)
endif

# -----------------------------------------------------------------------------
