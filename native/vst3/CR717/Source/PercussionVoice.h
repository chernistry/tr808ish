#pragma once

#include "Voice.h"

class ClapVoice : public Voice
{
public:
    void prepare(double sr, int maxBlockSize) override
    {
        sampleRate = sr;
        level.reset(sr, 0.02);
        tone.reset(sr, 0.02);
        pan.reset(sr, 0.02);
        
        filter.setCoefficients(juce::IIRCoefficients::makeBandPass(sr, 1500.0));
    }

    void trigger(float velocity) override
    {
        env = velocity;
        pulseIndex = 0;
        samplesUntilNextPulse = 0;
        active = true;
        updateSmoothedValues();
    }

    bool isActive() const override { return active && (env > 0.0001f || pulseIndex < 4); }

    void renderNextBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples) override
    {
        if (!active) return;

        // TR-808 spec: 4 pulses at 0, 12ms, 24ms, 36ms
        const int pulseTimes[4] = { 0, static_cast<int>(sampleRate * 0.012), 
                                     static_cast<int>(sampleRate * 0.024), 
                                     static_cast<int>(sampleRate * 0.036) };

        for (int i = 0; i < numSamples; ++i)
        {
            // Trigger pulses at specified times
            if (pulseIndex < 4 && samplesUntilNextPulse <= 0)
            {
                env = 0.9f - (pulseIndex * 0.15f);  // Decreasing amplitude
                pulseIndex++;
                if (pulseIndex < 4)
                    samplesUntilNextPulse = pulseTimes[pulseIndex] - pulseTimes[pulseIndex - 1];
            }
            samplesUntilNextPulse--;

            if (env <= 0.0001f && pulseIndex >= 4)
            {
                active = false;
                break;
            }

            float noise = (random.nextFloat() * 2.0f - 1.0f) * env;
            noise = filter.processSingleSampleRaw(noise);
            
            float sample = noise * level.getNextValue();
            
            // Tail decay: 150ms (TR-808 spec)
            float decayRate = std::exp(-1.0f / (0.15f * static_cast<float>(sampleRate)));
            env *= decayRate;

            applyPan(buffer, startSample + i, numSamples, sample);
        }
    }

private:
    float env = 0.0f;
    int pulseIndex = 0;
    int samplesUntilNextPulse = 0;
    bool active = false;
    juce::Random random;
    juce::IIRFilter filter;
};

class RimShotVoice : public Voice
{
public:
    void prepare(double sr, int maxBlockSize) override
    {
        sampleRate = sr;
        level.reset(sr, 0.02);
        tune.reset(sr, 0.02);
        pan.reset(sr, 0.02);
        
        // TR-808 spec: BP center ~2.5 kHz
        filter.setCoefficients(juce::IIRCoefficients::makeBandPass(sr, 2500.0));
    }

    void trigger(float velocity) override
    {
        phase = 0.0f;
        env = velocity;
        active = true;
        updateSmoothedValues();
    }

    bool isActive() const override { return active && env > 0.0001f; }

    void renderNextBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples) override
    {
        if (!active) return;

        for (int i = 0; i < numSamples; ++i)
        {
            if (env <= 0.0001f)
            {
                active = false;
                break;
            }

            // Metallic tone
            float freq = (540.0f + tune.getNextValue() * 100.0f) / static_cast<float>(sampleRate);
            float tone = std::sin(phase * juce::MathConstants<float>::twoPi);
            phase += freq;
            if (phase >= 1.0f) phase -= 1.0f;

            // Noise
            float noise = (random.nextFloat() * 2.0f - 1.0f);
            noise = filter.processSingleSampleRaw(noise);

            float sample = (tone * 0.3f + noise * 0.7f) * env * level.getNextValue();
            
            // TR-808 spec: Expo decay ~30ms
            float decayRate = std::exp(-1.0f / (0.03f * static_cast<float>(sampleRate)));
            env *= decayRate;

            applyPan(buffer, startSample + i, numSamples, sample);
        }
    }

private:
    float phase = 0.0f;
    float env = 0.0f;
    bool active = false;
    juce::Random random;
    juce::IIRFilter filter;
};
