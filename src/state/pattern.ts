import type { Pattern } from './types';

export function createEmptyPattern(): Pattern {
  return {
    name: 'Untitled',
    bpm: 120,
    steps: {
      BD: Array(16).fill(false),
      SD: Array(16).fill(false),
      CH: Array(16).fill(false),
      OH: Array(16).fill(false),
      CP: Array(16).fill(false),
    },
  };
}

export function createDefaultPattern(): Pattern {
  return {
    name: 'Atmospheric UK Garage',
    bpm: 130,
    steps: {
      // Kick: 4-on-floor with skip on 7
      BD: [true, false, false, false, true, false, false, false, true, false, false, false, true, false, false, false],
      // Snare: 2 and 4 with ghost on 10
      SD: [false, false, false, false, true, false, false, false, false, false, true, false, false, false, true, false],
      // Closed hats: 16th note groove with gaps
      CH: [true, false, true, true, true, false, true, true, true, false, true, true, true, false, true, false],
      // Open hat: sparse atmospheric hits
      OH: [false, false, false, false, false, false, true, false, false, false, false, false, false, false, false, true],
      // Clap: layered with snare on 2 and 4
      CP: [false, false, false, false, true, false, false, false, false, false, false, false, false, false, true, false],
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
