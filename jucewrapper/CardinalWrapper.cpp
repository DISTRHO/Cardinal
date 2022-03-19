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

// --------------------------------------------------------------------------------------------------------------------

class ParameterFromDPF : public juce::AudioProcessorParameter
{
    PluginExporter& plugin;
    const ParameterEnumerationValues& enumValues;
    const ParameterRanges& ranges;
    const uint32_t hints;
    const uint index;
    bool* const updatedPtr;
    mutable juce::StringArray dpfValueStrings;

public:
    ParameterFromDPF(PluginExporter& plugin_, const uint index_, bool* const updatedPtr_)
        : plugin(plugin_),
          enumValues(plugin_.getParameterEnumValues(index_)),
          ranges(plugin_.getParameterRanges(index_)),
          hints(plugin_.getParameterHints(index_)),
          index(index_),
          updatedPtr(updatedPtr_) {}

    void setValueNotifyingHostFromDPF(const float newValue)
    {
        setValueNotifyingHost(ranges.getNormalizedValue(newValue));
        *updatedPtr = false;
    }

protected:
    float getValue() const override
    {
        return ranges.getNormalizedValue(plugin.getParameterValue(index));
    }

    void setValue(const float newValue) override
    {
        *updatedPtr = true;
        plugin.setParameterValue(index, ranges.getUnnormalizedValue(newValue));
    }

    float getDefaultValue() const override
    {
        return ranges.getNormalizedValue(plugin.getParameterDefault(index));
    }

    juce::String getName(const int maximumStringLength) const override
    {
        if (maximumStringLength <= 0)
            return juce::String(plugin.getParameterName(index).buffer());

        return juce::String(plugin.getParameterName(index).buffer(), static_cast<size_t>(maximumStringLength));
    }

    juce::String getLabel() const override
    {
        return plugin.getParameterUnit(index).buffer();
    }

    int getNumSteps() const override
    {
        if (hints & kParameterIsBoolean)
            return 2;

        if (enumValues.restrictedMode)
            return enumValues.count;

        if (hints & kParameterIsInteger)
            return ranges.max - ranges.min;

        return juce::AudioProcessorParameter::getNumSteps();
    }

    bool isDiscrete() const override
    {
        if (hints & (kParameterIsBoolean|kParameterIsInteger))
            return true;

        if (enumValues.restrictedMode)
            return true;

        return false;
    }

    bool isBoolean() const override
    {
        return (hints & kParameterIsBoolean) != 0x0;
    }

    juce::String getText(const float normalizedValue, const int maximumStringLength) const override
    {
        float value = ranges.getUnnormalizedValue(normalizedValue);

        if (hints & kParameterIsBoolean)
        {
            const float midRange = ranges.min + (ranges.max - ranges.min) * 0.5f;
            value = value > midRange ? ranges.max : ranges.min;
        }
        else if (hints & kParameterIsInteger)
        {
            value = std::round(value);
        }

        if (enumValues.restrictedMode)
        {
            for (uint32_t i=0; i < enumValues.count; ++i)
            {
                if (d_isEqual(enumValues.values[i].value, value))
                {
                    if (maximumStringLength <= 0)
                        return juce::String(enumValues.values[i].label);

                    return juce::String(enumValues.values[i].label, static_cast<size_t>(maximumStringLength));
                }
            }
        }

        juce::String text;
        if (hints & kParameterIsInteger)
            text = juce::String(static_cast<int>(value));
        else
            text = juce::String(value);

        if (maximumStringLength <= 0)
            return text;

        return juce::String(text.toRawUTF8(), static_cast<size_t>(maximumStringLength));
    }

    float getValueForText(const juce::String& text) const override
    {
        if (enumValues.restrictedMode)
        {
            for (uint32_t i=0; i < enumValues.count; ++i)
            {
                if (text == enumValues.values[i].label.buffer())
                    return ranges.getNormalizedValue(enumValues.values[i].value);
            }
        }

        float value;
        if (hints & kParameterIsInteger)
            value = std::atoi(text.toRawUTF8());
        else
            value = std::atof(text.toRawUTF8());

        return ranges.getFixedAndNormalizedValue(value);
    }

    bool isAutomatable() const override
    {
        return (hints & kParameterIsAutomatable) != 0x0;
    }

    juce::String getCurrentValueAsText() const override
    {
        const float value = plugin.getParameterValue(index);

        if (enumValues.restrictedMode)
        {
            for (uint32_t i=0; i < enumValues.count; ++i)
            {
                if (d_isEqual(enumValues.values[i].value, value))
                    return juce::String(enumValues.values[i].label);
            }
        }

        if (hints & kParameterIsInteger)
            return juce::String(static_cast<int>(value));

        return juce::String(value);
    }

