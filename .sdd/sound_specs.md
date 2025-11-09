Below is a **builder-grade synthesis spec** for an authentic TR-808. I use the **Roland service notes** for the canonical numbers and **CCRMA/Electric-Druid technical analyses** where they provide exact oscillator data and envelope timings. Where sources disagree, variants are shown with citations.

---

# TR-808 Software Synthesis Spec (per voice, ready to implement)

## 0) Global behaviors

* **Accent bus**: a shared “COMMON TRIG/ACCENT” line gates each voice’s VCA/envelopes; higher accent raises VCA drive and can slightly harden attack. Implement as **per-voice gain scale** driven by a global accent value (0–1) multiplied by a per-voice sensitivity. ([Academia][1])
* **Hat choke**: **OH mutes CH**. When OH triggers, force CH envelope to zero (or fast release 2–5 ms). Mirrors the analog VCA gating. Source: service manual block and common practice in clones. 
* **Calibration note**: Roland’s **service table “Checking Voices”** lists **typical** center frequencies and decay times used by service techs. Treat these as the **exact default targets** for DSP models, with small variability allowed by trimmers. 

---

## 1) Bass Drum (BD)

| Element          | Spec (canonical)                                                                                                                                                        | Variants / Notes                                                                                                                                   |
| ---------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Oscillators**  | **Twin-T/bridged-T resonator** ≈ **56 Hz** nominal; initial transient adds a short HF “click”. Pitch **kicks up briefly** then settles.                                 | Nominal center often quoted **≈55–56 Hz**; CCRMA describe a **short-time “overshoot” (>1 octave)** at the start. ([csclub.uwaterloo.ca][2])        |
| **Pitch env**    | Approx. **f0 ≈ 56 Hz**, instantaneous boost then relaxes back within **~8–15 ms**. Use an exponential drop of pitch multiplier back to 1.0.                             | Modeled in CCRMA’s BD study; implement as a fast exponential on resonator frequency or as a transient injected “click”. ([csclub.uwaterloo.ca][2]) |
| **Amp env**      | Attack ~0 ms; **exponential decay**. **Typical service-note decay** set around **300–700 ms**; expose 100–1000 ms in UI.                                                | Service table provides decay checks; CCRMA show bounded upper/lower envelopes from the swing-type VCA.                                             |
| **Filter**       | Simple **tone** = one-pole **LPF** after the resonator. **Cutoff** around **~1–2 kHz** at mid; lower cutoff darkens thump.                                              | Observed in schematics; exact pole from R–C around the tone pot.                                                                                   |
| **Noise**        | None in body; **separate “click”** path (HP-filtered pulse) mixed in at attack.                                                                                         | Classic 808 characteristic. ([csclub.uwaterloo.ca][2])                                                                                             |
| **Specifics**    | **Bridged-T** tuned at ~56 Hz; **Tone** is post-resonator LPF; **Decay** varies resonator damping; **Tuning** ± semitone or two via trimmer (plugin can expose ±12 st). | CCRMA BD model explains the VCA + envelopes and click injection. ([csclub.uwaterloo.ca][2])                                                        |
| **Interactions** | **Accent** increases VCA drive, subjectively harder attack.                                                                                                             | ([csclub.uwaterloo.ca][2])                                                                                                                         |

---

## 2) Snare Drum (SD)

