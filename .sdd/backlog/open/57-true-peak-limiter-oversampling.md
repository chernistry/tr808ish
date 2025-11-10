# 57 — True-Peak Limiter with Oversampling (Web Audio)

Read `.sdd/architect.md`, `.sdd/best_practices_vst.md` (§4 Anti-aliasing), and `.sdd/coding_rules.md` first.

Context
- Spec: `.sdd/backlog/tasks/004-rehaul-sounds-implement.md` (§1.2 Limiter)
- Goal: Lookahead limiter with true-peak detection (oversampled), clean brick-wall behavior, minimal artifacts.

Dependencies
- Ticket 56 (compressor) — order in master chain: Compressor → Limiter
- Ticket 62 (calibration & tests) — for meter alignment and ppk checks

Scope
- Peak limiter with lookahead 0–10 ms (default 5 ms)
- True-peak detection via 4× oversampling (linear-phase FIR upsampler)
- Threshold −20..0 dBFS, Ceiling −0.3..0 dBFS, Knee 0–3 dB
- Release 10–1000 ms + Auto
- Optional oversampling bypass for CPU save

Acceptance Criteria
- No overshoots beyond Ceiling with true-peak on (±0.1 dB tolerance)
- Kick/snare transients preserved with reasonable release; no obvious distortion at −0.3 dBFS ceiling
- Unit test renders a transient buffer through the limiter; verifies peaks <= ceiling (OfflineAudioContext)

Implementation Steps
1) Create `src/audio/dynamics/limiter.ts` with oversampled detection path
2) 4× upsample detector path using `OfflineAudioContext`-free FIR (precomputed taps)
3) Lookahead via DelayNode; gain computer applies fast attack to clamp peaks
4) Parameter smoothing for threshold/ceiling/release
5) Wire into master chain after compressor
6) Tests in `tests/unit/dynamics/limiter.test.ts`

Affected Files
- `src/audio/dynamics/limiter.ts` (new)
- `src/audio/mix/master-bus.ts` (chain)
- `tests/unit/dynamics/limiter.test.ts` (new)

Risks & Mitigations
- CPU from oversampling: allow toggle (on by default); use short FIR
- Latency from lookahead: expose; default acceptable for browser synth

