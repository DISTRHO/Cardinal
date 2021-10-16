#!/usr/bin/make -f
# Makefile for Cardinal #
# --------------------- #
# Created by falkTX
#

DEP_PATH = $(abspath ../src/Rack/dep)

# --------------------------------------------------------------
# Import base definitions

USE_NANOVG_FBO = true
USE_RGBA = true
include ../dpf/Makefile.base.mk

# --------------------------------------------------------------
# Files to build

PLUGIN_FILES  = plugins.cpp

# Fundamental
PLUGIN_FILES += $(filter-out Fundamental/src/plugin.cpp,$(wildcard Fundamental/src/*.cpp))

# --------------------------------------------------------------
# Build setup

BUILD_DIR = ../build/plugins

PLUGIN_OBJS = $(PLUGIN_FILES:%=$(BUILD_DIR)/%.o)

ifeq ($(MACOS),true)
BASE_FLAGS += -DARCH_MAC
else ifeq ($(WINDOWS),true)
BASE_FLAGS += -DARCH_WIN
else
BASE_FLAGS += -DARCH_LIN
endif

BUILD_C_FLAGS += -std=gnu11

BASE_FLAGS += -I../dpf/dgl/src/nanovg
BASE_FLAGS += -I../src
BASE_FLAGS += -I../src/neon-compat
BASE_FLAGS += -I../src/Rack/include
BASE_FLAGS += -I../src/Rack/dep/include
# BASE_FLAGS += -I../src/Rack/dep/filesystem/include
# BASE_FLAGS += -I../src/Rack/dep/fuzzysearchdatabase/src
BASE_FLAGS += -I../src/Rack/dep/glfw/include
BASE_FLAGS += -I../src/Rack/dep/nanosvg/src
# BASE_FLAGS += -I../src/Rack/dep/osdialog
BASE_FLAGS += -I../src/Rack/dep/oui-blendish
BASE_FLAGS += -I../src/Rack/dep/pffft
BASE_FLAGS += -pthread

ifeq ($(WINDOWS),true)
BASE_FLAGS += -Imingw-compat
BASE_FLAGS += -Imingw-std-threads
endif

# --------------------------------------------------------------
# FIXME lots of warnings from VCV side

BASE_FLAGS += -Wno-unused-parameter
BASE_FLAGS += -Wno-unused-variable

# --------------------------------------------------------------
# Build targets

TARGET = plugins.a

all: $(TARGET)

clean:
	rm -f $(TARGET)
	rm -rf $(BUILD_DIR)

# --------------------------------------------------------------
# Build commands

$(TARGET): $(PLUGIN_OBJS)
	@echo "Creating $@"
	$(SILENT)rm -f $@
	$(SILENT)$(AR) crs $@ $^

$(BUILD_DIR)/plugins.cpp.o: plugins.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -DpluginInstance=pluginInstance__Fundamental -c -o $@

$(BUILD_DIR)/Fundamental/%.cpp.o: Fundamental/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -DpluginInstance=pluginInstance__Fundamental -c -o $@

# --------------------------------------------------------------

-include $(PLUGIN_OBJS:%.o=%.d)

# --------------------------------------------------------------
