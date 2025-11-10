#pragma once

#include <juce_dsp/juce_dsp.h>

class MasterDynamics
{
public:
    void prepare(double sampleRate, int maxBlockSize)
    {
        spec = {sampleRate, static_cast<juce::uint32>(maxBlockSize), 2};
        
        // RMS detector window (10ms)
        rmsWindowSize = static_cast<int>(sampleRate * 0.01);
        rmsBuffer.resize(rmsWindowSize * 2, 0.0f);
        rmsIndex = 0;
        
        // Lookahead delay
        lookaheadDelay.prepare(spec);
        lookaheadDelay.setMaximumDelayInSamples(static_cast<int>(sampleRate * 0.005)); // 5ms max
        
        // SC HPF (12dB/oct = 2-pole Butterworth)
        scHpf.prepare(spec);
        scHpf.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 80.0f);
        
        // Limiter
        limiter.prepare(spec);
        limiter.setThreshold(-0.5f);
        limiter.setRelease(50.0f);
        
        // Smoothed parameters
        smoothedThreshold.reset(sampleRate, 0.02);
        smoothedRatio.reset(sampleRate, 0.02);
        smoothedAttack.reset(sampleRate, 0.02);
        smoothedRelease.reset(sampleRate, 0.02);
        smoothedKnee.reset(sampleRate, 0.02);
        smoothedMakeup.reset(sampleRate, 0.02);
        smoothedScHpf.reset(sampleRate, 0.05);
        
        // Auto-makeup RMS tracking (300ms)
        autoMakeupWindowSize = static_cast<int>(sampleRate * 0.3);
        autoMakeupBuffer.resize(autoMakeupWindowSize * 2, 0.0f);
        autoMakeupIndex = 0;
        
        gainReduction = 0.0f;
        envelopeState = 0.0f;
    }

    void setThreshold(float db) { smoothedThreshold.setTargetValue(db); }
    void setRatio(float r) { smoothedRatio.setTargetValue(r); }
    void setAttack(float ms) { smoothedAttack.setTargetValue(ms); }
    void setRelease(float ms) { smoothedRelease.setTargetValue(ms); }
    void setKnee(float db) { smoothedKnee.setTargetValue(db); }
    void setMakeup(float db) { smoothedMakeup.setTargetValue(db); }
    void setAutoMakeup(bool enabled) { autoMakeupEnabled = enabled; }
    void setScHpfFreq(float hz) { smoothedScHpf.setTargetValue(hz); }
    void setDetectorMode(bool useRms) { rmsMode = useRms; }
    void setLookahead(float ms) { lookaheadMs = ms; }
    void setLimiterThreshold(float db) { limiter.setThreshold(db); }

    void process(juce::AudioBuffer<float>& buffer, bool compEnabled, bool limiterEnabled)
    {
        juce::ScopedNoDenormals noDenormals;
        
        if (compEnabled)
            processCompressor(buffer);
        
        if (limiterEnabled)
        {
            juce::dsp::AudioBlock<float> block(buffer);
            limiter.process(juce::dsp::ProcessContextReplacing<float>(block));
        }
        
        softClip(buffer);
    }

    float getGainReduction() const { return gainReduction; }

    void reset()
    {
        lookaheadDelay.reset();
        scHpf.reset();
        limiter.reset();
        std::fill(rmsBuffer.begin(), rmsBuffer.end(), 0.0f);
        std::fill(autoMakeupBuffer.begin(), autoMakeupBuffer.end(), 0.0f);
        rmsIndex = 0;
        autoMakeupIndex = 0;
        gainReduction = 0.0f;
        envelopeState = 0.0f;
    }

