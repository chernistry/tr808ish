# 62 — Calibration, Meter Alignment, Tests, and Performance

Read `.sdd/architect.md`, `.sdd/coding_rules.md`, and Ticket 48 (meters) first.

Context
- Spec: `.sdd/backlog/tasks/004-rehaul-sounds-implement.md` (global validation)
- Goal: Calibrate gain structure across voices → FX → dynamics; align meters; add unit tests for dynamics/FX; verify performance budgets.

Dependencies
- Tickets 56–59 (dynamics/FX) and 60 (voice params)
- Ticket 48 (UI meters) and 49 (mixer)

Scope
- Level calibration at −18 dBFS nominal; headroom for bus comp/limiter per spec
- Verify compressor GR ranges, limiter ceiling (true-peak), clipper gain-comp
- Validate reverb/delay tails & feedback stability
- Performance: ensure no long tasks; measure CPU p95 during playback

Acceptance Criteria
- True-peak limiter prevents overshoots beyond ceiling (±0.1 dB)
- Bus comp GR within expected range on test loop; SC-HPF effect measurable
- Clipper maintains loudness within ±0.5 dB (Mix=50%)
- E2E playback shows 0 long tasks; memory <100MB after 5 min; jitter <2 ms

Implementation Steps
1) Add fixtures: test loops/impulses under `tests/fixtures/`
2) Unit tests for compressor/limiter/clipper and FX using OfflineAudioContext
3) Integration test: full chain render and meter reading assertion
4) Perf script: dev-only measure CPU/jitter and log summary

Affected Files
- `tests/fixtures/*`, `tests/unit/dynamics/*.test.ts`, `tests/unit/fx/*.test.ts`
- `tests/integration/audio-chain.test.ts`
- `src/ui/visualizer.ts` (optional perf panel hooks)

Risks & Mitigations
- Cross-browser differences: focus Chrome stable; note Safari quirks
- OfflineAudioContext limitations: test relative properties, not absolute spectra

