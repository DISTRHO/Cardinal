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
    TimePosition timePosition;

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

        juce::AudioPlayHead* const playhead = getPlayHead();
        juce::AudioPlayHead::CurrentPositionInfo posInfo;

        if (playhead != nullptr && playhead->getCurrentPosition(posInfo))
        {
            timePosition.playing   = posInfo.isPlaying;
            timePosition.bbt.valid = true;

            // ticksPerBeat is not possible with JUCE
            timePosition.bbt.ticksPerBeat = 1920.0;

            if (posInfo.timeInSamples >= 0)
                timePosition.frame = posInfo.timeInSamples;
            else
                timePosition.frame = 0;

            timePosition.bbt.beatsPerMinute = posInfo.bpm;

            const double ppqPos    = std::abs(posInfo.ppqPosition);
            const int    ppqPerBar = posInfo.timeSigNumerator * 4 / posInfo.timeSigDenominator;
            const double barBeats  = (std::fmod(ppqPos, ppqPerBar) / ppqPerBar) * posInfo.timeSigNumerator;
            const double rest      =  std::fmod(barBeats, 1.0);

            timePosition.bbt.bar         = static_cast<int32_t>(ppqPos) / ppqPerBar + 1;
            timePosition.bbt.beat        = static_cast<int32_t>(barBeats - rest + 0.5) + 1;
            timePosition.bbt.tick        = rest * timePosition.bbt.ticksPerBeat;
            timePosition.bbt.beatsPerBar = posInfo.timeSigNumerator;
            timePosition.bbt.beatType    = posInfo.timeSigDenominator;

            if (posInfo.ppqPosition < 0.0)
            {
                --timePosition.bbt.bar;
                timePosition.bbt.beat = posInfo.timeSigNumerator - timePosition.bbt.beat + 1;
                timePosition.bbt.tick = timePosition.bbt.ticksPerBeat - timePosition.bbt.tick - 1;
            }

            timePosition.bbt.barStartTick = timePosition.bbt.ticksPerBeat*
                                            timePosition.bbt.beatsPerBar*
                                            (timePosition.bbt.bar-1);
        }
        else
        {
            timePosition.frame     = 0;
            timePosition.playing   = false;
            timePosition.bbt.valid = false;
        }

        plugin.setTimePosition(timePosition);

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

class CardinalWrapperEditor : public juce::AudioProcessorEditor,
                              private juce::Timer
{
    UIExporter* ui;
    void* const dspPtr;

    static void editParamFunc(void* ptr, uint32_t rindex, bool started) {}
    static void setParamFunc(void* ptr, uint32_t rindex, float value) {}
    static void setStateFunc(void* ptr, const char* key, const char* value) {}
    static void sendNoteFunc(void* ptr, uint8_t channel, uint8_t note, uint8_t velo) {}

    static void setSizeFunc(void* ptr, uint width, uint height)
    {
        CardinalWrapperEditor* const editor = static_cast<CardinalWrapperEditor*>(ptr);
        DISTRHO_SAFE_ASSERT_RETURN(editor != nullptr,);

       #ifdef DISTRHO_OS_MAC
        UIExporter* const ui = editor->ui;
        DISTRHO_SAFE_ASSERT_RETURN(ui != nullptr,);

        const double scaleFactor = ui->getScaleFactor();
        width /= scaleFactor;
        height /= scaleFactor;
       #endif

        editor->setSize(width, height);
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

        startTimer(1000.0 / 60.0);
    }

    ~CardinalWrapperEditor() override
    {
        stopTimer();
        delete ui;
    }

    void timerCallback() override
    {
        repaint();
    }

    void paint(juce::Graphics&) override
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

            if (getAudioProcessor()->wrapperType == juce::AudioProcessor::wrapperType_Standalone)
            {
                const double scaleFactor = ui->getScaleFactor();
                ui->setWindowOffset(4 * scaleFactor, 30 * scaleFactor);
            }
        }

        ui->plugin_idle();
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
