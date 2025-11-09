#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class Voice
{
public:
    Voice() = default;
    virtual ~Voice() = default;

    virtual void prepare(double sampleRate, int maxBlockSize) = 0;
    virtual void trigger(float velocity) = 0;
    virtual bool isActive() const = 0;
    virtual void renderNextBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples) = 0;
    
    void setLevel(float level) { targetLevel = level; }
    void setTune(float semitones) { targetTune = semitones; }
    void setDecay(float decay) { targetDecay = decay; }
    void setTone(float tone) { targetTone = tone; }

protected:
    double sampleRate = 44100.0;
    
    // Smoothed parameters
    juce::SmoothedValue<float> level{1.0f};
    juce::SmoothedValue<float> tune{0.0f};
    juce::SmoothedValue<float> decay{0.5f};
    juce::SmoothedValue<float> tone{0.5f};
    
    float targetLevel = 1.0f;
    float targetTune = 0.0f;
    float targetDecay = 0.5f;
    float targetTone = 0.5f;
    
    void updateSmoothedValues()
    {
        level.setTargetValue(targetLevel);
        tune.setTargetValue(targetTune);
        decay.setTargetValue(targetDecay);
        tone.setTargetValue(targetTone);
    }
};
