Below is a production-ready spec for CR-717’s master dynamics, FX, and enhanced voice parameters. All ranges are calibrated for **both** JUCE/VST3 and Web Audio. Key load-bearing claims are cited inline.

---

# 1) MASTER DYNAMICS CHAIN

## 1.1 Bus Compressor (SSL/API-style, clean digital core)

**Algorithm**: feed-forward RMS detector (switchable to peak), soft knee, optional feedback mode. Sidechain HPF. Lookahead optional.

**Parameter spec**

| Parameter     |                                   Range | Default | Unit | Curve/Type                         | Notes                                                                                                          |
| ------------- | --------------------------------------: | ------: | ---- | ---------------------------------- | -------------------------------------------------------------------------------------------------------------- |
| Attack        |                            **0.05–100** |  **10** | ms   | Log scale                          | Lower bound covers Distressor-class fast attacks (50 µs)                                                       |
| Release       |                      **10–2000** + Auto | **100** | ms   | Log scale + program-dependent auto | SSL bus comp offers 0.1–1.2 s + Auto; Distressor up to 3 s. Our range spans both.                              |
| Ratio         | 1:1, 2:1, **4:1**, 8:1, 10:1, 20:1, ∞:1 | **4:1** | —    | stepped                            | SSL offers 2/4/10:1; Distressor adds more including 20:1 & NUKE.                                               |
| Threshold     |                            **–40 to 0** | **–12** | dBFS | linear                             | Broad enough for light bus to heavy squash.                                                                    |
| Knee          |                                **0–12** |   **3** | dB   | soft                               | Pro-C 2 supports very wide variable knee (up to **72 dB**); we ship musical 0–12 dB.                           |
| Makeup        |                          **–12 to +24** |   **0** | dB   | linear + optional auto             | Auto targets post-GR RMS ≈ input RMS (simple running RMS over 300 ms).                                         |
| Sidechain HPF |                              **20–500** |  **80** | Hz   | 12 dB/oct                          | Mirrors SSL SC-HPF range.                                                                                      |
| Detector      |                  **RMS (10 ms)** / Peak | **RMS** | —    | windowed RMS                       | RMS window typical for musical compression; Peak for drums with sharp transients.                              |
| Lookahead     |                                 **0–5** |   **0** | ms   | linear                             | Keep default 0 for live feel; compressor may also run **up to 20 ms** like Pro-C 2 when latency is acceptable. |

**Rationale**
Ranges map to classic hardware: SSL G-comp timing and ratios, API-style sidechain shaping, Distressor-class extremes. Citations show the canonical parameter sets (SSL: attack/release/ratios; Distressor: 50 µs attack & long release; Pro-C 2: lookahead & variable knee).

**Trade-offs**
Lookahead >0 ms adds latency; prefer 0 ms live, 1–5 ms in offline bounce. Sidechain HPF reduces pumping on BD but can under-compress low-toms.

**Implementation notes (JUCE/WebAudio)**

* **JUCE**: `dsp::Compressor` as a baseline detector/GR, or custom RMS detector + gain computer (soft knee). Sidechain HPF via `dsp::IIR::Filter` on SC path. Parameter smoothing with `SmoothedValue` (exp).
* **Web Audio**: Detector with `DynamicsCompressorNode` is too opaque; implement custom RMS + `GainNode` for GR.

**Validation**
Pink-noise + kick loop at –18 dBFS. Aim 2–4 dB GR on peaks with default settings. Compare timing vs SSL-style settings (e.g., 10 ms / Auto). SSL doc values as reference.

**Confidence**: ✅ High (well-documented ranges).

---

## 1.2 Limiter (True-Peak capable)

**Algorithm**: lookahead peak limiter with true-peak option (oversampled inter-sample detection per ITU-R BS.1770 / EBU Tech 3341).

