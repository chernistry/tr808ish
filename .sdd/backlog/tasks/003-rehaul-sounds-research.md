# Research: Master Dynamics, FX, and Enhanced Voice Parameters for CR-717

I need detailed technical specifications for implementing professional-grade dynamics processing, effects, and enhanced synthesis parameters in the CR-717 drum synth (both web and VST3 versions). All parameters must be production-ready with proper calibration ranges.

---

## 1. MASTER DYNAMICS CHAIN

### 1.1 Compressor Specifications

**For each parameter provide:**
- **Attack time**: range (ms), curve type (linear/log), typical values for drums
- **Release time**: range (ms), curve type, auto-release behavior
- **Ratio**: range (1:1 to ∞:1), stepped or continuous, typical drum bus values
- **Threshold**: range (dBFS), default value
- **Knee**: hard/soft knee, knee width range (dB)
- **Makeup gain**: range (dB), auto-makeup algorithm
- **Sidechain filter**: HPF frequency (Hz) to avoid bass pumping
- **Detection**: peak vs RMS, window size (ms)
- **Lookahead**: time (ms), buffer size implications

**Reference implementations to analyze:**
- SSL Bus Compressor (attack/release curves)
- API 2500 (thrust/feedback modes)
- Empirical Labs Distressor (ratios, saturation)
- Modern digital: FabFilter Pro-C 2, Waves SSL G-Master

**Desired format:**
```
Attack: 0.1–100ms, logarithmic scale, default 10ms
Release: 10–1000ms, logarithmic + auto mode (1.5× attack), default 100ms
Ratio: 1:1, 2:1, 4:1, 8:1, 20:1, ∞:1 (stepped), default 4:1
Threshold: -40 to 0 dBFS, default -12 dBFS
Knee: 0–12 dB, default 3 dB (soft)
Makeup: 0–24 dB + auto (target -3 dBFS RMS)
Sidechain HPF: 20–500 Hz, default 80 Hz
Detection: RMS 10ms window
Lookahead: 0–5ms, default 0ms (real-time priority)
```

### 1.2 Limiter Specifications

**For each parameter provide:**
- **Threshold**: range (dBFS), typical mastering values
- **Release time**: range (ms), adaptive release behavior
- **Lookahead**: time (ms), latency trade-offs
- **Oversampling**: factor (2x/4x/8x), CPU cost vs quality
- **Ceiling**: output ceiling (dBFS), intersample peak handling
- **Knee**: soft knee width (dB) or brick-wall
- **Algorithm type**: peak, RMS, true-peak (ITU-R BS.1770)

**Reference implementations:**
- FabFilter Pro-L 2 (true-peak, oversampling)
- Waves L2 (ARC, release curves)
- iZotope Ozone Maximizer
- Voxengo Elephant

**Desired format:**
```
Threshold: -20 to 0 dBFS, default -0.3 dBFS
Release: 10–1000ms + auto (program-dependent), default auto
Lookahead: 0–10ms, default 5ms (acceptable latency)
Oversampling: 4x (balance quality/CPU)
Ceiling: -0.3 to 0 dBFS, default -0.1 dBFS (true-peak)
Knee: 0–3 dB, default 0 dB (brick-wall)
Algorithm: True-peak (ITU-R BS.1770-4)
```

### 1.3 Soft Clipper Specifications

**For each parameter provide:**
- **Drive/Input gain**: range (dB), pre-clipping gain
- **Threshold**: where clipping starts (dBFS or 0–100%)
- **Curve type**: tanh, atan, polynomial (x - x³/3), asymmetric
- **Oversampling**: required factor to avoid aliasing
- **Mix/Dry-Wet**: parallel processing amount (%)
- **Output gain**: post-clipping compensation (dB)

**Reference implementations:**
- Decapitator (saturation curves A/E/N/T)
- FabFilter Saturn 2 (multiband, curves)
- Soundtoys Decapitator
- Analog modeling: tape saturation (Studer, Ampex curves)

**Desired format:**
```
Drive: 0–24 dB, default 0 dB
Threshold: 0–100%, default 80% (-1.9 dBFS)
Curve: tanh (smooth), atan (harder), polynomial (vintage)
Oversampling: 4x minimum for nonlinear processing
Mix: 0–100%, default 100% (full wet)
Output: -12 to +12 dB, auto-compensate by drive amount
```

---

## 2. FX PROCESSING (REVERB & DELAY)

### 2.1 Reverb Specifications

