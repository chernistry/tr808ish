# CR-717 Drum Synth VST3 — UI/UX Redesign Specification (2025)

## 1) Executive Summary

* Dark, studio-grade UI with TR-inspired accents. High contrast, no skeuomorphic chrome. WCAG 2.2 AA contrast and 24×24 px minimum interactive targets.
* Information architecture: four zones — **Header/Transport**, **Kit & Patterns**, **Voice Strips** (12 voices), **Master & FX**. Mirrors leading drum tools and hardware (cell grid + per-voice editing), reducing cognitive load.
* Grid: 12-column, 8 px base spacing, component-responsive layout. Scales 1100×720 → 1440×900 → 1680×980. Material’s 8 dp rhythm ensures visual consistency.
* Typography: Inter for UI, JetBrains Mono for values. Sizes tuned for 1×/2×. Inter is screen-optimized and open-source.
* Sequencer: optional module v1, designed in. 16 steps visible by default, expandable to 32. Keyboardable grid semantics per WAI-ARIA APG.
* Feedback: meters at 30–60 Hz, step highlight at 60 fps. Micro-interactions 150–250 ms with eased curves (Material/Apple).
* Accessibility: full keyboard traversal, visible focus, color-blind safe accents, ARIA roles for grids and controls.
* Performance/JUCE: strict dirty-region repainting, `setBufferedToImage()` for static widgets, HiDPI aware, avoid per-repaint allocations.
* Drag-&-drop: OS-level external DnD for MIDI (temp file) with fallback Save. Preset selection must update pattern UI immediately (state graph). Pattern behaviors align with 808/Drum Rack conventions.
* Defaults match industry patterns from Battery, Geist, XO, Drum Rack; measurements here are implementable and consistent with modern DAW plugin expectations.

---

## 2) Design System

### 2.1 Colors (dark theme, hex at 1×)

| Token        |       Hex | Use                                |
| ------------ | --------: | ---------------------------------- |
| `--bg-0`     | `#121214` | App background (AA with text)      |
| `--bg-1`     | `#181a1f` | Panels                             |
| `--bg-2`     | `#22252b` | Controls surface                   |
| `--elev-1`   | `#2a2f36` | Raised cards/strips                |
| `--text-0`   | `#f2f4f8` | Primary text (≥4.5:1 on `--bg-1`)  |
| `--text-1`   | `#b8c0cc` | Secondary text                     |
| `--text-2`   | `#7c8899` | Muted                              |
| `--border-0` | `#343a40` | Dividers (1 px)                    |
| `--accent-0` | `#ff6b35` | Primary accent (TR-orange)         |
| `--accent-1` | `#42d9ff` | Secondary accent (HH/visual focus) |
| `--ok`       | `#2ed573` | Success                            |
| `--warn`     | `#ffd166` | Warning                            |
| `--err`      | `#ff4757` | Error                              |

* Contrast: keep body text ≥4.5:1, large text ≥3:1.
* Color-blind safety: pair accent with shape/pattern, not color alone. Follow WCAG non-color cues.

### 2.2 Typography

* **Primary UI**: Inter (400/500/600/700). **Values/mono**: JetBrains Mono (500). Inter is designed for UIs.
* **Scale** (base 14 px, ratio ≈1.25): 11, 12, **14**, 16, 20, 24, 30.
* **Usage**

  * H2 section titles 20 px/1.3, semibold
  * H3 group titles 16 px/1.3, semibold
  * Labels 12–14 px/1.25, medium
  * Numeric readouts 12–14 px/1.1, mono

### 2.3 Spacing & Elevation

* Base spacing: **8 px grid**; micro-spacing 4 px allowed. Material’s 8 dp system is industry standard.
* Elevation levels (drop-shadow on dark):

  * L0 flat, no shadow
  * L1 panel: 0 px y/6 px blur/12%
  * L2 card: 2 px y/12 px blur/16%
  * L3 modal: 6 px y/24 px blur/20%
    Aligns with Material elevation guidance.

### 2.4 Motion

