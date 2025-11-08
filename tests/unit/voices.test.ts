import { describe, it, expect } from 'vitest';
import { playBD } from '../../src/audio/voices/bassDrum';
import { playSD } from '../../src/audio/voices/snareDrum';
import { playCH } from '../../src/audio/voices/closedHiHat';
import { playOH } from '../../src/audio/voices/openHiHat';
import { playCP } from '../../src/audio/voices/clap';

describe('Bass Drum', () => {
  it('should render without errors', async () => {
    const ctx = new OfflineAudioContext(1, 44100, 44100);
    playBD(ctx, 0);
    const buffer = await ctx.startRendering();

    const data = buffer.getChannelData(0);
    const hasSound = data.some((v) => Math.abs(v) > 0.001);
    expect(hasSound).toBe(true);
  });

  it('should have correct duration', async () => {
    const ctx = new OfflineAudioContext(1, 44100, 44100);
    playBD(ctx, 0);
    const buffer = await ctx.startRendering();

    expect(buffer.duration).toBe(1);
  });

  it('should have peak amplitude near start', async () => {
    const ctx = new OfflineAudioContext(1, 44100, 44100);
    playBD(ctx, 0);
    const buffer = await ctx.startRendering();

    const data = buffer.getChannelData(0);
    const firstSamples = data.slice(0, 4410);
    const maxAmplitude = Math.max(...firstSamples.map(Math.abs));

    expect(maxAmplitude).toBeGreaterThan(0.1);
  });
});

describe('Snare Drum', () => {
  it('should render without errors', async () => {
    const ctx = new OfflineAudioContext(1, 44100, 44100);
    playSD(ctx, 0);
    const buffer = await ctx.startRendering();

    const data = buffer.getChannelData(0);
    const hasSound = data.some((v) => Math.abs(v) > 0.001);
    expect(hasSound).toBe(true);
  });

  it('should have correct duration', async () => {
    const ctx = new OfflineAudioContext(1, 44100, 44100);
    playSD(ctx, 0);
    const buffer = await ctx.startRendering();

    expect(buffer.duration).toBe(1);
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
