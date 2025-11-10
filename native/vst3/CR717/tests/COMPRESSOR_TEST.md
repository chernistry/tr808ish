# Master Bus Compressor - Manual Test Plan

## Unit Test (Offline Render)

The unit test in `tests/unit/dsp/test_compressor.cpp` verifies:

1. **Basic Gain Reduction**: -6dB signal with -12dB threshold, 4:1 ratio → expect ~4.5dB GR
2. **Soft Knee**: Signal at threshold with 6dB knee → expect minimal GR
3. **Below Threshold**: -24dB signal with -12dB threshold → expect ~0dB GR
4. **Ratio Steps**: Test all ratios (1:1, 2:1, 4:1, 8:1, 10:1, 20:1, ∞:1)

To build and run (requires JUCE integration):
```bash
# Add to CMakeLists.txt:
# add_executable(test_compressor tests/unit/dsp/test_compressor.cpp)
# target_link_libraries(test_compressor PRIVATE juce::juce_dsp juce::juce_audio_basics)
```

## Manual DAW Test

### Test 1: Default Settings (2-4dB GR on kick+mix)
1. Load plugin in DAW
2. Create pattern with BD on steps 1, 5, 9, 13
3. Set compressor: Threshold=-12dB, Ratio=4:1, Attack=10ms, Release=100ms
4. Play pattern at -18dBFS input level
5. **Expected**: 2-4dB gain reduction on kick hits

### Test 2: Attack/Release Behavior
1. Set Attack=0.05ms (fast) → should catch transients
2. Set Attack=100ms (slow) → should let transients through
3. Set Release=10ms (fast) → quick recovery
4. Set Release=2000ms (slow) → pumping effect
5. **Expected**: Audible difference in transient handling

### Test 3: SC-HPF (Sidechain High-Pass Filter)
1. Pattern with BD + CH (closed hi-hat)
2. SC-HPF=20Hz → compressor reacts to kick (pumping)
3. SC-HPF=200Hz → compressor ignores kick, reacts to hats
4. **Expected**: Less pumping on kick with higher SC-HPF

### Test 4: Soft Knee
1. Signal near threshold
2. Knee=0dB (hard) → abrupt compression
3. Knee=12dB (soft) → gentle compression
4. **Expected**: Smoother, more musical compression with soft knee

### Test 5: Auto-Makeup
1. Set Threshold=-20dB, Ratio=8:1 (heavy compression)
2. Auto-Makeup=OFF, Makeup=0dB → quiet output
3. Auto-Makeup=ON → output level compensated
4. **Expected**: Similar perceived loudness with auto-makeup

### Test 6: Lookahead
1. Fast transient (clap or snare)
2. Lookahead=0ms → slight overshoot
3. Lookahead=5ms → cleaner transient handling
4. **Expected**: Tighter control with lookahead (at cost of 5ms latency)

### Test 7: RMS vs Peak Detector
1. Pattern with sustained sounds (OH, CY)
2. Detector=RMS → smooth, average-based compression
3. Detector=Peak → faster, transient-based compression
4. **Expected**: RMS feels more musical, Peak more aggressive

## Acceptance Criteria

✓ Default settings yield 2-4dB GR on kick+mix at -18dBFS  
✓ Attack/Release behave logarithmically and feel musical  
✓ SC-HPF reduces pumping on BD without dulling hats  
✓ No clicks/pops on parameter change (SmoothedValue)  
✓ Soft knee provides smooth compression curve  
✓ Auto-makeup maintains perceived loudness  
✓ Lookahead improves transient handling  
✓ RMS/Peak modes offer distinct character

## Performance Check

- CPU usage should remain <10% on typical pattern
- No denormals (ScopedNoDenormals in place)
- No allocations in audio thread
- Stable block processing time