**For each parameter provide:**
- **Algorithm type**: Freeverb, Schroeder, convolution, plate/room/hall
- **Room size**: range (m² or 0–100%), early reflections pattern
- **Damping/HF decay**: frequency (Hz), decay time multiplier
- **Pre-delay**: range (ms), typical drum values
- **Wet/Dry mix**: range (%), per-voice send levels
- **Diffusion**: range (%), density of reflections
- **Width/Stereo**: mono/stereo, width control
- **Early/Late balance**: ER vs reverb tail mix
- **Modulation**: chorus/shimmer depth (Hz, %)

**Reference implementations:**
- Valhalla Room/VintageVerb (algorithm details)
- Lexicon 224/480L (plate/hall characteristics)
- Freeverb (open-source, comb+allpass structure)
- Zynaptiq Adaptiverb

**Desired format:**
```
Algorithm: Freeverb (8 comb + 4 allpass), stereo
Room size: 0–100%, default 50% (~20m² equivalent)
Damping: 1–16 kHz, default 8 kHz (-6dB/oct above)
Pre-delay: 0–100ms, default 20ms (drums)
Wet/Dry: 0–100%, default 30%
Diffusion: 0–100%, default 70%
Width: 0–100%, default 100% (full stereo)
Early/Late: 0–100% (0=only tail), default 50%
Modulation: 0.1–2 Hz, 0–10% depth, default off
```

### 2.2 Delay Specifications

**For each parameter provide:**
- **Time**: range (ms or BPM-synced), max buffer size
- **Feedback**: range (%), self-oscillation threshold
- **Wet/Dry mix**: range (%), per-voice send levels
- **Filter**: HPF/LPF in feedback loop (Hz, Q)
- **Stereo mode**: mono, ping-pong, stereo spread
- **Modulation**: chorus/flanger depth (Hz, ms)
- **Saturation**: feedback path saturation (drive amount)
- **Tempo sync**: note divisions (1/4, 1/8, 1/16, dotted, triplet)

**Reference implementations:**
- Echoboy (tape/analog delay models)
- H-Delay (analog BBD modeling)
- Valhalla Delay
- Roland Space Echo RE-201

**Desired format:**
```
Time: 1–2000ms or BPM-synced (1/64 to 4 bars), default 375ms (1/8 @ 120 BPM)
Feedback: 0–95%, default 40% (oscillation at ~98%)
Wet/Dry: 0–100%, default 25%
HPF: 20–1000 Hz, default 100 Hz (mud removal)
LPF: 1k–20k Hz, default 8 kHz (analog darkening)
Stereo: mono, ping-pong (L→R→L), stereo (independent L/R)
Modulation: 0.1–5 Hz, 0–10ms depth, default off
Saturation: 0–12 dB drive in feedback, default 0 dB
Tempo sync: on/off, divisions array
```

---

## 3. ENHANCED PER-VOICE PARAMETERS

### 3.1 Filter Parameters (per voice)

**For each parameter provide:**
- **Filter types**: LP/HP/BP/Notch, pole count (12/24 dB/oct)
- **Cutoff frequency**: range (Hz), musical scaling (log)
- **Resonance/Q**: range (0–100% or Q factor), self-oscillation
- **Envelope amount**: modulation depth (-100% to +100%)
- **Envelope shape**: attack/decay times (ms), curve type
- **Key tracking**: cutoff follows pitch (0–100%)
- **Drive**: pre-filter saturation (dB)

**Reference implementations:**
- Moog ladder filter (24dB/oct LP, resonance)
- State Variable Filter (SVF) - simultaneous LP/HP/BP
- Oberheim SEM filter (12dB/oct multimode)
- Korg MS-20 filter (aggressive resonance)

**Desired format:**
```
Type: LP 24dB (Moog-style), LP 12dB, HP 12dB, BP 12dB
Cutoff: 20 Hz – 20 kHz, logarithmic, default per-voice:
  - BD: 80 Hz (LP), SD: 3 kHz (BP), HH: 8 kHz (HP)
Resonance: 0–100%, default 10% (Q ≈ 0.7 to 10)
Envelope: -100% to +100%, default +50% (opens filter)
Attack: 0–100ms, Decay: 10–2000ms, exponential curve
Key tracking: 0–100%, default 0% (drums don't track)
Drive: 0–24 dB, default 0 dB
```

### 3.2 Pan & Stereo Parameters

**For each parameter provide:**
- **Pan position**: range (L100–C–R100 or -100 to +100)
- **Stereo width**: per-voice width control (0–200%)
- **Pan law**: -3dB, -4.5dB, -6dB (constant power/linear)
- **Random pan**: per-hit randomization amount (%)

