# CR-717 Drum Machine - Architecture Specification

**Project:** cr717  
**Domain:** audio-synthesis  
**Stack:** Node.js/Web Audio API  
**Year:** 2025

---

## Goals & Non-Goals

### Goals
1. **Authentic 808 sound** - Synthesize BD, SD, CH, OH, CP using Web Audio API oscillators and noise
2. **Stable timing** - 16-step sequencer with <2ms jitter at 60-240 BPM
3. **Browser-native** - No backend, no external audio libraries, works offline
4. **Responsive UI** - Desktop + tablet, visual step highlighting during playback
5. **Pattern persistence** - Save/load patterns via localStorage

### Non-Goals
1. **Sample-based playback** - Pure synthesis only (no .wav files)
2. **Backend/cloud sync** - Fully client-side application
3. **Mobile optimization** - Focus on desktop/tablet (mobile is P3)
4. **Advanced effects** - No reverb, delay, compression (keep it minimal)
5. **Multi-pattern sequencing** - Single pattern at a time (no song mode)

---

## Architecture Overview

### High-Level Diagram
```
┌─────────────────────────────────────────────────────────────┐
│                        Browser                               │
│  ┌───────────────────────────────────────────────────────┐  │
│  │                    UI Layer                            │  │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────┐           │  │
│  │  │ Sequencer│  │ Controls │  │ Pattern  │           │  │
│  │  │   Grid   │  │ (BPM/etc)│  │ Manager  │           │  │
│  │  └────┬─────┘  └────┬─────┘  └────┬─────┘           │  │
│  └───────┼─────────────┼─────────────┼──────────────────┘  │
│          │             │             │                      │
│  ┌───────▼─────────────▼─────────────▼──────────────────┐  │
│  │              Application State                        │  │
│  │  - Current pattern (16x5 grid)                       │  │
│  │  - Playback state (playing/stopped)                  │  │
│  │  - BPM, current step                                 │  │
│  └───────┬──────────────────────────┬───────────────────┘  │
│          │                          │                      │
│  ┌───────▼──────────┐      ┌────────▼──────────────────┐  │
│  │   Scheduler      │      │   Storage Manager         │  │
│  │  (Look-ahead)    │      │   (localStorage)          │  │
│  └───────┬──────────┘      └───────────────────────────┘  │
│          │                                                 │
│  ┌───────▼─────────────────────────────────────────────┐  │
│  │            Audio Engine (Web Audio API)             │  │
│  │  ┌──────┐  ┌──────┐  ┌──────┐  ┌──────┐  ┌──────┐ │  │
│  │  │  BD  │  │  SD  │  │  CH  │  │  OH  │  │  CP  │ │  │
│  │  │Voice │  │Voice │  │Voice │  │Voice │  │Voice │ │  │
│  │  └──┬───┘  └──┬───┘  └──┬───┘  └──┬───┘  └──┬───┘ │  │
│  │     └─────────┴─────────┴─────────┴─────────┘      │  │
│  │                    AudioContext                     │  │
│  │                    (destination)                    │  │
│  └─────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

### Data Schema
```typescript
// Pattern data structure
interface Pattern {
  name: string;
  bpm: number;
  steps: {
    BD: boolean[16];  // Bass Drum
    SD: boolean[16];  // Snare Drum
    CH: boolean[16];  // Closed Hi-Hat
    OH: boolean[16];  // Open Hi-Hat
    CP: boolean[16];  // Clap
  };
}

