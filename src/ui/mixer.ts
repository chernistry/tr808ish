import { voiceParams } from '../state/voiceParams';

export class Mixer {
  private container: HTMLElement;
  private voices = ['BD', 'SD', 'LT', 'MT', 'HT', 'RS', 'CP', 'CH', 'OH', 'CY', 'RD', 'CB'];

  constructor(containerId: string) {
    const el = document.getElementById(containerId);
    if (!el) throw new Error(`Container ${containerId} not found`);
    this.container = el;
    this.render();
    this.attachEvents();
  }

  private render(): void {
    this.container.innerHTML = `
      <div class="mixer-strip">
        ${this.voices.map(voice => `
          <div class="voice-control" data-voice="${voice}">
            <span class="voice-name">${voice}</span>
            <input type="range" 
                   min="0" 
                   max="1" 
                   step="0.01" 
                   value="${voiceParams[voice as keyof typeof voiceParams].level}" 
                   class="voice-slider"
                   data-voice="${voice}">
            <span class="voice-level">${Math.round(voiceParams[voice as keyof typeof voiceParams].level * 100)}%</span>
          </div>
        `).join('')}
      </div>
    `;
  }

  private attachEvents(): void {
    const sliders = this.container.querySelectorAll('.voice-slider');
    
    sliders.forEach(slider => {
      slider.addEventListener('input', (e) => {
        const target = e.target as HTMLInputElement;
        const voice = target.dataset.voice!;
        const value = parseFloat(target.value);
        
        // Update the level in the global voiceParams object
        this.updateVoiceLevel(voice, value);
        
        // Update the display
        const display = target.parentElement?.querySelector('.voice-level');
        if (display) {
          display.textContent = `${Math.round(value * 100)}%`;
        }
      });
    });
  }

  private updateVoiceLevel(voice: string, level: number): void {
    // Update the level in the global voiceParams object
    // This affects the actual audio parameters
    if (voice in voiceParams) {
      const voiceKey = voice as keyof typeof voiceParams;
      if ('level' in voiceParams[voiceKey]) {
        (voiceParams[voiceKey] as any).level = level;
      }
    }
  }
}