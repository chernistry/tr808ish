#include "../../../Source/MasterDynamics.h"
#include <cassert>
#include <cmath>
#include <iostream>

void testBasicGainReduction()
{
    MasterDynamics comp;
    comp.prepare(48000.0, 512);
    
    // Configure: -12dB threshold, 4:1 ratio, fast attack/release
    comp.setThreshold(-12.0f);
    comp.setRatio(4.0f);
    comp.setAttack(1.0f);
    comp.setRelease(50.0f);
    comp.setKnee(0.0f);
    comp.setMakeup(0.0f);
    comp.setAutoMakeup(false);
    comp.setDetectorMode(true); // RMS
    comp.setLookahead(0.0f);
    
    // Create test buffer: -6dB signal (6dB over threshold)
    juce::AudioBuffer<float> buffer(2, 512);
    float testLevel = juce::Decibels::decibelsToGain(-6.0f);
    
    for (int ch = 0; ch < 2; ++ch)
        for (int i = 0; i < 512; ++i)
            buffer.setSample(ch, i, testLevel);
    
    // Process
    comp.process(buffer, true, false);
    
    // Expected GR: (levelDb - threshold) * (1 - 1/ratio)
    // (-6 - (-12)) * (1 - 1/4) = 6 * 0.75 = 4.5 dB
    float gr = comp.getGainReduction();
    
    std::cout << "Test: Basic GR - Expected ~-4.5dB, Got: " << gr << "dB" << std::endl;
    assert(gr < -2.0f && gr > -8.0f); // Allow envelope settling
}

void testSoftKnee()
{
    MasterDynamics comp;
    comp.prepare(48000.0, 512);
    
    comp.setThreshold(-12.0f);
    comp.setRatio(4.0f);
    comp.setAttack(1.0f);
    comp.setRelease(50.0f);
    comp.setKnee(6.0f); // 6dB knee
    comp.setMakeup(0.0f);
    comp.setAutoMakeup(false);
    comp.setDetectorMode(true);
    comp.setLookahead(0.0f);
    
    // Signal at threshold (should have minimal GR with soft knee)
    juce::AudioBuffer<float> buffer(2, 512);
    float testLevel = juce::Decibels::decibelsToGain(-12.0f);
    
    for (int ch = 0; ch < 2; ++ch)
        for (int i = 0; i < 512; ++i)
            buffer.setSample(ch, i, testLevel);
    
    comp.process(buffer, true, false);
    float gr = comp.getGainReduction();
    
    std::cout << "Test: Soft Knee - Expected minimal GR at threshold, Got: " << gr << "dB" << std::endl;
    assert(gr > -2.0f); // Should be very gentle
}

void testNoCompressionBelowThreshold()
{
    MasterDynamics comp;
    comp.prepare(48000.0, 512);
    
    comp.setThreshold(-12.0f);
    comp.setRatio(4.0f);
    comp.setAttack(1.0f);
    comp.setRelease(50.0f);
    comp.setKnee(0.0f);
    comp.setMakeup(0.0f);
    comp.setAutoMakeup(false);
    comp.setDetectorMode(true);
    comp.setLookahead(0.0f);
    
    // Signal below threshold
    juce::AudioBuffer<float> buffer(2, 512);
    float testLevel = juce::Decibels::decibelsToGain(-24.0f);
    
    for (int ch = 0; ch < 2; ++ch)
        for (int i = 0; i < 512; ++i)
            buffer.setSample(ch, i, testLevel);
    
    comp.process(buffer, true, false);
    float gr = comp.getGainReduction();
    
    std::cout << "Test: Below Threshold - Expected ~0dB GR, Got: " << gr << "dB" << std::endl;
    assert(gr > -1.0f); // Minimal GR
}

void testRatioSteps()
{
    MasterDynamics comp;
    comp.prepare(48000.0, 512);
    
    comp.setThreshold(-12.0f);
    comp.setAttack(1.0f);
    comp.setRelease(50.0f);
    comp.setKnee(0.0f);
    comp.setMakeup(0.0f);
    comp.setAutoMakeup(false);
    comp.setDetectorMode(true);
    comp.setLookahead(0.0f);
    
    // Test different ratios
    const float ratios[] = {1.0f, 2.0f, 4.0f, 8.0f, 10.0f, 20.0f, 100.0f};
    float testLevel = juce::Decibels::decibelsToGain(-6.0f); // 6dB over threshold
    
    for (float ratio : ratios)
    {
        comp.reset();
        comp.setRatio(ratio);
        
        juce::AudioBuffer<float> buffer(2, 512);
        for (int ch = 0; ch < 2; ++ch)
            for (int i = 0; i < 512; ++i)
                buffer.setSample(ch, i, testLevel);
        
        comp.process(buffer, true, false);
        float gr = comp.getGainReduction();
        
        std::cout << "Test: Ratio " << ratio << ":1 - GR: " << gr << "dB" << std::endl;
        
        if (ratio > 1.0f)
            assert(gr < -1.0f); // Should have GR
    }
}

int main()
{
    std::cout << "=== MasterDynamics Compressor Unit Tests ===" << std::endl;
    
    testBasicGainReduction();
    testSoftKnee();
    testNoCompressionBelowThreshold();
    testRatioSteps();
    
    std::cout << "\n✓ All tests passed!" << std::endl;
    return 0;
}