| Parameter    |                Range |  Default | Unit | Curve/Type                                             | Notes                                                                  |
| ------------ | -------------------: | -------: | ---- | ------------------------------------------------------ | ---------------------------------------------------------------------- |
| Threshold    |         **–20 to 0** | **–0.3** | dBFS | linear                                                 | Set to desired loudness; ceiling separate.                             |
| Release      |   **10–1000** + Auto | **Auto** | ms   | program-dependent                                      | Auto behaves like ARC-style, rate tied to GR & crest.                  |
| Lookahead    |             **0–10** |    **5** | ms   | linear                                                 | Pro-L 2 exposes lookahead; 5 ms is common compromise.                  |
| Oversampling |      **1×/2×/4×/8×** |   **4×** | —    | linear-phase up & min-phase down (quality/CPU balance) | Elephant ships up to **8×**; true-peak detection requires ≥4× @48 kHz. |
| Ceiling      |        **–1.0 to 0** | **–0.1** | dBFS | linear                                                 | Use **–1.0 dBTP** for streaming safety; –0.1 for offline stems.        |
| Knee         |              **0–3** |    **0** | dB   | brick/soft                                             | 0 dB = brick-wall behavior.                                            |
| Mode         | Peak / **True-Peak** |   **TP** | —    | ITU BS.1770-4                                          | True-peak per ITU spec/EBU implementation guidance.                    |

**Trade-offs**
Higher oversampling => more CPU and latency; report PDC in VST3. 4× is a good default; 8× for master print. (Elephant user guide references oversampling options; ITU/EBU define TP detection.)

**Implementation notes**

* **JUCE**: `dsp::Oversampling` around a lookahead peak limiter; oversample the detector and final clip stage. Report latency to host.
* **Web Audio**: ScriptProcessor is deprecated; use `AudioWorklet` for sample-accurate lookahead buffer.

**Validation**
Feed 1 kHz sine @ –0.2 dBFS. Ensure output ≤ ceiling and **true-peak** meter (ITU) stays ≤ target.

**Confidence**: ✅ High.

---

## 1.3 Soft Clipper (Waveshaper)

**Algorithm**: selectable curves with oversampling. Primary choices and equations (J.O. Smith, CCRMA):

* **tanh**: (y = \tanh(gx)) (tube-like even/odd mix)
* **atan**: (y = \frac{2}{\pi}\arctan(gx)) (harder)
* **cubic**: (y = x - \frac{x^3}{3}) (vintage soft-clip)

| Parameter    |               Range |  Default | Unit | Curve/Type                      | Notes                                                                                 |
| ------------ | ------------------: | -------: | ---- | ------------------------------- | ------------------------------------------------------------------------------------- |
| Drive/Input  |            **0–24** |    **0** | dB   | pre-gain                        | Apply auto-comp = –drive to maintain loudness.                                        |
| Threshold    |           **0–100** |   **80** | %    | normalized                      | Internal scaling to ±0 dBFS at 100 %.                                                 |
| Curve        | tanh / atan / cubic | **tanh** | —    | selectable                      | Equations per CCRMA notes.                                                            |
| Oversampling |     **1×/2×/4×/8×** |   **4×** | —    | linear-phase up, min-phase down | Nonlinear stage must be oversampled to reduce aliasing. Use JUCE `dsp::Oversampling`. |
| Mix          |           **0–100** |  **100** | %    | dry/wet                         | Parallel drive possible.                                                              |
| Output       |      **–12 to +12** |    **0** | dB   | post-gain                       | Auto compensation switchable.                                                         |

**Confidence**: ✅ High (math & practice well-known).

---

# 2) FX PROCESSING

## 2.1 Reverb (Freeverb-class room/plate)

**Algorithm**: **Freeverb-style** (8 comb + 4 all-pass per channel), stereo variant. Tunable damping & diffusion.

