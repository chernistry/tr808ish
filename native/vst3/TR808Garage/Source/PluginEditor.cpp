#include "PluginEditor.h"

using namespace DesignTokens;

TR808GarageEditor::TR808GarageEditor(TR808GarageProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setSize(1100, 620);
    setResizable(true, true);
    setResizeLimits(1100, 620, 1650, 930);
    
    // Pattern bank buttons
    const char* patternNames[] = {"A", "B", "C", "D", "E", "F", "G", "H"};
    for (int i = 0; i < 8; ++i)
    {
        patternButtons[i].setButtonText(patternNames[i]);
        patternButtons[i].setColour(juce::TextButton::buttonColourId, Colors::bgSecondary);
        patternButtons[i].setColour(juce::TextButton::buttonOnColourId, Colors::accent);
        patternButtons[i].setColour(juce::TextButton::textColourOffId, Colors::textSecondary);
        patternButtons[i].setColour(juce::TextButton::textColourOnId, Colors::textPrimary);
        patternButtons[i].onClick = [this, i] {
            selectedPattern = i;
            processor.setCurrentProgram(i);
            repaint();
        };
        addAndMakeVisible(patternButtons[i]);
    }
    
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
    
    // Master level
    masterLevelLabel.setText("Master", juce::dontSendNotification);
    masterLevelLabel.setJustificationType(juce::Justification::centred);
    masterLevelLabel.setColour(juce::Label::textColourId, Colors::textPrimary);
    masterLevelLabel.setFont(juce::Font(Typography::md));
    addAndMakeVisible(masterLevelLabel);
    
    masterLevelSlider.setSliderStyle(juce::Slider::LinearVertical);
    masterLevelSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    masterLevelSlider.setColour(juce::Slider::trackColourId, Colors::bgTertiary);
    masterLevelSlider.setColour(juce::Slider::thumbColourId, Colors::accent);
    masterLevelSlider.setColour(juce::Slider::textBoxTextColourId, Colors::textSecondary);
    addAndMakeVisible(masterLevelSlider);
    masterLevelAttachment = std::make_unique<SliderAttachment>(
        processor.getAPVTS(), ParamIDs::masterLevel, masterLevelSlider);
    
    // Voice controls with colors
    setupVoiceControls(bdControls, "BD", Colors::instBD, ParamIDs::bdLevel, ParamIDs::bdTune, 
                      ParamIDs::bdDecay, ParamIDs::bdTone);
    setupVoiceControls(sdControls, "SD", Colors::instSD, ParamIDs::sdLevel, ParamIDs::sdTune, 
                      ParamIDs::sdDecay, ParamIDs::sdSnappy);
    setupVoiceControls(chControls, "CH", Colors::instHH, ParamIDs::chLevel, nullptr, 
                      nullptr, ParamIDs::chTone);
    setupVoiceControls(ohControls, "OH", Colors::instHH, ParamIDs::ohLevel, nullptr, 
                      ParamIDs::ohDecay, ParamIDs::ohTone);
    setupVoiceControls(cpControls, "CP", Colors::instCP, ParamIDs::cpLevel, nullptr, 
                      nullptr, ParamIDs::cpTone);
    setupVoiceControls(rsControls, "RS", Colors::instRS, ParamIDs::rsLevel, ParamIDs::rsTune, 
                      nullptr, nullptr);
    
    startTimerHz(60);
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
    
    auto setupSlider = [this](juce::Slider& slider, const char* paramID, 
                             std::unique_ptr<SliderAttachment>& attachment) {
        if (paramID == nullptr) return;
        
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 16);
        slider.setColour(juce::Slider::rotarySliderFillColourId, Colors::accent);
        slider.setColour(juce::Slider::thumbColourId, Colors::textPrimary);
        slider.setColour(juce::Slider::textBoxTextColourId, Colors::textSecondary);
        slider.setColour(juce::Slider::trackColourId, Colors::bgTertiary);
        addAndMakeVisible(slider);
        attachment = std::make_unique<SliderAttachment>(processor.getAPVTS(), paramID, slider);
    };
    
    setupSlider(vc.levelSlider, levelID, vc.levelAttachment);
    setupSlider(vc.tuneSlider, tuneID, vc.tuneAttachment);
    setupSlider(vc.decaySlider, decayID, vc.decayAttachment);
    setupSlider(vc.toneSlider, toneID, vc.toneAttachment);
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
    int voiceWidth = (area.getWidth() - Spacing::md * 4) / 3;
    
    for (int i = 0; i < 6; ++i)
    {
        int col = i % 3;
        int row = i / 3;
        int x = area.getX() + Spacing::md + col * (voiceWidth + Spacing::md);
        int y = voiceY + row * (voiceHeight + Spacing::md);
        
        juce::Colour color = Colors::accent;
        if (i == 0) color = Colors::instBD;
        else if (i == 1) color = Colors::instSD;
        else if (i == 2) color = Colors::instHH;
        else if (i == 3) color = Colors::instHH;
        else if (i == 4) color = Colors::instCP;
        else if (i == 5) color = Colors::instRS;
        
        drawVoicePanel(juce::Rectangle<int>(x, y, voiceWidth, voiceHeight), color);
    }
}

