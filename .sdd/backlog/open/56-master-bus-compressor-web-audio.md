# 56 — Master Bus Compressor (Web Audio, SSL/API-style)

Read `.sdd/architect.md` and `.sdd/coding_rules.md` first.

Context
- Spec: `.sdd/backlog/tasks/004-rehaul-sounds-implement.md` (§1.1 Bus Compressor)
- Goal: Authentic, musical drum-bus compression with RMS detector, soft knee, SC-HPF, optional lookahead. Web Audio implementation with deterministic, tested behavior.

Dependencies
- Ticket 02 (scheduler core) — for stable scheduling when measuring GR
- Optional: Ticket 62 (calibration & tests) for final gain structure validation

Scope
- Implement custom feed-forward RMS detector (10 ms window, switchable Peak)
- Soft-knee gain computer; stepped ratios (1:1, 2:1, 4:1, 8:1, 10:1, 20:1, ∞:1)
- Threshold, Attack (0.05–100 ms), Release (10–2000 ms + Auto), Knee (0–12 dB)
- Makeup (−12..+24 dB) with optional Auto-Makeup ≈ equal-RMS target
- Sidechain HPF (20–500 Hz, 12 dB/oct) in detector path
- Lookahead 0–5 ms (default 0), tiny delay line for signal and detector

Acceptance Criteria
- Default settings yield ~2–4 dB GR on kick+mix loop at −18 dBFS input
- Attack/Release behave logarithmically and feel musical; Auto-Release adapts
- SC-HPF reduces pumping on BD without dulling hats (audible A/B)
- No clicks/pops on parameter change (smoothing applied)
- Unit test verifies GR envelope against synthetic step inputs (OfflineAudioContext)

Implementation Steps
1) Create `src/audio/dynamics/compressor.ts`
   - Export `createCompressor(ctx: AudioContext, opts: CompressorOptions): Compressor`
   - Internals: SC path (BiquadFilterNode highpass), RMS windowed detector, gain computer, smoothers
2) Add lookahead: shared circular buffer/delay node (ScriptProcessor replacement with AudioWorklet avoided — use minimal DelayNode)
3) Parameter smoothing: exponential for times, linear for threshold/makeup
4) Auto-Makeup: running RMS integration (≈300 ms) pre/post, compute delta, clamp
5) Hook into master chain in `src/audio/mix/master-bus.ts`
6) Tests: `tests/unit/dynamics/compressor.test.ts` using `OfflineAudioContext`

Affected Files
- `src/audio/dynamics/compressor.ts` (new)
- `src/audio/mix/master-bus.ts` (wire-in)
- `tests/unit/dynamics/compressor.test.ts` (new)

Risks & Mitigations
- Latency from lookahead: default 0; expose control; document trade-off
- CPU from RMS/filters: use simple IIR HPF, fixed-size window; avoid allocations

---

## API (draft)
```ts
export interface CompressorOptions {
  attackMs: number; // 0.05–100
  releaseMs: number; // 10–2000 or 0 = auto
  ratio: 1|2|4|8|10|20|"inf";
  thresholdDb: number; // -40..0
  kneeDb: number; // 0..12
  makeupDb: number; // -12..+24
  autoMakeup: boolean;
  scHpfHz: number; // 20..500
  detector: 'rms'|'peak';
  lookaheadMs: number; // 0..5
}
```