| Parameter |                   Range |      Default | Unit | Curve/Type     | Notes                                                   |
| --------- | ----------------------: | -----------: | ---- | -------------- | ------------------------------------------------------- |
| Algorithm | Freeverb / Plate / Hall | **Freeverb** | —    | comb+allpass   | Baseline, CPU-light.                                    |
| Size      |               **0–100** |       **50** | %    | perceptual     | Maps to comb delay lengths.                             |
| Damping   |              **1k–16k** |       **8k** | Hz   | LP in feedback | Controls HF decay.                                      |
| Pre-delay |               **0–100** |       **20** | ms   | linear         | Common drum pre-delay region.                           |
| Wet       |               **0–100** |       **30** | %    | linear         | Per-voice send drives this bus; keep master Wet 0–50 %. |
| Diffusion |               **0–100** |       **70** | %    | linear         | Higher = denser tail.                                   |
| Width     |               **0–100** |      **100** | %    | stereo         | Mid/Side blend.                                         |
| ER/Tail   |               **0–100** |       **50** | %    | balance        | 0 = tail only; 100 = ER only.                           |
| Mod rate  |               **0.1–2** |      **off** | Hz   | sine           | Optional slow chorus of all-pass taps.                  |

**Confidence**: ⚠️ Medium (algorithm baseline is clear; musical defaults vary by genre).

## 2.2 Delay

| Parameter  |                                Range |       Default | Unit    | Curve/Type    | Notes                                                                |
| ---------- | -----------------------------------: | ------------: | ------- | ------------- | -------------------------------------------------------------------- |
| Time       | **1–2000** or synced **1/64–4 bars** |       **375** | ms      | linear/log    | 375 ms ≈ 1/8 @ 120 BPM.                                              |
| Feedback   |                             **0–95** |        **40** | %       | linear        | Self-oscillation ~98–100 %.                                          |
| Wet        |                            **0–100** |        **25** | %       | linear        | Per-voice send feeds it.                                             |
| HPF        |                          **20–1000** |       **100** | Hz      | 12 dB/oct     | Keeps low-end clean.                                                 |
| LPF        |                           **1k–20k** |        **8k** | Hz      | 12 dB/oct     | Analog darkening.                                                    |
| Stereo     |            mono / stereo / ping-pong | **ping-pong** | —       | mode          | Widely used for drums (technical definition standard across delays). |
| Modulation |                 **0.1–5** / **0–10** |       **off** | Hz / ms | chorus-like   | Subtle movement.                                                     |
| Saturation |                             **0–12** |         **0** | dB      | feedback path | Optional tape-ish.                                                   |

**Confidence**: ⚠️ Medium (ranges are industry practice; exact values depend on style).

---

# 3) ENHANCED PER-VOICE PARAMETERS

## 3.1 Filters (per voice)

| Parameter |                         Range |                         Default | Unit | Curve/Type     | Notes                                                             |
| --------- | ----------------------------: | ------------------------------: | ---- | -------------- | ----------------------------------------------------------------- |
| Type      | LP24, LP12, HP12, BP12, Notch |                   **LP24** (BD) | —    | SVF/Moog-style | Use zero-delay SVF where possible (nonlinear stages oversampled). |
| Cutoff    |                  **20–20000** | BD **80**, SD **3k**, HH **8k** | Hz   | log            | Musical defaults per voice.                                       |
| Resonance |                **Q 0.7–10.0** |                     **Q ≈ 0.8** | —    | continuous     | Cap before self-oscillation except when desired.                  |
| Env Amt   |                 **–100–+100** |                         **+50** | %    | bipolar        | Opens/closes filter by envelope.                                  |
| Env A/D   |           **0–100 / 10–2000** |                     **0 / 200** | ms   | exp            | Use exp curves for natural decay.                                 |
| Key Track |                     **0–100** |                           **0** | %    | linear         | Usually 0 for drums.                                              |
| Drive     |                      **0–24** |                           **0** | dB   | pre-sat        | Oversample if nonlinear drive on.                                 |

**Confidence**: ⚠️ Medium (common instrument practice).

## 3.2 Pan & Stereo

| Parameter  |         Range | Default | Unit | Curve/Type     | Notes                             |
| ---------- | ------------: | ------: | ---- | -------------- | --------------------------------- |
| Pan        | **–100–+100** |   **0** | %    | constant-power | **–3 dB** pan law recommended.    |
| Width      |     **0–200** | **100** | %    | M/S gain       | 0 = mono, 200 = extra wide.       |
| Pan Law    |    –3/–4.5/–6 |  **–3** | dB   | constant-power | –3 dB widely used in DAWs.        |
| Random Pan |     **0–100** |   **0** | %    | per-hit        | Jitter with seeded RNG per voice. |

