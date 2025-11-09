#include "TopBar.h"

TopBar::TopBar(juce::AudioProcessorValueTreeState& apvts_,
               std::function<void()> onPlay,
               std::function<void()> onStop,
               std::function<void()> onPresetBrowser,
               std::function<void(int)> onBankSelect,
               std::function<void()> onPatternCopy,
               std::function<void()> onPatternPaste,
               std::function<void()> onPatternClear)
    : apvts(apvts_),
      playCallback(std::move(onPlay)),
      stopCallback(std::move(onStop)),
      presetBrowserCallback(std::move(onPresetBrowser)),
      bankSelectCallback(std::move(onBankSelect)),
      patternCopyCallback(std::move(onPatternCopy)),
      patternPasteCallback(std::move(onPatternPaste)),
      patternClearCallback(std::move(onPatternClear))
{
    using namespace DesignTokens;
    
    // Transport buttons
    playButton.setButtonText("▶");
    playButton.setColour(juce::TextButton::buttonColourId, Colors::success);
    playButton.setColour(juce::TextButton::textColourOffId, Colors::textPrimary);
    playButton.onClick = [this] {
        if (playCallback) playCallback();
        isPlaying = true;
        updatePlayButtonState();
    };
    addAndMakeVisible(playButton);
    
    stopButton.setButtonText("■");
    stopButton.setColour(juce::TextButton::buttonColourId, Colors::danger);
    stopButton.setColour(juce::TextButton::textColourOffId, Colors::textPrimary);
    stopButton.onClick = [this] {
        if (stopCallback) stopCallback();
        isPlaying = false;
        updatePlayButtonState();
    };
    addAndMakeVisible(stopButton);
    
    // Host sync indicator
    hostSyncLabel.setText("HOST", juce::dontSendNotification);
    hostSyncLabel.setFont(juce::FontOptions(Typography::sm));
    hostSyncLabel.setColour(juce::Label::textColourId, Colors::textMuted);
    hostSyncLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(hostSyncLabel);
    
    // BPM control
    bpmLabel.setText("BPM", juce::dontSendNotification);
    bpmLabel.setFont(juce::FontOptions(Typography::sm));
    bpmLabel.setColour(juce::Label::textColourId, Colors::textSecondary);
    addAndMakeVisible(bpmLabel);
    
    bpmSlider.setRange(60.0, 240.0, 1.0);
    bpmSlider.setValue(120.0);
    bpmSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    bpmSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    bpmSlider.setColour(juce::Slider::trackColourId, Colors::accent);
    bpmSlider.setColour(juce::Slider::backgroundColourId, Colors::bgTertiary);
    bpmSlider.onValueChange = [this] { updateBPMDisplay(); };
    addAndMakeVisible(bpmSlider);
    
    bpmValueLabel.setText("120", juce::dontSendNotification);
    bpmValueLabel.setFont(juce::FontOptions(Typography::md).withStyle("Bold"));
    bpmValueLabel.setColour(juce::Label::textColourId, Colors::textPrimary);
    bpmValueLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(bpmValueLabel);
    
    // Swing control
    swingLabel.setText("SWING", juce::dontSendNotification);
    swingLabel.setFont(juce::FontOptions(Typography::sm));
    swingLabel.setColour(juce::Label::textColourId, Colors::textSecondary);
    addAndMakeVisible(swingLabel);
    
    swingSlider.setRange(0.0, 75.0, 1.0);
    swingSlider.setValue(0.0);
    swingSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    swingSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    swingSlider.setColour(juce::Slider::trackColourId, Colors::accent);
    swingSlider.setColour(juce::Slider::backgroundColourId, Colors::bgTertiary);
    swingSlider.onValueChange = [this] {
        swingValueLabel.setText(juce::String((int)swingSlider.getValue()) + "%", 
                               juce::dontSendNotification);
    };
    addAndMakeVisible(swingSlider);
    
    swingValueLabel.setText("0%", juce::dontSendNotification);
    swingValueLabel.setFont(juce::FontOptions(Typography::md));
    swingValueLabel.setColour(juce::Label::textColourId, Colors::textPrimary);
    swingValueLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(swingValueLabel);
    
    // Preset browser button
    presetBrowserButton.setButtonText("PRESETS");
    presetBrowserButton.setColour(juce::TextButton::buttonColourId, Colors::bgElevated);
    presetBrowserButton.setColour(juce::TextButton::textColourOffId, Colors::textPrimary);
    presetBrowserButton.onClick = [this] {
        if (presetBrowserCallback) presetBrowserCallback();
    };
    addAndMakeVisible(presetBrowserButton);
    
    // Pattern bank buttons (A-H)
    const char* bankLabels[] = {"A", "B", "C", "D", "E", "F", "G", "H"};
    for (int i = 0; i < 8; ++i)
    {
        bankButtons[i].setButtonText(bankLabels[i]);
        bankButtons[i].setColour(juce::TextButton::buttonColourId, Colors::bgTertiary);
        bankButtons[i].setColour(juce::TextButton::buttonOnColourId, Colors::accent);
        bankButtons[i].setColour(juce::TextButton::textColourOffId, Colors::textSecondary);
        bankButtons[i].setColour(juce::TextButton::textColourOnId, Colors::textPrimary);
        bankButtons[i].setClickingTogglesState(true);
        bankButtons[i].setRadioGroupId(1001);
        bankButtons[i].onClick = [this, i] {
            currentBank = i;
            if (bankSelectCallback) bankSelectCallback(i);
        };
        addAndMakeVisible(bankButtons[i]);
    }
    bankButtons[0].setToggleState(true, juce::dontSendNotification);
    
    // Pattern operation buttons
    copyButton.setButtonText("COPY");
    copyButton.setColour(juce::TextButton::buttonColourId, Colors::bgElevated);
    copyButton.setColour(juce::TextButton::textColourOffId, Colors::textSecondary);
    copyButton.onClick = [this] {
        if (patternCopyCallback) patternCopyCallback();
    };
    addAndMakeVisible(copyButton);
    
    pasteButton.setButtonText("PASTE");
    pasteButton.setColour(juce::TextButton::buttonColourId, Colors::bgElevated);
    pasteButton.setColour(juce::TextButton::textColourOffId, Colors::textSecondary);
    pasteButton.onClick = [this] {
        if (patternPasteCallback) patternPasteCallback();
    };
    addAndMakeVisible(pasteButton);
    
    clearButton.setButtonText("CLEAR");
    clearButton.setColour(juce::TextButton::buttonColourId, Colors::bgElevated);
    clearButton.setColour(juce::TextButton::textColourOffId, Colors::textSecondary);
    clearButton.onClick = [this] {
        if (patternClearCallback) patternClearCallback();
    };
    addAndMakeVisible(clearButton);
    
    // MIDI drag source
    midiDragSource = std::make_unique<MidiDragSource>();
    addAndMakeVisible(*midiDragSource);
    
    // Start timer for UI updates (30 Hz)
    startTimerHz(30);
}

