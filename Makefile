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

ifeq ($(SYSDEPS),true)

ifneq ($(shell pkg-config --exists jansson && echo true),true)
$(error jansson dpendency not installed/available)
endif
ifneq ($(shell pkg-config --exists libarchive && echo true),true)
$(error libarchive dpendency not installed/available)
endif
ifneq ($(shell pkg-config --exists samplerate && echo true),true)
$(error samplerate dpendency not installed/available)
endif
ifneq ($(shell pkg-config --exists speexdsp && echo true),true)
$(error speexdsp dpendency not installed/available)
endif

endif

# --------------------------------------------------------------

cardinal: deps dgl plugins
	$(MAKE) all -C src

deps:
ifneq ($(SYSDEPS),true)
	$(MAKE) all -C deps
endif

dgl:
	$(MAKE) USE_NANOVG_FBO=true -C dpf/dgl opengl
	# $(MAKE) opengl -C dpf/dgl USE_NANOVG_FBO=true

plugins: deps
	$(MAKE) all -C plugins

resources: cardinal
	$(MAKE) resources -C plugins

ifneq ($(CROSS_COMPILING),true)
gen: cardinal resources dpf/utils/lv2_ttl_generator
	@$(CURDIR)/dpf/utils/generate-ttl.sh

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

install:
	install -d $(DESTDIR)$(PREFIX)/bin
	install -d $(DESTDIR)$(PREFIX)/lib/lv2
	install -d $(DESTDIR)$(PREFIX)/lib/vst
	install -d $(DESTDIR)$(PREFIX)/lib/vst3
	install -d $(DESTDIR)$(PREFIX)/share/Cardinal

	cp -rL bin/Cardinal.lv2  $(DESTDIR)$(PREFIX)/lib/lv2/
	cp -rL bin/Cardinal.vst  $(DESTDIR)$(PREFIX)/lib/vst/
	cp -rL bin/Cardinal.vst3 $(DESTDIR)$(PREFIX)/lib/vst3/

	install -m 755 bin/Cardinal$(APP_EXT) $(DESTDIR)$(PREFIX)/bin/
	cp -rL bin/Cardinal.lv2/resources/* $(DESTDIR)$(PREFIX)/share/Cardinal/

# --------------------------------------------------------------

.PHONY: deps plugins
