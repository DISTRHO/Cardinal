/*
 * ZamAudio plugins For Cardinal
 * Copyright (C) 2014-2019 Damien Zammit <damien@zamaudio.com>
 * Copyright (C) 2022 Filipe Coelho <falktx@falktx.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * For a full copy of the GNU General Public License see the LICENSE file.
 */

#pragma once

#include "plugin.hpp"

struct ZamAudioModuleWidget : ModuleWidget {
};

template<int size>
struct FundamentalBlackKnob : RoundKnob {
	static constexpr const float kSize = size;
	static constexpr const float kHalfSize = size * 0.5f;
	float scale;

	FundamentalBlackKnob() {
		if (size <= 22) {
			setSvg(Svg::load(asset::plugin(pluginInstance, "res/components/knob-marker-small.svg")));
			bg->setSvg(Svg::load(asset::plugin(pluginInstance, "res/components/knob-small.svg")));
		} else {
			setSvg(Svg::load(asset::plugin(pluginInstance, "res/components/knob-marker.svg")));
			bg->setSvg(Svg::load(asset::plugin(pluginInstance, "res/components/knob.svg")));
		}

		scale = size / sw->box.size.x;
		box.size = Vec(size, size);
		bg->box.size = Vec(size, size);
	}

	void draw(const DrawArgs& args) override {
		nvgSave(args.vg);
		nvgScale(args.vg, scale, scale);
		RoundKnob::draw(args);
		nvgRestore(args.vg);
	}
};