**Desired format:**
```
Pan: -100 to +100 (L to R), default 0 (center)
Width: 0–200%, default 100% (0=mono, 200=extra wide)
Pan law: -3dB (constant power), default
Random: 0–100%, default 0% (0=fixed, 100=full random per hit)
```

### 3.3 Velocity & Accent

**For each parameter provide:**
- **Velocity curve**: linear, exponential, logarithmic, S-curve
- **Velocity → Level**: mapping range (dB at vel=1 to vel=127)
- **Velocity → Filter**: cutoff modulation amount (Hz or %)
- **Velocity → Decay**: envelope time modulation (%)
- **Accent amount**: additional gain (dB), filter boost (Hz)
- **Accent curve**: how accent scales with velocity

**Reference implementations:**
- TR-808 accent circuit (+6dB, faster attack)
- MIDI velocity standards (GM Level 1)
- Modern drum machines (Elektron, MPC velocity curves)

**Desired format:**
```
Curve: exponential (vel² scaling), default
Level: 0 dB @ vel=127, -24 dB @ vel=1, default
Filter: +0 to +2 octaves @ vel=127, default +1 oct
Decay: 50–150% @ vel range, default 100% (no change)
Accent: +6 dB, +1 octave filter, -20% decay (snappier)
Accent curve: linear with velocity (0% @ vel=1, 100% @ vel=127)
```

### 3.4 Pitch Envelope Enhancements

**For each parameter provide:**
- **Curve shapes**: linear, exponential (τ constant), logarithmic
- **Start/End frequency**: ratio or Hz, per-voice defaults
- **Envelope time**: range (ms), velocity modulation
- **Pitch amount**: depth control (semitones or ratio)
- **Retrigger mode**: mono (legato) vs poly (always retrigger)

**Desired format:**
```
Curve: exponential (e^(-t/τ)), τ = decay_time / 5
Start freq: 2–4× end freq (BD: 150→50 Hz = 3×)
End freq: base pitch per voice
Time: 1–500ms, default per-voice (BD: 80ms, SD: 30ms)
Amount: 0–400% (0=no envelope, 100=default, 400=extreme)
Retrigger: always (drum machine behavior)
```

---

## 4. ANALOG MODELING & CHARACTER

### 4.1 Component Drift & Instability

**For each parameter provide:**
- **Pitch drift**: amount (cents), drift rate (Hz), per-voice or global
- **Timing jitter**: per-step timing variance (ms), humanization
- **Amplitude drift**: gain variance (dB), slow LFO rate
- **Temperature drift**: long-term pitch shift (cents/hour simulation)

**Reference implementations:**
- UAD Neve/SSL (analog modeling)
- Arturia V Collection (component aging)
- Softube Console 1 (analog drift)
- VCV Rack (voltage drift simulation)

**Desired format:**
```
Pitch drift: ±5 cents max, 0.1–1 Hz LFO, per-voice independent
Timing jitter: ±2ms max, Gaussian distribution, per-hit
Amplitude drift: ±0.5 dB, 0.05 Hz LFO, per-voice
Temperature: ±10 cents over 60min (cosmetic, slow drift)
```

### 4.2 Analog Noise Floor

**For each parameter provide:**
- **Noise type**: white, pink, 1/f (flicker), hum (50/60 Hz)
- **Noise level**: dBFS below signal, frequency shaping
- **Hiss**: high-frequency noise (tape/preamp simulation)
- **Hum**: mains frequency + harmonics (transformer bleed)
- **Crackle**: vinyl/tape artifacts (impulse rate, amplitude)

**Desired format:**
```
White noise: -60 dBFS, flat 20Hz–20kHz
Pink noise: -65 dBFS, -3dB/octave (circuit noise)
Hiss: -55 dBFS, HPF 2kHz (tape hiss)
Hum: -70 dBFS @ 60Hz + 120Hz + 180Hz (transformer)
Crackle: 0.1–1 impulses/sec, -40 dBFS (vinyl dust)
```

### 4.3 Saturation & Harmonic Distortion

**For each parameter provide:**
- **Saturation stages**: per-voice, mix bus, master
- **Harmonic content**: odd/even harmonics ratio, THD %
- **Curve type**: tube (soft, even harmonics), transistor (hard, odd)
- **Drive range**: input gain before saturation (dB)
- **Bias**: asymmetric clipping (even harmonic generation)

**Reference implementations:**
- Decapitator (A/E/N/T/P models)
- Soundtoys Radiator (tube/transistor)
- Slate VCC (console modeling)
- Waves NLS (Neve/SSL/API)

