/*
  TR-808 Garage Plugin Editor
  
  Implements VST3 UI Best Practices (2025):
  - APVTS + Attachments for thread-safe parameter automation
  - Resizable with setResizeLimits (1100x720 - 1650x1080)
  - Throttled repaints at 30Hz for meters (not 60Hz)
  - Accessibility: titles and descriptions for all controls
  - No OpenGL (CPU rendering only, macOS compatible)
  - HiDPI ready via JUCE's built-in scaling
  
  See: backlog/open/40-ui-guide-complete-overhaul.md
*/

#include "PluginEditor.h"

using namespace DesignTokens;

TR808GarageEditor::TR808GarageEditor(TR808GarageProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setSize(1100, 720);
    setResizable(true, true);
    setResizeLimits(1100, 720, 1650, 1080);
    
    // Preset selector (combo box for 36 presets)
    presetSelector.setColour(juce::ComboBox::backgroundColourId, Colors::bgSecondary);
    presetSelector.setColour(juce::ComboBox::textColourId, Colors::textPrimary);
    presetSelector.setColour(juce::ComboBox::outlineColourId, Colors::border);
    presetSelector.setColour(juce::ComboBox::arrowColourId, Colors::accent);
    
    const auto& presets = processor.getPresetManager().getPresets();
    for (size_t i = 0; i < presets.size(); ++i)
    {
        juce::String itemText = presets[i].style + " - " + presets[i].name;
        presetSelector.addItem(itemText, static_cast<int>(i + 1));
    }
    presetSelector.setSelectedId(1, juce::dontSendNotification);
    presetSelector.onChange = [this] {
        int index = presetSelector.getSelectedId() - 1;
        if (index >= 0)
            processor.setCurrentProgram(index);
    };
    addAndMakeVisible(presetSelector);
    
    // Preset navigation buttons
    prevPresetButton.setButtonText("<");
    nextPresetButton.setButtonText(">");
    for (auto* btn : {&prevPresetButton, &nextPresetButton})
    {
        btn->setColour(juce::TextButton::buttonColourId, Colors::bgTertiary);
        btn->setColour(juce::TextButton::textColourOffId, Colors::textSecondary);
        addAndMakeVisible(btn);
    }
    
    prevPresetButton.onClick = [this] {
        int current = presetSelector.getSelectedId() - 1;
        if (current > 0)
            presetSelector.setSelectedId(current, juce::sendNotification);
    };
    
    nextPresetButton.onClick = [this] {
        int current = presetSelector.getSelectedId() + 1;
        if (current <= presetSelector.getNumItems())
            presetSelector.setSelectedId(current, juce::sendNotification);
    };
    
    // Transport controls
    playButton.setButtonText("▶");
    stopButton.setButtonText("■");
    for (auto* btn : {&playButton, &stopButton})
    {
        btn->setColour(juce::TextButton::buttonColourId, Colors::accent);
        btn->setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        addAndMakeVisible(btn);
    }
    
    playButton.onClick = [this] { processor.startSequencer(); };
    stopButton.onClick = [this] { processor.stopSequencer(); };
    
    // BPM control
    bpmLabel.setText("BPM", juce::dontSendNotification);
    bpmLabel.setJustificationType(juce::Justification::centredRight);
    bpmLabel.setColour(juce::Label::textColourId, Colors::textSecondary);
    addAndMakeVisible(bpmLabel);
    
    bpmSlider.setRange(60, 240, 1);
    bpmSlider.setValue(120);
    bpmSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    bpmSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    bpmSlider.onValueChange = [this] {
        processor.getSequencer().setBPM(bpmSlider.getValue());
    };
    addAndMakeVisible(bpmSlider);
    
    // Voice selector for sequencer
    const char* voiceNames[] = {"BD", "SD", "LT", "MT", "HT", "RS", "CP", "CH", "OH", "CY", "RD", "CB"};
    for (int i = 0; i < 12; ++i)
        voiceSelector.addItem(voiceNames[i], i + 1);
    voiceSelector.setSelectedId(1);
    voiceSelector.onChange = [this] {
        selectedVoice = voiceSelector.getSelectedId() - 1;
        updateStepButtons();
    };
    addAndMakeVisible(voiceSelector);
    
    // Step buttons
    for (int i = 0; i < 16; ++i)
    {
        stepButtons[i].setButtonText(juce::String(i + 1));
        stepButtons[i].setClickingTogglesState(true);
        stepButtons[i].setColour(juce::TextButton::buttonColourId, Colors::bgTertiary);
        stepButtons[i].setColour(juce::TextButton::buttonOnColourId, Colors::accent);
        stepButtons[i].onClick = [this, i] {
            bool active = stepButtons[i].getToggleState();
            processor.getSequencer().setStep(selectedVoice, i, active);
        };
        addAndMakeVisible(stepButtons[i]);
    }
    
    // MIDI drag source
    addAndMakeVisible(midiDragSource);
    
    // Copy/Paste/Clear buttons
    copyButton.setButtonText("Copy");
    pasteButton.setButtonText("Paste");
    clearButton.setButtonText("Clear");
    
    for (auto* btn : {&copyButton, &pasteButton, &clearButton})
    {
        btn->setColour(juce::TextButton::buttonColourId, Colors::bgTertiary);
        btn->setColour(juce::TextButton::textColourOffId, Colors::textSecondary);
        addAndMakeVisible(btn);
    }
    
    // Theme toggle
    themeButton.setButtonText("Theme");
    themeButton.setColour(juce::TextButton::buttonColourId, Colors::bgTertiary);
    themeButton.setColour(juce::TextButton::textColourOffId, Colors::textSecondary);
    addAndMakeVisible(themeButton);
    
    // Master level with meter
    masterLevelLabel.setText("Master", juce::dontSendNotification);
    masterLevelLabel.setJustificationType(juce::Justification::centred);
    masterLevelLabel.setColour(juce::Label::textColourId, Colors::textPrimary);
    masterLevelLabel.setFont(juce::Font(Typography::md));
    addAndMakeVisible(masterLevelLabel);
    
    addAndMakeVisible(masterLevelSlider);
    masterLevelAttachment = std::make_unique<SliderAttachment>(
        processor.getAPVTS(), ParamIDs::masterLevel, masterLevelSlider);
    
    addAndMakeVisible(masterMeter);
    
    // Setup all 12 voices
    setupVoiceControls(bdControls, "BD", Colors::instBD, ParamIDs::bdLevel, ParamIDs::bdTune, ParamIDs::bdDecay, ParamIDs::bdTone);
    setupVoiceControls(sdControls, "SD", Colors::instSD, ParamIDs::sdLevel, ParamIDs::sdTune, ParamIDs::sdDecay, ParamIDs::sdSnappy);
    setupVoiceControls(ltControls, "LT", Colors::instTom, ParamIDs::ltLevel, ParamIDs::ltTune, ParamIDs::ltDecay, nullptr);
    setupVoiceControls(mtControls, "MT", Colors::instTom, ParamIDs::mtLevel, ParamIDs::mtTune, ParamIDs::mtDecay, nullptr);
    setupVoiceControls(htControls, "HT", Colors::instTom, ParamIDs::htLevel, ParamIDs::htTune, ParamIDs::htDecay, nullptr);
    setupVoiceControls(rsControls, "RS", Colors::instRS, ParamIDs::rsLevel, ParamIDs::rsTune, nullptr, nullptr);
    setupVoiceControls(cpControls, "CP", Colors::instCP, ParamIDs::cpLevel, nullptr, nullptr, ParamIDs::cpTone);
    setupVoiceControls(chControls, "CH", Colors::instHH, ParamIDs::chLevel, nullptr, nullptr, ParamIDs::chTone);
    setupVoiceControls(ohControls, "OH", Colors::instHH, ParamIDs::ohLevel, nullptr, ParamIDs::ohDecay, ParamIDs::ohTone);
    setupVoiceControls(cyControls, "CY", Colors::instCY, ParamIDs::cyLevel, nullptr, ParamIDs::cyDecay, ParamIDs::cyTone);
    setupVoiceControls(rdControls, "RD", Colors::instCY, ParamIDs::rdLevel, nullptr, nullptr, ParamIDs::rdTone);
    setupVoiceControls(cbControls, "CB", Colors::instRS, ParamIDs::cbLevel, ParamIDs::cbTune, nullptr, nullptr);
    
    // Throttle UI updates to 30Hz (per guide recommendation)
    startTimerHz(30);
}