TopBar::~TopBar()
{
    stopTimer();
}

void TopBar::paint(juce::Graphics& g)
{
    using namespace DesignTokens;
    
    g.fillAll(Colors::bgSecondary);
    
    // Draw separator line at bottom
    g.setColour(Colors::border);
    g.drawLine(0.0f, (float)getHeight(), (float)getWidth(), (float)getHeight(), 1.0f);
    
    // Draw section separators
    const int sectionX[] = {200, 450, 650, 900};
    for (int x : sectionX)
    {
        if (x < getWidth())
        {
            g.setColour(Colors::border.withAlpha(0.3f));
            g.drawLine((float)x, Spacing::md, (float)x, (float)getHeight() - Spacing::md, 1.0f);
        }
    }
}

void TopBar::resized()
{
    using namespace DesignTokens;
    
    auto area = getLocalBounds().reduced(Spacing::md);
    const int buttonHeight = 32;
    const int smallButtonWidth = 60;
    const int mediumButtonWidth = 80;
    
    // Transport section (left)
    auto transportArea = area.removeFromLeft(180);
    auto transportRow = transportArea.removeFromTop(buttonHeight);
    playButton.setBounds(transportRow.removeFromLeft(smallButtonWidth));
    transportRow.removeFromLeft(Spacing::sm);
    stopButton.setBounds(transportRow.removeFromLeft(smallButtonWidth));
    
    area.removeFromLeft(Spacing::lg);
    
    // Host sync indicator
    auto hostSyncArea = area.removeFromLeft(60);
    hostSyncLabel.setBounds(hostSyncArea.removeFromTop(buttonHeight));
    
    area.removeFromLeft(Spacing::md);
    
    // BPM section
    auto bpmArea = area.removeFromLeft(200);
    auto bpmLabelArea = bpmArea.removeFromTop(16);
    bpmLabel.setBounds(bpmLabelArea);
    auto bpmControlArea = bpmArea.removeFromTop(buttonHeight);
    bpmSlider.setBounds(bpmControlArea.removeFromLeft(120));
    bpmControlArea.removeFromLeft(Spacing::sm);
    bpmValueLabel.setBounds(bpmControlArea);
    
    area.removeFromLeft(Spacing::lg);
    
    // Swing section
    auto swingArea = area.removeFromLeft(180);
    auto swingLabelArea = swingArea.removeFromTop(16);
    swingLabel.setBounds(swingLabelArea);
    auto swingControlArea = swingArea.removeFromTop(buttonHeight);
    swingSlider.setBounds(swingControlArea.removeFromLeft(100));
    swingControlArea.removeFromLeft(Spacing::sm);
    swingValueLabel.setBounds(swingControlArea);
    
    area.removeFromLeft(Spacing::lg);
    
    // Preset browser
    auto presetArea = area.removeFromLeft(mediumButtonWidth);
    presetBrowserButton.setBounds(presetArea.removeFromTop(buttonHeight));
    
    area.removeFromLeft(Spacing::lg);
    
    // Pattern bank buttons (A-H)
    auto bankArea = area.removeFromLeft(320);
    auto bankRow = bankArea.removeFromTop(buttonHeight);
    const int bankButtonWidth = 36;
    for (int i = 0; i < 8; ++i)
    {
        bankButtons[i].setBounds(bankRow.removeFromLeft(bankButtonWidth));
        if (i < 7) bankRow.removeFromLeft(4);
    }
    
    area.removeFromLeft(Spacing::lg);
    
    // Pattern operations
    auto opsArea = area.removeFromLeft(240);
    auto opsRow = opsArea.removeFromTop(buttonHeight);
    copyButton.setBounds(opsRow.removeFromLeft(70));
    opsRow.removeFromLeft(Spacing::sm);
    pasteButton.setBounds(opsRow.removeFromLeft(70));
    opsRow.removeFromLeft(Spacing::sm);
    clearButton.setBounds(opsRow.removeFromLeft(70));
    
    // MIDI drag source (right side)
    if (area.getWidth() > 80)
    {
        area.removeFromLeft(Spacing::lg);
        auto midiArea = area.removeFromRight(80);
        midiDragSource->setBounds(midiArea.removeFromTop(buttonHeight));
    }
}

