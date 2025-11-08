# TR-808 Synth

![TR-808 Synth Screenshot](./assets/screenshot.png)

A simple drum sequencer created with AI in about half an hour, inspired by the classic TR-808 but not pretending to replicate the original. Use it freely in your projects!


This project demonstrates the SDD (Spec-Driven Development) approach - a methodology for rapid development using AI agents with structured prompts and templates. For more details on SDD, check out [SDDRush](https://github.com/chernistry/SDDRush).

All prompts used in this project are available in both locations - the SDDRush repo and the sdd directory within this project.

## Features

- **12 synthesized drum voices** (BD, SD, LT, MT, HT, RS, CP, CH, OH, CY, RD, CB) with Web Audio API synthesis
- **16-step sequencer grid** for each voice with visual step highlighting
- **Professional VST-style UI** with dark theme and gradient panels
- **Individual voice mixer** with level controls for each instrument
- **Parameter controls** for per-voice customization (level, tone, decay, tuning, etc.)
- **Default atmospheric UK garage pattern** pre-loaded
- **Adjustable tempo** (60-240 BPM) with swing control
- **Pattern saving/loading** via browser storage
- **Real-time step highlighting** during playback
- **Theme toggle** (light/dark mode)
- **Keyboard shortcuts** support for efficient operation
- **Responsive design** optimized for desktop and tablet

## How to Use

```bash
# Install dependencies
npm install

# Start development server
npm run dev

# Build for production
npm run build
```

### Keyboard Shortcuts:
- Space: Play/Stop
- +/-: Increase/Decrease BPM by 5
- T: Toggle theme (light/dark)
- D: Load default pattern
- ?: Show help
- Escape: Close help
- 1-5: Select voice
- Enter: Toggle step
- Arrow keys: Navigate steps

### UI Controls:
- **Transport**: Play, Stop, BPM slider
- **Mixer**: Individual level controls for each voice
- **Parameters**: Click instrument label to access voice-specific parameters
- **Pattern Grid**: Click steps to toggle on/off
- **Swing**: Adjust swing percentage for groove feel

## Tech Stack

- **Frontend**: TypeScript
- **Audio**: Web Audio API with look-ahead scheduler
- **Build**: Vite 6
- **Testing**: Vitest 4 + Playwright
- **Linting**: ESLint + Prettier

## Architecture

- **State Management**: Centralized store with pub/sub pattern
- **Audio Engine**: Look-ahead scheduler with precise timing (<2ms jitter)
- **UI Components**: Modular, reusable components (sequencer grid, mixer, controls)
- **Design System**: CSS variables for consistent theming and styling

## Performance

- Bundle size: <10 KB (3.13 KB gzipped)
- Timing accuracy: <2ms jitter at 120 BPM
- Memory usage: <100 MB
- No long tasks during playback

## License

MIT