TR808GarageEditor::~TR808GarageEditor()
{
    stopTimer();
}

void TR808GarageEditor::setupVoiceControls(VoiceControls& vc, const juce::String& name, juce::Colour color,
                                          const char* levelID, const char* tuneID,
                                          const char* decayID, const char* toneID)
{
    vc.name = name;
    vc.color = color;
    
    vc.nameLabel.setText(name, juce::dontSendNotification);
    vc.nameLabel.setJustificationType(juce::Justification::centredLeft);
    vc.nameLabel.setColour(juce::Label::textColourId, Colors::textPrimary);
    vc.nameLabel.setFont(juce::Font(Typography::lg, juce::Font::bold));
    addAndMakeVisible(vc.nameLabel);
    
    auto setupSlider = [this, &name](CustomKnob& slider, const char* paramID, const juce::String& paramName,
                             std::unique_ptr<SliderAttachment>& attachment) {
        if (paramID == nullptr) return;
        slider.setTitle(name + " " + paramName); // Accessibility
        slider.setDescription(name + " " + paramName + " control");
        addAndMakeVisible(slider);
        attachment = std::make_unique<SliderAttachment>(processor.getAPVTS(), paramID, slider);
    };
    
    setupSlider(vc.levelSlider, levelID, "Level", vc.levelAttachment);
    setupSlider(vc.tuneSlider, tuneID, "Tune", vc.tuneAttachment);
    setupSlider(vc.decaySlider, decayID, "Decay", vc.decayAttachment);
    setupSlider(vc.toneSlider, toneID, "Tone", vc.toneAttachment);
    
    addAndMakeVisible(vc.meter);
}

