#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "DesignTokens.h"

class TR808GarageEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit TR808GarageEditor(TR808GarageProcessor&);
    ~TR808GarageEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void paintHeader(juce::Graphics& g, juce::Rectangle<int> area);
    void paintPatternBank(juce::Graphics& g, juce::Rectangle<int> area);
    void paintVoiceSection(juce::Graphics& g, juce::Rectangle<int> area);
    void paintFooter(juce::Graphics& g, juce::Rectangle<int> area);

    TR808GarageProcessor& processor;

    // Pattern bank
    juce::TextButton patternButtons[8];
    juce::TextButton copyButton, pasteButton, clearButton;
    int selectedPattern = 0;
    
    // Theme toggle
    juce::TextButton themeButton;
    
    // Parameter attachments
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    
    // Master
    juce::Slider masterLevelSlider;
    std::unique_ptr<SliderAttachment> masterLevelAttachment;
    juce::Label masterLevelLabel;
    
    // Voice controls
    struct VoiceControls {
        juce::String name;
        juce::Colour color;
        juce::Label nameLabel;
        juce::Slider levelSlider, tuneSlider, decaySlider, toneSlider;
        std::unique_ptr<SliderAttachment> levelAttachment, tuneAttachment, decayAttachment, toneAttachment;
    };
    
    VoiceControls bdControls, sdControls, chControls, ohControls, cpControls, rsControls;

    void setupVoiceControls(VoiceControls& vc, const juce::String& name, juce::Colour color,
                           const char* levelID, const char* tuneID, 
                           const char* decayID, const char* toneID);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TR808GarageEditor)
};
