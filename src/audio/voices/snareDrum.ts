export function playSD(ctx: AudioContext, time: number): void {
  // Tonal component
  const osc = new OscillatorNode(ctx, { type: 'triangle', frequency: 180 });
  const oscGain = new GainNode(ctx, { gain: 0.7 });

  // Noise component
  const buffer = ctx.createBuffer(1, ctx.sampleRate * 0.2, ctx.sampleRate);
  const data = buffer.getChannelData(0);
  for (let i = 0; i < data.length; i++) {
    data[i] = Math.random() * 2 - 1;
  }

  const noise = new AudioBufferSourceNode(ctx, { buffer });
  const noiseFilter = new BiquadFilterNode(ctx, {
    type: 'highpass',
    frequency: 1000,
    Q: 1,
  });
  const noiseGain = new GainNode(ctx, { gain: 0.5 });

  // Connect
  osc.connect(oscGain);
  noise.connect(noiseFilter).connect(noiseGain);

  const master = new GainNode(ctx, { gain: 1.0 });
  oscGain.connect(master);
  noiseGain.connect(master);
  master.connect(ctx.destination);

  // Envelope
  master.gain.setValueAtTime(1.0, time);
  master.gain.exponentialRampToValueAtTime(0.0001, time + 0.15);

  osc.start(time);
  noise.start(time);
  osc.stop(time + 0.2);
}