* Durations: 100 ms (tap/press), **150–250 ms** (toggles, tabs), 250–300 ms (panel transitions). Apple/Material ranges.
* Easing: cubic-bezier(0.2, 0, 0, 1) for in-out; spring-like only for subtle meters.

---

## 3) Component Library (1× exact sizes)

### 3.1 Rotary Knobs

* **Sizes**: S 32 px, **M 48 px** (default), L 64 px.
* **Arc**: 300° sweep (−150° → +150°).
* **Track**: base 2 px; value 4 px.
* **Pointer**: 1 px × 40% radius line; center dot 2 px.
* **Label**: 12–14 px below; value inline on focus (tooltip if space tight).
* **States**:

  * Hover: +6% brightness; ring glow `rgba(255,107,53,0.25)`
  * Active/drag: knob scale 1.03×, ring 2 px stronger
  * Disabled: 40% opacity, no glow
* **Drag**: 100 px travel = full range; Shift = fine (10× slower).
* **Detents**: snap at center/zero; tick marks every 25%.
* Rationale: wide 300° sweep improves precision; consistent with common plugin practice. (General UX and APG patterns for clear affordance.)

### 3.2 Vertical Faders

* **Tracks**: Narrow 24×140 px; **Standard 28×180 px**; Wide 32×220 px.
* Thumb 22×10 px, 4 px radius.
* Graduations every 6 dB; bold at 0 dB.
* Center detent for pan/balance.
* Keyboard: Up/Down small step, Shift for fine.

### 3.3 Step Sequencer Pads

* **Pad**: **36×36 px**, gap 4 px; corner radius 4 px.
* **States**:

  * Off `#2a2f36`
  * On `#3c434d`
  * Active (current step) `#ff6b35` + outer glow `rgba(255,107,53,.35)`
  * Trigger flash: 120 ms opacity pulse to 1.0
* ARIA grid roles and keyboard traversal (arrows, Space/Enter), meeting WCAG target size (≥24×24 px).

### 3.4 Buttons

* **Sizes**: S 60×24, **M 88×32**, L 112×40; radius 6 px.
* **Styles**: Primary (accent fill), Secondary (stroke), Tertiary (ghost).
* **States**: hover +5% lightness; pressed −5% scale 0.98; disabled 40% opacity.

### 3.5 Tabs / Segments

* Height 32 px; indicator 2 px accent underline with 150 ms slide.

### 3.6 Labels & Text

* Minimum contrast AA; do not rely on color alone for status.

### 3.7 Meters

* Peak bars 4 px wide, hold 500 ms; GR meter for compressor placed near master; 30–60 Hz repaint budget.

---

## 4) Layout Specification

### 4.1 Window & Grid

* **Default**: 1280×800 px. **Min**: 1100×720. **Max**: 1920×1080 (free).
* **Grid**: **12 columns**, 16 px gutters, 24 px outer margin. 8 px baseline inside components.
* **Sections** (height ratios @1280×800):

  * Header/Transport (64 px)
  * Kit & Patterns (Preset browser + bank buttons) (120 px)
  * **Voice Strips** (12) with per-voice knobs (flex area)
  * Master & FX (bottom dock 120 px)
* **Sequencer module** (optional v1): expandable panel under Voice Strips; shows 16 or 32 steps; scroll if <1100 px width.
* Patterns reflect 16-step tradition (808 and many drum plugins).

### 4.2 Transport & Banks

* Left: Play/Stop (Primary), BPM, Swing; Right: Preset dropdown, Bank A–H, Copy/Paste/Clear.
* Battery’s cell paradigm (grid of sound slots) and Drum Rack’s pad mapping inform kit layout.

---

## 5) Interaction Patterns

* **Keyboard**:

  * Global: Space Play/Stop, ↑↓ BPM, `[` `]` swing, 1–8 bank, Cmd/Ctrl+S save preset.
  * Grid (sequencer): arrows move, Enter/Space toggle, Shift+arrows select range. ARIA grid pattern.
