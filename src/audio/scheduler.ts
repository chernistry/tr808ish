import type { Scheduler, SchedulerCallback } from './types';

export class AudioScheduler implements Scheduler {
  private intervalId: number | null = null;
  private nextNoteTime = 0;
  private currentStep = 0;
  private readonly scheduleAheadTime = 0.1;
  private readonly tickInterval = 25;
  private running = false;
  private swing: number = 0; // 0-0.75

  setSwing(amount: number): void {
    this.swing = Math.max(0, Math.min(0.75, amount));
  }

  start(ctx: AudioContext, bpm: number, onStep: SchedulerCallback): void {
    if (this.running) return;

    const stepDuration = 60 / bpm / 4;
    this.nextNoteTime = ctx.currentTime;
    this.currentStep = 0;
    this.running = true;

    this.intervalId = window.setInterval(() => {
      const end = ctx.currentTime + this.scheduleAheadTime;
      while (this.nextNoteTime < end && this.running) {
        // Apply swing to odd steps
        const swingOffset =
          this.currentStep % 2 === 1 ? this.swing * stepDuration * 0.5 : 0;

        onStep(this.currentStep, this.nextNoteTime + swingOffset);

        this.nextNoteTime += stepDuration;
        this.currentStep = (this.currentStep + 1) % 16;
      }
    }, this.tickInterval);
  }

  stop(): void {
    this.running = false;
    if (this.intervalId !== null) {
      clearInterval(this.intervalId);
      this.intervalId = null;
    }
  }

  getCurrentStep(): number {
    return this.currentStep;
  }

  isRunning(): boolean {
    return this.running;
  }
}
