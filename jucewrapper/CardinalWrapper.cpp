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

#include <juce_audio_processors/juce_audio_processors.h>

#define createPlugin createStaticPlugin
#include "src/DistrhoPluginInternal.hpp"
#include "src/DistrhoUIInternal.hpp"

START_NAMESPACE_DISTRHO

#if 0

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
#endif

// -----------------------------------------------------------------------------------------------------------

class CardinalWrapperProcessor  : public juce::AudioProcessor
{
    friend class CardinalWrapperEditor;

    PluginExporter plugin;

    static bool writeMidi(void* ptr, const MidiEvent& midiEvent)
    {
        return false;
    }

    static bool requestParameterValueChange(void* ptr, uint32_t index, float value)
    {
        return false;
    }

    static bool updateStateValue(void* ptr, const char* key, const char* value)
    {
        return false;
    }

public:
    CardinalWrapperProcessor()
        : plugin(this, writeMidi, requestParameterValueChange, updateStateValue)
    {
        if (const double sampleRate = getSampleRate())
            plugin.setSampleRate(sampleRate);

        if (const int blockSize = getBlockSize())
            plugin.setBufferSize(blockSize);

//         for (uint i=0; i<plugin.getParameterCount(); ++i)
//             addParameter(new ParameterForDPF(plugin, i));
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

        const int numSamples = buffer.getNumSamples();
        DISTRHO_SAFE_ASSERT_INT_RETURN(numSamples > 0, numSamples,);

        DISTRHO_SAFE_ASSERT_RETURN(buffer.getNumChannels() == 2,);

        const float* audioBufferIn[2];
        float* audioBufferOut[2];
        audioBufferIn[0] = buffer.getReadPointer(0);
        audioBufferIn[1] = buffer.getReadPointer(1);
        audioBufferOut[0] = buffer.getWritePointer(0);
        audioBufferOut[1] = buffer.getWritePointer(1);

        plugin.run(audioBufferIn, audioBufferOut, numSamples, nullptr, 0);
    }

    double getTailLengthSeconds() const override
    {
        return 0.0;
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

// -----------------------------------------------------------------------------------------------------------

class CardinalWrapperEditor : public juce::AudioProcessorEditor
{
    UIExporter* ui;
    void* const dspPtr;

    static void editParamFunc(void* ptr, uint32_t rindex, bool started) {}
    static void setParamFunc(void* ptr, uint32_t rindex, float value) {}
    static void setStateFunc(void* ptr, const char* key, const char* value) {}
    static void sendNoteFunc(void* ptr, uint8_t channel, uint8_t note, uint8_t velo) {}

    static void setSizeFunc(void* ptr, uint width, uint height)
    {
        static_cast<CardinalWrapperEditor*>(ptr)->setSize(width, height);
    }

    static bool fileRequestFunc(void* ptr, const char* key) { return false; }

public:
    CardinalWrapperEditor(CardinalWrapperProcessor& cardinalProcessor)
        : juce::AudioProcessorEditor(cardinalProcessor),
          ui(nullptr),
          dspPtr(cardinalProcessor.plugin.getInstancePointer())
    {
        setOpaque(true);
        setResizable(true, false);
        // setResizeLimits(648, 538, -1, -1);
        setSize(1228, 666);
    }

    ~CardinalWrapperEditor() override
    {
        delete ui;
    }

    void paint(juce::Graphics&)
    {
        if (ui == nullptr)
        {
            auto peer = getPeer();
            d_stdout("peer is %p", peer);

            auto handle = peer->getNativeHandle();
            d_stdout("handle is %p", handle);

            auto proc = getAudioProcessor();
            d_stdout("proc is %p", proc);

            ui = new UIExporter(this,
                (uintptr_t)handle,
                proc->getSampleRate(),
                editParamFunc,
                setParamFunc,
                setStateFunc,
                sendNoteFunc,
                setSizeFunc,
                fileRequestFunc,
                nullptr, // bundlePath
                dspPtr,
                0.0 // scaleFactor
            );
        }

        ui->plugin_idle();
        repaint();
    }
};

juce::AudioProcessorEditor* CardinalWrapperProcessor::createEditor()
{
    return new CardinalWrapperEditor(*this);
}

// -----------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------------------------------------

juce::AudioProcessor* createPluginFilter()
{
    // set valid but dummy values
    d_nextBufferSize = 512;
    d_nextSampleRate = 48000.0;
    return new DISTRHO_NAMESPACE::CardinalWrapperProcessor;
}

// -----------------------------------------------------------------------------------------------------------