**Confidence**: ✅ High.

## 3.3 Velocity & Accent

| Mapping      |                            Range |      Default | Unit | Notes                                  |
| ------------ | -------------------------------: | -----------: | ---- | -------------------------------------- |
| Curve        | linear / log / **exp** / S-curve | **exp (v²)** | —    | Feels natural for drums.               |
| Level        |      **–24 at v=1 → 0 at v=127** |            — | dB   | Simple mapping.                        |
| Filter       |                     **0–+2 oct** |   **+1 oct** | —    | Velocity-to-cutoff.                    |
| Decay        |                     **50–150 %** |    **100 %** | %    | Env time scaling.                      |
| Accent       |                           **+6** |       **+6** | dB   | TR-808-style gain bump; faster attack. |
| Accent curve |                        **0–100** |   **linear** | %    | Accent × velocity.                     |

**Confidence**: ⚠️ Medium (808 accent behavior is analog; +6 dB is a practical target).

## 3.4 Pitch Envelope

| Parameter | Range | Default | Unit | Curve/Type | Notes |
|---|---:|---:|---|---|
| Curve | linear / **exp** | **exp** | — | (e^{-t/\tau}) | Natural decay. |
| Start→End | **2–4× base** → base | **BD 150→50** | Hz | ratio | Per-voice sensible defaults. |
| Time | **1–500** | **BD 80**, **SD 30** | ms | exp | Sample-accurate retrigger. |
| Amount | **0–400** | **100** | % | depth | 100 % = “classic”. |
| Retrigger | **always** | **always** | — | mono/poly | Drums retrigger per hit. |

**Confidence**: ⚠️ Medium.

---

# 4) ANALOG MODELING & CHARACTER

## 4.1 Drift & Instability

| Parameter     |     Range |        Default | Unit     | Notes                          |
| ------------- | --------: | -------------: | -------- | ------------------------------ |
| Pitch drift   | **±0–10** |         **±5** | cents    | Per-voice random LFO 0.1–1 Hz. |
| Timing jitter |  **±0–5** |         **±2** | ms       | Gaussian; per-hit.             |
| Amp drift     |  **±0–1** |       **±0.5** | dB       | Slow LFO ~0.05 Hz.             |
| Temp drift    | **±0–20** | **±10/60 min** | cents/hr | Cosmetic long-term shift.      |

**Confidence**: ❓ Low (taste-dependent; implement as options).

## 4.2 Noise Floor

| Type    |       Level | Shaping           | Notes                     |
| ------- | ----------: | ----------------- | ------------------------- |
| White   |    –60 dBFS | flat              | Base hiss.                |
| Pink    |    –65 dBFS | –3 dB/oct         | Circuit-like.             |
| Hiss    |    –55 dBFS | HPF 2 kHz         | Tape/preamp vibe.         |
| Hum     |    –70 dBFS | 60 Hz + harmonics | Regional 50/60 Hz option. |
| Crackle | 0.1–1 imp/s | random impulses   | Vinyl dust.               |

**Confidence**: ❓ Low (genre/taste).

## 4.3 Saturation & Harmonics

| Stage           |                           Range | Default | Notes                                 |
| --------------- | ------------------------------: | ------: | ------------------------------------- |
| Per-voice drive |                         0–12 dB |    0 dB | Subtle color.                         |
| Master drive    |                         0–18 dB |    0 dB | Heavier glue.                         |
| THD target      |                         0.1–5 % |   0.5 % | Even/odd mix.                         |
| Curve           | tube (tanh) / transistor (hard) |    tanh | Bias –10 %..+10 % for even harmonics. |

**Confidence**: ⚠️ Medium.

---

# 5) DSP QUALITY

## 5.1 Oversampling

