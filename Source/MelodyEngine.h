#pragma once

#include <JuceHeader.h>

#include <array>
#include <random>
#include <vector>

class MelodyEngine
{
public:
    struct ChordChoice
    {
        int root = 0;
        int type = 0;
    };

    struct NoteEvent
    {
        double beat = 0.0;
        double lengthBeats = 0.25;

        int note = 60;
        int velocity = 90;
        int channel = 1;
    };

    struct Settings
    {
        int bars = 4;
        int melodyDensity = 65;
        int counterDensity = 45;
        int complexity = 55;
        int humanise = 10;
    };

    MelodyEngine();

    static juce::StringArray
    rootNames();

    static juce::StringArray
    chordNames();

    static std::vector<int>
    chordIntervals(
        int chordType);

    std::vector<NoteEvent>
    generate(
        const Settings& settings,
        const std::array<
            ChordChoice,
            4>& progression);

private:
    std::mt19937 rng;

    int randomInt(
        int low,
        int high);

    bool chance(
        int percent);

    int nearestPitchFromPool(
        int previous,
        const std::vector<int>& pool,
        bool favorStep);

    std::vector<int>
    makePool(
        const ChordChoice& chord,
        int low,
        int high) const;
};
