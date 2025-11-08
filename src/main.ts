import { getAudioContext } from './audio/audioContext';
import { AudioScheduler } from './audio/scheduler';
import { playBD } from './audio/voices/bassDrum';
import { playSD } from './audio/voices/snareDrum';
import { playCH } from './audio/voices/closedHiHat';
import { playOH } from './audio/voices/openHiHat';
import { playCP } from './audio/voices/clap';
import { Controls } from './ui/controls';
import { SequencerGrid } from './ui/sequencerGrid';
import { Visualizer } from './ui/visualizer';
import { store } from './state/store';
import { toggleStep, createDefaultPattern } from './state/pattern';
import { loadPattern, savePattern } from './storage/localStorage';

const scheduler = new AudioScheduler();
const voiceMap = { BD: playBD, SD: playSD, CH: playCH, OH: playOH, CP: playCP };
const voiceLevels = { BD: 1.0, SD: 1.0, CH: 0.7, OH: 0.8, CP: 0.6 };

// Load saved pattern or use default
const savedPattern = loadPattern();
if (savedPattern) {
  store.setState({ pattern: savedPattern });
}

// Initialize UI
const grid = new SequencerGrid('sequencer', {
  onStepToggle: (instrument, step) => {
    const state = store.getState();
    const updated = toggleStep(
      state.pattern,
      instrument as keyof typeof voiceMap,
      step
    );
    store.setState({ pattern: updated });
    savePattern(updated);
  },
});

// Function to load pattern into UI
function loadPatternToUI(): void {
  const state = store.getState();
  
  // Clear all steps first
  Object.keys(state.pattern.steps).forEach((instrument) => {
    for (let step = 0; step < 16; step++) {
      grid.setStepActive(instrument, step, false);
    }
  });
  
  // Set active steps
  Object.entries(state.pattern.steps).forEach(([instrument, steps]) => {
    steps.forEach((active, step) => {
      if (active) {
        grid.setStepActive(instrument, step, true);
      }
    });
  });
  
  // Update BPM
  const bpmSlider = document.getElementById('bpm-slider') as HTMLInputElement;
  const bpmDisplay = document.getElementById('bpm-display') as HTMLElement;
  if (bpmSlider && bpmDisplay) {
    bpmSlider.value = state.pattern.bpm.toString();
    bpmDisplay.textContent = state.pattern.bpm.toString();
  }
  
  updateStatus(`Loaded: ${state.pattern.name}`);
}

// Load default pattern function
function loadDefaultPattern(): void {
  const defaultPattern = createDefaultPattern();
  store.setState({ pattern: defaultPattern });
  savePattern(defaultPattern);
  loadPatternToUI();
}

// Restore pattern state in UI
loadPatternToUI();

const visualizer = new Visualizer((step) => {
  grid.highlightStep(step);
});

const controls = new Controls('controls', {
  onPlay: () => {
    const ctx = getAudioContext();
    if (!ctx) return;

    const bpm = controls.getBpm();

    scheduler.start(ctx, bpm, (step, time) => {
      const currentState = store.getState();
      Object.entries(currentState.pattern.steps).forEach(([instrument, steps]) => {
        if (steps[step]) {
          const voice = voiceMap[instrument as keyof typeof voiceMap];
          voice(ctx, time);
        }
      });
    });

    visualizer.start(bpm);
    store.setState({ playback: { isPlaying: true, currentStep: 0, bpm } });
    updateStatus('Playing');
  },

  onStop: () => {
    scheduler.stop();
    visualizer.stop();
    grid.highlightStep(-1);
    store.setState({
      playback: { isPlaying: false, currentStep: 0, bpm: controls.getBpm() },
    });
    updateStatus('Stopped');
  },

  onBpmChange: (bpm) => {
    const state = store.getState();
    if (state.playback.isPlaying) {
      scheduler.stop();
      visualizer.stop();
      const ctx = getAudioContext();
      if (ctx) {
        scheduler.start(ctx, bpm, (step, time) => {
          const currentState = store.getState();
          Object.entries(currentState.pattern.steps).forEach(([instrument, steps]) => {
            if (steps[step]) {
              const voice = voiceMap[instrument as keyof typeof voiceMap];
              voice(ctx, time);
            }
          });
        });
        visualizer.start(bpm);
      }
    }
    store.setState({ playback: { ...state.playback, bpm } });
  },
});

