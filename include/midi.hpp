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
 * This file is an edited version of VCVRack's midi.hpp
 * Copyright (C) 2016-2021 VCV.
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 */

#pragma once

#include "choc/choc_SmallVector.h"

namespace rack {
/** Abstraction for all MIDI drivers in Rack */
namespace midi {


struct Message {
	/** Initialized to 3 empty bytes. */
	choc::SmallVector<uint8_t, 3> bytes;
	/** The Engine frame timestamp of the Message.
	For output messages, the frame when the message was generated.
	For input messages, the frame when it is intended to be processed.
	-1 for undefined, to be sent or processed immediately.
	*/
	int64_t frame = -1;

	Message() {
		bytes.resize(3);
	}

	int getSize() const {
		return bytes.size();
	}
	void setSize(int size) {
		bytes.resize(size);
	}

	uint8_t getChannel() const {
		if (bytes.size() < 1)
			return 0;
		return bytes[0] & 0xf;
	}
	void setChannel(uint8_t channel) {
		if (bytes.size() < 1)
			return;
		bytes[0] = (bytes[0] & 0xf0) | (channel & 0xf);
	}

	uint8_t getStatus() const {
		if (bytes.size() < 1)
			return 0;
		return bytes[0] >> 4;
	}
	void setStatus(uint8_t status) {
		if (bytes.size() < 1)
			return;
		bytes[0] = (bytes[0] & 0xf) | (status << 4);
	}

	uint8_t getNote() const {
		if (bytes.size() < 2)
			return 0;
		return bytes[1];
	}
	void setNote(uint8_t note) {
		if (bytes.size() < 2)
			return;
		bytes[1] = note & 0x7f;
	}

	uint8_t getValue() const {
		if (bytes.size() < 3)
			return 0;
		return bytes[2];
	}
	void setValue(uint8_t value) {
		if (bytes.size() < 3)
			return;
		bytes[2] = value & 0x7f;
	}

	std::string toString() const;

	int64_t getFrame() const {
		return frame;
	}

	void setFrame(int64_t frame) {
		this->frame = frame;
	}
};


/* NOTE all the other MIDI stuff (drivers, ports etc) is purposefully missing here, unwanted in Cardinal
 */
struct Port;


} // namespace midi
} // namespace rack
