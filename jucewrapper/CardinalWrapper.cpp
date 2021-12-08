/*
 * DISTRHO Cardinal Plugin
 * Copyright (C) 2021 Filipe Coelho <falktx@falktx.com>
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

#include <juce_audio_processors/juce_audio_processors.h>

class CardinalWrapperProcessor  : public juce::AudioProcessor
{
public:
    CardinalWrapperProcessor()
    {}

    ~CardinalWrapperProcessor() override
    {}

    void prepareToPlay (double sampleRate, int samplesPerBlock) override
    {}

    void releaseResources() override
    {}
};

class CardinalWrapperEditor : public juce::AudioProcessorEditor
{
public:
    CardinalWrapperEditor(CardinalWrapperProcessor&)
    {}
    ~CardinalWrapperEditor() override
    {}
};
