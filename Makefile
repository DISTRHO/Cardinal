#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

include dpf/Makefile.base.mk

all: cardinal deps dgl plugins gen resources

# --------------------------------------------------------------
# Build config

PREFIX  ?= /usr/local
DESTDIR ?=
SYSDEPS ?= false

# --------------------------------------------------------------
# Check for system-wide dependencies

# HAVE_LIBARCHIVE = $(shell pkg-config --exists libarchive && echo true)
#
# libjansson.a
# libsamplerate.a
# libspeexdsp.a
# libzstd.a

# --------------------------------------------------------------

cardinal: deps dgl plugins
	$(MAKE) all -C src

deps:
	$(MAKE) all -C deps

dgl:
	$(MAKE) USE_NANOVG_FBO=true USE_RGBA=true -C dpf/dgl opengl
	# $(MAKE) opengl -C dpf/dgl USE_NANOVG_FBO=true USE_RGBA=true

plugins: deps
	$(MAKE) all -C plugins

resources: cardinal gen
	$(MAKE) resources -C plugins

ifneq ($(CROSS_COMPILING),true)
gen: cardinal dpf/utils/lv2_ttl_generator
	@$(CURDIR)/dpf/utils/generate-ttl.sh
ifeq ($(MACOS),true)
	@$(CURDIR)/dpf/utils/generate-vst-bundles.sh
endif

dpf/utils/lv2_ttl_generator:
	$(MAKE) -C dpf/utils/lv2-ttl-generator
else
gen:
endif

# --------------------------------------------------------------

clean:
	$(MAKE) clean -C deps
	$(MAKE) clean -C dpf/dgl
	$(MAKE) clean -C dpf/utils/lv2-ttl-generator
	$(MAKE) clean -C plugins
	$(MAKE) clean -C src
	rm -rf bin build

# --------------------------------------------------------------

.PHONY: deps plugins