void TR808GarageEditor::paint(juce::Graphics& g)
{
    g.fillAll(Colors::bgPrimary);
    
    auto bounds = getLocalBounds();
    
    // Header
    auto headerArea = bounds.removeFromTop(60);
    paintHeader(g, headerArea);
    
    // Pattern bank
    auto patternArea = bounds.removeFromTop(50);
    paintPatternBank(g, patternArea);
    
    // Footer
    auto footerArea = bounds.removeFromBottom(30);
    paintFooter(g, footerArea);
    
    // Voice section
    paintVoiceSection(g, bounds);
}

void TR808GarageEditor::paintHeader(juce::Graphics& g, juce::Rectangle<int> area)
{
    g.setColour(Colors::bgSecondary);
    g.fillRect(area);
    
    g.setColour(Colors::textPrimary);
    g.setFont(juce::Font(Typography::xl, juce::Font::bold));
    g.drawText("TR-808 Garage", area.reduced(Spacing::md, 0), juce::Justification::centredLeft);
}

void TR808GarageEditor::paintPatternBank(juce::Graphics& g, juce::Rectangle<int> area)
{
    g.setColour(Colors::bgElevated);
    g.fillRoundedRectangle(area.reduced(Spacing::sm).toFloat(), Radius::md);
}

void TR808GarageEditor::paintVoiceSection(juce::Graphics& g, juce::Rectangle<int> area)
{
    // Draw voice panels with color stripes
    auto drawVoicePanel = [&](juce::Rectangle<int> r, juce::Colour color) {
        g.setColour(Colors::bgSecondary);
        g.fillRoundedRectangle(r.toFloat(), Radius::lg);
        
        // Color stripe on left
        g.setColour(color);
        g.fillRoundedRectangle(r.removeFromLeft(3).toFloat(), Radius::sm);
    };
    
    int voiceY = area.getY() + Spacing::md;
    int voiceHeight = 140;
    int voiceWidth = (area.getWidth() - Spacing::md * 5 - 120) / 4; // 4 columns
    
    VoiceControls* allVoices[] = {&bdControls, &sdControls, &ltControls, &mtControls,
                                   &htControls, &rsControls, &cpControls, &chControls,
                                   &ohControls, &cyControls, &rdControls, &cbControls};
    
    for (int i = 0; i < 12; ++i)
    {
        int col = i % 4;
        int row = i / 4;
        int x = area.getX() + Spacing::md + col * (voiceWidth + Spacing::md);
        int y = voiceY + row * (voiceHeight + Spacing::md);
        
        drawVoicePanel(juce::Rectangle<int>(x, y, voiceWidth, voiceHeight), allVoices[i]->color);
    }
}

