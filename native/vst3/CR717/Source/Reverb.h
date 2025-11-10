#pragma once

#include <juce_dsp/juce_dsp.h>

class AlgorithmicReverb
{
public:
    void prepare(double sampleRate, int maxBlockSize)
    {
        this->sampleRate = sampleRate;
        
        juce::dsp::ProcessSpec spec;
        spec.sampleRate = sampleRate;
        spec.maximumBlockSize = static_cast<juce::uint32>(maxBlockSize);
        spec.numChannels = 2;
        reverb.prepare(spec);
        
        // Pre-delay (max 100ms)
        preDelayLine.prepare(spec);
        preDelayLine.setMaximumDelayInSamples(static_cast<int>(sampleRate * 0.1));
        preDelayLine.setDelay(static_cast<float>(sampleRate * 0.02)); // 20ms default
        
        juce::dsp::Reverb::Parameters params;
        params.roomSize = 0.5f;
        params.damping = 0.5f;
        params.wetLevel = 0.33f;
        params.dryLevel = 0.0f;
        params.width = 1.0f;
        params.freezeMode = 0.0f;
        reverb.setParameters(params);
        
        diffusion = 0.7f;
    }

    void setRoomSize(float size)
    {
        auto params = reverb.getParameters();
        params.roomSize = juce::jlimit(0.0f, 1.0f, size);
        reverb.setParameters(params);
    }

    void setDamping(float damp)
    {
        auto params = reverb.getParameters();
        params.damping = juce::jlimit(0.0f, 1.0f, damp);
        reverb.setParameters(params);
    }

    void setWidth(float width)
    {
        auto params = reverb.getParameters();
        params.width = juce::jlimit(0.0f, 1.0f, width);
        reverb.setParameters(params);
    }

    void setWetLevel(float wet)
    {
        auto params = reverb.getParameters();
        params.wetLevel = juce::jlimit(0.0f, 1.0f, wet);
        reverb.setParameters(params);
    }

    void setPreDelay(float ms)
    {
        float delaySamples = (ms * 0.001f) * sampleRate;
        preDelayLine.setDelay(delaySamples);
    }

    void setDiffusion(float diff)
    {
        diffusion = juce::jlimit(0.0f, 1.0f, diff);
    }

    void process(juce::AudioBuffer<float>& buffer)
    {
        juce::dsp::AudioBlock<float> block(buffer);
        
        // Apply pre-delay
        juce::dsp::ProcessContextReplacing<float> preDelayContext(block);
        preDelayLine.process(preDelayContext);
        
        // Apply reverb
        juce::dsp::ProcessContextReplacing<float> context(block);
        reverb.process(context);
    }

    void reset()
    {
        reverb.reset();
        preDelayLine.reset();
    }

private:
    juce::dsp::Reverb reverb;
    juce::dsp::DelayLine<float> preDelayLine{8192};
    double sampleRate = 48000.0;
    float diffusion = 0.7f;
};
