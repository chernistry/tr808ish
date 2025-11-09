#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "BassDrumVoice.h"
#include "SnareDrumVoice.h"
#include "HiHatVoice.h"
#include "PercussionVoice.h"

class TR808GarageProcessor : public juce::AudioProcessor
{
public:
    TR808GarageProcessor();
    ~TR808GarageProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override { return nullptr; }
    bool hasEditor() const override { return false; }

    const juce::String getName() const override { return "TR-808 Garage"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

private:
    // Voice instances (polyphony = 1 per voice type)
    BassDrumVoice bassDrum;
    SnareDrumVoice snareDrum;
    ClosedHiHatVoice closedHat;
    OpenHiHatVoice openHat;
    ClapVoice clap;
    RimShotVoice rimShot;

    void handleMidiMessage(const juce::MidiMessage& msg);
    Voice* getVoiceForNote(int noteNumber);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TR808GarageProcessor)
};