| Item     | Recommendation                                                            | Notes                                                          |
| -------- | ------------------------------------------------------------------------- | -------------------------------------------------------------- |
| Factor   | **4×** for clipper, **2×** for saturation, **1×** for linear              | Based on typical plugin practice & JUCE oversampling facility. |
| Filter   | Up: linear-phase FIR; Down: min-phase IIR/FIR                             | Balance latency/CPU (Elephant shows up to 8× for quality).     |
| Latency  | Report PDC                                                                | E.g., 4× IIR ≈ low double-digit samples at 48 kHz.             |
| CPU      | 2×≈2.2×, 4×≈4.5×, 8×≈9×                                                   | Rule-of-thumb scaling.                                         |
| Apply to | **Soft clipper (4×)**, resonant filters (2×), master limiter TP path (4×) | ITU/EBU TP detection relies on oversampling.                   |

**Confidence**: ✅ High (standards + vendor docs).

## 5.2 Parameter smoothing & automation

| Item          | Spec                                                                                                                   | Notes                                          |
| ------------- | ---------------------------------------------------------------------------------------------------------------------- | ---------------------------------------------- |
| Fast params   | **10–50 ms**, exp                                                                                                      | Filters, pitch, amp. `SmoothedValue` exp ramp. |
| Slow params   | **50–200 ms**                                                                                                          | Reverb size, width.                            |
| VST3          | **Sample-accurate automation** via `IParameterChanges`/`IParamValueQueue`, apply at sample offsets inside `process()`. | Official portal & SDK doc.                     |
| Web           | Smooth per-sample in `AudioWorkletProcessor`                                                                           | Avoid zipper noise.                            |
| Zipper limits | <0.1 dB gain step, <10 Hz cutoff step                                                                                  | Practical audibility thresholds.               |

**Confidence**: ✅ High.

## 5.3 Denormal handling

| Item    | Spec                                                      | Notes                        |
| ------- | --------------------------------------------------------- | ---------------------------- |
| FTZ     | `_MM_SET_FLUSH_ZERO_MODE` or **JUCE `ScopedNoDenormals`** | Enable in audio thread.      |
| DAZ     | `_MM_SET_DENORMALS_ZERO_MODE`                             | SSE path.                    |
| Epsilon | **1e-20** added in IIR/reverb loops                       | Prevent CPU spikes at tails. |
| Impact  | 10–50 % CPU reduction on silence                          | Measurable in long decays.   |

**Confidence**: ✅ High.

---

# 6) MODULATION & ADVANCED FEATURES

## 6.1 LFO (per voice or global matrix)

| Parameter    |                                                     Range | Default | Unit | Notes                                           |
| ------------ | --------------------------------------------------------: | ------: | ---- | ----------------------------------------------- |
| Waveforms    |                      sine, triangle, square, saw ↑/↓, S&H |    sine | —    |                                                 |
| Rate         |                         **0.01–20** or synced 1/64–4 bars |     1.0 | Hz   | Free or tempo sync.                             |
| Depth        |                                                 **0–100** |       0 | %    | Per destination.                                |
| Destinations | pitch (±2 oct), filter (±2 oct), amp (±12 dB), pan (±100) |       — | —    |                                                 |
| Phase        |                                                 **0–360** |       0 | °    | Reset on trigger.                               |
| Polarity     |                                          unipolar/bipolar | depends | —    | Bipolar for pitch/pan, unipolar for amp/filter. |

**Confidence**: ✅ High (standard synth practice).

## 6.2 AHDSR Envelopes

| Stage    |                      Range | Default | Curve | Notes                       |
| -------- | -------------------------: | ------: | ----- | --------------------------- |
| Attack   |                  **0–100** |   **0** | exp   | Fast percussive.            |
| Hold     |                  **0–500** |   **0** | flat  | Useful for clap/snare body. |
| Decay    |                **10–5000** | **200** | exp   | Natural.                    |
| Sustain  |                  **0–100** |   **0** | %     | Drums usually 0.            |
| Release  |                **10–2000** | **200** | exp   | For OH choke behavior.      |
| Velocity | A –50 %, D ±30 %, S ±20 dB |       — | —     |                             |

**Confidence**: ⚠️ Medium.

## 6.3 Microtonal / Tuning

