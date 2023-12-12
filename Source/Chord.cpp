/*
  ==============================================================================

    Chord.cpp
    Created: 10 Dec 2023 9:37:43pm
    Author:  watchan

  ==============================================================================
*/
#include "Chord.h"


//Getter
/// Get Root Number of this Chord
int Chord::getRootNoteNumber()
{
    return this->rootNoteNumber;
}

int Chord::getNumberOfNotes()
{
    return this->notes.size();
}

vector<int> Chord::getNotes()
{
    return this->notes;
}

//Setter
void Chord::setRootNoteNumber(int rootNoteNumber)
{
    this->rootNoteNumber = rootNoteNumber;
}


// Add and Remove note
int Chord::getIndexOfNote(vector<int> chord, int noteNumber)
{
    auto itr = std::find(chord.begin(), chord.end(), noteNumber);
    size_t index = std::distance ( chord.begin(), itr);
    if(index != chord.size())
    {
        return index;
    }
    else{
        return -1;
    }
    
}

/// You can add the new note to this chord.
/// - Parameter noteNumber: Note number you want to add to this chord.
void Chord::addNoteToChord(int noteNumber){
    
    if(getIndexOfNote(this->notes, noteNumber)== -1)
    {
        //含まれていなければnoteNumberをchordに格納
        this->notes.push_back(noteNumber);
        
      
         
        //低い音から順に並べる
        sort(this->notes.begin(), this->notes.end());
        
       
        //最低音が更新されたらrootNoteNumberを更新する
        if(noteNumber < rootNoteNumber)
        {
            rootNoteNumber = noteNumber;
        }
         
        
        
    }
}

void Chord::removeNoteNumberFromChord(int noteNumber)
{
    int index = getIndexOfNote(this->notes, noteNumber);
    //chordの中にnoteNumberが存在することが確認できたら
    if(index != -1)
    {
        //chordの中の要素が1つ以上あれば
        if(this->notes.size()>1)
        {
            
            /*
            //削除対象がrootNoteNumberだった場合
            if(this->notes.at(index)== rootNoteNumber)
            {
                //2番めに低い音をrootNoteNumberとして更新 ：理由 rootNoteNumberが削除されルートがわからなくため
                rootNoteNumber = this->notes.at(index+1);
            }
             
             */
            
        }
        
        //見つかったnoteNumberをchordからeraseする
        this->notes.erase(std::cbegin(this->notes) + index);

    }
    
}

std::string getNoteName(int midiNote) {
    static const std::vector<std::string> noteNames = {
        "C", "C#", "D", "D#", "E", "F",
        "F#", "G", "G#", "A", "A#", "B"
    };
    return noteNames[midiNote % 12];
}

const std::vector<std::pair<std::string, std::set<int>>> chordFormulas = {
    {"maj13",  {0, 4, 7, 11, 2, 5}},  // 6音
    {"m11",    {0, 3, 7, 10, 5}},    // 5音
    {"maj9",   {0, 4, 7, 11, 2}},    // 5音
    {"9",      {0, 4, 7, 10, 2}},    // 5音
    {"min9",   {0, 3, 7, 10, 2}},    // 5音
    {"7(#9)",  {0, 4, 7, 10, 1}},    // 5音
    {"7(b13)", {0, 4, 7, 10, 8}},    // 5音
    {"9(b5)",  {0, 4, 7, 10, 6}},    // 5音
    {"9",      {0, 4,  10, 2}},    // 4音
    {"7",      {0, 4, 7, 10}},       // 4音
    {"maj7",   {0, 4, 7, 11}},       // 4音
    {"dim7",   {0, 3, 6, 9}},        // 4音
    {"min6",   {0, 3, 7, 9}},       // 4音
    {"min7",   {0, 3, 7, 10}},       // 4音
    {"m7b5",   {0, 3, 6, 10}},       // 4音
    {"aug7",   {0, 4, 8, 10}},       // 4音
    {"sus4",   {0, 5, 7}},           // 3音
    {"sus2",   {0, 2, 7}},           // 3音
    {"aug",    {0, 4, 8}},           // 3音
    {"dim",    {0, 3, 6}},           // 3音
    {"maj",    {0, 4, 7}},           // 3音
    {"min",    {0, 3, 7}}            // 3音
};

std::string getInversionName(int bassDegree, const std::set<int>& formula) {
    std::vector<int> degrees(formula.begin(), formula.end());
    std::sort(degrees.begin(), degrees.end());

    for (size_t i = 0; i < degrees.size(); ++i) {
        if (degrees[i] == bassDegree) {
            if (i == 0) return ""; // Root position
            else if (i == 1) return " (1st Inversion)";
            else if (i == 2) return " (2nd Inversion)";
            else if (i == 3) return " (3rd Inversion)";
        }
    }
    return " (Inversion)";
}

std::string Chord::getChordName() {
    const std::vector<int>& midiNotesInput = notes;
    if (midiNotesInput.empty()) return "-";

    std::set<int> noteSet(midiNotesInput.begin(), midiNotesInput.end());
    std::vector<int> midiNotes(noteSet.begin(), noteSet.end());

    for (int rootNote : midiNotes) {
        std::set<int> intervals;
        for (int note : midiNotes) {
            int interval = (note - rootNote + 120) % 12;
            intervals.insert(interval);
        }

        for (const auto& [name, formula] : chordFormulas) {
            if (intervals == formula) {
                int bassNote = *std::min_element(midiNotes.begin(), midiNotes.end());
                int bassDegree = (bassNote - rootNote + 120) % 12;
                std::string inversion = getInversionName(bassDegree, formula);

                this->setRootNoteNumber(rootNote);
                return getNoteName(rootNote) + name + inversion;
            }
        }
    }

    return "-";
}

void Chord::showChord()
{
    
    std::cout << "showChord:";
    for(int i=0 ; i<notes.size(); i++)
    {
        std::cout << notes.at(i) << " ";
    }
    
    std::cout << "Chord:" + this->getChordName()+"\n";
    
    std::cout << "degree:";
    for(int i=0 ; i<notes.size(); i++)
    {
        std::cout << notes.at(i) %12 << " ";
    }
    
}

