# CR-717 Coding Rules

**Project:** cr717  
**Stack:** Node.js/Web Audio API  
**Domain:** audio-synthesis  
**Year:** 2025

---

## Language & Style

**TypeScript:**
- Version: 5.3+
- Strict mode: `"strict": true` in tsconfig.json
- No `any` types (use `unknown` if needed)
- Explicit return types on public functions

**Naming:**
- Files: `kebab-case.ts` (e.g., `bass-drum.ts`)
- Classes/Interfaces: `PascalCase` (e.g., `AudioScheduler`)
- Functions/Variables: `camelCase` (e.g., `playBassDrum`)
- Constants: `UPPER_SNAKE_CASE` (e.g., `SAMPLE_RATE`)

**Formatting:**
- Prettier: `printWidth: 88`, `singleQuote: true`, `semi: true`
- ESLint: `@typescript-eslint/recommended`
- Line length: 88 characters max

**Commands:**
```bash
npm run format  # prettier --write "src/**/*.ts"
npm run lint    # eslint . --ext .ts
```

---

## Framework & Project Layout

**Structure:**
```
src/
├── audio/              # Web Audio API logic
│   ├── audioContext.ts # Singleton AudioContext
│   ├── scheduler.ts    # Look-ahead scheduler
│   ├── types.ts        # Audio interfaces
│   └── voices/         # Drum voice synthesizers
├── ui/                 # DOM manipulation
│   ├── sequencerGrid.ts
│   ├── controls.ts
│   └── visualizer.ts
├── state/              # State management
│   ├── store.ts
│   └── pattern.ts
├── storage/            # localStorage
│   └── localStorage.ts
└── main.ts             # Entry point
```

**Module Rules:**
- One class/interface per file
- Export only what's needed (no `export *`)
- Imports: absolute from `src/` (via tsconfig paths)
- No circular dependencies

**Environment:**
- Dev: `npm run dev` (Vite dev server)
- Build: `npm run build` (production bundle)
- No env vars needed (fully client-side)

---

## API & Contracts

**Internal APIs:**
```typescript
// Audio Engine API
interface Voice {
  play(context: AudioContext, time: number): void;
}

// Scheduler API
interface Scheduler {
  start(bpm: number, pattern: Pattern, onStep: (step: number) => void): void;
  stop(): void;
  getCurrentStep(): number;
}

// Storage API
interface StorageManager {
  savePattern(pattern: Pattern): void;
  loadPattern(): Pattern | null;
}
```

**Error Handling:**
- Use `try/catch` for async operations
- Throw typed errors: `class AudioInitError extends Error`
- Never swallow errors silently
- Log errors to console in dev, silent in prod

**Validation:**
- BPM: 60-240 range
- Pattern steps: exactly 16 per instrument
- localStorage data: validate JSON schema on load

---

## Testing

**Coverage Targets:**
- Overall: ≥80%
- Critical paths (scheduler, voices): ≥90%

**Libraries:**
- Unit: **Vitest** (fast, Vite-native)
- E2E: **Playwright** (cross-browser)
- Mocking: Vitest built-in mocks

**Test Structure:**
```typescript
// tests/unit/voices.test.ts
import { describe, it, expect } from 'vitest';
import { playBD } from '@/audio/voices/bassDrum';

describe('Bass Drum', () => {
  it('should play with correct pitch envelope', async () => {
    const ctx = new OfflineAudioContext(1, 44100, 44100);
    playBD(ctx, 0);
    const buffer = await ctx.startRendering();
    expect(buffer.getChannelData(0).some(v => v !== 0)).toBe(true);
  });
});
```

**Test Types:**
- **Unit:** Voices (OfflineAudioContext), scheduler logic, storage
- **Integration:** Audio + scheduler, UI + state
- **E2E:** Full user flows (Playwright)
- **Performance:** Long Tasks API, jitter measurement

**Commands:**
```bash
npm test              # Run all unit tests
npm run test:watch    # Watch mode
npm run e2e           # Run E2E tests
npm run test:coverage # Generate coverage report
```

---

## Security

**No Backend:** Fully client-side, no auth needed

