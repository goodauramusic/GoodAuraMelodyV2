#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

GoodAuraMelodyAudioProcessor::GoodAuraMelodyAudioProcessor()
: AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
    generateNewPhrase();
}

void GoodAuraMelodyAudioProcessor::prepareToPlay(double sr, int)
{
    sampleRateHz = sr;
    fallbackBeatPosition = 0.0;
}

void GoodAuraMelodyAudioProcessor::releaseResources() {}

bool GoodAuraMelodyAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void GoodAuraMelodyAudioProcessor::setChord(int slot, int root, int type)
{
    if (slot < 0 || slot >= 4) return;

    progression[(size_t)slot].root = juce::jlimit(0, 11, root);
    progression[(size_t)slot].type = juce::jmax(0, type);
}

void GoodAuraMelodyAudioProcessor::generateNewPhrase()
{
    MelodyEngine::Settings s;
    s.bars = 4;
    s.melodyDensity = melodyDensity.load();
    s.counterDensity = counterDensity.load();
    s.complexity = complexity.load();
    s.humanise = humanise.load();

    auto newPhrase = engine.generate(s, progression);

    const juce::ScopedLock lock(phraseLock);
    phrase = std::move(newPhrase);
}

void GoodAuraMelodyAudioProcessor::processBlock(
    juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    double bpm = 120.0;
    double blockStartBeat = fallbackBeatPosition;
    bool hostPlaying = true;

    if (auto* ph = getPlayHead())
    {
        if (auto pos = ph->getPosition())
        {
            if (auto b = pos->getBpm())
                bpm = *b;

            if (auto ppq = pos->getPpqPosition())
                blockStartBeat = *ppq;

            hostPlaying = pos->getIsPlaying();
        }
    }

    const double samplesPerBeat =
        sampleRateHz * 60.0 / juce::jmax(1.0, bpm);

    const double blockBeats =
        buffer.getNumSamples() / samplesPerBeat;

    const double loopLength = 16.0;

    if (!hostPlaying)
    {
        fallbackBeatPosition = blockStartBeat;
        return;
    }

    const juce::ScopedLock lock(phraseLock);

    auto emit =
        [&](double localStart,
            double localEnd,
            double absWindowStart)
        {
            for (const auto& ev : phrase)
            {
                if (ev.beat < localStart ||
                    ev.beat >= localEnd)
                    continue;

                const double absBeat =
                    absWindowStart +
                    (ev.beat - localStart);

                const double delta =
                    absBeat - blockStartBeat;

                const int samplePos =
                    juce::jlimit(
                        0,
                        juce::jmax(
                            0,
                            buffer.getNumSamples() - 1),
                        (int)std::floor(
                            delta * samplesPerBeat));

                int off =
                    samplePos +
                    juce::jmax(
                        1,
                        (int)std::floor(
                            ev.lengthBeats *
                            samplesPerBeat));

                off =
                    juce::jmin(
                        buffer.getNumSamples() - 1,
                        off);

                midi.addEvent(
                    juce::MidiMessage::noteOn(
                        ev.channel,
                        ev.note,
                        (juce::uint8)ev.velocity),
                    samplePos);

                midi.addEvent(
                    juce::MidiMessage::noteOff(
                        ev.channel,
                        ev.note),
                    off);
            }
        };

    const double localStart =
        std::fmod(
            std::fmod(
                blockStartBeat,
                loopLength) +
            loopLength,
            loopLength);

    const double localEnd =
        localStart + blockBeats;

    if (localEnd <= loopLength)
    {
        emit(
            localStart,
            localEnd,
            blockStartBeat);
    }
    else
    {
        emit(
            localStart,
            loopLength,
            blockStartBeat);

        emit(
            0.0,
            localEnd - loopLength,
            blockStartBeat +
            (loopLength - localStart));
    }

    fallbackBeatPosition =
        blockStartBeat + blockBeats;
}

void GoodAuraMelodyAudioProcessor::getStateInformation(
    juce::MemoryBlock& dest)
{
    juce::ValueTree s("GoodAuraMelody");

    s.setProperty(
        "melodyDensity",
        melodyDensity.load(),
        nullptr);

    s.setProperty(
        "counterDensity",
        counterDensity.load(),
        nullptr);

    s.setProperty(
        "complexity",
        complexity.load(),
        nullptr);

    s.setProperty(
        "humanise",
        humanise.load(),
        nullptr);

    for (int i = 0; i < 4; ++i)
    {
        s.setProperty(
            "root" + juce::String(i),
            progression[(size_t)i].root,
            nullptr);

        s.setProperty(
            "type" + juce::String(i),
            progression[(size_t)i].type,
            nullptr);
    }

    if (auto xml = s.createXml())
        copyXmlToBinary(*xml, dest);
}

void GoodAuraMelodyAudioProcessor::setStateInformation(
    const void* data,
    int size)
{
    if (auto xml =
        getXmlFromBinary(data, size))
    {
        if (auto s =
            juce::ValueTree::fromXml(*xml);
            s.isValid())
        {
            melodyDensity =
                (int)s.getProperty(
                    "melodyDensity", 65);

            counterDensity =
                (int)s.getProperty(
                    "counterDensity", 45);

            complexity =
                (int)s.getProperty(
                    "complexity", 55);

            humanise =
                (int)s.getProperty(
                    "humanise", 10);

            for (int i = 0; i < 4; ++i)
            {
                progression[(size_t)i].root =
                    (int)s.getProperty(
                        "root" + juce::String(i),
                        0);

                progression[(size_t)i].type =
                    (int)s.getProperty(
                        "type" + juce::String(i),
                        0);
            }

            generateNewPhrase();
        }
    }
}

juce::AudioProcessorEditor*
GoodAuraMelodyAudioProcessor::createEditor()
{
    return new GoodAuraMelodyAudioProcessorEditor(*this);
}

juce::AudioProcessor*
JUCE_CALLTYPE createPluginFilter()
{
    return new GoodAuraMelodyAudioProcessor();
}
