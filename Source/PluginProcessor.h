#pragma once

#include <JuceHeader.h>
#include "MelodyEngine.h"

#include <array>
#include <atomic>
#include <vector>

class GoodAuraMelodyAudioProcessor
    : public juce::AudioProcessor
{
public:
    GoodAuraMelodyAudioProcessor();

    ~GoodAuraMelodyAudioProcessor()
        override = default;

    void prepareToPlay(
        double sampleRate,
        int samplesPerBlock) override;

    void releaseResources() override;

    bool isBusesLayoutSupported(
        const BusesLayout& layouts)
        const override;

    void processBlock(
        juce::AudioBuffer<float>&,
        juce::MidiBuffer&) override;

    juce::AudioProcessorEditor*
    createEditor() override;

    bool hasEditor() const override
    {
        return true;
    }

    const juce::String getName()
        const override
    {
        return "Good Aura Melody";
    }

    bool acceptsMidi() const override
    {
        return true;
    }

    bool producesMidi() const override
    {
        return true;
    }

    bool isMidiEffect() const override
    {
        return false;
    }

    double getTailLengthSeconds()
        const override
    {
        return 0.0;
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

    const juce::String
    getProgramName(int) override
    {
        return {};
    }

    void changeProgramName(
        int,
        const juce::String&) override
    {
    }

    void getStateInformation(
        juce::MemoryBlock&) override;

    void setStateInformation(
        const void*,
        int) override;

    void generateNewPhrase();

    void setChord(
        int slot,
        int root,
        int type);

    const std::array<
        MelodyEngine::ChordChoice,
        4>&
    getProgression() const
    {
        return progression;
    }

    std::atomic<int>
        melodyDensity {65};

    std::atomic<int>
        counterDensity {45};

    std::atomic<int>
        complexity {55};

    std::atomic<int>
        humanise {10};

private:
    MelodyEngine engine;

    std::array<
        MelodyEngine::ChordChoice,
        4>
    progression
    {{
        {0, 2},
        {9, 3},
        {2, 3},
        {7, 4}
    }};

    std::vector<
        MelodyEngine::NoteEvent>
    phrase;

    juce::CriticalSection
        phraseLock;

    double sampleRateHz =
        44100.0;

    double fallbackBeatPosition =
        0.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        GoodAuraMelodyAudioProcessor
    )
};
