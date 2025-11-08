import { initAudio } from '../audio/audioContext';

export interface ControlsCallbacks {
  onPlay: () => void;
  onStop: () => void;
  onBpmChange: (bpm: number) => void;
}

export class Controls {
  private playBtn: HTMLButtonElement;
  private stopBtn: HTMLButtonElement;
  private bpmSlider: HTMLInputElement;
  private bpmDisplay: HTMLElement;

  constructor(containerId: string, callbacks: ControlsCallbacks) {
    const container = document.getElementById(containerId);
    if (!container) throw new Error(`Container ${containerId} not found`);

    // Only create Play/Stop buttons
    container.innerHTML = `
      <button id="play-btn" class="control-btn">▶ Play</button>
      <button id="stop-btn" class="control-btn">■ Stop</button>
    `;

    this.playBtn = container.querySelector('#play-btn')!;
    this.stopBtn = container.querySelector('#stop-btn')!;
    
    // Use existing BPM controls from HTML
    this.bpmSlider = document.getElementById('bpm-slider') as HTMLInputElement;
    this.bpmDisplay = document.getElementById('bpm-display') as HTMLElement;

    this.playBtn.addEventListener('click', async () => {
      await initAudio();
      callbacks.onPlay();
      this.setPlaying(true);
    });

    this.stopBtn.addEventListener('click', () => {
      callbacks.onStop();
      this.setPlaying(false);
    });

    this.bpmSlider.addEventListener('input', () => {
      const bpm = parseInt(this.bpmSlider.value, 10);
      this.bpmDisplay.textContent = bpm.toString();
      callbacks.onBpmChange(bpm);
    });
  }

  setPlaying(playing: boolean): void {
    this.playBtn.disabled = playing;
    this.stopBtn.disabled = !playing;
  }

  getBpm(): number {
    return parseInt(this.bpmSlider.value, 10);
  }
}
