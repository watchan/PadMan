/*
 ==============================================================================
 
 Chord.cpp
 Created: 10 Dec 2023 9:37:43pm
 Author:  watchan
 
 ==============================================================================
 */
#include "Chord.h"


// Getter =============================================//
/// Return Root Number of this Chord
int Chord::getRootNoteNumber()
{
    return this->rootNoteNumber;
}

/// Return Number of Notes. Return the size of vector<int>
int Chord::getNumberOfNotes()
{
    return this->notes.size();
}

/// Return vector<int> notes to access each note.
vector<int> Chord::getNotes()
{
    return this->notes;
}


/// Return the number from the first note.
/// - Parameters:
///   - notes: From vector<int> notes
///   - noteNumber: Note number you want to find.
int Chord::getIndexOfNote(vector<int> notes, int noteNumber)
{
    auto itr = std::find(notes.begin(), notes.end(), noteNumber);
    size_t index = std::distance ( notes.begin(), itr);
    if(index != notes.size())
    {
        return index;
    }
    else{
        return -1;
    }
    
}

// Setter =============================================//

/// Set root note for decide the degree or chord name
/// - Parameter rootNoteNumber: The lowest note in all notes.
void Chord::setRootNoteNumber(int rootNoteNumber)
{
    this->rootNoteNumber = rootNoteNumber;
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

/// Remove Note From Chord
/// - Parameter noteNumber: If this chord has noteNumber, remove the note from this chord.
void Chord::removeNoteNumberFromChord(int noteNumber)
{
    int index = getIndexOfNote(this->notes, noteNumber);
    //chordの中にnoteNumberが存在することが確認できたら
    if(index != -1)
    {
        //chordの中の要素が1つ以上あれば
        if(this->notes.size()>1)
        {
            
            //削除対象がrootNoteNumberだった場合
            if(this->notes.at(index)== rootNoteNumber)
            {
                //2番めに低い音をrootNoteNumberとして更新 ：理由 rootNoteNumberが削除されルートがわからなくため
                rootNoteNumber = this->notes.at(index+1);
            }
            
        }
        
        //見つかったnoteNumberをchordからeraseする
        this->notes.erase(std::cbegin(this->notes) + index);
        
    }
    
}



/// Show all notes for debug
void Chord::showChord()
{
    std::cout << "showChord:";
    for(int i=0 ; i<notes.size(); i++)
    {
        std::cout << notes.at(i) << " ";
    }
    
    std::cout << "\n";
}