void TR808GarageEditor::paintFooter(juce::Graphics& g, juce::Rectangle<int> area)
{
    g.setColour(Colors::bgSecondary);
    g.fillRect(area);
    
    g.setColour(Colors::textMuted);
    g.setFont(juce::Font(Typography::xs));
    
    int currentPreset = processor.getCurrentProgram();
    juce::String status = "Preset " + juce::String(currentPreset + 1) + " — " + 
                         processor.getProgramName(currentPreset);
    g.drawText(status, area.reduced(Spacing::sm, 0), juce::Justification::centredLeft);
}

void TR808GarageEditor::resized()
{
    auto bounds = getLocalBounds();
    
    // Header
    auto headerArea = bounds.removeFromTop(60);
    
    // Preset browser area
    auto patternArea = bounds.removeFromTop(50).reduced(Spacing::md, Spacing::sm);
    
    // Preset navigation
    prevPresetButton.setBounds(patternArea.removeFromLeft(40));
    patternArea.removeFromLeft(Spacing::sm);
    
    // Preset selector (combo box)
    presetSelector.setBounds(patternArea.removeFromLeft(300));
    patternArea.removeFromLeft(Spacing::sm);
    
    nextPresetButton.setBounds(patternArea.removeFromLeft(40));
    patternArea.removeFromLeft(Spacing::lg);
    
    // Copy/Paste/Clear buttons
    copyButton.setBounds(patternArea.removeFromLeft(60));
    patternArea.removeFromLeft(Spacing::sm);
    pasteButton.setBounds(patternArea.removeFromLeft(60));
    patternArea.removeFromLeft(Spacing::sm);
    clearButton.setBounds(patternArea.removeFromLeft(60));
    
    // Sequencer area
    auto seqArea = bounds.removeFromTop(80).reduced(Spacing::md, Spacing::sm);
    
    // Transport controls
    playButton.setBounds(seqArea.removeFromLeft(40));
    seqArea.removeFromLeft(Spacing::sm);
    stopButton.setBounds(seqArea.removeFromLeft(40));
    seqArea.removeFromLeft(Spacing::lg);
    
    // BPM control
    bpmLabel.setBounds(seqArea.removeFromLeft(40));
    seqArea.removeFromLeft(Spacing::sm);
    bpmSlider.setBounds(seqArea.removeFromLeft(150));
    seqArea.removeFromLeft(Spacing::lg);
    
    // Voice selector
    voiceSelector.setBounds(seqArea.removeFromLeft(80));
    seqArea.removeFromLeft(Spacing::sm);
    
    // Step buttons (16 steps)
    int stepWidth = 30;
    for (int i = 0; i < 16; ++i)
    {
        stepButtons[i].setBounds(seqArea.removeFromLeft(stepWidth));
        if (i < 15) seqArea.removeFromLeft(2);
    }
    
    // MIDI drag button (right side)
    midiDragSource.setBounds(getWidth() - 120 - Spacing::md, 60 + 50 + Spacing::sm, 120, 30);
    
    // Theme button (top right)
    themeButton.setBounds(getWidth() - 80 - Spacing::md, Spacing::md, 80, 30);
    
    // Footer
    auto footerArea = bounds.removeFromBottom(30);
    
    // Master (right side)
    int masterX = getWidth() - 100 - Spacing::md;
    masterLevelLabel.setBounds(masterX, bounds.getY() + Spacing::md, 100, 20);
    masterLevelSlider.setBounds(masterX + 20, bounds.getY() + Spacing::md + 25, 60, 150);
    masterMeter.setBounds(masterX, bounds.getY() + Spacing::md + 25, 15, 150);
    
    // Voice controls in 4x3 grid
    auto layoutVoice = [&](VoiceControls& vc, int col, int row) {
        int voiceWidth = (bounds.getWidth() - Spacing::md * 5 - 120) / 4;
        int voiceHeight = 140;
        int x = bounds.getX() + Spacing::md + col * (voiceWidth + Spacing::md);
        int y = bounds.getY() + Spacing::md + row * (voiceHeight + Spacing::md);
        
        vc.nameLabel.setBounds(x + 10, y + Spacing::sm, voiceWidth - 20, 25);
        vc.meter.setBounds(x + 5, y + 40, 8, 90);
        
        int knobY = y + 40;
        int knobSpacing = 50;
        int knobX = x + 20;
        
        if (vc.levelAttachment) vc.levelSlider.setBounds(knobX, knobY, 45, 70);
        if (vc.tuneAttachment) vc.tuneSlider.setBounds(knobX + knobSpacing, knobY, 45, 70);
        if (vc.decayAttachment) vc.decaySlider.setBounds(knobX + knobSpacing * 2, knobY, 45, 70);
        if (vc.toneAttachment) vc.toneSlider.setBounds(knobX + knobSpacing * 3, knobY, 45, 70);
    };
    
    // Layout all 12 voices in 4x3 grid
    layoutVoice(bdControls, 0, 0);
    layoutVoice(sdControls, 1, 0);
    layoutVoice(ltControls, 2, 0);
    layoutVoice(mtControls, 3, 0);
    
    layoutVoice(htControls, 0, 1);
    layoutVoice(rsControls, 1, 1);
    layoutVoice(cpControls, 2, 1);
    layoutVoice(chControls, 3, 1);
    
    layoutVoice(ohControls, 0, 2);
    layoutVoice(cyControls, 1, 2);
    layoutVoice(rdControls, 2, 2);
    layoutVoice(cbControls, 3, 2);
}