**Desired format:**
```
Stages: per-voice (subtle), master (optional heavy)
THD: 0.1–5% @ 0dB, default 0.5% (vintage warmth)
Curve: tube (tanh, 2nd/3rd harmonics), transistor (hard clip, 3rd/5th)
Drive: 0–18 dB, default 0 dB (clean)
Bias: -10% to +10% (0=symmetric), default 0%
```

---

## 5. ANTI-ALIASING & DSP QUALITY

### 5.1 Oversampling Strategy

**For each parameter provide:**
- **Oversampling factor**: 2x, 4x, 8x, 16x - when to use each
- **Filter type**: FIR (linear phase) vs IIR (minimum phase)
- **Latency**: samples added, compensation strategy
- **CPU cost**: multiplier per factor (2x ≈ 2.5× CPU)
- **Stages to oversample**: which processes need it (saturation, clipper, nonlinear filters)

**Reference implementations:**
- JUCE dsp::Oversampling (FIR/IIR options)
- iZotope oversampling (adaptive)
- FabFilter (linear phase FIR)

**Desired format:**
```
Factor: 4x for soft clipper, 2x for saturation, 1x for linear processes
Filter: IIR (minimum phase, lower latency), FIR for mastering
Latency: IIR 4x ≈ 12 samples @ 48kHz, report to host
CPU: 2x ≈ 2.2×, 4x ≈ 4.5×, 8x ≈ 9× (diminishing returns)
Apply to: soft clipper (4x), saturation (2x), resonant filters (2x)
```

### 5.2 Parameter Smoothing

**For each parameter provide:**
- **Smoothing time**: range (ms), per-parameter type
- **Curve type**: linear, exponential (1-pole), S-curve
- **Sample-accurate**: VST3 parameter changes with sample offset
- **Zipper noise threshold**: max step size before audible clicks

**Reference implementations:**
- JUCE SmoothedValue (1-pole exponential)
- Steinberg VST3 sample-accurate automation
- Cockos WDL (IPlug2 parameter smoothing)

**Desired format:**
```
Time: 10–50ms (fast params like filter), 50–200ms (slow params like reverb size)
Curve: exponential (τ = time/5), 1-pole lowpass
Sample-accurate: process IParameterChanges with sample offset in VST3
Threshold: <0.1 dB step for gain, <10 Hz for filter cutoff
```

### 5.3 Denormal Handling

**For each parameter provide:**
- **Flush-to-zero (FTZ)**: CPU flags to set (SSE, NEON)
- **Denormals-are-zero (DAZ)**: additional flag
- **Manual epsilon**: add tiny DC offset (1e-20) to feedback loops
- **Performance impact**: CPU usage with/without denormal flush

**Desired format:**
```
FTZ: enable via _MM_SET_FLUSH_ZERO_MODE (SSE) or JUCE ScopedNoDenormals
DAZ: enable via _MM_SET_DENORMALS_ZERO_MODE
Epsilon: add 1e-20 to IIR feedback, reverb comb filters
Impact: 10–50% CPU reduction on silence/decay tails
```

---

## 6. MODULATION & ADVANCED FEATURES

### 6.1 LFO Specifications

**For each parameter provide:**
- **Waveforms**: sine, triangle, square, saw, random (S&H)
- **Rate**: range (Hz or BPM-synced), free/synced modes
- **Depth**: modulation amount per destination
- **Destinations**: pitch, filter cutoff, amplitude, pan
- **Phase**: initial phase (0–360°), reset on trigger
- **Polarity**: unipolar (0–100%) vs bipolar (-100% to +100%)

**Desired format:**
```
Waveforms: sine, triangle, square, saw up/down, random
Rate: 0.01–20 Hz or BPM-synced (1/64 to 4 bars)
Depth: 0–100% per destination
Destinations: pitch (±2 oct), filter (±2 oct), amp (±12 dB), pan (±100)
Phase: 0–360°, reset on note-on (drums)
Polarity: bipolar for pitch/pan, unipolar for filter/amp
```

### 6.2 Envelope Shapes (AHDSR)

**For each parameter provide:**
- **Attack**: range (ms), curve (linear/exp/log)
- **Hold**: range (ms), flat sustain before decay
- **Decay**: range (ms), curve type
- **Sustain**: level (0–100%), only for sustained notes
- **Release**: range (ms), curve type
- **Velocity scaling**: how velocity affects each stage

**Desired format:**
```
Attack: 0–100ms, exponential (fast start, slow end)
Hold: 0–500ms, flat (useful for clap/snare body)
Decay: 10–5000ms, exponential (natural decay)
Sustain: 0–100%, default 0% (drums are percussive)
Release: 10–2000ms, exponential (for open hi-hat choke)
Velocity: scales attack (-50%), decay (±30%), sustain level (±20 dB)
```

