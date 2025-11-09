#include "MidiDragSource.h"

MidiDragSource::MidiDragSource()
{
    using namespace DesignTokens;
    
    setSize(100, 32);
    
    // Setup fallback buttons (initially hidden)
    saveButton.setButtonText("Save");
    saveButton.setColour(juce::TextButton::buttonColourId, Colors::bgElevated);
    saveButton.setColour(juce::TextButton::textColourOffId, Colors::textSecondary);
    saveButton.onClick = [this] { saveMidiFile(); };
    addChildComponent(saveButton);
    
    revealButton.setButtonText("Show");
    revealButton.setColour(juce::TextButton::buttonColourId, Colors::bgElevated);
    revealButton.setColour(juce::TextButton::textColourOffId, Colors::textSecondary);
    revealButton.onClick = [this] { revealMidiFile(); };
    addChildComponent(revealButton);
    
    // Help text
    setHelpText("Drag to DAW timeline or use Save/Show buttons");
}

MidiDragSource::~MidiDragSource()
{
    cleanupTempFiles();
}

void MidiDragSource::setMidiFile(const juce::MidiFile& file)
{
    midiFile = file;
}

void MidiDragSource::paint(juce::Graphics& g)
{
    using namespace DesignTokens;
    
    auto bounds = getLocalBounds();
    
    // Background
    g.setColour(isHovering ? Colors::bgElevated.brighter(0.1f) : Colors::bgElevated);
    g.fillRoundedRectangle(bounds.toFloat(), Radius::md);
    
    // Border
    g.setColour(isHovering ? Colors::accent : Colors::border);
    g.drawRoundedRectangle(bounds.toFloat().reduced(0.5f), Radius::md, isDragging ? 2.0f : 1.0f);
    
    // Icon and text
    if (!showFallbackButtons)
    {
        g.setColour(isHovering ? Colors::accent : Colors::textSecondary);
        g.setFont(juce::FontOptions(Typography::sm));
        
        // Draw drag icon
        auto iconArea = bounds.removeFromLeft(20);
        g.drawText("⬇", iconArea, juce::Justification::centred);
        
        // Draw text
        g.drawText("MIDI", bounds, juce::Justification::centred);
    }
}

void MidiDragSource::resized()
{
    using namespace DesignTokens;
    
    if (showFallbackButtons)
    {
        auto area = getLocalBounds().reduced(2);
        const int buttonWidth = (area.getWidth() - Spacing::xs) / 2;
        
        saveButton.setBounds(area.removeFromLeft(buttonWidth));
        area.removeFromLeft(Spacing::xs);
        revealButton.setBounds(area);
    }
}

void MidiDragSource::mouseDown(const juce::MouseEvent& e)
{
    dragStartPos = e.getPosition();
    
    // Right-click shows fallback options
    if (e.mods.isPopupMenu())
    {
        juce::PopupMenu menu;
        menu.addItem(1, "Save MIDI File...");
        menu.addItem(2, "Reveal Last Saved File", lastSavedFile.existsAsFile());
        
        menu.showMenuAsync(juce::PopupMenu::Options(), [this](int result) {
            if (result == 1) saveMidiFile();
            else if (result == 2) revealMidiFile();
        });
    }
}

void MidiDragSource::mouseDrag(const juce::MouseEvent& e)
{
    if (!isDragging && e.getDistanceFromDragStart() > 5)
    {
        isDragging = true;
        performDragAndDrop();
    }
}

void MidiDragSource::mouseUp(const juce::MouseEvent&)
{
    isDragging = false;
    repaint();
}

void MidiDragSource::mouseEnter(const juce::MouseEvent&)
{
    isHovering = true;
    repaint();
}

void MidiDragSource::mouseExit(const juce::MouseEvent&)
{
    isHovering = false;
    repaint();
}

void MidiDragSource::performDragAndDrop()
{
    // Create temporary MIDI file
    auto tempDir = juce::File::getSpecialLocation(juce::File::tempDirectory);
    auto timestamp = juce::Time::getCurrentTime().toMilliseconds();
    tempFile = tempDir.getChildFile("CR717_Pattern_" + juce::String(timestamp) + ".mid");
    
    // Write MIDI file in background thread to avoid blocking UI
    juce::Thread::launch([this, tempFile = this->tempFile]() mutable {
        juce::FileOutputStream stream(tempFile);
        if (stream.openedOk())
        {
            midiFile.writeTo(stream);
            stream.flush();
        }
    });
    
    // Small delay to ensure file is written
    juce::Thread::sleep(50);
    
    // Perform OS-level external drag & drop
    juce::StringArray files;
    files.add(tempFile.getFullPathName());
    
    // This performs actual OS-level drag-and-drop that DAWs can receive
    auto* container = juce::DragAndDropContainer::findParentDragContainerFor(this);
    if (container != nullptr)
    {
        container->performExternalDragDropOfFiles(files, false, this, [this]() {
            // Callback when drag completes
            isDragging = false;
            repaint();
            
            // Clean up temp file after a delay
            juce::Timer::callAfterDelay(5000, [this]() {
                cleanupTempFiles();
            });
        });
    }
    else
    {
        // Fallback: show save/reveal buttons if drag container not found
        showFallbackButtons = true;
        saveButton.setVisible(true);
        revealButton.setVisible(true);
        resized();
        repaint();
    }
}

void MidiDragSource::saveMidiFile()
{
    auto chooser = std::make_shared<juce::FileChooser>(
        "Save MIDI Pattern",
        juce::File::getSpecialLocation(juce::File::userDocumentsDirectory),
        "*.mid");
    
    auto flags = juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles;
    
    chooser->launchAsync(flags, [this, chooser](const juce::FileChooser& fc) {
        auto file = fc.getResult();
        if (file == juce::File{})
            return;
            
        juce::FileOutputStream stream(file);
        
        if (stream.openedOk())
        {
            midiFile.writeTo(stream);
            stream.flush();
            lastSavedFile = file;
            
            // Show success message
            juce::AlertWindow::showMessageBoxAsync(
                juce::AlertWindow::InfoIcon,
                "MIDI Saved",
                "Pattern saved to:\n" + file.getFullPathName(),
                "OK");
        }
        else
        {
            juce::AlertWindow::showMessageBoxAsync(
                juce::AlertWindow::WarningIcon,
                "Save Failed",
                "Could not save MIDI file.",
                "OK");
        }
    });
}

void MidiDragSource::revealMidiFile()
{
    if (lastSavedFile.existsAsFile())
    {
        lastSavedFile.revealToUser();
    }
    else if (tempFile.existsAsFile())
    {
        tempFile.revealToUser();
    }
}

void MidiDragSource::cleanupTempFiles()
{
    if (tempFile.existsAsFile())
    {
        tempFile.deleteFile();
    }
}
