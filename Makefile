#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

include dpf/Makefile.base.mk

all: dgl plugins gen resources

# --------------------------------------------------------------

dgl:
	$(MAKE) USE_NANOVG_FBO=true USE_RGBA=true -C dpf/dgl opengl

plugins: dgl
	$(MAKE) all -C plugins/Cardinal

ifneq ($(CROSS_COMPILING),true)
gen: plugins resources dpf/utils/lv2_ttl_generator
	@$(CURDIR)/dpf/utils/generate-ttl.sh
ifeq ($(MACOS),true)
	@$(CURDIR)/dpf/utils/generate-vst-bundles.sh
endif

resources: bin/Cardinal.lv2/res

bin/Cardinal.lv2/res: plugins
	ln -sf $(CURDIR)/plugins/Cardinal/Rack/res bin/Cardinal.lv2/res

dpf/utils/lv2_ttl_generator:
	$(MAKE) -C dpf/utils/lv2-ttl-generator
else
gen:
resources:
endif

# --------------------------------------------------------------

clean:
	$(MAKE) clean -C dpf/dgl
	$(MAKE) clean -C dpf/utils/lv2-ttl-generator
	$(MAKE) clean -C plugins/Cardinal
	rm -rf bin build
	rm -rf plugins/Cardinal/Rack/dep/bin
	rm -rf plugins/Cardinal/Rack/dep/include
	rm -rf plugins/Cardinal/Rack/dep/lib
	rm -rf plugins/Cardinal/Rack/dep/share
	rm -rf plugins/Cardinal/Rack/dep/curl-7.66.0
	rm -rf plugins/Cardinal/Rack/dep/glew-2.1.0
	rm -rf plugins/Cardinal/Rack/dep/jansson-2.12
	rm -rf plugins/Cardinal/Rack/dep/libarchive-3.4.3
	rm -rf plugins/Cardinal/Rack/dep/openssl-1.1.1d
	rm -rf plugins/Cardinal/Rack/dep/speexdsp-SpeexDSP-1.2rc3
	rm -rf plugins/Cardinal/Rack/dep/zstd-1.4.5

# --------------------------------------------------------------

.PHONY: plugins
