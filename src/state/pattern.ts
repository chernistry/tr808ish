import type { Pattern } from './types';

export function createEmptyPattern(): Pattern {
  return {
    name: 'Untitled',
    bpm: 120,
    steps: {
      BD: Array(16).fill(false),
      SD: Array(16).fill(false),
      LT: Array(16).fill(false),
      MT: Array(16).fill(false),
      HT: Array(16).fill(false),
      RS: Array(16).fill(false),
      CP: Array(16).fill(false),
      CH: Array(16).fill(false),
      OH: Array(16).fill(false),
      CY: Array(16).fill(false),
      RD: Array(16).fill(false),
      CB: Array(16).fill(false),
    },
  };
}

export function createDefaultPattern(): Pattern {
  return {
    name: 'Atmospheric UK Garage',
    bpm: 130,
    steps: {
      BD: [true, false, false, false, true, false, false, false, true, false, false, false, true, false, false, false],
      SD: [false, false, false, false, true, false, false, false, false, false, true, false, false, false, true, false],
      LT: Array(16).fill(false),
      MT: Array(16).fill(false),
      HT: Array(16).fill(false),
      RS: Array(16).fill(false),
      CP: [false, false, false, false, true, false, false, false, false, false, false, false, false, false, true, false],
      CH: [true, false, true, true, true, false, true, true, true, false, true, true, true, false, true, false],
      OH: [false, false, false, false, false, false, true, false, false, false, false, false, false, false, false, true],
      CY: Array(16).fill(false),
      RD: Array(16).fill(false),
      CB: Array(16).fill(false),
    },
  };
}

export function toggleStep(
  pattern: Pattern,
  instrument: keyof Pattern['steps'],
  step: number
): Pattern {
  return {
    ...pattern,
    steps: {
      ...pattern.steps,
      [instrument]: pattern.steps[instrument].map((active, i) =>
        i === step ? !active : active
      ),
    },
  };
}
