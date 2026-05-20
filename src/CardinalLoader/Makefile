#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

NAME = CardinalLoader
CARDINAL_VARIANT = loader
include ../Makefile.cardinal.mk

ifneq ($(CARDINAL_LOADER_IO),)
BUILD_CXX_FLAGS += -DCARDINAL_LOADER_IO=$(CARDINAL_LOADER_IO)
endif

ifneq ($(CARDINAL_LOADER_PARAMS),)
BUILD_CXX_FLAGS += -DCARDINAL_LOADER_PARAMS=$(CARDINAL_LOADER_PARAMS)
endif
