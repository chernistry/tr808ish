# 67 — UI: Controls Library — Vector Knobs, Faders, Buttons (JUCE)

Read Ticket 63 (LookAndFeel/theme) and 64 (vector graphics) first.

Context
- Implement the core interactive controls per 006 spec: rotary knobs (S/M/L), vertical faders (narrow/standard/wide), toggle buttons, segmented tabs. Exact sizes, arcs, detents, hover/active states, drag behavior, and labels with mono readouts.

Dependencies
- Ticket 63 (tokens/fonts), 64 (vector components), 65 (effects/animations)
- Ticket 61 (APVTS mapping) for parameter attachments

Scope
- RotaryKnob: S 32 px, M 48 px (default), L 64 px; 300° sweep (−150°→+150°)
  - Track: base 2 px, value 4 px; pointer 1 px × 40% R; center dot 2 px
  - States: hover +6% brightness with glow (accent-0 @ 25%); active scale 1.03×
  - Drag: 100 px travel = full range; Shift = fine (×0.1); detents at 0%/50%/100%
  - Label: 12–14 px below; value inline on focus; JetBrains Mono for numbers
- VerticalFader: tracks 24×140 (N), 28×180 (Std), 32×220 (W); thumb 22×10 px, r=4 px
  - Graduations every 6 dB, bold at 0 dB; center detent for pan
  - Keyboard: Up/Down step; Shift for fine; hover tint
- Buttons: primary/secondary text buttons; toggle buttons with on/off tint; segmented control

Acceptance Criteria
- Visuals match sizes and sweeps; hover/active/disabled states render per tokens
- Drag and keyboard interactions feel smooth; fine mode works reliably; detents snap
- APVTS attachments update control and reflect parameter changes without zippering
- HiDPI crispness verified at 1×/2×; no bitmap blur

Implementation Steps
1) Files:
   - `native/vst3/CR717/Source/ui/components/controls/RotaryKnob.h/.cpp`
   - `native/vst3/CR717/Source/ui/components/controls/VerticalFader.h/.cpp`
   - `native/vst3/CR717/Source/ui/components/controls/Buttons.h/.cpp`
2) Rendering
   - Use `juce::Path` for arcs/track; gradients from tokens; `DropShadow` subtle
   - Cache static paths per size; recompute only on size change
3) Interaction
   - Mouse drag with vertical delta; Shift for fine; constrain to 300° sweep
   - Keyboard focus: add focus outline per tokens; key handlers for increment
4) Mapping
   - Provide `std::unique_ptr<SliderAttachment>`/`ButtonAttachment` creation helpers
5) Tests/QA
   - Manual: check sizes, detents, fine mode; verify attachment sync; DPI 1×/2×

Affected Files
- `native/vst3/CR717/Source/ui/components/controls/*` (new)
- `native/vst3/CR717/Source/ui/theme/*` (token usage)

Risks & Mitigations
- Jank when dragging: use no allocations; precompute geometry; repaint minimal bounds
- Detent oscillation: apply small hysteresis around detent thresholds

