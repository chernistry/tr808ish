export interface Pattern {
  name: string;
  bpm: number;
  steps: {
    BD: boolean[];
    SD: boolean[];
    LT: boolean[];
    MT: boolean[];
    HT: boolean[];
    RS: boolean[];
    CP: boolean[];
    CH: boolean[];
    OH: boolean[];
    CY: boolean[];
    RD: boolean[];
    CB: boolean[];
  };
}

export interface PlaybackState {
  isPlaying: boolean;
  currentStep: number;
  bpm: number;
}

export interface AppState {
  pattern: Pattern;
  playback: PlaybackState;
}
