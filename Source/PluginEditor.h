#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

#include <array>

class GoodAuraMelodyAudioProcessorEditor
    : public juce::AudioProcessorEditor
{
public:
    explicit
    GoodAuraMelodyAudioProcessorEditor(
        GoodAuraMelodyAudioProcessor&);

    ~GoodAuraMelodyAudioProcessorEditor()
        override = default;

    void paint(
        juce::Graphics&) override;

    void resized() override;

private:
    GoodAuraMelodyAudioProcessor&
        processor;

    juce::Label title;
    juce::Label subtitle;
    juce::Label info;

    std::array<
        juce::ComboBox,
        4>
    rootBoxes;

    std::array<
        juce::ComboBox,
        4>
    chordBoxes;

    std::array<
        juce::Label,
        4>
    chordLabels;

    juce::Slider
        melodyDensity;

    juce::Slider
        counterDensity;

    juce::Slider
        complexity;

    juce::Slider
        humanise;

    juce::TextButton
        generateButton
        {
            "GENERATE NEW PHRASE"
        };

    void configureSlider(
        juce::Slider&,
        const juce::String&,
        int);

    void syncChord(
        int);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        GoodAuraMelodyAudioProcessorEditor
    )
};