    juce::StringArray getAllValueStrings() const override
    {
        if (dpfValueStrings.size() != 0)
            return dpfValueStrings;

        if (enumValues.restrictedMode)
        {
            for (uint32_t i=0; i < enumValues.count; ++i)
                dpfValueStrings.add(enumValues.values[i].label.buffer());

            return dpfValueStrings;
        }

        if (hints & kParameterIsBoolean)
        {
            if (hints & kParameterIsInteger)
            {
                dpfValueStrings.add(juce::String(static_cast<int>(ranges.min)));
                dpfValueStrings.add(juce::String(static_cast<int>(ranges.max)));
            }
            else
            {
                dpfValueStrings.add(juce::String(ranges.min));
                dpfValueStrings.add(juce::String(ranges.max));
            }
        }
        else if (hints & kParameterIsInteger)
        {
            const int imin = static_cast<int>(ranges.min);
            const int imax = static_cast<int>(ranges.max);

            for (int i=imin; i<=imax; ++i)
                dpfValueStrings.add(juce::String(i));
        }

        return dpfValueStrings;
    }
};

// --------------------------------------------------------------------------------------------------------------------

// unused in cardinal
static constexpr const requestParameterValueChangeFunc nullRequestParameterValueChangeFunc = nullptr;

// only needed for headless builds, which this wrapper never builds for
static constexpr const updateStateValueFunc nullUpdateStateValueFunc = nullptr;

// DSP/processor implementation
class CardinalWrapperProcessor : public juce::AudioProcessor
{
    friend class CardinalWrapperEditor;

    PluginExporter plugin;
    MidiEvent midiEvents[kMaxMidiEvents];
    TimePosition timePosition;
    const uint32_t parameterCount;

    juce::AudioProcessorParameter* bypassParameter;
    juce::MidiBuffer* currentMidiMessages;
    bool* updatedParameters;

public:
    CardinalWrapperProcessor()
        : plugin(this, writeMidiFunc, nullRequestParameterValueChangeFunc, nullUpdateStateValueFunc),
          parameterCount(plugin.getParameterCount()),
          bypassParameter(nullptr),
          currentMidiMessages(nullptr),
          updatedParameters(nullptr)
    {
        if (const double sampleRate = getSampleRate())
            plugin.setSampleRate(sampleRate);

        if (const int samplesPerBlock = getBlockSize())
            if (samplesPerBlock > 0)
                plugin.setBufferSize(static_cast<uint32_t>(samplesPerBlock));

        getBypassParameter();

        if (parameterCount != 0)
        {
            updatedParameters = new bool[parameterCount];
            std::memset(updatedParameters, 0, sizeof(bool)*parameterCount);

            for (uint i=0; i<parameterCount; ++i)
            {
                ParameterFromDPF* const param = new ParameterFromDPF(plugin, i, updatedParameters + i);
                addParameter(param);

                if (plugin.getParameterDesignation(i) == kParameterDesignationBypass)
                    bypassParameter = param;
            }
        }
    }

    ~CardinalWrapperProcessor() override
    {
        delete[] updatedParameters;
    }

protected:
    const juce::String getName() const override
    {
        return plugin.getName();
    }

    juce::StringArray getAlternateDisplayNames() const override
    {
        return juce::StringArray(plugin.getLabel());
    }

    void prepareToPlay(const double sampleRate, const int samplesPerBlock) override
    {
        DISTRHO_SAFE_ASSERT_RETURN(samplesPerBlock > 0,);

        plugin.deactivateIfNeeded();
        plugin.setSampleRate(sampleRate);
        plugin.setBufferSize(static_cast<uint32_t>(samplesPerBlock));
        plugin.activate();
    }

