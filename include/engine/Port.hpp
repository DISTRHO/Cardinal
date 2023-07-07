/*
 * DISTRHO Cardinal Plugin
 * Copyright (C) 2021-2022 Filipe Coelho <falktx@falktx.com>
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
 * This file is an edited version of VCVRack's engine/Port.hpp
 * Copyright (C) 2016-2021 VCV.
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 */

#pragma once

#include <common.hpp>
#include <engine/Light.hpp>

#include <list>

/** NOTE alignas is required in some systems in order to allow SSE usage. */
#define SIMD_ALIGN alignas(16)


namespace rack {
namespace engine {


/** This is inspired by the number of MIDI channels. */
static constexpr const int PORT_MAX_CHANNELS = 16;


struct Cable;


struct Port {
	/** Voltage of the port. */
	/** NOTE alignas is required in order to allow SSE usage.
	Consecutive data (like in a vector) would otherwise pack Ports in a way that breaks SSE. */
	union SIMD_ALIGN {
		/** Unstable API. Use getVoltage() and setVoltage() instead. */
		float voltages[PORT_MAX_CHANNELS] = {};
		/** DEPRECATED. Unstable API. Use getVoltage() and setVoltage() instead. */
		float value;
	};
	union {
		/** Number of polyphonic channels.
		DEPRECATED. Unstable API. Use set/getChannels() instead.
		May be 0 to PORT_MAX_CHANNELS.
		0 channels means disconnected.
		*/
		uint8_t channels = 0;
		/** DEPRECATED. Unstable API. Use isConnected() instead. */
		uint8_t active;
	};
	/** For rendering plug lights on cables.
	Green for positive, red for negative, and blue for polyphonic.
	*/
	Light plugLights[3];

	enum Type {
		INPUT,
		OUTPUT,
	};

	/** Sets the voltage of the given channel. */
	void setVoltage(float voltage, int channel = 0) {
		voltages[channel] = voltage;
	}

	/** Returns the voltage of the given channel.
	Because of proper bookkeeping, all channels higher than the input port's number of channels should be 0V.
	*/
	float getVoltage(int channel = 0) {
		return voltages[channel];
	}

	/** Returns the given channel's voltage if the port is polyphonic, otherwise returns the first voltage (channel 0). */
	float getPolyVoltage(int channel) {
		return isMonophonic() ? getVoltage(0) : getVoltage(channel);
	}

	/** Returns the voltage if a cable is connected, otherwise returns the given normal voltage. */
	float getNormalVoltage(float normalVoltage, int channel = 0) {
		return isConnected() ? getVoltage(channel) : normalVoltage;
	}

	float getNormalPolyVoltage(float normalVoltage, int channel) {
		return isConnected() ? getPolyVoltage(channel) : normalVoltage;
	}

	/** Returns a pointer to the array of voltages beginning with firstChannel.
	The pointer can be used for reading and writing.
	*/
	float* getVoltages(int firstChannel = 0) {
		return &voltages[firstChannel];
	}

	/** Copies the port's voltages to an array of size at least `channels`. */
	void readVoltages(float* v) {
		for (int c = 0; c < channels; c++) {
			v[c] = voltages[c];
		}
	}

	/** Copies an array of size at least `channels` to the port's voltages.
	Remember to set the number of channels *before* calling this method.
	*/
	void writeVoltages(const float* v) {
		for (int c = 0; c < channels; c++) {
			voltages[c] = v[c];
		}
	}

	/** Sets all voltages to 0. */
	void clearVoltages() {
		for (int c = 0; c < channels; c++) {
			voltages[c] = 0.f;
		}
	}

	/** Returns the sum of all voltages. */
	float getVoltageSum() {
		float sum = 0.f;
		for (int c = 0; c < channels; c++) {
			sum += voltages[c];
		}
		return sum;
	}

	/** Returns the root-mean-square of all voltages.
	Uses sqrt() which is slow, so use a custom approximation if calling frequently.
	*/
	float getVoltageRMS() {
		if (channels == 0) {
			return 0.f;
		}
		else if (channels == 1) {
			return std::fabs(voltages[0]);
		}
		else {
			float sum = 0.f;
			for (int c = 0; c < channels; c++) {
				sum += std::pow(voltages[c], 2);
			}
			return std::sqrt(sum);
		}
	}

	template <typename T>
	T getVoltageSimd(int firstChannel) {
		return T::load(&voltages[firstChannel]);
	}

	template <typename T>
	T getPolyVoltageSimd(int firstChannel) {
		return isMonophonic() ? getVoltage(0) : getVoltageSimd<T>(firstChannel);
	}

	template <typename T>
	T getNormalVoltageSimd(T normalVoltage, int firstChannel) {
		return isConnected() ? getVoltageSimd<T>(firstChannel) : normalVoltage;
	}

	template <typename T>
	T getNormalPolyVoltageSimd(T normalVoltage, int firstChannel) {
		return isConnected() ? getPolyVoltageSimd<T>(firstChannel) : normalVoltage;
	}

	template <typename T>
	void setVoltageSimd(T voltage, int firstChannel) {
		voltage.store(&voltages[firstChannel]);
	}

	/** Sets the number of polyphony channels.
	Also clears voltages of higher channels.
	If disconnected, this does nothing (`channels` remains 0).
	If 0 is given, `channels` is set to 1 but all voltages are cleared.
	*/
	void setChannels(int channels) {
		// If disconnected, keep the number of channels at 0.
		if (this->channels == 0) {
			return;
		}
		// Set higher channel voltages to 0
		for (int c = channels; c < this->channels; c++) {
			if (c >= PORT_MAX_CHANNELS)
				__builtin_unreachable();
			voltages[c] = 0.f;
		}
		// Don't allow caller to set port as disconnected
		if (channels == 0) {
			channels = 1;
		}
		this->channels = channels;
	}

	/** Returns the number of channels.
	If the port is disconnected, it has 0 channels.
	*/
	int getChannels() {
		return channels;
	}

	/** Returns whether a cable is connected to the Port.
	You can use this for skipping code that generates output voltages.
	*/
	bool isConnected() {
		return channels > 0;
	}

	/** Returns whether the cable exists and has 1 channel. */
	bool isMonophonic() {
		return channels == 1;
	}

	/** Returns whether the cable exists and has more than 1 channel. */
	bool isPolyphonic() {
		return channels > 1;
	}

	/** Use getNormalVoltage() instead. */
	DEPRECATED float normalize(float normalVoltage) {
		return getNormalVoltage(normalVoltage);
	}
};


struct Output : Port {
	/** List of cables connected to this port. */
	std::list<Cable*> cables;
};


struct Input : Port {};


} // namespace engine
} // namespace rack