**Dependencies:**
- Lock file: Commit `package-lock.json`
- Audit: `npm audit --production` in CI
- Updates: Review breaking changes before updating

**localStorage:**
- Validate JSON on load
- Catch `QuotaExceededError`
- No sensitive data (patterns only)

**CSP Header:**
```
Content-Security-Policy: default-src 'self'; script-src 'self'; style-src 'self' 'unsafe-inline'
```

**PII:** None (no user data collected)

---

## Observability

**Metrics:**
- Scheduler jitter: `performance.mark('step')` + `getOutputTimestamp()`
- Long tasks: `PerformanceObserver('longtask')`
- Memory: `performance.memory` (Chrome only)

**Logging:**
```typescript
// Dev only
if (import.meta.env.DEV) {
  console.debug('[Scheduler]', { step, time, drift });
}

// Errors always logged
console.error('[Audio]', error);
```

**Monitoring:**
- In-app debug panel (dev mode)
- No external analytics (privacy-first)

---

## Performance & Cost

**Budgets:**
- Bundle size: <50KB gzipped
- Main thread CPU: <30% p95 during playback
- Memory: <100MB during 5min session
- Long tasks: 0 during playback

**Profiling:**
```bash
npm run build        # Check bundle size
npm run preview      # Test production build
# Chrome DevTools → Performance tab
```

**Optimization:**
- Code splitting: Not needed (small app)
- Tree shaking: Enabled by Vite
- Minification: Enabled in production

**Cost:** $0 (static hosting, no runtime costs)

---

## Git & PR Process

**Branching:**
- Main: `master` (production-ready)
- Features: `feat/<ticket-number>-<description>`
- Fixes: `fix/<issue-description>`

**Commits:**
- Format: Conventional Commits
- Examples:
  - `feat(audio): add bass drum synthesis`
  - `fix(scheduler): correct jitter calculation`
  - `test(voices): add snare drum unit tests`
  - `docs(readme): update setup instructions`

**PR Checklist:**
- [ ] Tests pass (`npm test && npm run e2e`)
- [ ] Linting passes (`npm run lint`)
- [ ] Type checking passes (`tsc --noEmit`)
- [ ] Bundle size within budget
- [ ] No console errors in dev
- [ ] Tested in Chrome, Firefox, Safari
- [ ] Updated docs if needed

---

## Tooling

**Formatters:**
```json
// .prettierrc
{
  "semi": true,
  "singleQuote": true,
  "printWidth": 88,
  "tabWidth": 2,
  "trailingComma": "es5"
}
```

**Linters:**
```json
// .eslintrc.json
{
  "parser": "@typescript-eslint/parser",
  "extends": [
    "eslint:recommended",
    "plugin:@typescript-eslint/recommended",
    "prettier"
  ],
  "rules": {
    "@typescript-eslint/no-unused-vars": "error",
    "@typescript-eslint/explicit-function-return-type": "warn",
    "no-console": ["warn", { "allow": ["error", "warn"] }]
  }
}
```

**Pre-commit:**
```bash
# Optional: husky + lint-staged
npm install -D husky lint-staged
npx husky init
```

**CI Steps:**
```yaml
# .github/workflows/ci.yml
- run: npm ci
- run: npm run lint
- run: npm run typecheck
- run: npm test
- run: npm run build
- run: npm run e2e
```

---

## Frontend Standards

**Vanilla JS (No Framework):**
- Direct DOM manipulation
- Event delegation for performance
- `requestAnimationFrame` for animations

**Performance:**
- Web Vitals targets:
  - LCP: <2.5s
  - INP: <200ms
  - CLS: <0.1
- Bundle: <50KB gzipped
- No render-blocking resources

**Accessibility:**
- Keyboard navigation (Space = Play/Stop)
- ARIA labels on buttons
- Focus indicators
- Screen reader friendly

**Example:**
```typescript
// Good: Event delegation
sequencerGrid.addEventListener('click', (e) => {
  const target = e.target as HTMLElement;
  if (target.dataset.step) {
    toggleStep(Number(target.dataset.step));
  }
});

// Bad: Individual listeners
steps.forEach(step => {
  step.addEventListener('click', () => toggleStep(step.id));
});
```

