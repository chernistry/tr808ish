# CR-717 QA Checklist

## Automated Testing

### pluginval
- [ ] Run `./scripts/qa/run-pluginval.sh`
- [ ] All tests pass (strictness level 5)
- [ ] No crashes or assertions
- [ ] State save/load works correctly

## Manual Testing Matrix

### macOS Testing
- [ ] macOS 13 (Ventura) - Retina display
- [ ] macOS 14 (Sonoma) - Retina display
- [ ] macOS 15 (Sequoia) - Retina display
- [ ] Non-Retina display (if available)

### DAW Compatibility
- [ ] Ableton Live (latest)
  - [ ] Plugin loads correctly
  - [ ] All parameters automatable
  - [ ] Preset save/load works
  - [ ] MIDI input works
  - [ ] Audio output clean
- [ ] Reaper (latest)
  - [ ] Plugin loads correctly
  - [ ] All parameters automatable
  - [ ] Preset save/load works
  - [ ] MIDI input works
  - [ ] Audio output clean
- [ ] Logic Pro (macOS only)
  - [ ] Plugin loads correctly
  - [ ] All parameters automatable
  - [ ] Preset save/load works
  - [ ] MIDI input works
  - [ ] Audio output clean

### UI/UX Testing
- [ ] Window resizing works (1200x800 - 1800x1200)
- [ ] HiDPI scaling correct on Retina displays
- [ ] All knobs/faders respond to mouse
- [ ] Double-click reset works on all controls
- [ ] Shift/Alt fine/coarse adjustment works
- [ ] Keyboard navigation works (Tab, arrows)
- [ ] Focus rings visible
- [ ] Tooltips appear on hover
- [ ] Step sequencer pads toggle correctly
- [ ] Play/Stop buttons work
- [ ] BPM slider updates tempo
- [ ] Preset selector loads all 36 presets

### Audio Testing
- [ ] All 12 voices trigger correctly
- [ ] Bass drum sounds at 56 Hz
- [ ] Snare has body + noise
- [ ] Toms at correct frequencies (130/200/325 Hz)
- [ ] Hi-hats have metallic character
- [ ] Cymbals sustain correctly (1.2s / 1.9s)
- [ ] Clap has 4-pulse structure
- [ ] Rimshot is short and snappy
- [ ] No clicks/pops on voice triggers
- [ ] No audio dropouts
- [ ] Master level control works
- [ ] Per-voice levels work

### State Management
- [ ] Save project with plugin → close → reopen → state restored
- [ ] Preset changes persist across sessions
- [ ] Sequencer patterns save/load correctly
- [ ] Parameter automation recorded and played back
- [ ] Rapid preset switching doesn't crash

### Performance
- [ ] CPU usage reasonable (<10% on modern CPU)
- [ ] No UI lag during playback
- [ ] Meters update smoothly at 30Hz
- [ ] No memory leaks (check Activity Monitor)

## Known Issues
- Document any issues found during testing here

## Test Results
- Date tested: ___________
- Tester: ___________
- Platform: ___________
- DAW: ___________
- Result: PASS / FAIL
- Notes: ___________