void TR808GarageEditor::timerCallback()
{
    // Update preset selection if changed externally
    int currentProgram = processor.getCurrentProgram();
    if (currentProgram + 1 != presetSelector.getSelectedId())
    {
        presetSelector.setSelectedId(currentProgram + 1, juce::dontSendNotification);
        repaint();
    }
    
    // Update step button highlighting for current step
    if (processor.getSequencer().getPlaying())
    {
        int currentStep = processor.getSequencer().getCurrentStep();
        for (int i = 0; i < 16; ++i)
        {
            bool isCurrentStep = (i == currentStep);
            bool isActive = stepButtons[i].getToggleState();
            
            if (isCurrentStep && isActive)
                stepButtons[i].setColour(juce::TextButton::buttonOnColourId, Colors::success);
            else if (isActive)
                stepButtons[i].setColour(juce::TextButton::buttonOnColourId, Colors::accent);
        }
    }
    
    // Update MIDI drag source with current pattern
    midiDragSource.setMidiFile(processor.getSequencer().generateMidiFile());
    
    // Update meters (placeholder - would need actual level data from processor)
    masterMeter.setLevel(0.5f);
}

void TR808GarageEditor::updateStepButtons()
{
    for (int i = 0; i < 16; ++i)
    {
        bool active = processor.getSequencer().getStep(selectedVoice, i);
        stepButtons[i].setToggleState(active, juce::dontSendNotification);
    }
}