### 6.3 Microtonal & Tuning Tables

**For each parameter provide:**
- **Tuning systems**: 12-TET, just intonation, Pythagorean, custom scales
- **Scala format**: .scl file support for custom tunings
- **Per-voice tuning**: independent pitch tables
- **MIDI Tuning Standard**: MTS support (single-note, octave, full keyboard)

**Desired format:**
```
Default: 12-TET (A440), standard MIDI note mapping
Scala: support .scl files (scale) + .kbm (keyboard mapping)
Per-voice: allow detuning per drum voice (e.g., toms in 5ths)
MTS: support MIDI Tuning Standard (SysEx) for DAW integration
```

---

## 7. CALIBRATION & VALIDATION

### 7.1 Reference Levels

**Provide standard calibration points:**
- **Internal headroom**: peak level before clipping (dBFS)
- **Mix bus level**: target RMS per voice (dBFS)
- **Master output**: target loudness (LUFS), true-peak ceiling
- **Gain staging**: per-voice → mix → master (dB at each stage)

**Desired format:**
```
Internal: -6 dBFS peak per voice (headroom for mixing)
Mix bus: -12 dBFS RMS (8 voices playing)
Master: -9 LUFS integrated, -1 dBTP (true-peak)
Gain staging: voice (0 dB) → mix (-6 dB) → master (makeup to -9 LUFS)
```

### 7.2 Test Signals & Validation

**Provide test procedures:**
- **Frequency response**: sweep 20Hz–20kHz, measure ±0.5 dB flatness
- **THD+N**: measure at -6 dBFS, target <0.1% (clean) or spec'd amount (saturation)
- **IMD**: two-tone test (60Hz + 7kHz), measure intermodulation
- **Latency**: measure plugin delay compensation (PDC) in samples
- **CPU usage**: target % at 48kHz/64-sample buffer, 8-voice pattern

**Desired format:**
```
Frequency: ±0.5 dB, 20Hz–20kHz (no processing)
THD+N: <0.05% @ -6 dBFS (clean path), 0.5–5% (saturation on)
IMD: <0.01% (SMPTE 60Hz+7kHz 4:1)
Latency: report actual samples (e.g., 256 samples @ 4x oversampling)
CPU: <10% (single core, i5-8th gen, 48kHz/64 samples)
```

---

## 8. SOURCES TO CONSULT

**Academic & Standards:**
- AES papers on drum synthesis (Välimäki, Smith)
- ITU-R BS.1770-4 (loudness measurement)
- EBU R128 (broadcast loudness)
- MIDI 1.0/2.0 specifications

**Industry References:**
- Steinberg VST3 SDK (parameter ranges, automation)
- JUCE documentation (dsp module, oversampling, smoothing)
- iZotope Insight (metering standards)
- Bob Katz "Mastering Audio" (levels, dynamics)

**Reverse Engineering:**
- TR-808 service manual (original circuits)
- Behringer RD-8 analysis (modern clone)
- Open-source: MDA plugins, Airwindows (algorithms)
- KVR/Gearspace threads (user experiences, calibration)

**Plugin References:**
- FabFilter (Pro-C 2, Pro-L 2, Saturn 2) - UI/parameter design
- Valhalla DSP (reverb/delay algorithms)
- Soundtoys (saturation, modulation)
- Waves SSL/API (console modeling)

---

## 9. RESPONSE FORMAT

For each section, provide a **structured table** with:

| Parameter | Range | Default | Unit | Curve/Type | Notes |
|-----------|-------|---------|------|------------|-------|
| Attack | 0.1–100 | 10 | ms | Logarithmic | Drum-optimized |
| Threshold | -40–0 | -12 | dBFS | Linear | SSL-style |

Include:
- **Rationale**: why this range/default (reference to source)
- **Trade-offs**: quality vs CPU, latency vs accuracy
- **Implementation notes**: JUCE classes, algorithms, gotchas
- **Validation**: how to test (signals, measurements, listening)

**Confidence level** for each parameter:
- ✅ **High**: industry standard, well-documented
- ⚠️ **Medium**: common practice, some variation
- ❓ **Low**: subjective, needs A/B testing

---

## 10. DELIVERABLE

A comprehensive specification document with:
1. **All parameters** in copy-paste ready format (code comments, config files)
2. **Calibration procedures** (test signals, measurements)
3. **Reference implementations** (open-source code, papers)
4. **Trade-off analysis** (quality vs performance)
5. **Validation checklist** (how to verify correct implementation)

This will be used to implement production-ready dynamics, FX, and synthesis enhancements in both web (Web Audio API) and native (JUCE/VST3) versions of CR-717.
