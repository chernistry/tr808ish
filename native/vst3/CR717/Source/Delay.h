#pragma once

#include <juce_dsp/juce_dsp.h>

class TempoSyncDelay
{
public:
    enum StereoMode { Mono, PingPong, Stereo };
    
    void prepare(double sampleRate, int maxBlockSize)
    {
        this->sampleRate = sampleRate;
        
        int maxDelaySamples = static_cast<int>(sampleRate * 2.0); // 2 seconds max
        delayLineL.resize(maxDelaySamples, 0.0f);
        delayLineR.resize(maxDelaySamples, 0.0f);
        
        writePos = 0;
        lfoPhase = 0.0f;
        
        lpFilter.setCoefficients(juce::IIRCoefficients::makeLowPass(sampleRate, 8000.0));
        hpFilter.setCoefficients(juce::IIRCoefficients::makeHighPass(sampleRate, 200.0));
    }

    void setDelayTime(float beats, double bpm)
    {
        // Convert beats to seconds
        double delaySeconds = (beats * 60.0) / bpm;
        delaySamples = static_cast<int>(delaySeconds * sampleRate);
        delaySamples = juce::jlimit(1, static_cast<int>(delayLineL.size()) - 1, delaySamples);
    }

    void setFeedback(float fb)
    {
        feedback = juce::jlimit(0.0f, 0.95f, fb);
    }

    void setWetLevel(float wet)
    {
        wetLevel = juce::jlimit(0.0f, 1.0f, wet);
    }

    void setStereoMode(StereoMode mode)
    {
        stereoMode = mode;
    }

    void setModulation(float rateHz, float depthMs)
    {
        modRate = juce::jlimit(0.1f, 5.0f, rateHz);
        modDepth = juce::jlimit(0.0f, 10.0f, depthMs);
    }

    void process(juce::AudioBuffer<float>& buffer)
    {
        auto* leftChannel = buffer.getWritePointer(0);
        auto* rightChannel = buffer.getWritePointer(1);
        int numSamples = buffer.getNumSamples();

        for (int i = 0; i < numSamples; ++i)
        {
            // LFO for modulation
            float lfo = std::sin(lfoPhase * juce::MathConstants<float>::twoPi);
            lfoPhase += modRate / sampleRate;
            if (lfoPhase >= 1.0f) lfoPhase -= 1.0f;
            
            float modOffset = lfo * modDepth * 0.001f * sampleRate;
            
            // Read from delay line with modulation
            int baseReadPos = (writePos - delaySamples + delayLineL.size()) % delayLineL.size();
            int modReadPos = (baseReadPos + static_cast<int>(modOffset) + delayLineL.size()) % delayLineL.size();
            
            float delayedL = delayLineL[modReadPos];
            float delayedR = delayLineR[modReadPos];

            // Apply filters to feedback
            delayedL = lpFilter.processSingleSampleRaw(delayedL);
            delayedL = hpFilter.processSingleSampleRaw(delayedL);
            delayedR = lpFilter.processSingleSampleRaw(delayedR);
            delayedR = hpFilter.processSingleSampleRaw(delayedR);

            // Stereo mode processing
            float feedbackL = delayedL;
            float feedbackR = delayedR;
            
            if (stereoMode == PingPong)
            {
                // Swap channels in feedback path
                feedbackL = delayedR;
                feedbackR = delayedL;
            }
            else if (stereoMode == Mono)
            {
                // Sum to mono
                float mono = (delayedL + delayedR) * 0.5f;
                feedbackL = feedbackR = mono;
            }

            // Write to delay line (input + feedback)
            delayLineL[writePos] = leftChannel[i] + feedbackL * feedback;
            delayLineR[writePos] = rightChannel[i] + feedbackR * feedback;

            // Mix wet/dry
            leftChannel[i] = leftChannel[i] * (1.0f - wetLevel) + delayedL * wetLevel;
            rightChannel[i] = rightChannel[i] * (1.0f - wetLevel) + delayedR * wetLevel;

            writePos = (writePos + 1) % delayLineL.size();
        }
    }

    void reset()
    {
        std::fill(delayLineL.begin(), delayLineL.end(), 0.0f);
        std::fill(delayLineR.begin(), delayLineR.end(), 0.0f);
        writePos = 0;
        lfoPhase = 0.0f;
        lpFilter.reset();
        hpFilter.reset();
    }

private:
    double sampleRate = 44100.0;
    std::vector<float> delayLineL, delayLineR;
    int writePos = 0;
    int delaySamples = 0;
    float feedback = 0.5f;
    float wetLevel = 0.5f;
    float lfoPhase = 0.0f;
    float modRate = 0.0f;
    float modDepth = 0.0f;
    StereoMode stereoMode = Stereo;
    juce::IIRFilter lpFilter, hpFilter;
};
