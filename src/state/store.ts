import type { AppState } from './types';
import { createDefaultPattern } from './pattern';

type Listener = (state: AppState) => void;

class Store {
  private state: AppState;
  private listeners: Set<Listener> = new Set();

  constructor() {
    this.state = {
      pattern: createDefaultPattern(), // Use default UK garage pattern
      playback: {
        isPlaying: false,
        currentStep: 0,
        bpm: 130, // Match default pattern BPM
      },
    };
  }

  getState(): AppState {
    return this.state;
  }

  setState(partial: Partial<AppState>): void {
    this.state = { ...this.state, ...partial };
    this.notify();
  }

  subscribe(listener: Listener): () => void {
    this.listeners.add(listener);
    return () => this.listeners.delete(listener);
  }

  private notify(): void {
    this.listeners.forEach((listener) => listener(this.state));
  }
}

export const store = new Store();
