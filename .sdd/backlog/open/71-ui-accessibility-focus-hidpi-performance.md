# 71 — UI: Accessibility, Focus Management, HiDPI, and Performance (JUCE)

Read `.sdd/best_practices_vst.md` and tickets 63–70 first.

Context
- Ensure the redesigned UI meets accessibility, focus, and performance standards: keyboard traversal, visible focus, color‑blind safe accents, reduced motion preference, HiDPI crispness, and repaint discipline.

Dependencies
- Ticket 63 (theme tokens with focus/hover), 65 (animations), and all UI component tickets

Scope
- Keyboard traversal: Tab/Shift+Tab orders across Header → Kit → Sequencer → Voice Strips → Master; Arrow keys inside grids/knobs
- Focus ring: 2 px outline with `accent-1`, 2 px outer glow at 30% alpha; no reliance on color alone
- Reduced motion: theme flag disables scale/alpha animations; keeps hover tint only
- HiDPI: verify 1×/2×; avoid cached bitmaps where scaling causes blur; use vectors
- Repaint discipline: strict dirty regions; buffer static composites; measure repaint counts

Acceptance Criteria
- Full UI operable with keyboard; focus visible on all interactive elements
- Reduced motion toggled in theme; animations disabled accordingly
- No blurry assets at 2×; icons and controls remain crisp
- Resize and meters do not produce long tasks; profiling shows limited repaint rects

Implementation Steps
1) Focus management
   - Implement custom `FocusOrder` for sections and controls; ensure predictable tab order
   - Add focus painting in components using tokens
2) Reduced motion
   - Add `Theme::reducedMotion` flag; condition animators on this
3) HiDPI & assets
   - Enforce vector rendering for icons; drop any raster fallbacks
4) Performance tooling
   - Debug overlay (dev builds only) showing repaint regions and FPS meter for animations
   - Guidelines in README for profiling in plugin hosts
5) QA checklist
   - Keyboard path walkthrough; DPI checks; performance capture during playback

Affected Files
- `native/vst3/CR717/Source/ui/theme/Theme.*` (reduced motion)
- `native/vst3/CR717/Source/ui/components/*` (focus painting)
- `native/vst3/CR717/Source/ui/editor/*` (focus order)

Risks & Mitigations
- Platform focus quirks: normalize with explicit focus calls on mouse down when appropriate
- Over‑instrumentation in release: gate debug overlays behind JUCE_DEBUG