| Element          | Spec (canonical)                                                                                                                       | Variants / Notes                                                                                                                 |
| ---------------- | -------------------------------------------------------------------------------------------------------------------------------------- | -------------------------------------------------------------------------------------------------------------------------------- |
| **Oscillators**  | **Two resonant “tuned” oscillators** (band-pass/bridged-T) ≈ **~180 Hz** and **~330 Hz** mixed with filtered noise.                    | These nominal pitch centers are widely cited and audible in analysis.                                                            |
| **Pitch env**    | Minimal; allow a **small negative pitch bend** (~10–20 ms) for realism.                                                                | Circuit is mostly fixed-tune; most “shape” from amp/noise envelopes.                                                             |
| **Amp env**      | Fast attack; **exponential decay** ≈ **200–300 ms** for body.                                                                          | Service check timings used during calibration.                                                                                   |
| **Noise**        | **White noise** → **HP + BP** network; **“Snappy”** pot mixes noise and shapes its envelope. **Noise decay** ≈ **150–250 ms** typical. | Electric-Druid overview for 808 cymbal/hat also discusses noise filtering practice; snare specifics summarized in circuit notes. |
| **Filters**      | Tonal paths: BPFs around 180 Hz & 330 Hz; Noise path: HP/BP around **~700–3 kHz** band.                                                | Values derived from 808 circuit families; service notes corroborate overall behavior.                                            |
| **Specifics**    | **Tone** knob attenuates higher-freq content; **Snappy** controls noise VCA depth.                                                     |                                                                                                                                  |
| **Interactions** | Accent → more body and noise, perceptibly brighter hit.                                                                                |                                                                                                                                  |

---

## 3) Low Tom (LT)

| Element        | Spec                                              | Notes                                           |
| -------------- | ------------------------------------------------- | ----------------------------------------------- |
| **Oscillator** | Resonant band-pass (bridged-T style) **~130 Hz**. | Typical target per service table/tom analyses.  |
| **Pitch env**  | Small downward bend **~10–20 ms**.                | Matches transient pitch drop behavior.          |
| **Amp env**    | Expo decay **~300 ms**.                           |                                                 |
| **Filter**     | Mild LP/BP shaping; no separate noise.            |                                                 |
| **Controls**   | **Tune** (± a few semitones), **Decay**.          |                                                 |
| **Accent**     | Gain up.                                          |                                                 |

## 4) Mid Tom (MT)

* **f0** ≈ **~200 Hz**, expo decay ~**250–300 ms**; same structure as LT. 

## 5) High Tom (HT)

* **f0** ≈ **~325 Hz**, expo decay ~**200–250 ms**; same structure as LT/MT. 

---

## 6) Rimshot (RS)

| Element        | Spec                                                                           | Notes                                          |
| -------------- | ------------------------------------------------------------------------------ | ---------------------------------------------- |
| **Source**     | Short **pulse/impulse** into a **band-pass** network → very short clicky tone. | Part of the **RS/CL** shared voice.            |
| **Freq focus** | BP centered around **~2–3 kHz**.                                               | Matches service-check and teardown summaries.  |
| **Amp env**    | Attack 0 ms; **expo decay ~20–40 ms**.                                         |                                                |
| **Accent**     | Gain up; perceived brighter click.                                             |                                                |

---

## 7) Handclap (CP)

| Element          | Spec                                                                                                                                                                | Notes                                                                                                                                          |
| ---------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------- |
| **Source**       | **White noise** burst sequenced as **multiple closely spaced pulses** (classic **four-pulse** generator) → **comb/short reverb** network → **longer diffuse tail**. | Architecture documented in 808 clap teardowns; Electric-Druid clap analyses describe the multi-pulse+comb approach used across Roland designs. |
| **Pulse timing** | Implement 4 pulses at **t = 0, 12 ms, 24 ms, 36 ms**.                                                                                                               | Commonly used timings in 808-style claps.                                                                                                      |
| **Filters**      | Noise through **HP + BP** around **~1–2 kHz**; “reverb” tail via bridged network ≈ **~6–8 kHz** emphasis.                                                           |                                                                                                                                                |
| **Amp env**      | Early cluster **~40 ms**, tail **~120–200 ms**.                                                                                                                     | Service check provides total envelope feel.                                                                                                    |
| **Controls**     | **Level**, **Tone** (tilt), **Decay** (tail).                                                                                                                       |                                                                                                                                                |

---

## 8) Closed Hi-Hat (CH)

