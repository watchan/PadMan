/*
  ==============================================================================

    PadDevice.cpp
    Created: 27 Nov 2023 10:07:15pm
    Author:  watchan

  ==============================================================================
*/

#include "PadDevice.h"


/// Initialize Pad Device
/// - Parameters:
///   - row: Number of Pad row
///   - col: Number of Pad col
///   - transpose:Transpose
///   - startNoteNumber: MIDI Note Number of Pad Index 0
///   - octave: Octave
void PadDevice::init(int row, int col, int transpose, int startNoteNumber, int octave)
{
    setRow(row);
    setCol(col);
    
    
    for(int notenum = startNoteNumber + transpose * octave , padnum = 0 ; padnum < row * col ; padnum++)
    {
        
        // 2行目以降
        if(padnum / 8 > 0)
        {
            //1-3列の場合
            if(padnum % 8 < 3)
            {
                int tmpNoteNum = notenum - (3 - padnum % 8);
                
                pads.at(padnum).setNoteNumber(tmpNoteNum);
                //cells.at(cellnum).setButtonText(to_string(tmpNoteNum));
                
            }else
            {
                pads.at(padnum).setNoteNumber(notenum);
                //cells.at(cellnum).setButtonText(to_string(notenum));
                notenum++;
            }
        }else
        {
            pads.at(padnum).setNoteNumber(notenum);
            //cells.at(cellnum).setButtonText(to_string(notenum));
            notenum++;
        }
    }
    
}

//getter
int PadDevice::getOctave()
{
    return this->octave;
}

int PadDevice::getStartNote()
{
    return this->startNote;

}

bool PadDevice::getSustainState()
{
    return this->sustainState;
}

int PadDevice::getFinger()
{
    return this->finger;
}

int PadDevice::getMargin()
{
    return this->margin;
}

int PadDevice::getButtonSize()
{
    return this->buttonsize;
}

int PadDevice::getRow()
{
    return this->row;
}

int PadDevice::getCol()
{
    return this->col;
}


int PadDevice::getSize()
{
    return this->row * this->col;
}

bool PadDevice::getSustainPedalState()
{
    if(this->sustainState)
    {
        return true;
    }else{
        return false;
    }
}



//setter
void PadDevice::setOctave(int octave)
{
    this->octave = octave;
}

void PadDevice::setStartNote(int startNote)
{
    this->startNote = startNote;
}

void PadDevice::setSustainState(bool sustainState)
{
    this->sustainState = sustainState;
}

void PadDevice::setFinger(int finger)
{
    this->finger = finger;
}

void PadDevice::setMargin(int margin)
{
    this->margin = margin;
}

void PadDevice::setRow(int row)
{
    this->row = row;
}

void PadDevice::setCol(int col)
{
    this->col = col;
}
