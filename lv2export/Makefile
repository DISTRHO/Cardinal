#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

# --------------------------------------------------------------
# Build config

PREFIX  ?= /usr/local
DESTDIR ?=
SYSDEPS ?= false

# --------------------------------------------------------------
# Import base definitions

USE_NANOVG_FBO = true
include ../dpf/Makefile.base.mk

# --------------------------------------------------------------
# Build setup

BUILD_DIR = ../build/lv2export

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

BASE_FLAGS += -DHEADLESS
BASE_FLAGS += -fno-finite-math-only
BASE_FLAGS += -pthread

# might be needed later
# BASE_FLAGS += -I../include/neon-compat
# ifeq ($(SYSDEPS),true)
# BASE_FLAGS += -DCARDINAL_SYSDEPS
# BASE_FLAGS += $(shell pkg-config --cflags jansson libarchive samplerate speexdsp)
# else
# BASE_FLAGS += -DZSTDLIB_VISIBILITY=
# BASE_FLAGS += -I../src/Rack/dep/include
# endif
# BASE_FLAGS += -I../src/Rack/dep/filesystem/include
# BASE_FLAGS += -I../src/Rack/dep/fuzzysearchdatabase/src
# BASE_FLAGS += -I../src/Rack/dep/glfw/include
# BASE_FLAGS += -I../src/Rack/dep/nanosvg/src
# # BASE_FLAGS += -IRack/dep/osdialog
# BASE_FLAGS += -I../src/Rack/dep/oui-blendish
# BASE_FLAGS += -I../src/Rack/dep/pffft

ifeq ($(WINDOWS),true)
BASE_FLAGS += -D_USE_MATH_DEFINES
BASE_FLAGS += -DWIN32_LEAN_AND_MEAN
BASE_FLAGS += -I../include/mingw-compat
BASE_FLAGS += -I../include/mingw-std-threads
endif

# --------------------------------------------------------------
# lots of warnings from VCV side

BASE_FLAGS += -Wno-unused-parameter
BASE_FLAGS += -Wno-unused-variable

# --------------------------------------------------------------
# also from plugins

BASE_FLAGS += -Wno-deprecated-declarations
BASE_FLAGS += -Wno-implicit-fallthrough

ifeq ($(MACOS),true)
BASE_FLAGS += -Wno-unknown-warning-option
endif

# --------------------------------------------------------------
# stuff to include

BUILD_CXX_FLAGS += -I.
BUILD_CXX_FLAGS += -Iincludes
BUILD_CXX_FLAGS += -I../dpf/distrho
BUILD_CXX_FLAGS += -I../plugins

# --------------------------------------------------------------
# Build targets

PLUGINS = $(subst plugins/,,$(subst .cpp,,$(wildcard plugins/*.cpp)))

BINARIES   = $(PLUGINS:%=../bin/cardinal-%.lv2/plugin$(LIB_EXT))
RESOURCES  = $(PLUGINS:%=../bin/cardinal-%.lv2/manifest.ttl)
RESOURCES += $(PLUGINS:%=../bin/cardinal-%.lv2/plugin.ttl)

all: $(BINARIES) $(RESOURCES)

clean:
	rm -f $(TARGETS) $(BUILD_OBJS)

# --------------------------------------------------------------
# Build commands

../bin/cardinal-%.lv2/manifest.ttl: manifest.ttl.in
	-@mkdir -p $(shell dirname $@)
	sed -e "s/@LIB_EXT@/$(LIB_EXT)/" -e "s/@SLUG@/$*/" $< > $@

../bin/cardinal-%.lv2/plugin.ttl: ../bin/cardinal-%.lv2/plugin$(LIB_EXT)
	../dpf/utils/lv2_ttl_generator$(APP_EXT) $^ | tail -n +2 > $@

../bin/cardinal-%.lv2/plugin$(LIB_EXT): $(BUILD_DIR)/%.cpp.o
	-@mkdir -p $(shell dirname $@)
	$(SILENT)$(CXX) $< $(LINK_FLAGS) $(SHARED) -o $@

# --------------------------------------------------------------

$(BUILD_DIR)/aubileinstruments-%.cpp.o: plugins/aubileinstruments-%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -DSLUG='"aubileinstruments-$*"' \
		-DTEST \
		-I../plugins/AudibleInstruments/eurorack \
		-Wno-class-memaccess \
		-Wno-unused-local-typedefs \
		-c -o $@

$(BUILD_DIR)/%.cpp.o: plugins/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -DSLUG='"$*"' -c -o $@

# --------------------------------------------------------------

-include $(PLUGINS:%=$(BUILD_DIR)/%.cpp.d)

# --------------------------------------------------------------
