# 61 — Parameter Registry, UI Mapping, and State Persistence

Read `.sdd/architect.md` and `.sdd/coding_rules.md` first.

Context
- Spec: `.sdd/backlog/tasks/004-rehaul-sounds-implement.md` (global: expose parameters)
- Goal: Centralize parameters (IDs, ranges, defaults), bind to UI controls, persist in store/localStorage.

Dependencies
- Tickets 56–60 provide DSP endpoints
- Ticket 11 (localStorage persistence)

Scope
- Parameter registry `src/audio/params/registry.ts` with metadata (min/max/default, unit, scaling)
- UI mapping: controls ↔ engine setters; add knobs/faders/switches for new params
- State persistence: serialize/deserialize parameters with schema validation
- Automation IDs reserved for future VST parity (stable IDs)

Acceptance Criteria
- All new dynamics/FX/voice params appear in UI with correct ranges and tooltips
- Changes are reflected immediately in audio; persisted and restored on reload
- TypeScript strict, no any; unit tests validate serialization schema

Implementation Steps
1) Create registry with typed descriptors and helper coercion/scaling functions
2) Update `src/ui/controls.ts` to generate controls from registry where possible
3) Extend store in `src/state/store.ts` to hold new params; wire to localStorage
4) Add tests `tests/unit/params/registry.test.ts` and `tests/unit/state/serialization.test.ts`

Affected Files
- `src/audio/params/registry.ts` (new)
- `src/ui/controls.ts`, `src/state/store.ts`, `src/storage/localStorage.ts`
- `tests/unit/params/*`, `tests/unit/state/*`

Risks & Mitigations
- UI clutter: group controls per section (Voices/Dynamics/FX); hide advanced
- Backward-compat: version config schema; provide migration defaults

