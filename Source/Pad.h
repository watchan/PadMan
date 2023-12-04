/*
  ==============================================================================

    Pad.h
    Created: 27 Nov 2023 10:03:08pm
    Author:  watchan

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

using namespace std;

class Pad : public juce::TextButton
{
public:
    Pad(){};
    
    //setter
    void setDefaultColour();
    void setCurrentColour(juce::Colour newColour);
    void setCurrentColourMidiMessage(juce::MidiMessage message);
    void setDefaultColourMidiMessage();
    void returnDefaultColour();
    void setNoteNumber(int noteNumber);
    void setPadText(string text);
    void setSustainStatus(bool sustainStatus);
    
    
    //getter
    juce::MidiMessage getDefaultColourMidiMessage();
    int getNoteNumber();
    int getDegree(int root);
    std::string getDegreeName(int root);
    bool getSustainStatus();

 

private:
   
    int noteNumber;
    bool sustainStatus;
    
    juce::Colour defaultColour;
    juce::Colour currentColour;
    juce::MidiMessage orgColourMidi;
    juce::MidiMessage currentColourMidi;
    
};