// Playback state
interface PlaybackState {
  isPlaying: boolean;
  currentStep: number;
  nextNoteTime: number;
  scheduleAheadTime: number; // 0.1s
}
```

### External Integrations
- **None** - Fully self-contained browser application
- **localStorage** - Browser API for pattern persistence (quota: ~5-10MB)

---

## MCDM for Major Choices

### Decision: Timing Architecture

**Criteria & Weights** (BWM method):
- **PerfGain** (0.25) - Timing accuracy and jitter reduction
- **DevTime** (0.20) - Implementation complexity
- **Maintainability** (0.20) - Code clarity and debuggability
- **SecRisk** (0.10) - Security implications
- **Cost** (0.10) - Hosting/runtime costs
- **Scalability** (0.10) - Future expansion capability
- **DX** (0.05) - Developer experience

**Alternatives:**

| Alternative | PerfGain | DevTime | Maintainability | SecRisk | Cost | Scalability | DX | Weighted Score |
|-------------|----------|---------|-----------------|---------|------|-------------|----|----|
| **A) Main-thread Look-ahead** | 7 | 9 | 8 | 9 | 9 | 6 | 8 | **7.85** ✅ |
| B) AudioWorklet Clock | 9 | 5 | 6 | 9 | 8 | 9 | 5 | 7.35 |
| C) Web Worker Scheduler | 8 | 6 | 7 | 9 | 9 | 7 | 6 | 7.45 |

**Recommendation:** **Alternative A - Main-thread Look-ahead Scheduler**

**Rationale:**
- **Sufficient for MVP**: 16-step 808 at 240 BPM = ~156ms per step, well within main-thread capabilities
- **Simpler implementation**: No Worklet build complexity, easier debugging
- **Good enough timing**: <2ms jitter achievable with 25ms tick interval + 100ms look-ahead
- **Migration path**: Can upgrade to Worklet (Alt B) if jitter becomes issue

**Trade-offs:**
- ✅ Faster to implement and test
- ✅ No COOP/COEP header requirements
- ⚠️ Vulnerable to UI blocking (mitigated by keeping UI lightweight)
- ⚠️ Less suitable for >16 steps or complex DSP (not in scope)

**Rollback plan:** If jitter >2ms observed in production, migrate scheduler to Web Worker (Alt C) first, then Worklet (Alt B) if needed.

---

## Key Decisions (ADR-style)

### [ADR-001] Look-ahead Scheduler on Main Thread
**Context:** Need stable timing for 16-step sequencer at 60-240 BPM.  
**Decision:** Use main-thread `setInterval` with 100ms look-ahead window.  
**Alternatives:** AudioWorklet clock, Web Worker scheduler.  
**Rationale:** Simpler for MVP, sufficient timing accuracy, easier debugging.  
**Consequences:** Must keep UI lightweight to avoid blocking scheduler.

### [ADR-002] Pure Synthesis (No Samples)
**Context:** Need authentic 808 sounds.  
**Decision:** Synthesize all sounds using Web Audio API oscillators and noise.  
**Alternatives:** Use sample playback, hybrid approach.  
**Rationale:** Meets project requirement, smaller bundle size, more authentic to original 808 analog circuits.  
**Consequences:** More complex synthesis code, requires careful envelope tuning.

### [ADR-003] localStorage for Persistence
**Context:** Need to save/load patterns.  
**Decision:** Use localStorage with JSON serialization.  
**Alternatives:** IndexedDB, cloud storage, file download/upload.  
**Rationale:** Simplest API, synchronous, sufficient for single pattern (~1KB).  
**Consequences:** 5-10MB quota limit (not an issue), no cross-device sync.

### [ADR-004] Vanilla JavaScript (No Framework)
**Context:** Need responsive UI for sequencer grid.  
**Decision:** Use vanilla JS with direct DOM manipulation.  
**Alternatives:** React, Vue, Svelte.  
**Rationale:** Minimal bundle size, no build complexity, sufficient for simple UI.  
**Consequences:** More manual DOM updates, but UI is simple enough.

### [ADR-005] Gesture-Gated Audio Start
**Context:** Browser autoplay policies require user interaction.  
**Decision:** Show Play button that calls `audioContext.resume()`.  
**Alternatives:** Auto-start on page load (blocked), muted start.  
**Rationale:** Required by Safari/Chrome autoplay policies.  
**Consequences:** User must click Play before hearing sound (expected UX).

---

## Components

### 1. Audio Engine (`src/audio/`)
**Responsibility:** Web Audio API management, sound synthesis, timing.

**Modules:**
- `audioContext.ts` - AudioContext singleton, initialization
- `voices/` - Individual drum voice synthesizers
  - `bassDrum.ts` - BD synthesis (oscillator + pitch envelope)
  - `snareDrum.ts` - SD synthesis (oscillator + noise + filter)
  - `closedHiHat.ts` - CH synthesis (filtered noise burst)
  - `openHiHat.ts` - OH synthesis (longer filtered noise)
  - `clap.ts` - CP synthesis (noise bursts with delay)
- `scheduler.ts` - Look-ahead scheduler, step timing
- `types.ts` - Audio-related TypeScript interfaces

**Interfaces:**
```typescript
interface Voice {
  play(context: AudioContext, time: number): void;
}

