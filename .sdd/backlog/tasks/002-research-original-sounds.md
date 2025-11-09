I need detailed technical information about Roland TR-808 sound synthesis for software implementation. For each of the 12 instruments, precise synthesis parameters are required:

**Instruments for analysis:**
1. Bass Drum (BD)
2. Snare Drum (SD)
3. Low Tom (LT)
4. Mid Tom (MT)
5. High Tom (HT)
6. Rimshot (RS)
7. Handclap (CP)
8. Closed Hi-Hat (CH)
9. Open Hi-Hat (OH)
10. Crash Cymbal (CY)
11. Ride Cymbal (RD)
12. Cowbell (CB)

**For each instrument find:**

1. **Oscillators:**
   - Wave type (sine, triangle, square, noise)
   - Base frequency in Hz
   - Number of oscillators and their ratio
   - Pitch envelope (initial frequency → final frequency, time)

2. **Amplitude envelope (Envelope):**
   - Attack time (ms)
   - Decay time (ms)
   - Sustain level (if present)
   - Release time (ms)
   - Curve shape (linear, exponential)

3. **Filters:**
   - Type (lowpass, highpass, bandpass)
   - Cutoff frequency (Hz)
   - Resonance/Q factor
   - Filter envelope (if present)

4. **Noise component:**
   - Noise type (white, pink, filtered)
   - Mix level with tonal component (%)
   - Noise filtering (cutoff frequencies)

5. **Specific parameters:**
   - Distortion/saturation (if present)
   - Bridged-T network parameters (for BD)
   - Decay control range
   - Tone control range
   - Tuning range (semitones/Hz)

6. **Interactions:**
   - Hi-hat choke group (OH mutes CH)
   - Accent behavior (how velocity affects)

**Sources for search:**
- Original Roland TR-808 schematics
- Reverse engineering analyses (e.g., from Behringer RD-8)
- Scientific articles about drum synthesis
- Documentation from clone developers (Acidlab, Yocto, etc.)
- DSP forums (KVR, Gearspace, Sound on Sound)
- Roland patents on drum synthesis

**Response format:**
For each instrument provide a structured table with precise numerical values, not ranges. If there are multiple sources with different data - indicate all variants with links to sources.

Example desired format for one instrument:

**Bass Drum (BD):**
- OSC1: Sine wave, 65 Hz → 55 Hz, pitch decay 8ms
- OSC2: Triangle wave (bridged-T), 55 Hz, decay 200-800ms
- Amplitude envelope: instant attack, exponential decay 200-800ms
- Filter: 6dB/oct lowpass, 80-120 Hz (tone control)
- Distortion: soft clipping at 0.8 threshold
- Tuning range: ±12 semitones
- Accent: +6dB, faster attack

Find the most precise technical specifications for authentic TR-808 sound.