private:
    void processCompressor(juce::AudioBuffer<float>& buffer)
    {
        const int numSamples = buffer.getNumSamples();
        const int numChannels = buffer.getNumChannels();
        
        // Update SC HPF if needed
        float targetScHpf = smoothedScHpf.getNextValue();
        if (std::abs(targetScHpf - currentScHpf) > 0.1f)
        {
            currentScHpf = targetScHpf;
            scHpf.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(spec.sampleRate, currentScHpf);
        }
        
        // Lookahead delay setup
        int lookaheadSamples = static_cast<int>(spec.sampleRate * lookaheadMs * 0.001f);
        lookaheadDelay.setDelay(static_cast<float>(lookaheadSamples));
        
        // Attack/Release coefficients
        float attackCoeff = std::exp(-1.0f / (smoothedAttack.getNextValue() * 0.001f * spec.sampleRate));
        float releaseCoeff = std::exp(-1.0f / (smoothedRelease.getNextValue() * 0.001f * spec.sampleRate));
        
        float inputRms = 0.0f, outputRms = 0.0f;
        
        for (int i = 0; i < numSamples; ++i)
        {
            // Detector path: stereo sum
            float detectorSample = 0.0f;
            for (int ch = 0; ch < numChannels; ++ch)
                detectorSample += buffer.getSample(ch, i);
            detectorSample /= numChannels;
            
            // SC HPF
            float scFiltered = scHpf.processSample(detectorSample);
            
            // Level detection (RMS or Peak)
            float level = 0.0f;
            if (rmsMode)
            {
                // RMS window
                int idx = rmsIndex % rmsWindowSize;
                rmsBuffer[idx] = scFiltered * scFiltered;
                float sum = 0.0f;
                for (int j = 0; j < rmsWindowSize; ++j)
                    sum += rmsBuffer[j];
                level = std::sqrt(sum / rmsWindowSize);
                rmsIndex++;
            }
            else
            {
                level = std::abs(scFiltered);
            }
            
            float levelDb = juce::Decibels::gainToDecibels(level + 1e-6f);
            
            // Gain computer with soft knee
            float threshold = smoothedThreshold.getCurrentValue();
            float ratio = smoothedRatio.getCurrentValue();
            float knee = smoothedKnee.getCurrentValue();
            
            float gr = 0.0f;
            if (knee > 0.1f)
            {
                // Soft knee
                float kneeStart = threshold - knee * 0.5f;
                float kneeEnd = threshold + knee * 0.5f;
                
                if (levelDb < kneeStart)
                    gr = 0.0f;
                else if (levelDb > kneeEnd)
                    gr = (threshold - levelDb) + (levelDb - threshold) / ratio;
                else
                {
                    float x = levelDb - kneeStart;
                    float w = knee;
                    gr = (x * x) / (2.0f * w * ratio);
                }
            }
            else
            {
                // Hard knee
                if (levelDb > threshold)
                    gr = (threshold - levelDb) + (levelDb - threshold) / ratio;
            }
            
            // Envelope follower
            float targetGain = juce::Decibels::decibelsToGain(gr);
            if (targetGain < envelopeState)
                envelopeState = attackCoeff * envelopeState + (1.0f - attackCoeff) * targetGain;
            else
                envelopeState = releaseCoeff * envelopeState + (1.0f - releaseCoeff) * targetGain;
            
            // Track input RMS for auto-makeup
            if (autoMakeupEnabled)
            {
                int amIdx = autoMakeupIndex % autoMakeupWindowSize;
                autoMakeupBuffer[amIdx] = detectorSample * detectorSample;
                autoMakeupIndex++;
            }
            
            // Apply gain reduction with lookahead
            for (int ch = 0; ch < numChannels; ++ch)
            {
                float sample = buffer.getSample(ch, i);
                
                if (lookaheadSamples > 0)
                {
                    lookaheadDelay.pushSample(ch, sample);
                    sample = lookaheadDelay.popSample(ch);
                }
                
                sample *= envelopeState;
                
                // Track output RMS for auto-makeup
                if (autoMakeupEnabled && ch == 0)
                    outputRms += sample * sample;
                
                buffer.setSample(ch, i, sample);
            }
            
            if (autoMakeupEnabled && i == 0)
                inputRms = detectorSample * detectorSample;
        }
        
        // Auto-makeup calculation
        float makeup = smoothedMakeup.getNextValue();
        if (autoMakeupEnabled && numSamples > 0)
        {
            float inRms = 0.0f, outRms = 0.0f;
            for (int j = 0; j < autoMakeupWindowSize; ++j)
            {
                inRms += autoMakeupBuffer[j];
                outRms += autoMakeupBuffer[j] * envelopeState * envelopeState;
            }
            inRms = std::sqrt(inRms / autoMakeupWindowSize);
            outRms = std::sqrt(outRms / autoMakeupWindowSize);
            
            if (outRms > 1e-6f)
            {
                float autoGain = inRms / outRms;
                makeup = juce::Decibels::gainToDecibels(autoGain);
                makeup = juce::jlimit(-12.0f, 24.0f, makeup);
            }
        }
        
        // Apply makeup gain
        float makeupGain = juce::Decibels::decibelsToGain(makeup);
        buffer.applyGain(makeupGain);
        
        // Store GR for metering
        gainReduction = juce::Decibels::gainToDecibels(envelopeState);
    }

    void softClip(juce::AudioBuffer<float>& buffer)
    {
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            auto* data = buffer.getWritePointer(ch);
            for (int i = 0; i < buffer.getNumSamples(); ++i)
                data[i] = std::tanh(data[i] * 0.9f);
        }
    }

    juce::dsp::ProcessSpec spec;
    juce::dsp::DelayLine<float> lookaheadDelay{1024};
    juce::dsp::IIR::Filter<float> scHpf;
    juce::dsp::Limiter<float> limiter;
    
    juce::SmoothedValue<float> smoothedThreshold, smoothedRatio, smoothedAttack, smoothedRelease;
    juce::SmoothedValue<float> smoothedKnee, smoothedMakeup, smoothedScHpf;
    
    std::vector<float> rmsBuffer, autoMakeupBuffer;
    int rmsWindowSize = 0, rmsIndex = 0;
    int autoMakeupWindowSize = 0, autoMakeupIndex = 0;
    
    float gainReduction = 0.0f, envelopeState = 0.0f;
    float currentScHpf = 80.0f, lookaheadMs = 0.0f;
    bool rmsMode = true, autoMakeupEnabled = false;
};
