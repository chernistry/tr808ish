# 60 — Voices: Enhanced Parameters (Filters, Envelopes, Pan/Width)

Read `.sdd/architect.md` and `.sdd/coding_rules.md` first.

Context
- Spec: `.sdd/backlog/tasks/004-rehaul-sounds-implement.md` (§3.1 Filter Parameters, §3.2 Pan & Stereo)
- Goal: Extend existing voices (BD, SD, CH, OH, CP) with calibrated filter/envelope parameters; add per-voice pan and stereo controls.

Dependencies
- Tickets 03–06 (base voices) — already implemented
- Ticket 61 (parameter registry & UI mapping)

Scope
- Filters per voice: LP/HP/BP 12/24 dB as appropriate; default cutoffs per voice
  - BD: LP around 80 Hz; SD: BP ~3 kHz; HH: HP ~8 kHz
- Resonance 0–100%; Envelope amount −100..+100%; Attack 0–100 ms; Decay 10–2000 ms
- Drive 0–24 dB (pre/post filter as ADR), key tracking 0–100% (default 0%)
- Pan −100..+100 (constant-power −3 dB), Stereo width 0–200% where applicable (OH/CP)
- Respect choke between CH/OH

Acceptance Criteria
- Each voice exposes new params with sensible defaults matching spec
- Filters/envelopes modulate timbre as expected without zipper noise
- Pan law constant-power; width collapses to mono at 0% and expands to 200%
- Unit tests render parameter sweeps per voice (OfflineAudioContext) and assert energy shift consistent with cutoff/resonance changes

Implementation Steps
1) Add filter/envelope modules under `src/audio/voices/common/`
2) Update each voice class in `src/audio/voices/` to insert filter and apply env amount
3) Implement `pan.ts` utility for constant-power pan and `stereo-width.ts` mid/side
4) Parameter smoothing where needed (cutoff, resonance, drive)
5) Tests: `tests/unit/voices/*-params.test.ts` with OfflineAudioContext

Affected Files
- `src/audio/voices/common/filter.ts`, `envelope.ts`, `pan.ts`, `stereo-width.ts` (new)
- `src/audio/voices/*` (augment)
- `tests/unit/voices/*`

Risks & Mitigations
- CPU increase: reuse nodes per trigger; keep graph minimal; avoid allocations
- Stability: clamp resonance/Q to avoid self-oscillation unless intended