| Item      | Spec                         | Notes          |
| --------- | ---------------------------- | -------------- |
| Default   | 12-TET, A440                 |                |
| Scala     | `.scl` + `.kbm` import       | Custom scales. |
| Per-voice | Detune toms, cowbell ratios  |                |
| MTS       | MIDI Tuning Standard (SysEx) | DAW-friendly.  |

**Confidence**: ⚠️ Medium.

---

# 7) CALIBRATION & VALIDATION

## 7.1 Levels

| Stage              | Target                                        | Notes                           |
| ------------------ | --------------------------------------------- | ------------------------------- |
| Per-voice headroom | **–6 dBFS peak**                              | Prevent internal clip.          |
| Mix bus RMS        | **–12 dBFS** with 8 voices                    | Leaves limiter margin.          |
| Master             | **–9 LUFS** (electronic), **–1 dBTP** ceiling | LUFS/TP per **EBU/ITU** meters. |

**Confidence**: ⚠️ Medium (genre-dependent loudness target; TP ceiling grounded in standards).

## 7.2 Tests

| Test               | Target                          | How                              |
| ------------------ | ------------------------------- | -------------------------------- |
| Frequency response | ±0.5 dB (20 Hz–20 kHz)          | Disable FX; sweep.               |
| THD+N              | <0.05 % clean, 0.5–5 % with sat | Sine @ –6 dBFS.                  |
| IMD                | <0.01 %                         | SMPTE 60 Hz+7 kHz 4:1.           |
| Latency            | Reported PDC matches actual     | Toggle oversampling & lookahead. |
| CPU                | <10 % @ 48 k/64s, 8-voice       | Measure per host meter.          |

---

# 8) IMPLEMENTATION NOTES (PLATFORM)

* **VST3**: handle **sample-accurate automation** inside `process()` by iterating `IParameterChanges` queues and applying at sample offsets; keep UI/controller decoupled per VST3 portal guidance.
* **JUCE**: prefer `SmoothedValue`/`LinearSmoothedValue` for per-sample ramps; beware linear overshoot edge case (see JUCE forum).
* **Denormals**: guard audio thread with `ScopedNoDenormals`.
* **True-Peak**: implement ITU-style oversampled peak measurement or reuse a BS.1770 conformant meter when limiter TP is enabled.

---

# 9) QUICK PRESETS (STARTER)

* **Bus Glue**: Attack 10 ms, Release Auto, Ratio 4:1, SC-HPF 80 Hz, GR ≈ 2–3 dB. SSL-style.
* **Punch**: Attack 3 ms, Release 100 ms, Ratio 8:1, Lookahead 1 ms, SC-HPF 120 Hz.
* **Limiter Print**: TP on, Lookahead 5 ms, OS 4×, Ceiling –1.0 dBTP, Release Auto.

---

# 10) EVIDENCE LINKS

* **SSL Bus Compressor** timings, ratios, SC-HPF 20–500 Hz.
* **API 2500** attack/release, FF/FB, Thrust detector.
* **Distressor** attack 50 µs–30 ms, release to 3 s, ratios incl. NUKE.
* **FabFilter Pro-C 2**: lookahead up to 20 ms, wide knee.
* **FabFilter Pro-L 2**: true-peak option; lookahead; oversampling guidance.
* **EBU Tech 3341** & **ITU-R BS.1770-4**: true-peak spec.
* **Voxengo Elephant**: up to 8× OS; user-guide notes.
* **Freeverb** structure (8 comb + 4 all-pass).
* **Pan law** explanation (–3 dB constant-power).
* **JUCE**: `ScopedNoDenormals`, `dsp::Oversampling`, `SmoothedValue`.
* **VST3**: sample-accurate `IParameterChanges`.

---

## Notes on uncertainty

* Reverb defaults and delay voicing are taste-dependent; values provided reflect common drum practice rather than a single standard (⚠️ Medium).
* Accent behavior in analog 808 circuits varies with supply/voice; we standardize **+6 dB** and faster attack as a musical target (⚠️ Medium).

If you want, I can output this as a JUCE `AudioProcessorValueTreeState` schema and Web Audio JSON config so you can wire it directly.
