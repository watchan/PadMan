/*
  ==============================================================================

    PadDevice.h
    Created: 27 Nov 2023 10:07:24pm
    Author:  watchan

  ==============================================================================
*/

#pragma once
#include "Pad.h"
#define MAX_PADNUM 200
class PadDevice
{
public:
    

    //Pad1つ1つをPadとして扱う
    std::vector<Pad> pads{MAX_PADNUM};
  
    //init
    void init(int row, int col, int key, int startNoteNumber, int octave);
    
    //getter
    int getOctave();
    int getStartNote();
    bool getSustainState();
    int getFinger();
    int getMargin();
    int getButtonSize();
    int getRow();
    int getCol();
    int getSize();
    bool getSustainPedalState();

    
    
    //setter
    void setOctave(int octave);
    void setRoteNoteNumber(int rootNoteNumber);
    void setStartNote(int startNote);
    void setSustainState(bool sustainState);
    void setFinger(int finger);
    void setMargin(int margin);
    void setButtonSize(int buttonsize);
    void setRow(int row);
    void setCol(int col);
    
    
private:
    
    int octave = 4;
    int transepose = 0;
    int startNote = 0;
    bool sustainState = false;
    int finger = 5;
    int margin = 10;
    int buttonsize = 50;
    int row = 8;
    int col = 8;
    
    
};
