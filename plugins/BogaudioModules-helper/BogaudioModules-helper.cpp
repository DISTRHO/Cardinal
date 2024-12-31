/*
 * DISTRHO Cardinal Plugin
 * Copyright (C) 2021-2024 Filipe Coelho <falktx@falktx.com>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "../BogaudioModules/src/bogaudio.hpp"

#include "../BogaudioModules/src/follower_base.hpp"
#include "../BogaudioModules/src/VCF.hpp"

namespace bogaudio {

constexpr float FollowerBase::efGainMinDecibels;
constexpr float FollowerBase::efGainMaxDecibels;
constexpr float VCF::maxFrequency;
constexpr float VCF::minFrequency;

}
