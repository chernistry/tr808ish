# 59 — FX: Reverb (Freeverb-like) and Delay (Tempo-Sync, Filters, Ping-Pong)

Read `.sdd/architect.md`, `.sdd/best_practices_vst.md` (§4), and `.sdd/coding_rules.md` first.

Context
- Spec: `.sdd/backlog/tasks/004-rehaul-sounds-implement.md` (§2.1 Reverb, §2.2 Delay)
- Goal: Lightweight, musical reverb and delay with per-voice sends and master returns.

Dependencies
- Ticket 49 (UI Mixer & Sends) — existing spec for sends/returns
- Ticket 61 (parameter mapping) — to expose controls and persistence

Scope — Reverb
- Algorithm: Freeverb-like (8 comb + 4 allpass), stereo
- Room size 0–100% (default 50%), Damping 1–16 kHz (default 8 kHz)
- Pre-delay 0–100 ms (default 20 ms), Wet/Dry 0–100% (default 30%)
- Diffusion 0–100% (default 70%), Width 0–100% (default 100%)

Scope — Delay
- Time 1–2000 ms or BPM-synced (1/64..4 bars), default 375 ms
- Feedback 0–95% (oscillation ~98%), Wet/Dry 0–100% (default 25%)
- Feedback loop filters: HPF 20–1000 Hz, LPF 1 k–20 kHz
- Stereo modes: mono, ping-pong, stereo
- Optional modulation: 0.1–5 Hz, 0–10 ms

Acceptance Criteria
- Sends from voices reach FX returns; Wet/Dry behaves as expected
- Delay ping-pong alternates L/R; filters darken repeats musically
- Reverb tail respects damping and predelay; width control collapses to mono at 0%
- Unit tests render simple impulses and check decay/envelope behavior (OfflineAudioContext)

Implementation Steps
1) Create `src/audio/fx/reverb.ts` and `src/audio/fx/delay.ts`
2) Add `src/audio/mix/sends.ts` to manage per-voice sends → FX returns
3) Provide tempo-sync helper in `src/audio/utils/tempo.ts` (note division → ms)
4) Wire returns into master chain pre-dynamics (or ADR if post)
5) Tests: `tests/unit/fx/reverb.test.ts`, `tests/unit/fx/delay.test.ts`

Affected Files
- `src/audio/fx/reverb.ts`, `src/audio/fx/delay.ts` (new)
- `src/audio/mix/sends.ts` (new), `src/audio/mix/master-bus.ts`
- `tests/unit/fx/*.test.ts`

Risks & Mitigations
- CPU from stereo reverb: optimize comb/allpass counts; parameterize quality
- Feedback instability: clamp feedback < 0.98; denormal guards