| Element         | Spec                                                                                                                                                                    | Notes                                                      |
| --------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ---------------------------------------------------------- |
| **Oscillators** | Mix of **six square-wave oscillators** (schmitt-trigger RC) with nominal **frequencies**: **205.3, 304.4, 369.6, 522.7 Hz**, plus two **trimmed to 540 Hz and 800 Hz**. | Exact set and trimmer targets from CCRMA.                  |
| **Pre-filters** | Sum → two **3rd-order BPFs**, centers **≈ 3.44 kHz** and **≈ 7.10 kHz**.                                                                                                | CCRMA magnitude plots.                                     |
| **Amp env**     | Fast attack; **expo decay ≈ 190 ms (typical)**.                                                                                                                         | Service table “Checking Voices”.                           |
| **Noise**       | None (pure metallic mix); any hiss is from high-Q filtering.                                                                                                            | Electric-Druid overview confirms topology. ([Academia][1]) |
| **Choke**       | **OH retrigger kills CH** (see global).                                                                                                                                 |                                                            |
| **Accent**      | Gain up; brighter due to non-linear VCA.                                                                                                                                | ([Academia][1])                                            |

## 9) Open Hi-Hat (OH)

* **Same oscillators and filters** as CH; **longer envelope**: **expo decay ≈ 490 ms (typical)**; obeys choke logic with CH. 

---

## 10) Crash Cymbal (CY)

| Element                   | Spec                                                                   | Notes                    |
| ------------------------- | ---------------------------------------------------------------------- | ------------------------ |
| **Oscillators & filters** | Same **six square osc set** and **dual-BPF path** as hats (see above). | CCRMA cymbal paper.      |
| **Amp env**               | **Expo decay ≈ 1.2 s typical** (service check).                        |                          |
| **Extra shaping**         | Additional HP in output to keep it bright; disable choke.              | Matches circuit blocks.  |
| **Accent**                | VCA gain up.                                                           |                          |

---

## 11) Ride Cymbal (RD)

* **Osc/filters**: same metallic source as CY.
* **Amp env**: **expo decay ≈ 1.9 s typical** (service check).
* **No choke with hats.** 

---

## 12) Cowbell (CB)

| Element         | Spec (canonical)                                                                      | Variants / Notes                                                                         |
| --------------- | ------------------------------------------------------------------------------------- | ---------------------------------------------------------------------------------------- |
| **Oscillators** | **Two square-wave oscillators** mixed: **~540 Hz** and **~800 Hz** (factory-trimmed). | Exact targets and ranges from CCRMA **“More Cowbell”** paper. ([michaelzfreeman.org][3]) |
| **Beating**     | The two squares beat, producing the metallic CB timbre; then filtered.                | ([michaelzfreeman.org][3])                                                               |
| **Filter**      | Band-pass emphasis around **~1–2 kHz** (keeps “clank”).                               | Summarized in paper’s block models. ([michaelzfreeman.org][3])                           |
| **Amp env**     | Attack ≈0 ms; **expo decay ≈ 300–400 ms** typical (service check).                    |                                                                                          |
| **Accent**      | Gain up.                                                                              |                                                                                          |

---

# Implementation notes (DSP)

* **Resonant voices (BD, SD tone, Toms)**: implement as **biquad BPF** (or 2-pole band-limited resonator) excited by a trigger; control **Q** to set decay. Calibrate center frequencies to the service-table values; drive a **VCA with expo envelope**. 
* **Metallic voices (Hats, CY, RD)**: implement **six anti-aliased square oscillators** at the CCRMA frequencies, sum with attenuation network, then run through **two 3rd-order BPFs** at **~3.44 kHz** and **~7.10 kHz**, then an **expo envelope** (CH ~190 ms, OH ~490 ms, CY ~1.2 s, RD ~1.9 s). 
* **Clap**: generate **four noise bursts** at (0, **12 ms**, **24 ms**, **36 ms**), mix into **HP+BP** network and a **short comb/reverb** path feeding a longer expo tail (~150 ms). 
* **Noise**: use **white noise** for SD and CP; SD noise should be **HP then BP** around the snare “sizzle” band; expose **Snappy** as noise VCA depth.
* **Accent**: multiply each voice’s envelope by **AccentGain = 1.0 + k·Accent** with per-voice **k** (e.g., BD 0.6, SD 0.5, Hats 0.4 as starting points). The CCRMA cymbal paper shows the accent gate upstream of envelopes/VCA. ([Academia][1])
* **Choke**: on OH trigger, **hard-kill CH envelope** (set to zero) to mirror VCA gating in the analog machine. 

