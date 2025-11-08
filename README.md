# TR-808 Drum Machine

Web-based emulation of the classic Roland TR-808 drum machine using Web Audio API.

## Features

- 5 synthesized drum voices (BD, SD, CH, OH, CP)
- 16-step sequencer
- Adjustable BPM (60-240)
- Pattern save/load via localStorage
- Visual step highlighting

## Tech Stack

- **Frontend**: Vanilla TypeScript
- **Audio**: Web Audio API
- **Build**: Vite 6
- **Testing**: Vitest 4 + Playwright
- **Linting**: ESLint + Prettier

## Setup

```bash
# Install dependencies
npm install

# Start dev server
npm run dev

# Build for production
npm run build

# Run tests
npm run test        # Unit tests
npm run e2e         # E2E tests
npm run lint        # Linting
```

## Project Structure

```
src/
├── audio/          # Web Audio API logic
│   ├── voices/     # Drum voice synthesizers
│   └── scheduler.ts
├── ui/             # UI components
├── state/          # State management
└── storage/        # localStorage persistence

tests/
├── unit/           # Vitest unit tests
└── e2e/            # Playwright E2E tests
```

## Browser Support

- Chrome 90+
- Firefox 88+
- Safari 14+

## License

MIT