interface Scheduler {
  start(bpm: number, pattern: Pattern): void;
  stop(): void;
  getCurrentStep(): number;
}
```

**Dependencies:** Web Audio API (browser native)

---

### 2. UI Layer (`src/ui/`)
**Responsibility:** User interface, event handling, visual feedback.

**Modules:**
- `sequencerGrid.ts` - 16x5 step grid, click handlers
- `controls.ts` - Play/Stop, BPM slider, pattern buttons
- `visualizer.ts` - Current step highlighting
- `dom.ts` - DOM utilities, element creation

**Interfaces:**
```typescript
interface SequencerGrid {
  render(pattern: Pattern): void;
  onStepToggle(callback: (instrument: string, step: number) => void): void;
  highlightStep(step: number): void;
}
```

**Dependencies:** DOM API

---

### 3. State Management (`src/state/`)
**Responsibility:** Application state, pattern data, playback state.

**Modules:**
- `store.ts` - Central state store (simple observable)
- `pattern.ts` - Pattern data structure and operations
- `playback.ts` - Playback state management

**Interfaces:**
```typescript
interface Store {
  getState(): AppState;
  setState(partial: Partial<AppState>): void;
  subscribe(listener: (state: AppState) => void): () => void;
}
```

---

### 4. Storage Manager (`src/storage/`)
**Responsibility:** Pattern persistence via localStorage.

**Modules:**
- `localStorage.ts` - Save/load patterns
- `serialization.ts` - JSON serialization/deserialization

**Interfaces:**
```typescript
interface StorageManager {
  savePattern(pattern: Pattern): void;
  loadPattern(): Pattern | null;
  clearPattern(): void;
}
```

---

## API Contracts

### Audio Engine API
```typescript
// Initialize audio context (call on user gesture)
function initAudio(): Promise<AudioContext>

// Play individual voice
function playVoice(
  voice: 'BD' | 'SD' | 'CH' | 'OH' | 'CP',
  context: AudioContext,
  time: number
): void

// Scheduler control
function startScheduler(
  context: AudioContext,
  bpm: number,
  pattern: Pattern,
  onStep: (step: number) => void
): void

function stopScheduler(): void
```

### Storage API
```typescript
// Save pattern to localStorage
function savePattern(pattern: Pattern): void
// Returns: void
// Throws: QuotaExceededError if storage full

// Load pattern from localStorage
function loadPattern(): Pattern | null
// Returns: Pattern or null if not found
// Throws: SyntaxError if corrupted data
```

---

## Data Model

### Pattern Model
```typescript
interface Pattern {
  name: string;           // Pattern name
  bpm: number;            // 60-240
  steps: {
    BD: boolean[16];
    SD: boolean[16];
    CH: boolean[16];
    OH: boolean[16];
    CP: boolean[16];
  };
}

// Default empty pattern
const EMPTY_PATTERN: Pattern = {
  name: 'Untitled',
  bpm: 120,
  steps: {
    BD: Array(16).fill(false),
    SD: Array(16).fill(false),
    CH: Array(16).fill(false),
    OH: Array(16).fill(false),
    CP: Array(16).fill(false),
  }
};
```

### localStorage Schema
```
Key: "tr808:v1:pattern"
Value: JSON.stringify(Pattern)
Size: ~1KB per pattern
```

**Migration Policy:** Version prefix (`v1`) allows future schema changes. On load, check version and migrate if needed.

---

## Quality & Operations

### Testing Strategy

**Unit Tests** (Vitest)
- Voice synthesis: render with OfflineAudioContext, verify envelope shapes
- Scheduler: mock time, verify step advancement
- Storage: save/load round-trip, quota handling
- Pattern operations: toggle steps, BPM validation

**Integration Tests**
- Audio + Scheduler: verify first sound within 150ms of start
- UI + State: click step → state updates → UI reflects change

**E2E Tests** (Playwright)
- Load page → click Play → verify audio starts
- Toggle steps → verify visual feedback
- Change BPM → verify tempo changes
- Save/load pattern → verify persistence

**Performance Tests**
- Long Tasks: 0 tasks >50ms during playback
- Jitter: measure step timing variance <2ms
- Memory: <100MB during 5min playback

### Observability

**Metrics:**
- `scheduler.jitter` - Step timing variance (ms)
- `scheduler.drift` - Cumulative drift from ideal time (ms)
- `longtasks.count` - Number of >50ms tasks during playback
- `audio.latency` - Time from click to first sound (ms)

**Logging:**
- Console debug logs for scheduler ticks (dev only)
- Error logs for audio context failures
- Warning logs for localStorage quota issues

**Monitoring:**
- In-app debug panel showing real-time jitter/drift
- Performance.mark/measure for key operations

### Security

**Threats & Mitigations:**
- **XSS**: No user-generated content, static HTML
- **Supply chain**: Lock dependencies, `npm audit` in CI
- **Data exfiltration**: No network requests, fully offline
- **localStorage poisoning**: Validate JSON on load, catch errors

**CSP Header:**
```
Content-Security-Policy: default-src 'self'; script-src 'self'; style-src 'self' 'unsafe-inline'
```

### CI/CD Pipeline

**Build:**
```bash
npm run build  # Vite build
```

**Quality Gates:**
```bash
npm run lint        # ESLint
npm run typecheck   # TypeScript
npm run test        # Vitest unit tests
npm run e2e         # Playwright E2E tests
```

**Deployment:**
- Static hosting (Netlify/Vercel/GitHub Pages)
- No server-side logic required
- Atomic deploys (no downtime)

---

## Stop Rules & Preconditions

### Go Preconditions
✅ Modern browser (Chrome 90+, Firefox 88+, Safari 14+)  
✅ JavaScript enabled  
✅ Web Audio API support  
✅ localStorage available (not in private mode)

### No-Go Conditions
❌ Web Audio API not supported → Show error message  
❌ localStorage disabled → Disable save/load, show warning  
❌ Autoplay blocked → Require user gesture (expected)

### Halt & Escalate If:
- Jitter consistently >5ms → Investigate scheduler, consider Worklet migration
- Memory leak detected (>200MB) → Profile and fix before release
- Audio glitches/dropouts → Check for blocking operations in scheduler

---

## SLOs & Guardrails

### Service Level Objectives

| Metric | Target | Measurement |
|--------|--------|-------------|
| **Timing Jitter** | ≤2ms @ 120 BPM | `getOutputTimestamp()` vs scheduled time |
| **Audio Latency** | ≤150ms | Time from Play click to first sound |
| **Long Tasks** | 0 during playback | PerformanceObserver('longtask') |
| **Memory** | <100MB | performance.memory (Chrome) |
| **Load Time** | <2s on 3G | Lighthouse |

### Performance Budgets
- **Bundle size**: <50KB gzipped
- **Main thread CPU**: <30% p95 during playback
- **Audio dropouts**: 0 per session
- **localStorage**: <10KB per pattern

### Cost Guardrails
- **Hosting**: Free tier (static hosting)
- **No runtime costs**: Fully client-side

---

# CR-717 Architecture - Implementation Plan & Backlog

## Affected Modules/Files

### Files to Create

**Core Structure:**
```
src/
├── main.ts                    # Entry point, initialization
├── audio/
│   ├── audioContext.ts        # AudioContext singleton
│   ├── scheduler.ts           # Look-ahead scheduler
│   ├── types.ts               # Audio interfaces
│   └── voices/
│       ├── bassDrum.ts        # BD synthesis
│       ├── snareDrum.ts       # SD synthesis
│       ├── closedHiHat.ts     # CH synthesis
│       ├── openHiHat.ts       # OH synthesis
│       └── clap.ts            # CP synthesis
├── ui/
│   ├── sequencerGrid.ts       # 16x5 grid UI
│   ├── controls.ts            # Play/Stop/BPM controls
│   ├── visualizer.ts          # Step highlighting
│   └── dom.ts                 # DOM utilities
├── state/
│   ├── store.ts               # State management
│   ├── pattern.ts             # Pattern data structure
│   └── playback.ts            # Playback state
└── storage/
    ├── localStorage.ts        # Pattern persistence
    └── serialization.ts       # JSON serialization

