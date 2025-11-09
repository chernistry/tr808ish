#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_basics/juce_audio_basics.h>

class MidiDragSource : public juce::Component
{
public:
    MidiDragSource()
    {
        setSize(100, 30);
    }
    
    void setMidiFile(const juce::MidiFile& file)
    {
        midiFile = file;
    }
    
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(0xff3a3a3a));
        g.setColour(juce::Colour(0xffff6b35));
        g.drawRect(getLocalBounds(), 2);
        
        g.setFont(12.0f);
        g.drawText("⬇ Drag MIDI", getLocalBounds(), juce::Justification::centred);
    }
    
    void mouseDrag(const juce::MouseEvent& e) override
    {
        if (e.getDistanceFromDragStart() > 5 && !isDragging)
        {
            isDragging = true;
            
            // Create temporary MIDI file
            juce::File tempFile = juce::File::getSpecialLocation(juce::File::tempDirectory)
                                    .getChildFile("TR808_Pattern.mid");
            
            juce::FileOutputStream stream(tempFile);
            if (stream.openedOk())
            {
                midiFile.writeTo(stream);
                stream.flush();
            }
            
            // Start drag operation
            juce::StringArray files;
            files.add(tempFile.getFullPathName());
            
            auto* container = juce::DragAndDropContainer::findParentDragContainerFor(this);
            if (container != nullptr)
            {
                container->startDragging(juce::var(files), this);
            }
        }
    }
    
    void mouseUp(const juce::MouseEvent&) override
    {
        isDragging = false;
    }
    
private:
    juce::MidiFile midiFile;
    bool isDragging = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiDragSource)
};
