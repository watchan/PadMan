/*
  ==============================================================================

    Pad.h
    Created: 26 Nov 2023 7:14:36pm
    Author:  watchan

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class Pad : public juce::TextButton
{
public:
    Pad(){};
    
    //setter
    //void setColour(juce::Colour newColour);
    void setOrgColour();
    void setCurrentColour(juce::Colour newColour);
    void setCurrentColourMidiMessage(juce::MidiMessage message);
    void setOrgColourMidiMessage();
    void returnOrgColour();
    void setNoteNumber(int noteNumber);
    void setSustainStatus(bool sustainStatus);
    
    
    //getter
    juce::MidiMessage getOrgColourMidiMessage();
    int getNoteNumber();
    int getDegree(int root);
    std::string getDegreeName(int root);
    bool getSustainStatus();
    
 

private:
   
    int noteNumber;
    bool sustainStatus;
    
    juce::Colour orgColour;
    juce::Colour currentColour;
    juce::MidiMessage orgColourMidi;
    juce::MidiMessage currentColourMidi;
    
};