tests/
├── unit/
│   ├── voices.test.ts         # Voice synthesis tests
│   ├── scheduler.test.ts      # Scheduler tests
│   └── storage.test.ts        # Storage tests
└── e2e/
    └── playback.spec.ts       # Playwright E2E tests

index.html                     # Main HTML
styles.css                     # Minimal CSS
vite.config.ts                 # Vite configuration
tsconfig.json                  # TypeScript config
playwright.config.ts           # Playwright config
```

---

## Implementation Steps

### Phase 1: Foundation (Days 1-2)
1. **Setup project structure** - Vite + TypeScript + Vitest + Playwright
2. **Create AudioContext singleton** - Initialize on user gesture
3. **Implement basic scheduler** - Look-ahead with 100ms window
4. **Create Pattern data structure** - TypeScript interfaces

### Phase 2: Audio Engine (Days 3-5)
5. **Implement BD voice** - Oscillator + pitch envelope (150→50Hz)
6. **Implement SD voice** - Oscillator + noise + filter
7. **Implement CH voice** - Filtered noise burst (70ms decay)
8. **Implement OH voice** - Longer filtered noise (200ms decay)
9. **Implement CP voice** - Multiple noise bursts with delay
10. **Wire voices to scheduler** - Play on scheduled steps

### Phase 3: UI (Days 6-7)
11. **Create sequencer grid** - 16x5 buttons, toggle on click
12. **Create controls** - Play/Stop button, BPM slider
13. **Implement step highlighting** - Visual feedback during playback
14. **Connect UI to state** - Two-way data binding

### Phase 4: Persistence (Day 8)
15. **Implement localStorage save/load** - JSON serialization
16. **Add pattern name input** - Editable pattern name
17. **Handle quota errors** - Graceful degradation

### Phase 5: Testing & Polish (Days 9-10)
18. **Write unit tests** - Voices, scheduler, storage
19. **Write E2E tests** - Playwright playback scenarios
20. **Performance optimization** - Profile and fix bottlenecks
21. **Cross-browser testing** - Chrome, Firefox, Safari

---

## Backlog (Tickets)

### Ticket 01: Project Setup & Foundation
**File:** `backlog/open/01-project-setup.md`

**Objective:** Initialize project with Vite, TypeScript, and testing tools.

**Acceptance Criteria:**
- ✅ Vite project created with TypeScript
- ✅ Vitest configured for unit tests
- ✅ Playwright configured for E2E tests
- ✅ ESLint + Prettier configured
- ✅ `npm run dev` starts dev server
- ✅ `npm run build` produces optimized bundle
- ✅ `npm run test` runs unit tests
- ✅ `npm run e2e` runs E2E tests

**Steps:**
1. `npm create vite@latest cr717 -- --template vanilla-ts`
2. Install dependencies: `vitest`, `@playwright/test`, `eslint`, `prettier`
3. Create config files: `vite.config.ts`, `vitest.config.ts`, `playwright.config.ts`
4. Setup project structure: `src/`, `tests/`, `public/`
5. Create basic `index.html` with canvas/grid placeholder

**Affected Files:**
- `package.json` - Dependencies and scripts
- `vite.config.ts` - Vite configuration
- `tsconfig.json` - TypeScript strict mode
- `playwright.config.ts` - E2E test configuration
- `.eslintrc.json` - Linting rules

**Tests:**
- Verify `npm run dev` starts without errors
- Verify `npm run build` produces `dist/` folder
- Verify `npm test` runs (even if no tests yet)

**Risks:**
- Dependency conflicts → Use exact versions from package-lock.json

**Dependencies:** None

---

### Ticket 02: AudioContext & Scheduler Core
**File:** `backlog/open/02-audio-scheduler.md`

**Objective:** Implement AudioContext singleton and look-ahead scheduler.

**Acceptance Criteria:**
- ✅ AudioContext initializes on user gesture
- ✅ Scheduler ticks every 25ms
- ✅ Schedules notes 100ms ahead
- ✅ Tracks current step (0-15)
- ✅ Jitter <2ms measured via `getOutputTimestamp()`

**Steps:**
1. Create `src/audio/audioContext.ts`:
   ```typescript
   let audioContext: AudioContext | null = null;
   export async function initAudio(): Promise<AudioContext> {
     if (!audioContext) {
       audioContext = new AudioContext({ latencyHint: 'interactive' });
       await audioContext.resume();
     }
     return audioContext;
   }
   ```

2. Create `src/audio/scheduler.ts`:
   ```typescript
   export class Scheduler {
     private intervalId: number | null = null;
     private nextNoteTime: number = 0;
     private currentStep: number = 0;
     private scheduleAheadTime: number = 0.1;
     
     start(ctx: AudioContext, bpm: number, onStep: (step: number, time: number) => void) {
       const stepDuration = 60 / bpm / 4; // 16th notes
       this.nextNoteTime = ctx.currentTime;
       
       this.intervalId = window.setInterval(() => {
         const end = ctx.currentTime + this.scheduleAheadTime;
         while (this.nextNoteTime < end) {
           onStep(this.currentStep, this.nextNoteTime);
           this.nextNoteTime += stepDuration;
           this.currentStep = (this.currentStep + 1) % 16;
         }
       }, 25);
     }
     
     stop() {
       if (this.intervalId) clearInterval(this.intervalId);
     }
   }
   ```

3. Create unit test in `tests/unit/scheduler.test.ts`
4. Measure jitter using `performance.mark()` and `getOutputTimestamp()`

**Affected Files:**
- `src/audio/audioContext.ts` (new)
- `src/audio/scheduler.ts` (new)
- `src/audio/types.ts` (new) - TypeScript interfaces
- `tests/unit/scheduler.test.ts` (new)

**Tests:**
- Unit: Mock time, verify step advancement
- Unit: Verify jitter calculation
- Integration: Start/stop scheduler, verify no memory leaks

**Risks:**
- Autoplay blocked → Mitigated by user gesture requirement
- Jitter >2ms → Profile and optimize interval timing

**Dependencies:** Ticket 01

---

### Ticket 03: Bass Drum (BD) Voice
**File:** `backlog/open/03-voice-bass-drum.md`

**Objective:** Implement BD synthesis with pitch envelope.

**Acceptance Criteria:**
- ✅ BD plays on trigger with authentic 808 sound
- ✅ Pitch drops from 150Hz to 50Hz over 80ms
- ✅ Amplitude envelope: attack 0ms, decay 350ms
- ✅ Unit test verifies envelope shape in OfflineAudioContext

**Steps:**
1. Create `src/audio/voices/bassDrum.ts`:
   ```typescript
   export function playBD(ctx: AudioContext, time: number): void {
     const osc = new OscillatorNode(ctx, { type: 'sine', frequency: 150 });
     const gain = new GainNode(ctx, { gain: 1.0 });
     
     osc.connect(gain).connect(ctx.destination);
     
     // Pitch envelope: 150Hz → 50Hz over 80ms
     osc.frequency.setValueAtTime(150, time);
     osc.frequency.exponentialRampToValueAtTime(50, time + 0.08);
     
     // Amplitude envelope: 1.0 → 0.0001 over 350ms
     gain.gain.setValueAtTime(1.0, time);
     gain.gain.exponentialRampToValueAtTime(0.0001, time + 0.35);
     
     osc.start(time);
     osc.stop(time + 0.5);
   }
   ```

2. Create unit test using OfflineAudioContext:
   ```typescript
   test('BD has correct pitch envelope', async () => {
     const ctx = new OfflineAudioContext(1, 44100, 44100);
     playBD(ctx, 0);
     const buffer = await ctx.startRendering();
     // Verify frequency content at t=0 and t=0.08
   });
   ```

**Affected Files:**
- `src/audio/voices/bassDrum.ts` (new)
- `tests/unit/voices.test.ts` (new)

**Tests:**
- Unit: Render with OfflineAudioContext, verify envelope
- Integration: Play BD via scheduler, verify timing

**Risks:**
- Sound not authentic → Adjust envelope parameters iteratively

**Dependencies:** Ticket 02

---

### Ticket 04: Snare Drum (SD) Voice
**File:** `backlog/open/04-voice-snare-drum.md`

**Objective:** Implement SD synthesis with oscillator + noise.

**Acceptance Criteria:**
- ✅ SD plays with tonal + noise components
- ✅ Oscillator: 180Hz triangle wave
- ✅ Noise: filtered white noise (highpass 1kHz)
- ✅ Envelope: 150ms decay

**Steps:**
1. Create `src/audio/voices/snareDrum.ts`:
   ```typescript
   export function playSD(ctx: AudioContext, time: number): void {
     // Tonal component (oscillator)
     const osc = new OscillatorNode(ctx, { type: 'triangle', frequency: 180 });
     const oscGain = new GainNode(ctx, { gain: 0.7 });
     
     // Noise component
     const buffer = ctx.createBuffer(1, ctx.sampleRate * 0.2, ctx.sampleRate);
     const data = buffer.getChannelData(0);
     for (let i = 0; i < data.length; i++) data[i] = Math.random() * 2 - 1;
     
     const noise = new AudioBufferSourceNode(ctx, { buffer });
     const noiseFilter = new BiquadFilterNode(ctx, { 
       type: 'highpass', 
       frequency: 1000, 
       Q: 1 
     });
     const noiseGain = new GainNode(ctx, { gain: 0.5 });
     
     // Connect
     osc.connect(oscGain);
     noise.connect(noiseFilter).connect(noiseGain);
     
     const master = new GainNode(ctx, { gain: 1.0 });
     oscGain.connect(master);
     noiseGain.connect(master);
     master.connect(ctx.destination);
     
     // Envelope
     master.gain.setValueAtTime(1.0, time);
     master.gain.exponentialRampToValueAtTime(0.0001, time + 0.15);
     
     osc.start(time);
     noise.start(time);
     osc.stop(time + 0.2);
   }
   ```

**Affected Files:**
- `src/audio/voices/snareDrum.ts` (new)
- `tests/unit/voices.test.ts` (update)

**Tests:**
- Unit: Verify oscillator + noise mix
- Integration: Play SD via scheduler

**Risks:**
- Noise generation performance → Pre-generate noise buffer

**Dependencies:** Ticket 02

---

### Ticket 05: Hi-Hat Voices (CH & OH)
**File:** `backlog/open/05-voice-hi-hats.md`

**Objective:** Implement closed and open hi-hat synthesis.

**Acceptance Criteria:**
- ✅ CH: Short filtered noise burst (70ms decay)
- ✅ OH: Longer filtered noise burst (200ms decay)
- ✅ Both use highpass filter (8kHz)

**Steps:**
1. Create `src/audio/voices/closedHiHat.ts` and `openHiHat.ts`
2. Use filtered white noise with different decay times
3. Add unit tests for both voices

**Affected Files:**
- `src/audio/voices/closedHiHat.ts` (new)
- `src/audio/voices/openHiHat.ts` (new)
- `tests/unit/voices.test.ts` (update)

**Tests:**
- Unit: Verify decay times (70ms vs 200ms)
- Integration: Play both via scheduler

**Risks:**
- Sounds too similar → Adjust filter Q and decay

**Dependencies:** Ticket 02

---

### Ticket 06: Clap (CP) Voice
**File:** `backlog/open/06-voice-clap.md`

**Objective:** Implement clap synthesis with multiple noise bursts.

**Acceptance Criteria:**
- ✅ CP: 3-4 noise bursts with slight delay
- ✅ Bandpass filter (1-3kHz)
- ✅ Total duration ~100ms

**Steps:**
1. Create `src/audio/voices/clap.ts`
2. Generate 3-4 short noise bursts at t, t+0.01, t+0.02, t+0.03
3. Apply bandpass filter and envelope

**Affected Files:**
- `src/audio/voices/clap.ts` (new)
- `tests/unit/voices.test.ts` (update)

**Tests:**
- Unit: Verify multiple bursts timing
- Integration: Play CP via scheduler

**Risks:**
- Sounds unnatural → Adjust burst timing and filter

**Dependencies:** Ticket 02

---

### Ticket 07: Sequencer Grid UI
**File:** `backlog/open/07-sequencer-grid-ui.md`

**Objective:** Create 16x5 step grid with click handlers.

**Acceptance Criteria:**
- ✅ 16 columns (steps) × 5 rows (instruments)
- ✅ Click toggles step on/off
- ✅ Visual feedback (active/inactive states)
- ✅ Responsive layout (desktop + tablet)

**Steps:**
1. Create `src/ui/sequencerGrid.ts`
2. Generate 80 buttons (16×5) with data attributes
3. Add click event listeners
4. Style with CSS grid

**Affected Files:**
- `src/ui/sequencerGrid.ts` (new)
- `src/ui/dom.ts` (new) - DOM utilities
- `styles.css` (update)
- `index.html` (update)

**Tests:**
- E2E: Click step → verify state change
- E2E: Verify responsive layout on different viewports

**Risks:**
- Performance with 80 buttons → Use event delegation

**Dependencies:** Ticket 01

---

### Ticket 08: Playback Controls
**File:** `backlog/open/08-playback-controls.md`

**Objective:** Create Play/Stop button and BPM slider.

**Acceptance Criteria:**
- ✅ Play button starts playback (gesture-gated)
- ✅ Stop button stops playback
- ✅ BPM slider (60-240) updates tempo in real-time
- ✅ Visual feedback (playing/stopped state)

**Steps:**
1. Create `src/ui/controls.ts`
2. Add Play/Stop button with click handler
3. Add BPM slider with input handler
4. Connect to scheduler start/stop

**Affected Files:**
- `src/ui/controls.ts` (new)
- `index.html` (update)
- `styles.css` (update)

**Tests:**
- E2E: Click Play → verify audio starts
- E2E: Change BPM → verify tempo changes
- E2E: Click Stop → verify audio stops

**Risks:**
- Autoplay blocked → Show clear CTA to click Play

**Dependencies:** Tickets 02, 07

---

### Ticket 09: Step Highlighting
**File:** `backlog/open/09-step-highlighting.md`

**Objective:** Visual feedback showing current step during playback.

**Acceptance Criteria:**
- ✅ Current step column highlighted
- ✅ Highlighting synced with audio (via `getOutputTimestamp()`)
- ✅ Smooth animation (no jank)

**Steps:**
1. Create `src/ui/visualizer.ts`
2. Use `requestAnimationFrame` to update highlight
3. Calculate current step from `audioContext.getOutputTimestamp()`
4. Add/remove CSS class for highlighting

**Affected Files:**
- `src/ui/visualizer.ts` (new)
- `styles.css` (update)

**Tests:**
- E2E: Verify highlighting advances during playback
- Performance: Verify no long tasks during highlighting

**Risks:**
- Highlighting lag → Optimize RAF callback

**Dependencies:** Tickets 02, 07, 08

---

### Ticket 10: State Management
**File:** `backlog/open/10-state-management.md`

**Objective:** Central state store for pattern and playback state.

**Acceptance Criteria:**
- ✅ Store holds pattern data (16×5 grid)
- ✅ Store holds playback state (playing/stopped, BPM, current step)
- ✅ Subscribe/notify pattern for state changes
- ✅ TypeScript interfaces for all state

**Steps:**
1. Create `src/state/store.ts` - Simple observable store
2. Create `src/state/pattern.ts` - Pattern data structure
3. Create `src/state/playback.ts` - Playback state
4. Connect UI and audio engine to store

**Affected Files:**
- `src/state/store.ts` (new)
- `src/state/pattern.ts` (new)
- `src/state/playback.ts` (new)
- `tests/unit/state.test.ts` (new)

**Tests:**
- Unit: State updates trigger subscribers
- Unit: Pattern operations (toggle step, set BPM)

**Risks:**
- Over-engineering → Keep it simple (no Redux/MobX)

**Dependencies:** Ticket 01

---

### Ticket 11: localStorage Persistence
**File:** `backlog/open/11-localstorage-persistence.md`

**Objective:** Save/load patterns via localStorage.

**Acceptance Criteria:**
- ✅ Save pattern on change
- ✅ Load pattern on page load
- ✅ Handle quota errors gracefully
- ✅ Validate JSON on load

**Steps:**
1. Create `src/storage/localStorage.ts`
2. Implement save/load with JSON serialization
3. Add error handling for quota exceeded
4. Add validation for loaded data

**Affected Files:**
- `src/storage/localStorage.ts` (new)
- `src/storage/serialization.ts` (new)
- `tests/unit/storage.test.ts` (new)

**Tests:**
- Unit: Save/load round-trip
- Unit: Handle corrupted data
- Unit: Handle quota exceeded

**Risks:**
- localStorage disabled → Show warning, disable save/load

**Dependencies:** Ticket 10

---

### Ticket 12: Integration & E2E Tests
**File:** `backlog/open/12-integration-e2e-tests.md`

**Objective:** Comprehensive test coverage for full user flows.

**Acceptance Criteria:**
- ✅ E2E: Load page → click Play → verify audio
- ✅ E2E: Toggle steps → verify pattern changes
- ✅ E2E: Change BPM → verify tempo changes
- ✅ E2E: Save/load pattern → verify persistence
- ✅ Performance: 0 long tasks during playback

**Steps:**
1. Write Playwright tests in `tests/e2e/playback.spec.ts`
2. Test all user flows
3. Add performance assertions (Long Tasks API)
4. Test cross-browser (Chrome, Firefox, Safari)

**Affected Files:**
- `tests/e2e/playback.spec.ts` (new)
- `playwright.config.ts` (update)

**Tests:**
- E2E: Full user journey
- Performance: Long tasks, jitter, memory

**Risks:**
- Flaky tests → Add proper waits and retries

**Dependencies:** All previous tickets

---

### Ticket 13: Performance Optimization
**File:** `backlog/open/13-performance-optimization.md`

**Objective:** Meet all performance SLOs.

**Acceptance Criteria:**
- ✅ Bundle size <50KB gzipped
- ✅ Jitter <2ms @ 120 BPM
- ✅ 0 long tasks during playback
- ✅ Memory <100MB during 5min playback
- ✅ Load time <2s on 3G

**Steps:**
1. Profile with Chrome DevTools
2. Optimize bundle (tree-shaking, code-splitting)
3. Optimize scheduler (reduce interval if needed)
4. Optimize UI updates (throttle, RAF)
5. Run Lighthouse audit

**Affected Files:**
- `vite.config.ts` (optimization settings)
- Various files (performance fixes)

**Tests:**
- Performance: Lighthouse score >90
- Performance: Bundle size check
- Performance: Memory profiling

**Risks:**
- SLOs not met → May need Worklet migration

**Dependencies:** Ticket 12

---

### Ticket 14: Cross-Browser Testing & Polish
**File:** `backlog/open/14-cross-browser-polish.md`

**Objective:** Ensure compatibility and polish UX.

**Acceptance Criteria:**
- ✅ Works in Chrome 90+, Firefox 88+, Safari 14+
- ✅ Responsive on desktop and tablet
- ✅ Accessible (keyboard navigation, ARIA labels)
- ✅ Error messages for unsupported browsers
- ✅ Loading states and feedback

**Steps:**
1. Test in all target browsers
2. Add keyboard shortcuts (Space = Play/Stop)
3. Add ARIA labels for accessibility
4. Add error handling for unsupported features
5. Polish CSS and animations

**Affected Files:**
- Various files (browser compatibility fixes)
- `styles.css` (polish)
- `index.html` (ARIA labels)

**Tests:**
- E2E: Run in all browsers
- Manual: Keyboard navigation
- Manual: Screen reader testing

**Risks:**
- Safari quirks → May need workarounds

**Dependencies:** Ticket 13

---

## Ticket Dependency Graph

```
01 (Setup)
  ├─→ 02 (Audio/Scheduler)
  │     ├─→ 03 (BD Voice)
  │     ├─→ 04 (SD Voice)
  │     ├─→ 05 (HH Voices)
  │     └─→ 06 (CP Voice)
  ├─→ 07 (Grid UI)
  │     ├─→ 08 (Controls)
  │     └─→ 09 (Highlighting)
  └─→ 10 (State)
        └─→ 11 (Storage)

All → 12 (Integration Tests)
  └─→ 13 (Performance)
    └─→ 14 (Polish)
```

**Parallel Work Possible:**
- Tickets 03-06 (voices) can be done in parallel
- Tickets 07-09 (UI) can be done in parallel with voices
- Ticket 10-11 (state/storage) can be done in parallel with UI

**Critical Path:** 01 → 02 → 08 → 12 → 13 → 14

---

## Summary

**Total Tickets:** 14  
**Estimated Time:** 10-14 days  
**Critical Path:** 7 tickets  
**Parallel Work:** 6 tickets can be parallelized  

**Next Steps:**
1. Create ticket files in `backlog/open/`
2. Start with Ticket 01 (Project Setup)
3. Follow dependency order
4. Move completed tickets to `backlog/closed/`
