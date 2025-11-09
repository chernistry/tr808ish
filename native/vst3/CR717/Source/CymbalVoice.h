#pragma once

#include "Voice.h"

// TR-808 spec: Six square-wave oscillators at exact frequencies (same as hats)
static const float CYMBAL_OSC_FREQS[6] = { 205.3f, 304.4f, 369.6f, 522.7f, 540.0f, 800.0f };

class CymbalVoice : public Voice
{
public:
    void prepare(double sr, int) override
    {
        sampleRate = sr;
        level.reset(sr, 0.02);
        tone.reset(sr, 0.02);
        decay.reset(sr, 0.02);
        pan.reset(sr, 0.02);
        
        // Dual 3rd-order BPF at 3.44 kHz and 7.10 kHz (same as hats)
        bp1.setCoefficients(juce::IIRCoefficients::makeBandPass(sr, 3440.0, 2.0));
        bp2.setCoefficients(juce::IIRCoefficients::makeBandPass(sr, 7100.0, 2.0));
        // Additional HP for brightness
        hp.setCoefficients(juce::IIRCoefficients::makeHighPass(sr, 8000.0));
        
        for (int i = 0; i < 6; ++i) {
            phases[i] = 0.0f;
        }
    }

    void trigger(float velocity) override { env = velocity; active = true; updateSmoothedValues(); }
    bool isActive() const override { return active && env > 0.0001f; }

    void renderNextBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples) override
    {
        if (!active) return;

        for (int i = 0; i < numSamples; ++i)
        {
            if (env <= 0.0001f) { active = false; break; }
            
            // Generate 6 square oscillators
            float oscSum = 0.0f;
            for (int j = 0; j < 6; ++j) {
                float freq = CYMBAL_OSC_FREQS[j] / static_cast<float>(sampleRate);
                oscSum += (phases[j] < 0.5f ? 1.0f : -1.0f) * 0.15f;
                phases[j] += freq;
                if (phases[j] >= 1.0f) phases[j] -= 1.0f;
            }
            
            // Apply dual BPF + HP for brightness
            float filtered = bp1.processSingleSampleRaw(oscSum);
            filtered = bp2.processSingleSampleRaw(filtered);
            filtered = hp.processSingleSampleRaw(filtered);
            
            float sample = filtered * env * level.getNextValue();
            
            // Expo decay: 1.2s (TR-808 spec)
            float decayTime = 1.2f * (0.5f + decay.getNextValue() * 0.5f);
            float decayRate = std::exp(-1.0f / (decayTime * static_cast<float>(sampleRate)));
            env *= decayRate;

            applyPan(buffer, startSample + i, numSamples, sample);
        }
    }

private:
    float phases[6] = {0};
    float env = 0.0f;
    bool active = false;
    juce::IIRFilter bp1, bp2, hp;
};

class RideVoice : public Voice
{
public:
    void prepare(double sr, int) override
    {
        sampleRate = sr;
        level.reset(sr, 0.02);
        tone.reset(sr, 0.02);
        pan.reset(sr, 0.02);
        
        // Dual 3rd-order BPF at 3.44 kHz and 7.10 kHz (same as hats/cymbal)
        bp1.setCoefficients(juce::IIRCoefficients::makeBandPass(sr, 3440.0, 2.0));
        bp2.setCoefficients(juce::IIRCoefficients::makeBandPass(sr, 7100.0, 2.0));
        // Additional HP for brightness
        hp.setCoefficients(juce::IIRCoefficients::makeHighPass(sr, 7500.0));
        
        for (int i = 0; i < 6; ++i) {
            phases[i] = 0.0f;
        }
    }

    void trigger(float velocity) override { env = velocity; active = true; updateSmoothedValues(); }
    bool isActive() const override { return active && env > 0.0001f; }

    void renderNextBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples) override
    {
        if (!active) return;

        for (int i = 0; i < numSamples; ++i)
        {
            if (env <= 0.0001f) { active = false; break; }
            
            // Generate 6 square oscillators
            float oscSum = 0.0f;
            for (int j = 0; j < 6; ++j) {
                float freq = CYMBAL_OSC_FREQS[j] / static_cast<float>(sampleRate);
                oscSum += (phases[j] < 0.5f ? 1.0f : -1.0f) * 0.15f;
                phases[j] += freq;
                if (phases[j] >= 1.0f) phases[j] -= 1.0f;
            }
            
            // Apply dual BPF + HP for brightness
            float filtered = bp1.processSingleSampleRaw(oscSum);
            filtered = bp2.processSingleSampleRaw(filtered);
            filtered = hp.processSingleSampleRaw(filtered);
            
            float sample = filtered * env * level.getNextValue();
            
            // Expo decay: 1.9s (TR-808 spec, longer than cymbal)
            float decayRate = std::exp(-1.0f / (1.9f * static_cast<float>(sampleRate)));
            env *= decayRate;

            applyPan(buffer, startSample + i, numSamples, sample);
        }
    }

private:
    float phases[6] = {0};
    float env = 0.0f;
    bool active = false;
    juce::IIRFilter bp1, bp2, hp;
};

class CowbellVoice : public Voice
{
public:
    void prepare(double sr, int) override
    {
        sampleRate = sr;
        level.reset(sr, 0.02);
        tune.reset(sr, 0.02);
        pan.reset(sr, 0.02);
        filter.setCoefficients(juce::IIRCoefficients::makeBandPass(sr, 800.0));
    }

    void trigger(float velocity) override { phase1 = 0.0f; phase2 = 0.0f; env = velocity; active = true; updateSmoothedValues(); }
    bool isActive() const override { return active && env > 0.0001f; }

    void renderNextBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples) override
    {
        if (!active) return;

        for (int i = 0; i < numSamples; ++i)
        {
            if (env <= 0.0001f) { active = false; break; }
            float tuneVal = tune.getNextValue();
            float f1 = (540.0f + tuneVal * 50.0f) / static_cast<float>(sampleRate);
            float f2 = (800.0f + tuneVal * 70.0f) / static_cast<float>(sampleRate);
            
            float osc = std::sin(phase1 * juce::MathConstants<float>::twoPi) * 0.5f +
                       std::sin(phase2 * juce::MathConstants<float>::twoPi) * 0.5f;
            phase1 += f1; if (phase1 >= 1.0f) phase1 -= 1.0f;
            phase2 += f2; if (phase2 >= 1.0f) phase2 -= 1.0f;
            
            float sample = filter.processSingleSampleRaw(osc) * env * level.getNextValue();
            env *= 0.992f;
            applyPan(buffer, startSample + i, numSamples, sample);
        }
    }

private:
    float phase1 = 0.0f, phase2 = 0.0f, env = 0.0f;
    bool active = false;
    juce::IIRFilter filter;
};
