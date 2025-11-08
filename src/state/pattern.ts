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
    bpm: 138, // Authentic UK garage tempo
    steps: {
      // BD: Classic 2-step pattern with syncopation
      BD: [true, false, false, false, false, true, false, false, true, false, false, false, false, true, false, false],
      // SD: Classic backbeat snare pattern
      SD: [false, false, false, false, true, false, false, false, false, false, false, false, true, false, false, false],
      // LT: Deep sub texture
      LT: [false, false, false, false, false, false, false, true, false, false, false, false, false, false, false, false],
      // MT: Mid-frequency rhythmic element
      MT: [false, false, false, false, false, false, true, false, false, false, false, false, false, false, false, false],
      // HT: Higher tones for brightness
      HT: [false, false, false, false, false, false, false, false, false, true, false, false, false, false, false, false],
      // RS: Sharp rimshot accents
      RS: [false, false, false, false, false, false, false, false, false, false, true, false, false, false, false, false],
      // CP: Classic clap pattern
      CP: [false, false, false, false, true, false, false, false, false, false, false, false, true, false, false, false],
      // CH: Signature closed hi-hat pattern
      CH: [true, false, true, false, true, false, true, false, true, false, true, false, true, false, true, false],
      // OH: Open hi-hats for atmospheric space
      OH: [false, false, false, true, false, false, false, true, false, false, false, true, false, false, false, true],
      // CY: Crash for dramatic moments
      CY: [true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false],
      // RD: Ride for continuous texture
      RD: [false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true],
      // CB: Cowbell for UK garage flavor
      CB: [false, false, false, false, false, false, false, false, false, false, false, false, false, true, false, false],
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
