# 66 — UI: Editor Layout, 12‑Column Grid, and Resizing (JUCE)

Read  and native coding rules first.

Context
- Implement the modern, studio‑grade layout per task 006: four zones — Header/Transport, Kit & Patterns, Voice Strips (12), Master & FX. Use a 12‑column grid, 8 px spacing rhythm, and responsive breakpoints for 1100×720, 1440×900, 1680×980.

Dependencies
- Ticket 63 (LookAndFeel/theme/tokens)
- Ticket 64 (vector components) and 65 (effects/animations)
- Tickets 67–70 (controls/voice/master/transport) plug into this layout

Scope
- Editor minimum size: 1100×720; preferred: 1440×900; max tested: 1680×980
- Grid: 12 columns, 8 px gutters; column width computed per breakpoints
- Zones and exact metrics (at 1440×900 reference):
  - Header/Transport: height 64 px; spans 12 cols
  - Kit & Patterns bar: height 120 px; spans 12 cols
  - Voice Strips area: height 572 px (fills below); spans 9 cols (left)
  - Master & FX panel (right rail): width = 3 cols; spans from below Header to bottom
- Elevation: Header (L1), Kit (L1), Voice strips background (L0), Master panel (L2)
- Spacing: 8 px outer margins; 8 px between strips; 16 px between major zones

Acceptance Criteria
- Editor resizes smoothly across breakpoints with no overlap; all zones remain aligned to 12‑col grid and 8 px rhythm
- At 1100×720 and 1680×980 sizes, layout switches column width and heights but preserves proportions
- Master & FX occupies 3 columns on the right at all sizes; voice strips 9 columns; Header and Kit span full width
- Repaint regions limited to changed components; no excessive overdraw during resize

Implementation Steps
1) Files (native VST3 GUI):
   - 
   - 
   - 
   - 
   - 
2) Grid system
   - Use  with 12 columns; define a helper 
   - Compute column width 
   - Provide helper to get rect for col span: 
3) Breakpoints
   - If width < 1280 → treat as 1100×720 profile; else if < 1600 → 1440×900; else 1680×980 scaling
   - Heights per profile:
     - 1100×720: Header 56, Kit 104, Master width=3 cols, Voice area = remaining
     - 1440×900: Header 64, Kit 120
     - 1680×980: Header 72, Kit 128
4) Layout pass
   - Header: x=margin, y=margin, w=12 cols, h=HeaderH
   - Kit: y += HeaderH + 16; w=12 cols, h=KitH
   - Master: x = margin + (colW+gutter)*9, y = header.top, w = 3 cols, h = totalHeight - 2*margin
   - VoiceArea: x = margin, y = Kit.bottom + 16, w = 9 cols, h = bottom - y - margin
5) Elevation/FX
   - Apply LAF tokens for panel backgrounds; Add  to Master panel (L2) and subtle shadow for Header/Kit (L1)
6) Performance
   - Enable  for Header/Kit backgrounds; keep dynamic child controls unbuffered
7) Tests/QA
   - Manual resize at 1100×720, 1440×900, 1680×980; verify no overlap/gaps; DPI 1×/2×

Affected Files
-  (new)

Risks & Mitigations
- Gaps due to rounding: favor rounding col widths down and assign remainder to the last column span to avoid overflow
- Performance during resize: throttle animations; avoid heavy effects on frequently repainted panels

