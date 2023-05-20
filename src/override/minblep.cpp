/*
 * DISTRHO Cardinal Plugin
 * Copyright (C) 2021-2023 Filipe Coelho <falktx@falktx.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * For a full copy of the GNU General Public License see the LICENSE file.
 */

/**
 * This file is an edited version of VCVRack's dsp/minblep.cpp
 * Copyright (C) 2016-2023 VCV.
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 */

#include <dsp/minblep.hpp>
#include <dsp/fft.hpp>
#include <dsp/window.hpp>


namespace rack {
namespace dsp {


void minBlepImpulse(int z, int o, float* output) {
	// Symmetric sinc array with `z` zero-crossings on each side
	int n = 2 * z * o;
	float* x = (float*) pffft_aligned_malloc(sizeof(float) * n);
	for (int i = 0; i < n; i++) {
		float p = math::rescale((float) i, 0.f, (float)(n - 1), (float) - z, (float) z);
		x[i] = sinc(p);
	}

	// Apply window
	blackmanHarrisWindow(x, n);

	// Real cepstrum
	float* fx = (float*) pffft_aligned_malloc(sizeof(float) * 2 * n);
	// Valgrind complains that the array is uninitialized for some reason, unless we clear it.
	std::memset(fx, 0, sizeof(float) * 2 * n);
	RealFFT rfft(n);
	rfft.rfft(x, fx);
	// fx = log(abs(fx))
	fx[0] = std::log(std::fabs(fx[0]));
	for (int i = 1; i < n; i++) {
		fx[2 * i] = std::log(std::hypot(fx[2 * i], fx[2 * i + 1]));
		fx[2 * i + 1] = 0.f;
	}
	fx[1] = std::log(std::fabs(fx[1]));
	// Clamp values in case we have -inf
	for (int i = 0; i < 2 * n; i++) {
		fx[i] = std::fmax(-30.f, fx[i]);
	}
	rfft.irfft(fx, x);
	rfft.scale(x);

	// Minimum-phase reconstruction
	for (int i = 1; i < n / 2; i++) {
		x[i] *= 2.f;
	}
	for (int i = (n + 1) / 2; i < n; i++) {
		x[i] = 0.f;
	}
	rfft.rfft(x, fx);
	// fx = exp(fx)
	fx[0] = std::exp(fx[0]);
	for (int i = 1; i < n; i++) {
		float re = std::exp(fx[2 * i]);
		float im = fx[2 * i + 1];
		fx[2 * i] = re * std::cos(im);
		fx[2 * i + 1] = re * std::sin(im);
	}
	fx[1] = std::exp(fx[1]);
	rfft.irfft(fx, x);
	rfft.scale(x);

	// Integrate
	float total = 0.f;
	for (int i = 0; i < n; i++) {
		total += x[i];
		x[i] = total;
	}

	// Normalize
	float norm = 1.f / x[n - 1];
	for (int i = 0; i < n; i++) {
		x[i] *= norm;
	}

	std::memcpy(output, x, n * sizeof(float));

	// Cleanup
	pffft_aligned_free(x);
	pffft_aligned_free(fx);
}


} // namespace dsp
} // namespace rack
