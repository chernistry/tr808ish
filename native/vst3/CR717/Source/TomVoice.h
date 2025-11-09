#pragma once

#include "Voice.h"

class LowTomVoice : public Voice
{
public:
    void prepare(double sr, int) override
    {
        sampleRate = sr;
        level.reset(sr, 0.02);
        tune.reset(sr, 0.02);
        fineTune.reset(sr, 0.02);
        decay.reset(sr, 0.02);
        pan.reset(sr, 0.02);
    }

    void trigger(float velocity) override
    {
        phase = 0.0f;
        env = velocity;
        pitchEnvTime = 0.0f;
        active = true;
        updateSmoothedValues();
    }

    bool isActive() const override { return active && env > 0.0001f; }

    void renderNextBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples) override
    {
        if (!active) return;

        for (int i = 0; i < numSamples; ++i)
        {
            if (env <= 0.0001f) { active = false; break; }
            
            // TR-808 spec: 130 Hz with pitch bend (10-20ms downward)
            pitchEnvTime += 1.0f / static_cast<float>(sampleRate);
            float pitchBend = (pitchEnvTime < 0.015f) ? (1.0f + 0.05f * std::exp(-pitchEnvTime / 0.005f)) : 1.0f;
            
            float currentTune = tune.getNextValue() + fineTune.getNextValue();
            float baseFreq = 130.0f * std::pow(2.0f, currentTune / 12.0f);
            float freq = (baseFreq * pitchBend) / static_cast<float>(sampleRate);
            
            float sample = std::sin(phase * juce::MathConstants<float>::twoPi) * env * level.getNextValue();
            phase += freq;
            if (phase >= 1.0f) phase -= 1.0f;
            
            // Expo decay: 300ms
            float decayTime = 0.3f * (0.5f + decay.getNextValue() * 0.5f);
            float decayRate = std::exp(-1.0f / (decayTime * static_cast<float>(sampleRate)));
            env *= decayRate;

            applyPan(buffer, startSample + i, numSamples, sample);
        }
    }

private:
    float phase = 0.0f, env = 0.0f, pitchEnvTime = 0.0f;
    bool active = false;
};

class MidTomVoice : public Voice
{
public:
    void prepare(double sr, int) override
    {
        sampleRate = sr;
        level.reset(sr, 0.02);
        tune.reset(sr, 0.02);
        fineTune.reset(sr, 0.02);
        decay.reset(sr, 0.02);
        pan.reset(sr, 0.02);
    }

    void trigger(float velocity) override
    {
        phase = 0.0f;
        env = velocity;
        pitchEnvTime = 0.0f;
        active = true;
        updateSmoothedValues();
    }

    bool isActive() const override { return active && env > 0.0001f; }

    void renderNextBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples) override
    {
        if (!active) return;

        for (int i = 0; i < numSamples; ++i)
        {
            if (env <= 0.0001f) { active = false; break; }
            
            // TR-808 spec: 200 Hz with pitch bend (10-20ms downward)
            pitchEnvTime += 1.0f / static_cast<float>(sampleRate);
            float pitchBend = (pitchEnvTime < 0.015f) ? (1.0f + 0.05f * std::exp(-pitchEnvTime / 0.005f)) : 1.0f;
            
            float currentTune = tune.getNextValue() + fineTune.getNextValue();
            float baseFreq = 200.0f * std::pow(2.0f, currentTune / 12.0f);
            float freq = (baseFreq * pitchBend) / static_cast<float>(sampleRate);
            
            float sample = std::sin(phase * juce::MathConstants<float>::twoPi) * env * level.getNextValue();
            phase += freq;
            if (phase >= 1.0f) phase -= 1.0f;
            
            // Expo decay: 280ms
            float decayTime = 0.28f * (0.5f + decay.getNextValue() * 0.5f);
            float decayRate = std::exp(-1.0f / (decayTime * static_cast<float>(sampleRate)));
            env *= decayRate;

            applyPan(buffer, startSample + i, numSamples, sample);
        }
    }

private:
    float phase = 0.0f, env = 0.0f, pitchEnvTime = 0.0f;
    bool active = false;
};

class HighTomVoice : public Voice
{
public:
    void prepare(double sr, int) override
    {
        sampleRate = sr;
        level.reset(sr, 0.02);
        tune.reset(sr, 0.02);
        fineTune.reset(sr, 0.02);
        decay.reset(sr, 0.02);
        pan.reset(sr, 0.02);
    }

    void trigger(float velocity) override
    {
        phase = 0.0f;
        env = velocity;
        pitchEnvTime = 0.0f;
        active = true;
        updateSmoothedValues();
    }

    bool isActive() const override { return active && env > 0.0001f; }

    void renderNextBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples) override
    {
        if (!active) return;

        for (int i = 0; i < numSamples; ++i)
        {
            if (env <= 0.0001f) { active = false; break; }
            
            // TR-808 spec: 325 Hz with pitch bend (10-20ms downward)
            pitchEnvTime += 1.0f / static_cast<float>(sampleRate);
            float pitchBend = (pitchEnvTime < 0.015f) ? (1.0f + 0.05f * std::exp(-pitchEnvTime / 0.005f)) : 1.0f;
            
            float currentTune = tune.getNextValue() + fineTune.getNextValue();
            float baseFreq = 325.0f * std::pow(2.0f, currentTune / 12.0f);
            float freq = (baseFreq * pitchBend) / static_cast<float>(sampleRate);
            
            float sample = std::sin(phase * juce::MathConstants<float>::twoPi) * env * level.getNextValue();
            phase += freq;
            if (phase >= 1.0f) phase -= 1.0f;
            
            // Expo decay: 220ms
            float decayTime = 0.22f * (0.5f + decay.getNextValue() * 0.5f);
            float decayRate = std::exp(-1.0f / (decayTime * static_cast<float>(sampleRate)));
            env *= decayRate;

            applyPan(buffer, startSample + i, numSamples, sample);
        }
    }

private:
    float phase = 0.0f, env = 0.0f, pitchEnvTime = 0.0f;
    bool active = false;
};