    void releaseResources() override
    {
        plugin.deactivateIfNeeded();
    }

    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override
    {
        const int numSamples = buffer.getNumSamples();
        DISTRHO_SAFE_ASSERT_INT_RETURN(numSamples > 0, numSamples, midiMessages.clear());

        uint32_t midiEventCount = 0;

        for (const juce::MidiMessageMetadata midiMessage : midiMessages)
        {
            DISTRHO_SAFE_ASSERT_CONTINUE(midiMessage.numBytes > 0);
            DISTRHO_SAFE_ASSERT_CONTINUE(midiMessage.samplePosition >= 0);

            if (midiMessage.numBytes > static_cast<int>(MidiEvent::kDataSize))
                continue;

            MidiEvent& midiEvent(midiEvents[midiEventCount++]);

            midiEvent.frame = static_cast<uint32_t>(midiMessage.samplePosition);
            midiEvent.size = (static_cast<uint8_t>(midiMessage.numBytes));
            std::memcpy(midiEvent.data, midiMessage.data, midiEvent.size);

            if (midiEventCount == kMaxMidiEvents)
                break;
        }

        midiMessages.clear();

        const juce::ScopedValueSetter<juce::MidiBuffer*> cvs(currentMidiMessages, &midiMessages, nullptr);

        juce::AudioPlayHead* const playhead = getPlayHead();
        juce::AudioPlayHead::CurrentPositionInfo posInfo;

        if (playhead != nullptr && playhead->getCurrentPosition(posInfo))
        {
            timePosition.playing   = posInfo.isPlaying;
            timePosition.bbt.valid = true;

            // ticksPerBeat is not possible with JUCE
            timePosition.bbt.ticksPerBeat = 1920.0;

            if (posInfo.timeInSamples >= 0)
                timePosition.frame = static_cast<uint64_t>(posInfo.timeInSamples);
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

        DISTRHO_SAFE_ASSERT_RETURN(buffer.getNumChannels() == 2,);

        const float* audioBufferIn[2];
        float* audioBufferOut[2];
        audioBufferIn[0] = buffer.getReadPointer(0);
        audioBufferIn[1] = buffer.getReadPointer(1);
        audioBufferOut[0] = buffer.getWritePointer(0);
        audioBufferOut[1] = buffer.getWritePointer(1);

        plugin.run(audioBufferIn, audioBufferOut, static_cast<uint32_t>(numSamples), midiEvents, midiEventCount);
    }

    // fix compiler warning
    void processBlock(juce::AudioBuffer<double>&, juce::MidiBuffer&) override {}

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

    juce::AudioProcessorParameter* getBypassParameter() const override
    {
        return nullptr;
    }

    juce::AudioProcessorEditor* createEditor() override;

    bool hasEditor() const override
    {
        return true;
    }

    int getNumPrograms() override
    {
        return 1;
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
        return "Default";
    }

    void changeProgramName(int, const juce::String&) override
    {
    }

    void getStateInformation(juce::MemoryBlock& destData) override
    {
        juce::XmlElement xmlState("CardinalState");

        for (uint32_t i=0; i<parameterCount; ++i)
            xmlState.setAttribute(plugin.getParameterSymbol(i).buffer(), plugin.getParameterValue(i));

        for (uint32_t i=0, stateCount=plugin.getStateCount(); i<stateCount; ++i)
        {
            const String& key(plugin.getStateKey(i));
            xmlState.setAttribute(key.buffer(), plugin.getStateValue(key).buffer());
        }

        copyXmlToBinary(xmlState, destData);
    }

    void setStateInformation(const void* const data, const int sizeInBytes) override
    {
        std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
        DISTRHO_SAFE_ASSERT_RETURN(xmlState.get() != nullptr,);

        const juce::Array<juce::AudioProcessorParameter*>& parameters(getParameters());

        for (uint32_t i=0; i<parameterCount; ++i)
        {
            const double value = xmlState->getDoubleAttribute(plugin.getParameterSymbol(i).buffer(),
                                                              plugin.getParameterDefault(i));
            const float normalizedValue = plugin.getParameterRanges(i).getFixedAndNormalizedValue(value);
            parameters.getUnchecked(static_cast<int>(i))->setValueNotifyingHost(normalizedValue);
        }

        for (uint32_t i=0, stateCount=plugin.getStateCount(); i<stateCount; ++i)
        {
            const String& key(plugin.getStateKey(i));
            const juce::String value = xmlState->getStringAttribute(key.buffer(),
                                                                    plugin.getStateDefaultValue(i).buffer());
            plugin.setState(key, value.toRawUTF8());
        }
    }

private:
    static bool writeMidiFunc(void* const ptr, const MidiEvent& midiEvent)
    {
        CardinalWrapperProcessor* const processor = static_cast<CardinalWrapperProcessor*>(ptr);
        DISTRHO_SAFE_ASSERT_RETURN(processor != nullptr, false);

        const uint8_t* const data = midiEvent.size > MidiEvent::kDataSize ? midiEvent.dataExt : midiEvent.data;
        return processor->currentMidiMessages->addEvent(data,
                                                        static_cast<int>(midiEvent.size),
                                                        static_cast<int>(midiEvent.frame));
    }
};

// --------------------------------------------------------------------------------------------------------------------

// unused in cardinal
static constexpr const sendNoteFunc nullSendNoteFunc = nullptr;

// unwanted, juce file dialogs are ugly
static constexpr const fileRequestFunc nullFileRequestFunc = nullptr;

// UI/editor implementation
class CardinalWrapperEditor : public juce::AudioProcessorEditor,
                              private juce::AsyncUpdater,
                              private juce::Timer
{
    CardinalWrapperProcessor& cardinalProcessor;

    UIExporter* ui;
    void* const dspPtr;

public:
    CardinalWrapperEditor(CardinalWrapperProcessor& cardinalProc)
        : juce::AudioProcessorEditor(cardinalProc),
          cardinalProcessor(cardinalProc),
          ui(nullptr),
          dspPtr(cardinalProc.plugin.getInstancePointer())
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

protected:
    void handleAsyncUpdate() override
    {
        DISTRHO_SAFE_ASSERT_RETURN(ui != nullptr,);

        int width = static_cast<int>(ui->getWidth());
        int height = static_cast<int>(ui->getHeight());

       #ifdef DISTRHO_OS_MAC
        const double scaleFactor = ui->getScaleFactor();
        width /= scaleFactor;
        height /= scaleFactor;
       #endif

        setSize(width, height);
    }

    void timerCallback() override
    {
        if (ui == nullptr)
            return;

        for (uint32_t i=0; i<cardinalProcessor.parameterCount; ++i)
        {
            if (cardinalProcessor.updatedParameters[i])
            {
                cardinalProcessor.updatedParameters[i] = false;
                ui->parameterChanged(i, cardinalProcessor.plugin.getParameterValue(i));
            }
        }

        repaint();
    }

    void paint(juce::Graphics&) override
    {
        if (ui == nullptr)
        {
            juce::ComponentPeer* const peer = getPeer();
            DISTRHO_SAFE_ASSERT_RETURN(peer != nullptr,);

            void* const nativeHandle = peer->getNativeHandle();
            DISTRHO_SAFE_ASSERT_RETURN(nativeHandle != nullptr,);

            ui = new UIExporter(this,
                (uintptr_t)nativeHandle,
                cardinalProcessor.getSampleRate(),
                editParamFunc,
                setParamFunc,
                setStateFunc,
                nullSendNoteFunc,
                setSizeFunc,
                nullFileRequestFunc,
                nullptr, // bundlePath
                dspPtr,
                0.0 // scaleFactor
            );

            if (cardinalProcessor.wrapperType == juce::AudioProcessor::wrapperType_Standalone)
            {
                const double scaleFactor = ui->getScaleFactor();
                ui->setWindowOffset(4 * scaleFactor, 30 * scaleFactor);
            }
        }

        ui->plugin_idle();
    }

private:
    static void editParamFunc(void* const ptr, const uint32_t index, const bool started)
    {
        CardinalWrapperEditor* const editor = static_cast<CardinalWrapperEditor*>(ptr);
        DISTRHO_SAFE_ASSERT_RETURN(editor != nullptr,);

        CardinalWrapperProcessor& cardinalProcessor(editor->cardinalProcessor);

        if (started)
            cardinalProcessor.getParameters().getUnchecked(static_cast<int>(index))->beginChangeGesture();
        else
            cardinalProcessor.getParameters().getUnchecked(static_cast<int>(index))->endChangeGesture();
    }

    static void setParamFunc(void* const ptr, const uint32_t index, const float value)
    {
        CardinalWrapperEditor* const editor = static_cast<CardinalWrapperEditor*>(ptr);
        DISTRHO_SAFE_ASSERT_RETURN(editor != nullptr,);

        CardinalWrapperProcessor& cardinalProcessor(editor->cardinalProcessor);
        const juce::Array<juce::AudioProcessorParameter*>& parameters(cardinalProcessor.getParameters());
        juce::AudioProcessorParameter* const parameter = parameters.getUnchecked(static_cast<int>(index));
        static_cast<ParameterFromDPF*>(parameter)->setValueNotifyingHostFromDPF(value);
    }

    static void setStateFunc(void* const ptr, const char* const key, const char* const value)
    {
        CardinalWrapperEditor* const editor = static_cast<CardinalWrapperEditor*>(ptr);
        DISTRHO_SAFE_ASSERT_RETURN(editor != nullptr,);

        CardinalWrapperProcessor& cardinalProcessor(editor->cardinalProcessor);
        cardinalProcessor.plugin.setState(key, value);
    }

    static void setSizeFunc(void* const ptr, uint, uint)
    {
        CardinalWrapperEditor* const editor = static_cast<CardinalWrapperEditor*>(ptr);
        DISTRHO_SAFE_ASSERT_RETURN(editor != nullptr,);

        editor->triggerAsyncUpdate();
    }
};

juce::AudioProcessorEditor* CardinalWrapperProcessor::createEditor()
{
    return new CardinalWrapperEditor(*this);
}

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO

// --------------------------------------------------------------------------------------------------------------------

juce::AudioProcessor* createPluginFilter()
{
    // set valid but dummy values
    d_nextBufferSize = 512;
    d_nextSampleRate = 48000.0;
    return new DISTRHO_NAMESPACE::CardinalWrapperProcessor;
}

// --------------------------------------------------------------------------------------------------------------------
