#include "PluginProcessor.h"

// MIDI note mapping (GM Drum Map compatible)
// C1 (36) = BD, D1 (38) = SD, F#1 (42) = CH, A#1 (46) = OH, D#1 (39) = CP, C#1 (37) = RS

TR808GarageProcessor::TR808GarageProcessor()
    : AudioProcessor(BusesProperties()
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
}

void TR808GarageProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    setLatencySamples(0);
    
    bassDrum.prepare(sampleRate, samplesPerBlock);
    snareDrum.prepare(sampleRate, samplesPerBlock);
    closedHat.prepare(sampleRate, samplesPerBlock);
    openHat.prepare(sampleRate, samplesPerBlock);
    clap.prepare(sampleRate, samplesPerBlock);
    rimShot.prepare(sampleRate, samplesPerBlock);
}

void TR808GarageProcessor::releaseResources()
{
}

bool TR808GarageProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void TR808GarageProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                        juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    buffer.clear();

    // Process MIDI events
    for (const auto metadata : midiMessages)
    {
        const auto msg = metadata.getMessage();
        if (msg.isNoteOn())
            handleMidiMessage(msg);
    }

    // Render all active voices
    int numSamples = buffer.getNumSamples();
    
    if (bassDrum.isActive())
        bassDrum.renderNextBlock(buffer, 0, numSamples);
    
    if (snareDrum.isActive())
        snareDrum.renderNextBlock(buffer, 0, numSamples);
    
    if (closedHat.isActive())
        closedHat.renderNextBlock(buffer, 0, numSamples);
    
    if (openHat.isActive())
        openHat.renderNextBlock(buffer, 0, numSamples);
    
    if (clap.isActive())
        clap.renderNextBlock(buffer, 0, numSamples);
    
    if (rimShot.isActive())
        rimShot.renderNextBlock(buffer, 0, numSamples);
}

void TR808GarageProcessor::handleMidiMessage(const juce::MidiMessage& msg)
{
    int note = msg.getNoteNumber();
    float velocity = msg.getVelocity() / 127.0f;
    
    Voice* voice = getVoiceForNote(note);
    if (voice != nullptr)
        voice->trigger(velocity);
}

Voice* TR808GarageProcessor::getVoiceForNote(int noteNumber)
{
    switch (noteNumber)
    {
        case 36: return &bassDrum;      // C1 - Bass Drum
        case 38: return &snareDrum;     // D1 - Snare Drum
        case 42: return &closedHat;     // F#1 - Closed Hi-Hat
        case 46: return &openHat;       // A#1 - Open Hi-Hat
        case 39: return &clap;          // D#1 - Hand Clap
        case 37: return &rimShot;       // C#1 - Rim Shot
        default: return nullptr;
    }
}

void TR808GarageProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    juce::ignoreUnused(destData);
}

void TR808GarageProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    juce::ignoreUnused(data, sizeInBytes);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TR808GarageProcessor();
}
