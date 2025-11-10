# 68 — UI: Step Sequencer Pads, Transport, and Timeline (JUCE)

Read Ticket 66 (layout/grid) and 67 (controls) first.

Context
- Implement the sequencer UI per 006: 16 visible steps (expandable to 32), pad size 36×36 px with 4 px gaps, 4 px radius, playhead highlight 60 fps, meters 30–60 Hz, keyboardable grid semantics, and top transport (Play/Stop, Tempo, Swing).

Dependencies
- Ticket 66 (grid placement under Kit & Patterns/Voice area)
- Ticket 61 (APVTS mapping for tempo/swing and per-step state)

Scope
- Pads: 36×36 px, gap 4 px; states Off `#2a2f36`, On `#3c434b`, Accent `accent-0` (bright)
- Rows: at least BD/SD/CH/OH/CP rows; 16 columns; 32 via pager (<< < 1/2 > >>)
- Playhead: vertical bar 2 px with glow; move at 60 fps; highlight current step cell
- Transport: Play/Stop buttons 28×28 px; Tempo numeric (Inter 14 px), Swing knob (M 48 px)
- Interaction: click to toggle; Alt-click = accent; drag across to paint; Shift = erase
- Keyboard: Arrow keys move focus; Space toggles; Enter toggles; 1–8 selects page

Acceptance Criteria
- Pads render crisp; toggling is instant; painting across cells works with minimal repaint
- Playhead animation smooth at 60 fps on reference machine; meters at 33 Hz
- Keyboard navigation fully works; focus ring visible and accessible
- Tempo and Swing controls update via APVTS and reflect in engine (out of scope DSP here)

Implementation Steps
1) Files:
   - `native/vst3/CR717/Source/ui/components/sequencer/StepPad.h/.cpp`
   - `native/vst3/CR717/Source/ui/components/sequencer/PadGrid.h/.cpp`
   - `native/vst3/CR717/Source/ui/components/sequencer/TransportBar.h/.cpp`
2) Rendering
   - Pads via `juce::Path` rounded rect; tint per state; hover/active states per tokens
   - Playhead drawn in overlay component with timer at 16 ms (≈60 fps)
3) Interaction
   - Mouse drag to paint/erase across cells; modifiers per spec; double‑click clears row
   - Keyboard focus manager cycles cells; focus ring per tokens
4) Performance
   - Buffer static grid background; repaint only changed pads and playhead column
5) Tests/QA
   - Manual: toggle/paint; 16→32 pager; transport buttons; DPI 1×/2×; performance pass

Affected Files
- `native/vst3/CR717/Source/ui/components/sequencer/*` (new)
- `native/vst3/CR717/Source/ui/editor/sections/KitBar.*` (integration)

Risks & Mitigations
- Repaint cost at 60 fps: restrict to small region; avoid blurs during playhead motion
- Keyboard trap: ensure escape routes and consistent traversal order