---

## Backend Standards

**N/A** - Fully client-side application

---

## Governance & Decisions (MCDM)

**When to use:**
- Major architectural changes
- Library additions >10KB
- Breaking changes to APIs

**Criteria:**
- PerfGain (0.25)
- Maintainability (0.20)
- DevTime (0.20)
- Security (0.15)
- Cost (0.10)
- Scalability (0.05)
- DX (0.05)

**Example Decision:**
```markdown
## ADR-001: Look-ahead Scheduler vs AudioWorklet

| Alternative | PerfGain | Maintainability | DevTime | Security | Cost | Scalability | DX | Score |
|-------------|----------|-----------------|---------|----------|------|-------------|----|-------|
| Look-ahead  | 7        | 8               | 9       | 9        | 9    | 6           | 8  | 7.85  |
| Worklet     | 9        | 6               | 5       | 9        | 8    | 9           | 5  | 7.35  |

**Decision:** Look-ahead scheduler (simpler, sufficient for MVP)
```

---

## Acceptance Criteria

**Build:**
- [ ] `npm run build` succeeds
- [ ] Bundle size <50KB gzipped
- [ ] No TypeScript errors
- [ ] No ESLint errors

**Tests:**
- [ ] `npm test` passes (≥80% coverage)
- [ ] `npm run e2e` passes
- [ ] No console errors in tests

**Performance:**
- [ ] Jitter <2ms @ 120 BPM
- [ ] 0 long tasks during playback
- [ ] Memory <100MB after 5min

**Security:**
- [ ] `npm audit --production` clean
- [ ] No secrets in code
- [ ] localStorage validation works

**Docs:**
- [ ] README updated if needed
- [ ] JSDoc on public APIs
- [ ] ADRs for major decisions

---

## File Hygiene

**Rules:**
- Max file size: 400 LOC (split if larger)
- One responsibility per file
- Remove dead code immediately
- No commented-out code in commits

**Reusability:**
- Extract helpers to `src/utils/` if used 3+ times
- Keep voices independent (no shared state)
- Scheduler is stateful singleton (acceptable)

**State Management:**
- Avoid global mutable state
- Use store for shared state
- Pass dependencies explicitly

**Example:**
```typescript
// Good: Explicit dependencies
function playBD(ctx: AudioContext, time: number): void {
  // ...
}

// Bad: Hidden global dependency
let globalContext: AudioContext;
function playBD(time: number): void {
  globalContext.createOscillator(); // Where did this come from?
}
```

---

## Quick Reference

**Daily Commands:**
```bash
npm run dev          # Start dev server
npm test             # Run unit tests
npm run lint         # Check code style
npm run typecheck    # Check types
npm run build        # Production build
```

**Before Commit:**
```bash
npm run lint && npm run typecheck && npm test
```

**Before PR:**
```bash
npm run lint && npm run typecheck && npm test && npm run e2e && npm run build
```

---

## Anti-Patterns to Avoid

❌ **Don't:**
- Use `any` type
- Mutate function parameters
- Create global variables
- Write to localStorage in scheduler callback
- Use `ScriptProcessorNode` (deprecated)
- Start audio without user gesture
- Ignore TypeScript errors

✅ **Do:**
- Use strict TypeScript
- Keep functions pure when possible
- Pass dependencies explicitly
- Handle errors gracefully
- Use Web Audio API best practices
- Gate audio behind user interaction
- Write tests for critical paths

---

## Summary

**Core Principles:**
1. **Type Safety** - Strict TypeScript, no `any`
2. **Performance** - Meet all budgets, profile regularly
3. **Simplicity** - Vanilla JS, minimal dependencies
4. **Testing** - ≥80% coverage, E2E for critical flows
5. **Maintainability** - Small files, clear naming, no dead code

**Key Tools:**
- TypeScript 5.3+ (strict)
- Vite (build)
- Vitest (unit tests)
- Playwright (E2E)
- ESLint + Prettier (code quality)

**Success Metrics:**
- Bundle <50KB gzipped
- Jitter <2ms
- 0 long tasks during playback
- ≥80% test coverage
- 0 TypeScript/ESLint errors
