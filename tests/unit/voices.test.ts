import { describe, it, expect } from 'vitest';
import { playBD } from '../../src/audio/voices/bassDrum';
import { playSD } from '../../src/audio/voices/snareDrum';
import { playCH } from '../../src/audio/voices/closedHiHat';
import { playOH } from '../../src/audio/voices/openHiHat';
import { playCP } from '../../src/audio/voices/clap';
import { playLT } from '../../src/audio/voices/lowTom';
import { playMT } from '../../src/audio/voices/midTom';
import { playHT } from '../../src/audio/voices/highTom';
import { playRS } from '../../src/audio/voices/rimshot';
import { playCB } from '../../src/audio/voices/cowbell';
import { playCY } from '../../src/audio/voices/crashCymbal';
import { playRD } from '../../src/audio/voices/rideCymbal';

describe('Bass Drum', () => {
  it('should render without errors', async () => {
    const ctx = new OfflineAudioContext(1, 44100, 44100);
    playBD(ctx, 0);
    const buffer = await ctx.startRendering();
    const data = buffer.getChannelData(0);
    expect(data.some((v) => Math.abs(v) > 0.001)).toBe(true);
  });
});

describe('Snare Drum', () => {
  it('should render without errors', async () => {
    const ctx = new OfflineAudioContext(1, 44100, 44100);
    playSD(ctx, 0);
    const buffer = await ctx.startRendering();
    const data = buffer.getChannelData(0);
    expect(data.some((v) => Math.abs(v) > 0.001)).toBe(true);
  });
});

describe('Low Tom', () => {
  it('should render without errors', async () => {
    const ctx = new OfflineAudioContext(1, 44100, 44100);
    playLT(ctx, 0);
    const buffer = await ctx.startRendering();
    const data = buffer.getChannelData(0);
    expect(data.some((v) => Math.abs(v) > 0.001)).toBe(true);
  });
});

describe('Mid Tom', () => {
  it('should render without errors', async () => {
    const ctx = new OfflineAudioContext(1, 44100, 44100);
    playMT(ctx, 0);
    const buffer = await ctx.startRendering();
    const data = buffer.getChannelData(0);
    expect(data.some((v) => Math.abs(v) > 0.001)).toBe(true);
  });
});

describe('High Tom', () => {
  it('should render without errors', async () => {
    const ctx = new OfflineAudioContext(1, 44100, 44100);
    playHT(ctx, 0);
    const buffer = await ctx.startRendering();
    const data = buffer.getChannelData(0);
    expect(data.some((v) => Math.abs(v) > 0.001)).toBe(true);
  });
});

describe('Rimshot', () => {
  it('should render without errors', async () => {
    const ctx = new OfflineAudioContext(1, 44100, 44100);
    playRS(ctx, 0);
    const buffer = await ctx.startRendering();
    const data = buffer.getChannelData(0);
    expect(data.some((v) => Math.abs(v) > 0.001)).toBe(true);
  });
});

describe('Closed Hi-Hat', () => {
  it('should render without errors', async () => {
    const ctx = new OfflineAudioContext(1, 44100, 44100);
    playCH(ctx, 0);
    const buffer = await ctx.startRendering();
    const data = buffer.getChannelData(0);
    expect(data.some((v) => Math.abs(v) > 0.001)).toBe(true);
  });
});

describe('Open Hi-Hat', () => {
  it('should render without errors', async () => {
    const ctx = new OfflineAudioContext(1, 44100, 44100);
    playOH(ctx, 0);
    const buffer = await ctx.startRendering();
    const data = buffer.getChannelData(0);
    expect(data.some((v) => Math.abs(v) > 0.001)).toBe(true);
  });
});

describe('Clap', () => {
  it('should render without errors', async () => {
    const ctx = new OfflineAudioContext(1, 44100, 44100);
    playCP(ctx, 0);
    const buffer = await ctx.startRendering();
    const data = buffer.getChannelData(0);
    expect(data.some((v) => Math.abs(v) > 0.001)).toBe(true);
  });
});

describe('Cowbell', () => {
  it('should render without errors', async () => {
    const ctx = new OfflineAudioContext(1, 44100, 44100);
    playCB(ctx, 0);
    const buffer = await ctx.startRendering();
    const data = buffer.getChannelData(0);
    expect(data.some((v) => Math.abs(v) > 0.001)).toBe(true);
  });
});

describe('Crash Cymbal', () => {
  it('should render without errors', async () => {
    const ctx = new OfflineAudioContext(1, 44100, 44100);
    playCY(ctx, 0);
    const buffer = await ctx.startRendering();
    const data = buffer.getChannelData(0);
    expect(data.some((v) => Math.abs(v) > 0.001)).toBe(true);
  });
});

describe('Ride Cymbal', () => {
  it('should render without errors', async () => {
    const ctx = new OfflineAudioContext(1, 44100, 44100);
    playRD(ctx, 0);
    const buffer = await ctx.startRendering();
    const data = buffer.getChannelData(0);
    expect(data.some((v) => Math.abs(v) > 0.001)).toBe(true);
  });
});
