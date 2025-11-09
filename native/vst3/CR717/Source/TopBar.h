#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "DesignTokens.h"
#include "MidiDragSource.h"

/**
 * Top bar component containing:
 * - Transport controls (Play/Stop)
 * - Host sync indicator
 * - BPM control (host/local)
 * - Swing control
 * - Preset browser button
 * - Pattern bank selector (A-H)
 * - Pattern operations (Copy/Paste/Clear)
 * - MIDI drag-and-drop
 */
class TopBar : public juce::Component,
               private juce::Timer
{
public:
    TopBar(juce::AudioProcessorValueTreeState& apvts,
           std::function<void()> onPlay,
           std::function<void()> onStop,
           std::function<void()> onPresetBrowser,
           std::function<void(int)> onBankSelect,
           std::function<void()> onPatternCopy,
           std::function<void()> onPatternPaste,
           std::function<void()> onPatternClear);
    
    ~TopBar() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Update host sync state
    void setHostSyncActive(bool active);
    void setHostBPM(double bpm);
    void setIsPlaying(bool playing);
    
    // Pattern bank
    void setCurrentBank(int bank); // 0-7 for A-H
    int getCurrentBank() const { return currentBank; }
    
    // Swing
    void setSwing(float swing); // 0.0-0.75
    float getSwing() const;

private:
    void timerCallback() override;
    void updatePlayButtonState();
    void updateBPMDisplay();
    void updateHostSyncIndicator();
    
    juce::AudioProcessorValueTreeState& apvts;
    
    // Callbacks
    std::function<void()> playCallback;
    std::function<void()> stopCallback;
    std::function<void()> presetBrowserCallback;
    std::function<void(int)> bankSelectCallback;
    std::function<void()> patternCopyCallback;
    std::function<void()> patternPasteCallback;
    std::function<void()> patternClearCallback;
    
    // Transport
    juce::TextButton playButton;
    juce::TextButton stopButton;
    bool isPlaying = false;
    
    // Host sync
    juce::Label hostSyncLabel;
    bool hostSyncActive = false;
    double hostBPM = 120.0;
    
    // BPM control
    juce::Label bpmLabel;
    juce::Slider bpmSlider;
    juce::Label bpmValueLabel;
    bool useHostBPM = false;
    
    // Swing control
    juce::Label swingLabel;
    juce::Slider swingSlider;
    juce::Label swingValueLabel;
    
    // Preset browser
    juce::TextButton presetBrowserButton;
    
    // Pattern bank selector (A-H)
    juce::TextButton bankButtons[8];
    int currentBank = 0;
    
    // Pattern operations
    juce::TextButton copyButton;
    juce::TextButton pasteButton;
    juce::TextButton clearButton;
    
    // MIDI drag source
    std::unique_ptr<MidiDragSource> midiDragSource;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopBar)
};
