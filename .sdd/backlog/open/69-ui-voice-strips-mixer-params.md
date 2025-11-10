# 69 — UI: Voice Strips (12), Mixer Controls, and Per‑Voice Params (JUCE)

Read Ticket 66 (layout) and 67 (controls) first.

Context
- Implement 12 voice strips laid out across 9 columns of the grid. Each strip: label, color accent, Mute/Solo buttons, Level fader, Pan knob, 2–3 key params (e.g., Decay/Tone/Snappy), and send knobs if enabled. Sizes/spacing per 006; elevation L0 with subtle inner separators.

Dependencies
- Ticket 61 (APVTS mapping): per‑voice parameters and attachments
- Ticket 59 (FX sends UI) if sends are exposed in this pass

Scope
- Strip width at 1440×900: ~ (9 cols − gutters)/12 ≈ 84 px each; min 72 px at 1100 width
- Components per strip (top→bottom):
  - Label (12 px Inter), accent bar 2 px at top
  - Mute/Solo buttons 20×20 px side‑by‑side, 8 px gap below label
  - Pan knob (S 32 px), below: Decay knob (S 32 px), Tone/Snappy as needed
  - Level fader (Std 28×180 px) occupying remaining height; 0 dB mark bold; value readout 12 px mono below
- Hover states: buttons/knobs tint; fader thumb glow
- Context menu (right‑click): reset params to default, copy/paste strip settings

Acceptance Criteria
- All 12 strips render consistently; hover/active states match theme
- APVTS attachments for each control update parameters and reflect in DSP
- Resizing preserves minimum strip width and reflows labels without overlap
- Mute/Solo logic updates UI state instantly (audio behavior handled elsewhere)

Implementation Steps
1) Files:
   - `native/vst3/CR717/Source/ui/components/voices/VoiceStrip.h/.cpp`
   - `native/vst3/CR717/Source/ui/components/voices/StripHeader.h/.cpp`
   - `native/vst3/CR717/Source/ui/components/voices/StripBody.h/.cpp`
2) Layout within strip
   - Fixed header height 40 px; buttons row 24 px; knob row ~80 px; fader fills rest
   - Use `juce::FlexBox` vertical with 8 px gaps; internal padding 8 px
3) Rendering
   - Background `--bg-2`; inner separators with `--border-0` (1 px)
   - Accent bar uses per‑voice color; label mono baseline alignment
4) Attachments
   - Provide helpers to bind strip controls to APVTS with voice‑indexed parameter IDs
5) QA
   - Manual: Mute/Solo/Level/Pan/Decay/Tone; DPI 1×/2×; resize tests; keyboard focus order

Affected Files
- `native/vst3/CR717/Source/ui/components/voices/*` (new)
- `native/vst3/CR717/Source/ui/editor/sections/VoiceArea.*` (integrate 12 strips)

Risks & Mitigations
- Narrow width crowding: collapse labels to abbreviations at min width; tooltips on hover
- Too many paints: buffer static header area of each strip