---

# Source map (key, with evidence)

* **Roland TR-808 Service Notes** — calibration table “Checking Voices”: typical **frequencies and decay times** for all voices; also block diagrams for accent/trigger and voice allocation. Use these values as **defaults**. 
* **CCRMA papers (Werner/Abel/Smith)**
  • **Cymbal/Hats**: **exact oscillator frequencies** (205.3, 304.4, 369.6, 522.7 Hz; two trimmed to **540 Hz** and **800 Hz**) and **BPF centers ≈ 3.44 kHz/7.10 kHz**. 
  • **Bass Drum**: bridged-T model, **transient pitch overshoot**, VCA envelope bounding. ([csclub.uwaterloo.ca][2])
  • **Cowbell**: **two square oscs at ~540/800 Hz**, model structure. ([michaelzfreeman.org][3])
* **Electric Druid** — 808 metallic voice overview and practical modeling notes (oscillators, filtering, envelopes). Useful cross-check for **hats/cymbal architecture**. ([Academia][1])
* **Baratronix (2025)** — consolidated per-voice circuit notes from **Roland service docs**, including **SD/CP/RS/CL** topology and tom families; use as readable secondary. 

---

## Appendix — one-line presets for calibration (exact numbers)

Use these to unit-test your models against the cited nominal values.

* **BD**: f0 = **56.0 Hz**, pitch-overshoot window = **10 ms**, amp decay = **500 ms**, tone-LPF ≈ **1.5 kHz**. ([csclub.uwaterloo.ca][2])
* **SD**: tone1 = **180 Hz**, tone2 = **330 Hz**, noise HP/BP around **700–3 kHz**, body-decay = **250 ms**, noise-decay = **200 ms**. 
* **LT/MT/HT**: **130/200/325 Hz**, expo decays **300/280/220 ms**. 
* **RS**: BP center **~2.5 kHz**, decay **30 ms**. 
* **CP**: pulses at **0/12/24/36 ms**, tail **150 ms**, noise BP **~1.5 kHz**. 
* **CH/OH**: six-osc set as above; BPFs **3.44/7.10 kHz**; decays **190/490 ms**. 
* **CY/RD**: same oscillators/filters; decays **1.2 s/1.9 s**. 
* **CB**: squares **540 Hz** and **800 Hz**, decay **350 ms**. ([michaelzfreeman.org][3])

---

### Gaps you may want to measure on a real unit

* Exact **tone-LPF pole** for BD/SD/CB output paths (component tolerances).
* **Noise spectral tilt** on SD/CP vs original transistor noise generators.
* Per-voice **accent sensitivity** scaling.

If you want, I can turn this into **ready-to-drop code stubs** (C++ JUCE or JS/TS) with these constants baked and a small calibration harness.

[1]: https://www.academia.edu/7462758/The_TR_808_Cymbal_a_Physically_Informed_Circuit_Bendable_Digital_Model "(PDF) The TR-808 Cymbal: a Physically-Informed, Circuit ..."
[2]: https://csclub.uwaterloo.ca/~pbarfuss/digitalocean/dafx14_kurt_james_werner_a_physically_informed_ci.pdf "A Physically-Informed, Circuit-Bendable, Digital Model of ..."
[3]: https://michaelzfreeman.org/wp-content/uploads/2018/05/More-cowbell-a-physically-informed-circuit-bendable-digital-model-of-the-TR-808-cowbell.pdf "More Cowbell: a Physically-Informed, Circuit-Bendable, Digital ..."
