#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "DesignTokens.h"

/**
 * Component for dragging MIDI patterns out of the plugin to the DAW.
 * Supports OS-level drag-and-drop with fallback to Save/Reveal.
 */
class MidiDragSource : public juce::Component
{
public:
    MidiDragSource();
    ~MidiDragSource() override;
    
    void setMidiFile(const juce::MidiFile& file);
    const juce::MidiFile& getMidiFile() const { return midiFile; }
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    void mouseEnter(const juce::MouseEvent& e) override;
    void mouseExit(const juce::MouseEvent& e) override;
    
private:
    void performDragAndDrop();
    void saveMidiFile();
    void revealMidiFile();
    void cleanupTempFiles();
    
    juce::MidiFile midiFile;
    juce::File tempFile;
    juce::File lastSavedFile;
    juce::Point<int> dragStartPos;
    bool isDragging = false;
    bool isHovering = false;
    
    // Fallback buttons
    juce::TextButton saveButton;
    juce::TextButton revealButton;
    bool showFallbackButtons = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiDragSource)
};
