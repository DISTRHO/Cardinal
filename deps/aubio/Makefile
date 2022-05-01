#!/usr/bin/make -f
# Makefile for static aubio #
# ------------------------- #
# Created by falkTX
#

# --------------------------------------------------------------
# Import base definitions

USE_NANOVG_FBO = true
include ../../dpf/Makefile.base.mk

# --------------------------------------------------------------

BUILD_C_FLAGS += -DHAVE_CONFIG_H
BUILD_C_FLAGS += -I.
BUILD_C_FLAGS += -Isrc
BUILD_C_FLAGS += $(shell pkg-config --cflags fftw3f)

OBJS = \
	src/cvec.c.o \
	src/fvec.c.o \
	src/lvec.c.o \
	src/mathutils.c.o \
	src/pitch/pitch.c.o \
	src/pitch/pitchfcomb.c.o \
	src/pitch/pitchmcomb.c.o \
	src/pitch/pitchschmitt.c.o \
	src/pitch/pitchspecacf.c.o \
	src/pitch/pitchyin.c.o \
	src/pitch/pitchyinfast.c.o \
	src/pitch/pitchyinfft.c.o \
	src/spectral/fft.c.o \
	src/spectral/phasevoc.c.o \
	src/temporal/a_weighting.c.o \
	src/temporal/biquad.c.o \
	src/temporal/c_weighting.c.o \
	src/temporal/filter.c.o \
	src/temporal/resampler.c.o \
	src/utils/log.c.o \

# --------------------------------------------------------------

all: libaubio.a

clean:
	rm -f *.a src/*.d src/*.o src/*/*.d src/*/*.o

libaubio.a: $(OBJS)
	rm -f $@
	$(AR) crs $@ $^

# --------------------------------------------------------------

%.c.o: %.c
	$(CC) $< $(BUILD_C_FLAGS) -c -o $@

# --------------------------------------------------------------

-include $(OBJS:%.o=%.d)

# --------------------------------------------------------------
