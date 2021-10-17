#!/usr/bin/make -f
# Makefile for Cardinal #
# --------------------- #
# Created by falkTX
#

DEP_PATH = $(abspath ../src/Rack/dep)
OBJCOPY ?= objcopy

# --------------------------------------------------------------
# Import base definitions

USE_NANOVG_FBO = true
USE_RGBA = true
include ../dpf/Makefile.base.mk

# --------------------------------------------------------------
# Files to build

PLUGIN_FILES  = plugins.cpp

# Befaco
PLUGIN_FILES += $(filter-out Befaco/src/plugin.cpp,$(wildcard Befaco/src/*.cpp))
PLUGIN_BINARIES += Befaco/src/SpringReverbIR.pcm

# Fundamental
PLUGIN_FILES += $(filter-out Fundamental/src/plugin.cpp,$(wildcard Fundamental/src/*.cpp))

# --------------------------------------------------------------
# Resources to symlink

# TODO
# PLUGIN_RESOURCES = res/Befaco/res
# PLUGIN_RESOURCES = res/Befaco/res

# --------------------------------------------------------------
# Build setup

BUILD_DIR = ../build/plugins

PLUGIN_OBJS  = $(PLUGIN_FILES:%=$(BUILD_DIR)/%.o)
PLUGIN_OBJS += $(PLUGIN_BINARIES:%=$(BUILD_DIR)/%.bin.o)

ifeq ($(MACOS),true)
BASE_FLAGS += -DARCH_MAC
else ifeq ($(WINDOWS),true)
BASE_FLAGS += -DARCH_WIN
else
BASE_FLAGS += -DARCH_LIN
endif

BUILD_C_FLAGS += -std=gnu11

BASE_FLAGS += -I../dpf/dgl/src/nanovg
BASE_FLAGS += -I../dpf/distrho
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
BASE_FLAGS += -D_USE_MATH_DEFINES
BASE_FLAGS += -Imingw-compat
BASE_FLAGS += -Imingw-std-threads
BASE_FLAGS += -municode
endif

# --------------------------------------------------------------
# FIXME lots of warnings from VCV side

BASE_FLAGS += -Wno-unused-parameter
BASE_FLAGS += -Wno-unused-variable

# also lots of plugins not updated to v2 yet
BASE_FLAGS += -Wno-deprecated-declarations

# --------------------------------------------------------------
# temporary macro just to get the ball rolling

ifeq ($(EXE_WRAPPER),wine)
PLUGINS_DIR = Z:$(subst /,\\,$(CURDIR))
else
PLUGINS_DIR = $(CURDIR)
endif

BUILD_CXX_FLAGS += -DCARDINAL_PLUGINS_DIR='"$(PLUGINS_DIR)"'

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
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@

$(BUILD_DIR)/Befaco/%.cpp.o: Befaco/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -DpluginInstance=pluginInstance__Befaco -c -o $@

$(BUILD_DIR)/Fundamental/%.cpp.o: Fundamental/%.cpp
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Compiling $<"
	$(SILENT)$(CXX) $< $(BUILD_CXX_FLAGS) -DpluginInstance=pluginInstance__Fundamental -c -o $@

# copied and adjusted from VCVRack's compile.mk
$(BUILD_DIR)/%.bin.o: % res2c.py
	-@mkdir -p "$(shell dirname $(BUILD_DIR)/$<)"
	@echo "Generating and Compiling $<"
	$(shell ./res2c.py $< | $(CC) -x c - $(BUILD_C_FLAGS) -c -o $@)

# --------------------------------------------------------------

-include $(PLUGIN_OBJS:%.o=%.d)

# --------------------------------------------------------------