* **Drag & Drop**:

  * External MIDI: drag handle creates temp `.mid`, uses OS-level DnD; fallback **Save MIDI…** button. (Avoid internal-only DnD that many DAWs ignore.)
* **Tooltips**: 500 ms delay, 240 px max width, monospace values at 12–14 px.
* **Context menus**: right-click on a voice strip → copy/paste params, randomize, multi-out route.

---

## 6) Accessibility Guidelines

* WCAG 2.2 AA contrast, min target 24×24 px; keyboard access across all controls; visible focus (2 px ring).
* ARIA roles for grid (sequencer) and menu/button patterns per APG.
* Non-color cues (icons, labels) for on/off/armed states.

---

## 7) Performance & Technical (JUCE)

* **HiDPI/Scaling**: enable HiDPI awareness; supply vector/hi-res assets; use JUCE’s HiDPI support and set correct scale factor.
* **Repaints**: paint only dirty regions; coalesce timers; cache static components with `Component::setBufferedToImage(true)`; avoid allocating in `paint()`.
* **Animation budget**: ≤16 ms per frame at 60 Hz; throttle meters to 30–60 Hz.
* **Assets**: vector where possible; atlases for icons; avoid large raster shadows.

---

## 8) Component-by-Component Specs (details)

### 8.1 Voice Strip (per instrument)

* Width: 1 grid column (min 88 px), expands with window.
* Contents: Label (12 px), Mute/Solo (S buttons), Level fader (28×180), 2–4 knobs (M 48 px) for voice params, Send A/B (S knobs 32 px), Pan (S knob).
* Meters: vertical peak with −12/−6/0 dB ticks; clip indicator with 500 ms hold.

### 8.2 Master & FX

* Compressor, Limiter, Reverb, Delay small panels with consistent knob spacing. GR meter adjacent to comp.
* Bypass toggles (secondary buttons), preset dropdown for FX.

### 8.3 Kit & Preset Browser

* Left column list with tags/genres; right grid of presets (cards 220×64). Sorting and search bar. (Battery/Geist present similar browsers.)

### 8.4 Sequencer (module)

* 16 steps × voices. Pad size 36×36 (4 px gap). Header shows steps 1–16 with group markers every 4.
* Variation A–H row, Copy/Paste/Clear; humanize/shuffle controls right. 16-step aligns with 808 manual patterns; DAWs often show 16/32.

---

## 9) Reference Plugin Analysis (what to emulate)

| Plugin                      | Notable Patterns                                                                | Visual Style                                | Notes                                                                       |
| --------------------------- | ------------------------------------------------------------------------------- | ------------------------------------------- | --------------------------------------------------------------------------- |
| **NI Battery 4**            | Cell matrix (usually 4×12), per-cell color, browser on left, mixer/effects tabs | Dark neutral UI w/ functional color accents | The grid paradigm scales well for many drums; good preset browser taxonomy. |
| **FXpansion Geist 2**       | Multi-engine sequencer with pattern, scenes, pads; deep mixer                   | Dark, high-density, tabbed sections         | Shows how to expose deep features without clutter via tabs and clear zones. |
| **XLN XO**                  | Sample space browser (scatter plot), simple sequencer                           | Modern flat, cyan/blue accents              | Great discoverability; separates browsing from sequencing.                  |
| **Ableton Drum Rack**       | 16 pad grid, per-pad chains/racks, minimal chrome                               | Low-contrast flat with clear active states  | Efficient pad control and macro mapping; a good minimal baseline.           |
| **Arturia DrumBrute (hw)**  | Hardware 16-step row with per-voice encoders                                    | Modern skeuo for hardware, but readable     | Validates 16-step grouping and per-row parameters.                          |
| **Splice Beat Maker (web)** | Card-like pad rows, online workflows                                            | Flat, high contrast                         | Confirms web-inspired sequencing patterns used by producers.                |

*(Exact pixel sizes are proprietary or scale with window; we adopt implementable sizes above.)*

---

## 10) Accessibility & Usability Details