void TopBar::timerCallback()
{
    updateHostSyncIndicator();
}

void TopBar::setHostSyncActive(bool active)
{
    hostSyncActive = active;
    useHostBPM = active;
    updateHostSyncIndicator();
    updateBPMDisplay();
}

void TopBar::setHostBPM(double bpm)
{
    hostBPM = bpm;
    if (useHostBPM)
    {
        updateBPMDisplay();
    }
}

void TopBar::setIsPlaying(bool playing)
{
    isPlaying = playing;
    updatePlayButtonState();
}

void TopBar::setCurrentBank(int bank)
{
    if (bank >= 0 && bank < 8)
    {
        currentBank = bank;
        bankButtons[bank].setToggleState(true, juce::dontSendNotification);
    }
}

void TopBar::setSwing(float swing)
{
    swingSlider.setValue(swing * 100.0, juce::dontSendNotification);
    swingValueLabel.setText(juce::String((int)(swing * 100.0)) + "%", juce::dontSendNotification);
}

float TopBar::getSwing() const
{
    return (float)swingSlider.getValue() / 100.0f;
}

void TopBar::updatePlayButtonState()
{
    using namespace DesignTokens;
    
    if (isPlaying)
    {
        playButton.setColour(juce::TextButton::buttonColourId, Colors::success.brighter(0.2f));
        stopButton.setColour(juce::TextButton::buttonColourId, Colors::danger.darker(0.3f));
    }
    else
    {
        playButton.setColour(juce::TextButton::buttonColourId, Colors::success.darker(0.3f));
        stopButton.setColour(juce::TextButton::buttonColourId, Colors::danger);
    }
}

void TopBar::updateBPMDisplay()
{
    double displayBPM = useHostBPM ? hostBPM : bpmSlider.getValue();
    bpmValueLabel.setText(juce::String((int)displayBPM), juce::dontSendNotification);
    
    // Disable slider when using host BPM
    bpmSlider.setEnabled(!useHostBPM);
    bpmSlider.setAlpha(useHostBPM ? 0.5f : 1.0f);
}

void TopBar::updateHostSyncIndicator()
{
    using namespace DesignTokens;
    
    if (hostSyncActive)
    {
        hostSyncLabel.setColour(juce::Label::textColourId, Colors::success);
        hostSyncLabel.setColour(juce::Label::backgroundColourId, Colors::success.withAlpha(0.2f));
    }
    else
    {
        hostSyncLabel.setColour(juce::Label::textColourId, Colors::textMuted);
        hostSyncLabel.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    }
}
