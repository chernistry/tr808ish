# 70 — UI: Master & FX Panel (Meters, Returns), Presets Bar (JUCE)

Read Ticket 66 (layout) and 63 (theme) first.

Context
- Implement right‑rail Master & FX panel (3 cols width) with peak/RMS meters, output gain, soft clip indicator (UI), and FX returns (Reverb/Delay). Add Header/Transport integration: presets combobox, Save, Randomize, External MIDI DnD zone.

Dependencies
- Ticket 49 (UI mixer & sends) — ensures send linkage
- Ticket 57/58 (limiter/clipper) — for indicator states (UI only here)
- Ticket 61 (APVTS mapping) for parameters/state

Scope — Master & FX panel
- Meters: stereo peak with hold + RMS bars; 120 px height each; update 30–60 Hz timer
- Output gain: RotaryKnob M 48 px with mono readout; clip indicator pill (red) lights on over‑ceiling events
- FX returns: Reverb (Size/Decay/Mix), Delay (Time/Feedback/Mix/Sync toggle); knobs S/M
- Panel padding 12 px; 8 px gaps; elevation L2 with DropShadow per tokens

Scope — Header/Transport add‑ons
- Presets: combobox width 220 px; Save button 80×28 px; Randomize 28×28 icon button (dice)
- External MIDI DnD zone: 220×48 px rectangle with dashed border in Kit bar; on drop, emit temp MIDI file path (integration later)

Acceptance Criteria
- Meters animate smoothly without frame drops; peak hold decay visible; clip indicator behaves per UI spec
- FX controls update APVTS and reflect immediately in UI; presets combobox switches state
- DnD zone shows hover state and accepts OS drags (UI response only)

Implementation Steps
1) Files:
   - `native/vst3/CR717/Source/ui/components/master/MasterPanel.h/.cpp`
   - `native/vst3/CR717/Source/ui/components/master/Meters.h/.cpp`
   - `native/vst3/CR717/Source/ui/components/master/PresetsBar.h/.cpp`
2) Meters
   - Implement peak+RMS drawing with cached background grid; timer 16–33 ms
   - Peak hold as small cap with decay over ~1 s
3) FX returns
   - Group knobs with labels; tooltips; optional segmented switch for Delay sync
4) Presets bar
   - Styled combobox and buttons per LookAndFeelEx; focus/hover states
5) Performance
   - Buffer background; repaint only changing meter regions
6) QA
   - Manual: meters performance, knob updates, combobox navigation, DnD hover

Affected Files
- `native/vst3/CR717/Source/ui/components/master/*` (new)
- `native/vst3/CR717/Source/ui/editor/sections/Header.*`, `KitBar.*` (integration)

Risks & Mitigations
- Meter overdraw: clip repaint to narrow vertical strips; avoid blur in frequent paints
- DnD platform differences: keep scope to visual feedback; integration in later ticket