* **Focus management**: 2 px accent ring; Tab order: header → kits → voice strips → sequencer → master.
* **Keyboard help overlay**: `?` toggles overlay.
* **Color blindness**: pair color with icon/shape; avoid red/green as sole channel. WCAG non-color reliance.

---

## 11) Implementation Notes (JUCE)

* **Fonts**: bundle Inter + JetBrains Mono (ttf/otf).
* **Scalable units**: compute from `UiScale` (1.0 at 1280×800). Multiply sizes by scale; keep 1 px borders device-independent.
* **HiDPI**: on macOS, use NSHighResolutionCapable; on Windows, per-monitor DPI aware. JUCE handles scale; ensure vector icons.
* **Caching**: `setBufferedToImage(true)` for preset cards, logos, static panels.
* **Timers**: one UI timer dispatch for meters; coalesce repaints; avoid `repaint()` storms.
* **Testing**: verify clarity at 1× and 2×; test with Live/Studio One/Reaper.

---

## 12) Mockup Descriptions (for Figma)

* **Header**: left transport (Play/Stop 100×40, BPM knob M, Swing knob M), right preset dropdown 240 px, bank A–H segmented (8× 60×28).
* **Voice strip**: top label (12 px), Mute/Solo S buttons, fader, 3–4 knobs, meters.
* **Sequencer**: 16 columns, per-voice row; step LEDs; row label left.
* **Master**: comp/limiter (2×2 knobs + GR), reverb/delay (3–4 knobs + send meters), output meter.

---

## 13) Comparison Table (spec vs references)

| Area           | CR-717 Spec                            | References                                         |
| -------------- | -------------------------------------- | -------------------------------------------------- |
| Color/contrast | Dark neutral with AA contrast          | WCAG 2.2 AA, common across Battery/Geist UIs.      |
| Grid/spacing   | 12-col, 8 px base                      | Material 8 dp; DAWs favor dense but regular grids. |
| Sequencer      | 16/32 steps, grid keyboardable         | TR-808 16-step, Drum Rack pad grid.                |
| Typography     | Inter + Mono                           | Inter designed for screen UIs.                     |
| Motion         | 150–250 ms                             | Apple/Material motion guidance.                    |
| Accessibility  | WCAG 2.2 AA, ARIA grid                 | W3C WCAG/APG.                                      |
| JUCE perf      | Dirty repaints, buffered images, HiDPI | JUCE docs.                                         |

---

## 14) Recommendations

**P1 (ship in v1)**

* Implement design tokens, grid, and core components (knob/fader/button/pad).
* Header/Transport + Kit browser + Voice strips + Master dynamics.
* External MIDI drag-out with Save fallback.
* Keyboard navigation and AA contrast checks.

**P2**

* Sequencer module UI + pattern ops (copy/paste/randomize).
* FX preset system and per-voice sends.

**P3**

* Theme switch (light), per-voice color themes, advanced meters.

---

## 15) Appendices — Source Notes

* **Battery 4** manual confirms cell matrix & browser layout (grid-centric).
* **Geist 2** manual details sequencer/mixer/tabs organization.
* **XO** manual shows modern flat design and sample browser flow.
* **Drum Rack** section in Live manual documents pad grid paradigm.
* **TR-808** service/owner docs validate 16-step convention.
* **WCAG 2.2** for contrast/target size; **WAI-ARIA APG** for grid keyboarding.
* **Motion & spacing** from Apple/Material.
* **JUCE** docs for HiDPI and caching.

---

### Implementation checklist (JUCE)

* [ ] Token constants (colors, radii, spacing, type).
* [ ] `LookAndFeel` implementing knob/fader/button with sizes above.
* [ ] 12-col grid helper for layout.
* [ ] Focus ring painter; keyboard map; ARIA names.
* [ ] External MIDI drag with temp file.
* [ ] Meters timer @ 33 Hz; step highlight @ 60 Hz.
* [ ] HiDPI verification at 1×/2×; `setBufferedToImage(true)` on static composites.

If you want, I can output a Figma-ready token JSON and a JUCE `LookAndFeel` skeleton next.
