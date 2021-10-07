#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

include dpf/Makefile.base.mk

all: dgl plugins gen

# --------------------------------------------------------------

dgl:
	$(MAKE) -C dpf/dgl opengl

plugins: dgl
	$(MAKE) all -C plugins/CVCRack

ifneq ($(CROSS_COMPILING),true)
gen: plugins dpf/utils/lv2_ttl_generator
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
	$(MAKE) clean -C dpf/dgl
	$(MAKE) clean -C dpf/utils/lv2-ttl-generator
	$(MAKE) clean -C plugins/CVCRack
	rm -rf bin build
	rm -rf plugins/CVCRack/Rack/dep/bin
	rm -rf plugins/CVCRack/Rack/dep/include
	rm -rf plugins/CVCRack/Rack/dep/lib
	rm -rf plugins/CVCRack/Rack/dep/share
	rm -rf plugins/CVCRack/Rack/dep/glew-2.1.0
	rm -rf plugins/CVCRack/Rack/dep/jansson-2.12
	rm -rf plugins/CVCRack/Rack/dep/speexdsp-SpeexDSP-1.2rc3

# --------------------------------------------------------------

.PHONY: plugins
