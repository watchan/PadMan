/*
  ==============================================================================

    Chord.h
    Created: 10 Dec 2023 9:06:07pm
    Author:  watchan

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

using namespace std;

class Chord
{
public:
    Chord(){};
    
    //Setter
    int getRootNoteNumber();
    
    //Getter
    void setRootNoteNumber(int rootNoteNumber);
    
    //Find note number from chord
    int getIndexOfNote(vector<int> chord, int noteNumber);
    
    //Recognize and Get the chord name
    string recognizeChord(vector<int> chord);
    
    //Add note number to chord
    void addNoteToChord(int noteNumber);
    
    //Remove note from chord
    void removeNoteNumberFromChord(int noteNumber);
    
    //Print chord information to DBA
    void showChord();
    
     
private:
    
    vector<int> chord;
    int rootNoteNumber = 128;
    string chordName;
    string chordDegreeName;
    
    
};
