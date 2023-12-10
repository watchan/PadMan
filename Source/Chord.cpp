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



void Chord::showChord()
{
    std::cout << "showChord:";
    for(int i=0 ; i<notes.size(); i++)
    {
        std::cout << notes.at(i) << " ";
    }
    
    std::cout << "\n";
}

