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

#include "DistrhoPlugin.hpp"
#include "DistrhoUI.hpp"

DISTRHO_PLUGIN_EXPORT DISTRHO_NAMESPACE::Plugin* createSharedPlugin();
#define createPlugin ::createSharedPlugin
#include "src/DistrhoPluginInternal.hpp"

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------------------------------------

class ParameterForDPF : public juce::AudioProcessorParameter
{
    PluginExporter& plugin;
    const uint index;

public:
    ParameterForDPF(PluginExporter& plugin_, const uint index_)
        : plugin(plugin_),
          index(index_) {}

protected:
    float getValue() const override
    {
        return plugin.getParameterRanges(index).getNormalizedValue(plugin.getParameterValue(index));
    }

    void setValue(const float newValue) override
    {
        plugin.setParameterValue(index, plugin.getParameterRanges(index).getUnnormalizedValue(newValue));
    }

    float getDefaultValue() const override
    {
        return plugin.getParameterDefault(index);
    }

    juce::String getName(int) const override
    {
        return plugin.getParameterName(index).buffer();
    }

    juce::String getLabel() const override
    {
        return plugin.getParameterUnit(index).buffer();
    }

    float getValueForText(const juce::String& text) const override
    {
        return 0.0f;
    }
};

// -----------------------------------------------------------------------------------------------------------

class CardinalWrapperProcessor  : public juce::AudioProcessor
{
    PluginExporter plugin;

    static bool writeMidiCb(void* ptr, const MidiEvent& midiEvent)
    {
        return false;
    }

    static bool requestParameterValueChangeCb(void* ptr, uint32_t index, float value)
    {
        return false;
    }

public:
    CardinalWrapperProcessor()
        : plugin(this, writeMidiCb, requestParameterValueChangeCb)
    {
        for (uint i=0; i<plugin.getParameterCount(); ++i)
            addParameter(new ParameterForDPF(plugin, i));
    }

    ~CardinalWrapperProcessor() override
    {
    }

    const juce::String getName() const override
    {
        return plugin.getName();
    }

    juce::StringArray getAlternateDisplayNames() const override
    {
        return juce::StringArray(plugin.getLabel());
    }

    void prepareToPlay(double sampleRate, int samplesPerBlock) override
    {
        plugin.deactivateIfNeeded();
        plugin.setSampleRate(sampleRate);
        plugin.setBufferSize(samplesPerBlock);
        plugin.activate();
    }

    void releaseResources() override
    {
        plugin.deactivateIfNeeded();
    }

    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override
    {
        midiMessages.clear();
        // AudioPlayHead* getPlayHead()
    }

    double getTailLengthSeconds() const override
    {
        return true;
    }

    bool acceptsMidi() const override
    {
        return true;
    }

    bool producesMidi() const override
    {
        return true;
    }

    juce::AudioProcessorEditor* createEditor() override;

    bool hasEditor() const override
    {
        return true;
    }

    int getNumPrograms() override
    {
        return 0;
    }

    int getCurrentProgram() override
    {
        return 0;
    }

    void setCurrentProgram(int) override
    {
    }

    const juce::String getProgramName(int) override
    {
        return {};
    }

    void changeProgramName(int, const juce::String&) override
    {
    }

    void getStateInformation(juce::MemoryBlock& destData) override
    {
    }

    void setStateInformation(const void* data, int sizeInBytes) override
    {
    }
};

class CardinalWrapperEditor : public juce::AudioProcessorEditor
{
public:
    CardinalWrapperEditor(CardinalWrapperProcessor& processor)
        : juce::AudioProcessorEditor(processor)
    {}

    ~CardinalWrapperEditor() override
    {}
};

// -----------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------------------------------------

juce::AudioProcessor* createPluginFilter()
{
    return new DISTRHO_NAMESPACE::CardinalWrapperProcessor;
}

// -----------------------------------------------------------------------------------------------------------

#define DISTRHO_IS_STANDALONE 0
#include "src/DistrhoPlugin.cpp"
#include "src/DistrhoUtils.cpp"