// Mixer controls
function initMixer(): void {
  const mixer = document.getElementById('mixer');
  if (!mixer) return;

  Object.entries(voiceLevels).forEach(([voice, level]) => {
    const control = document.createElement('div');
    control.className = 'voice-control';
    control.innerHTML = `
      <span class="voice-name">${voice}</span>
      <input type="range" min="0" max="100" value="${level * 100}" 
             data-voice="${voice}" aria-label="${voice} level">
      <span class="voice-level">${Math.round(level * 100)}%</span>
    `;

    const slider = control.querySelector('input') as HTMLInputElement;
    const display = control.querySelector('.voice-level') as HTMLElement;

    slider.addEventListener('input', () => {
      const value = parseInt(slider.value, 10) / 100;
      voiceLevels[voice as keyof typeof voiceLevels] = value;
      display.textContent = `${Math.round(value * 100)}%`;
    });

    mixer.appendChild(control);
  });
}

// Keyboard shortcuts
document.addEventListener('keydown', (e) => {
  // Ignore if typing in input
  if (e.target instanceof HTMLInputElement) return;

  if (e.key === ' ') {
    e.preventDefault();
    const state = store.getState();
    if (state.playback.isPlaying) {
      controls.setPlaying(false);
      scheduler.stop();
      visualizer.stop();
      updateStatus('Stopped');
    } else {
      document.getElementById('play-btn')?.click();
    }
    return;
  }

  if (e.key === '+' || e.key === '=') {
    e.preventDefault();
    changeBPM(5);
    return;
  }

  if (e.key === '-' || e.key === '_') {
    e.preventDefault();
    changeBPM(-5);
    return;
  }

  if (e.key === '?') {
    e.preventDefault();
    toggleHelp();
    return;
  }

  if (e.key === 'Escape') {
    closeHelp();
    return;
  }

  if (e.key === 't' || e.key === 'T') {
    e.preventDefault();
    toggleTheme();
    return;
  }

  if (e.key === 'd' || e.key === 'D') {
    e.preventDefault();
    loadDefaultPattern();
  }
});

function changeBPM(delta: number): void {
  const slider = document.getElementById('bpm-slider') as HTMLInputElement;
  const current = parseInt(slider.value, 10);
  const newValue = Math.max(60, Math.min(240, current + delta));
  slider.value = newValue.toString();
  const event = new window.Event('input');
  slider.dispatchEvent(event);
}

function toggleHelp(): void {
  const overlay = document.getElementById('help-overlay');
  overlay?.classList.toggle('visible');
}

function closeHelp(): void {
  const overlay = document.getElementById('help-overlay');
  overlay?.classList.remove('visible');
}

function toggleTheme(): void {
  const html = document.documentElement;
  const current = html.getAttribute('data-theme');
  const newTheme = current === 'dark' ? 'light' : 'dark';
  html.setAttribute('data-theme', newTheme);
  const btn = document.getElementById('theme-toggle');
  if (btn) btn.textContent = newTheme === 'dark' ? '🌙' : '☀️';
}

function updateStatus(text: string): void {
  const status = document.getElementById('status-text');
  if (status) status.textContent = text;
}

// Performance monitoring
let lastTime = window.performance.now();
let frames = 0;

function updateFPS(): void {
  frames++;
  const now = window.performance.now();
  if (now - lastTime >= 1000) {
    const fps = Math.round((frames * 1000) / (now - lastTime));
    const perfText = document.getElementById('perf-text');
    if (perfText) perfText.textContent = `FPS: ${fps}`;
    frames = 0;
    lastTime = now;
  }
  requestAnimationFrame(updateFPS);
}

// Initialize
initMixer();
const initialState = store.getState();
updateStatus(`Ready - ${initialState.pattern.name}`);
requestAnimationFrame(updateFPS);

// Help button
document.getElementById('help-btn')?.addEventListener('click', toggleHelp);
document.getElementById('theme-toggle')?.addEventListener('click', toggleTheme);
document.getElementById('default-pattern-btn')?.addEventListener('click', loadDefaultPattern);

// Close help on overlay click
document.getElementById('help-overlay')?.addEventListener('click', (e) => {
  if (e.target === e.currentTarget) closeHelp();
});

console.log(`TR-808 initialized - ${initialState.pattern.name} @ ${initialState.pattern.bpm} BPM`);
console.log('Press ? for shortcuts, D for default pattern');
