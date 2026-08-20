#include "MelodyEngine.h"

#include <algorithm>
#include <cmath>

MelodyEngine::MelodyEngine()
    : rng(std::random_device{}())
{
}

juce::StringArray
MelodyEngine::rootNames()
{
    return
    {
        "C", "C#", "D", "Eb",
        "E", "F", "F#", "G",
        "Ab", "A", "Bb", "B"
    };
}

juce::StringArray
MelodyEngine::chordNames()
{
    return
    {
        "Major",
        "Minor",
        "Maj7",
        "Min7",
        "7",
        "Dim",
        "Sus2",
        "Sus4",
        "6",
        "m6",
        "9",
        "Maj9",
        "m9",
        "11",
        "m11",
        "13",
        "m13",
        "add9",
        "madd9",
        "7sus4"
    };
}

std::vector<int>
MelodyEngine::chordIntervals(
    int type)
{
    static const
    std::vector<
        std::vector<int>>
    table =
    {
        {0,4,7},
        {0,3,7},
        {0,4,7,11},
        {0,3,7,10},
        {0,4,7,10},
        {0,3,6},
        {0,2,7},
        {0,5,7},
        {0,4,7,9},
        {0,3,7,9},
        {0,4,7,10,14},
        {0,4,7,11,14},
        {0,3,7,10,14},
        {0,4,7,10,14,17},
        {0,3,7,10,14,17},
        {0,4,7,10,14,17,21},
        {0,3,7,10,14,17,21},
        {0,4,7,14},
        {0,3,7,14},
        {0,5,7,10}
    };

    return table[
        (size_t)juce::jlimit(
            0,
            (int)table.size() - 1,
            type)];
}

int MelodyEngine::randomInt(
    int low,
    int high)
{
    std::uniform_int_distribution<int>
        distribution(
            low,
            high);

    return distribution(rng);
}

bool MelodyEngine::chance(
    int percent)
{
    return
        randomInt(0, 99) <
        juce::jlimit(
            0,
            100,
            percent);
}

std::vector<int>
MelodyEngine::makePool(
    const ChordChoice& chord,
    int low,
    int high) const
{
    std::vector<int> pool;

    auto intervals =
        chordIntervals(
            chord.type);

    for (int midi = low;
         midi <= high;
         ++midi)
    {
        const int pitchClass =
            (midi -
             chord.root +
             120) % 12;

        for (int interval :
             intervals)
        {
            if ((interval % 12)
                == pitchClass)
            {
                pool.push_back(midi);
                pool.push_back(midi);
                break;
            }
        }
    }

    static const int
    colours[] =
    {
        2,
        5,
        9,
        11
    };

    for (int midi = low;
         midi <= high;
         ++midi)
    {
        const int pitchClass =
            (midi -
             chord.root +
             120) % 12;

        for (int colour :
             colours)
        {
            if (pitchClass == colour)
                pool.push_back(midi);
        }
    }

    return pool;
}

int
MelodyEngine::nearestPitchFromPool(
    int previous,
    const std::vector<int>& pool,
    bool favorStep)
{
    if (pool.empty())
        return previous;

    if (!favorStep)
    {
        return pool[
            (size_t)randomInt(
                0,
                (int)pool.size() - 1)];
    }

    std::vector<
        std::pair<int, int>>
    scored;

    for (int note : pool)
    {
        scored.push_back(
        {
            std::abs(
                note - previous),
            note
        });
    }

    std::sort(
        scored.begin(),
        scored.end(),
        [](auto a, auto b)
        {
            return
                a.first <
                b.first;
        });

    const int choices =
        juce::jmin(
            6,
            (int)scored.size());

    return scored[
        (size_t)randomInt(
            0,
            choices - 1)]
        .second;
}

std::vector<
    MelodyEngine::NoteEvent>
MelodyEngine::generate(
    const Settings& settings,
    const std::array<
        ChordChoice,
        4>& progression)
{
    std::vector<
        NoteEvent>
    events;

    int lastLead = 72;
    int lastCounter = 55;

    for (int bar = 0;
         bar < settings.bars;
         ++bar)
    {
        auto chord =
            progression[
                (size_t)(bar % 4)];

        auto intervals =
            chordIntervals(
                chord.type);

        const double barBeat =
            bar * 4.0;

        for (size_t i = 0;
             i < juce::jmin(
                (size_t)4,
                intervals.size());
             ++i)
        {
            int note =
                48 +
                chord.root +
                intervals[i];

            while (note > 67)
                note -= 12;

            while (note < 43)
                note += 12;

            events.push_back(
            {
                barBeat,
                3.85,
                note,
                64 -
                (int)i * 3,
                1
            });
        }

        auto leadPool =
            makePool(
                chord,
                60,
                88);

        auto counterPool =
            makePool(
                chord,
                43,
                69);

        const int divisions =
            settings.complexity >= 70
                ? 16
                : settings.complexity >= 40
                    ? 8
                    : 4;

        const double step =
            4.0 / divisions;

        for (int index = 0;
             index < divisions;
             ++index)
        {
            if (!chance(
                juce::jlimit(
                    5,
                    95,
                    settings.melodyDensity)))
                continue;

            if ((index == 0 ||
                 index == divisions - 1) &&
                chance(45))
                continue;

            int note =
                nearestPitchFromPool(
                    lastLead,
                    leadPool,
                    true);

            if (settings.complexity > 65 &&
                chance(18))
            {
                note +=
                    chance(50)
                    ? 1
                    : -1;
            }

            note =
                juce::jlimit(
                    60,
                    90,
                    note);

            lastLead = note;

            events.push_back(
            {
                barBeat +
                index * step,

                step *
                (chance(25)
                    ? 1.8
                    : 0.88),

                note,

                randomInt(
                    78,
                    105),

                2
            });
        }

        for (int index = 1;
             index < divisions;
             index += 2)
        {
            if (!chance(
                settings.counterDensity))
                continue;

            if (settings.melodyDensity > 75 &&
                chance(45))
                continue;

            int note =
                nearestPitchFromPool(
                    lastCounter,
                    counterPool,
                    true);

            if (lastLead > 74 &&
                chance(55))
            {
                note -= 2;
            }
            else if (
                lastLead < 68 &&
                chance(55))
            {
                note += 2;
            }

            note =
                juce::jlimit(
                    43,
                    71,
                    note);

            lastCounter = note;

            events.push_back(
            {
                barBeat +
                index * step,

                step * 0.85,

                note,

                randomInt(
                    52,
                    78),

                3
            });
        }
    }

    std::sort(
        events.begin(),
        events.end(),
        [](const NoteEvent& a,
           const NoteEvent& b)
        {
            if (a.beat == b.beat)
                return a.note < b.note;

            return a.beat < b.beat;
        });

    return events;
}