void TR808GarageEditor::paintFooter(juce::Graphics& g, juce::Rectangle<int> area)
{
    g.setColour(Colors::bgSecondary);
    g.fillRect(area);
    
    g.setColour(Colors::textMuted);
    g.setFont(juce::Font(Typography::xs));
    
    juce::String status = "Pattern " + juce::String(char('A' + selectedPattern)) + " — " + 
                         processor.getProgramName(selectedPattern);
    g.drawText(status, area.reduced(Spacing::sm, 0), juce::Justification::centredLeft);
}

void TR808GarageEditor::resized()
{
    auto bounds = getLocalBounds();
    
    // Header
    auto headerArea = bounds.removeFromTop(60);
    
    // Pattern bank
    auto patternArea = bounds.removeFromTop(50).reduced(Spacing::md, Spacing::sm);
    
    int btnWidth = 40;
    int btnSpacing = Spacing::sm;
    
    for (int i = 0; i < 8; ++i)
    {
        patternButtons[i].setBounds(patternArea.removeFromLeft(btnWidth));
        patternArea.removeFromLeft(btnSpacing);
    }
    
    patternArea.removeFromLeft(Spacing::lg);
    copyButton.setBounds(patternArea.removeFromLeft(60));
    patternArea.removeFromLeft(btnSpacing);
    pasteButton.setBounds(patternArea.removeFromLeft(60));
    patternArea.removeFromLeft(btnSpacing);
    clearButton.setBounds(patternArea.removeFromLeft(60));
    
    // Theme button (top right)
    themeButton.setBounds(getWidth() - 80 - Spacing::md, Spacing::md, 80, 30);
    
    // Footer
    auto footerArea = bounds.removeFromBottom(30);
    
    // Master (right side)
    int masterX = getWidth() - 100 - Spacing::md;
    masterLevelLabel.setBounds(masterX, bounds.getY() + Spacing::md, 100, 20);
    masterLevelSlider.setBounds(masterX, bounds.getY() + Spacing::md + 25, 100, 150);
    
    // Voice controls
    auto layoutVoice = [&](VoiceControls& vc, int col, int row) {
        int voiceWidth = (bounds.getWidth() - Spacing::md * 4 - 120) / 3;
        int voiceHeight = 140;
        int x = bounds.getX() + Spacing::md + col * (voiceWidth + Spacing::md);
        int y = bounds.getY() + Spacing::md + row * (voiceHeight + Spacing::md);
        
        vc.nameLabel.setBounds(x + 10, y + Spacing::sm, voiceWidth - 20, 25);
        
        int knobY = y + 40;
        int knobSpacing = 55;
        int knobX = x + 10;
        
        if (vc.levelAttachment) vc.levelSlider.setBounds(knobX, knobY, 50, 70);
        if (vc.tuneAttachment) vc.tuneSlider.setBounds(knobX + knobSpacing, knobY, 50, 70);
        if (vc.decayAttachment) vc.decaySlider.setBounds(knobX + knobSpacing * 2, knobY, 50, 70);
        if (vc.toneAttachment) vc.toneSlider.setBounds(knobX + knobSpacing * 3, knobY, 50, 70);
    };
    
    layoutVoice(bdControls, 0, 0);
    layoutVoice(sdControls, 1, 0);
    layoutVoice(chControls, 2, 0);
    layoutVoice(ohControls, 0, 1);
    layoutVoice(cpControls, 1, 1);
    layoutVoice(rsControls, 2, 1);
}

void TR808GarageEditor::timerCallback()
{
    // Update pattern selection if changed externally
    int currentProgram = processor.getCurrentProgram();
    if (currentProgram != selectedPattern)
    {
        selectedPattern = currentProgram;
        repaint();
    }
}
