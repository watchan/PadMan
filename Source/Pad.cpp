/*
  ==============================================================================

    Pad.cpp
    Created: 27 Nov 2023 10:03:03pm
    Author:  watchan

  ==============================================================================
*/

#include "Pad.h"


void Pad::setCurrentColour(juce::Colour newColour)
{
    this->currentColour = newColour;
}

void Pad::setDefaultColour()
{
    this->defaultColour = this->currentColour;
}

void Pad::setCurrentColourMidiMessage(juce::MidiMessage message)
{
    this->currentColourMidi = message;
}

void Pad::setDefaultColourMidiMessage()
{
    this->orgColourMidi = this->currentColourMidi;
}

void Pad::returnDefaultColour()
{
     setColour(juce::TextButton::buttonColourId,this->defaultColour);
}

void Pad::setNoteNumber(int noteNumber)
{
    this->noteNumber = noteNumber;
}

void Pad::setSustainStatus(bool sustainStatus)
{
    this->sustainStatus = sustainStatus;
}


//getter


bool Pad::getSustainStatus()
{
    return this->sustainStatus;
}

juce::MidiMessage Pad::getDefaultColourMidiMessage()
{
    return this->orgColourMidi;
}

int Pad::getNoteNumber()
{
    return this->noteNumber;
}

int Pad::getDegree(int root)
{
    int diff = this->noteNumber - root;
    int degree = diff % 12;
    
    
    if(degree < 0)
    {
        degree += 12;
    }
    
    return degree;
}


std::string Pad::getDegreeName(int root)
{
    std::string degName;
    int diff = noteNumber - root;
    
    int degree = diff % 12;
    
    if(degree < 0 ){
        degree += 12;
    }
    
     if(degree==0){
         degName = "R";
    }else if(degree==1){
        if(root + 12 < noteNumber){
            degName="b9";
        }else{
            degName="m2";
        }
    }else if(degree==2){
        
        if(root + 12 < noteNumber){
            degName="9";
        }else{
            degName="M2";
        }
    }else if(degree==3){
        if(root + 12 < noteNumber){
            degName="m3";//degName="#9";
        }else{
            degName="m3";
        }
    }else if(degree==4){
        degName="M3";
    }else if(degree==5){
        
        if(root + 12 < noteNumber){
            degName="11";
        }else{
            degName="P4";
        }
    }else if(degree==6){
        if(root + 12 < noteNumber){
           degName="#11";
        }else{
            degName="a4/d5";
        }
    }else if(degree==7){
            degName="P5";
    }else if(degree==8){
        if(root + 12 < noteNumber){

            degName="b13";
        }else{
            degName="m6";
        }
    }else if(degree==9){
        if(root + 12 < noteNumber){
          degName="13";
        }else{
            degName="M6";
        }
    }else if(degree==10){
        if(root + 12 < noteNumber){
           degName="#13";
        }else{
            degName="7";
        }
        
    }else if(degree==11){
        degName="M7";
    }
    
    return degName;
}

