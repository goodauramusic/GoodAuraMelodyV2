#include "PluginEditor.h"

GoodAuraMelodyAudioProcessorEditor::
GoodAuraMelodyAudioProcessorEditor(
    GoodAuraMelodyAudioProcessor& p)
    : AudioProcessorEditor(&p),
      processor(p)
{
    setSize(
        820,
        500);

    title.setText(
        "GOOD AURA MELODY",
        juce::dontSendNotification);

    title.setFont(
        juce::Font(
            juce::FontOptions(
                28.0f)
                .withStyle(
                    "Bold")));

    title.setColour(
        juce::Label::textColourId,
        juce::Colours::white);

    addAndMakeVisible(
        title);

    subtitle.setText(
        "4-CHORD MELODY + COUNTER-MELODY GENERATOR",
        juce::dontSendNotification);

    subtitle.setColour(
        juce::Label::textColourId,
        juce::Colours::lightgrey);

    addAndMakeVisible(
        subtitle);

    auto roots =
        MelodyEngine::rootNames();

    auto chords =
        MelodyEngine::chordNames();

    const auto& progression =
        processor.getProgression();

    for (int i = 0;
         i < 4;
         ++i)
    {
        chordLabels[
            (size_t)i]
            .setText(
                "CHORD " +
                juce::String(
                    i + 1),
                juce::dontSendNotification);

        chordLabels[
            (size_t)i]
            .setColour(
                juce::Label::textColourId,
                juce::Colours::white);

        addAndMakeVisible(
            chordLabels[
                (size_t)i]);

        rootBoxes[
            (size_t)i]
            .addItemList(
                roots,
                1);

        chordBoxes[
            (size_t)i]
            .addItemList(
                chords,
                1);

        rootBoxes[
            (size_t)i]
            .setSelectedId(
                progression[
                    (size_t)i]
                    .root + 1);

        chordBoxes[
            (size_t)i]
            .setSelectedId(
                progression[
                    (size_t)i]
                    .type + 1);

        rootBoxes[
            (size_t)i]
            .onChange =
            [this, i]
            {
                syncChord(i);
            };

        chordBoxes[
            (size_t)i]
            .onChange =
            [this, i]
            {
                syncChord(i);
            };

        addAndMakeVisible(
            rootBoxes[
                (size_t)i]);

        addAndMakeVisible(
            chordBoxes[
                (size_t)i]);
    }

    configureSlider(
        melodyDensity,
        "Melody Density",
        processor
            .melodyDensity
            .load());

    configureSlider(
        counterDensity,
        "Counter Density",
        processor
            .counterDensity
            .load());

    configureSlider(
        complexity,
        "Complexity",
        processor
            .complexity
            .load());

    configureSlider(
        humanise,
        "Humanise",
        processor
            .humanise
            .load());

    melodyDensity
        .onValueChange =
        [this]
        {
            processor
                .melodyDensity =
                (int)
                melodyDensity
                    .getValue();
        };

    counterDensity
        .onValueChange =
        [this]
        {
            processor
                .counterDensity =
                (int)
                counterDensity
                    .getValue();
        };

    complexity
        .onValueChange =
        [this]
        {
            processor
                .complexity =
                (int)
                complexity
                    .getValue();
        };

    humanise
        .onValueChange =
        [this]
        {
            processor
                .humanise =
                (int)
                humanise
                    .getValue();
        };

    generateButton
        .onClick =
        [this]
        {
            for (int i = 0;
                 i < 4;
                 ++i)
            {
                syncChord(i);
            }

            processor
                .generateNewPhrase();

            info.setText(
                "Generated • ch1 chords • ch2 melody • ch3 counter",
                juce::dontSendNotification);
        };

    addAndMakeVisible(
        generateButton);

    info.setText(
        "Press PLAY in FL Studio. The four-bar phrase loops.",
        juce::dontSendNotification);

    info.setColour(
        juce::Label::textColourId,
        juce::Colours::lightgrey);

    addAndMakeVisible(
        info);
}

void
GoodAuraMelodyAudioProcessorEditor::
configureSlider(
    juce::Slider& slider,
    const juce::String& name,
    int initial)
{
    slider.setName(
        name);

    slider.setSliderStyle(
        juce::Slider::
            RotaryHorizontalVerticalDrag);

    slider.setTextBoxStyle(
        juce::Slider::
            TextBoxBelow,
        false,
        72,
        22);

    slider.setRange(
        0,
        100,
        1);

    slider.setValue(
        initial);

    addAndMakeVisible(
        slider);
}

void
GoodAuraMelodyAudioProcessorEditor::
syncChord(
    int index)
{
    processor.setChord(
        index,

        rootBoxes[
            (size_t)index]
            .getSelectedId() - 1,

        chordBoxes[
            (size_t)index]
            .getSelectedId() - 1);
}

void
GoodAuraMelodyAudioProcessorEditor::
paint(
    juce::Graphics& g)
{
    g.fillAll(
        juce::Colour(
            0xff101116));

    g.setColour(
        juce::Colour(
            0xff1d2029));

    g.fillRoundedRectangle(
        18,
        18,
        (float)getWidth() - 36,
        (float)getHeight() - 36,
        16);

    g.setColour(
        juce::Colour(
            0xff323745));

    g.fillRoundedRectangle(
        32,
        100,
        (float)getWidth() - 64,
        130,
        12);

    g.setColour(
        juce::Colours::white);

    g.drawText(
        "MELODY",
        54,
        260,
        120,
        22,
        juce::Justification::left);

    g.drawText(
        "COUNTER",
        234,
        260,
        120,
        22,
        juce::Justification::left);

    g.drawText(
        "COMPLEXITY",
        414,
        260,
        120,
        22,
        juce::Justification::left);

    g.drawText(
        "HUMANISE",
        594,
        260,
        120,
        22,
        juce::Justification::left);
}

void
GoodAuraMelodyAudioProcessorEditor::
resized()
{
    title.setBounds(
        38,
        28,
        420,
        38);

    subtitle.setBounds(
        40,
        64,
        500,
        24);

    for (int i = 0;
         i < 4;
         ++i)
    {
        const int x =
            45 + i * 180;

        chordLabels[
            (size_t)i]
            .setBounds(
                x,
                112,
                120,
                20);

        rootBoxes[
            (size_t)i]
            .setBounds(
                x,
                140,
                70,
                34);

        chordBoxes[
            (size_t)i]
            .setBounds(
                x + 76,
                140,
                94,
                34);
    }

    melodyDensity.setBounds(
        38,
        288,
        150,
        130);

    counterDensity.setBounds(
        218,
        288,
        150,
        130);

    complexity.setBounds(
        398,
        288,
        150,
        130);

    humanise.setBounds(
        578,
        288,
        150,
        130);

    generateButton.setBounds(
        42,
        432,
        250,
        40);

    info.setBounds(
        310,
        432,
        465,
        40);
}
