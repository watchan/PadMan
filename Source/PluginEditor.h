/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

using namespace std;

//==============================================================================
/**
*/
class PadManAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                    private juce::MidiInputCallback,
                                    private juce::MidiKeyboardStateListener
{
public:
    PadManAudioProcessorEditor (PadManAudioProcessor&);
    ~PadManAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PadManAudioProcessor& audioProcessor;

    static juce::String getMidiMessageDescription (const juce::MidiMessage& m)
    {
        if (m.isNoteOn())           return "Note on "          + juce::MidiMessage::getMidiNoteName (m.getNoteNumber(), true, true, 3);
        if (m.isNoteOff())          return "Note off "         + juce::MidiMessage::getMidiNoteName (m.getNoteNumber(), true, true, 3);
        if (m.isProgramChange())    return "Program change "   + juce::String (m.getProgramChangeNumber());
        if (m.isPitchWheel())       return "Pitch wheel "      + juce::String (m.getPitchWheelValue());
        if (m.isAftertouch())       return "After touch "      + juce::MidiMessage::getMidiNoteName (m.getNoteNumber(), true, true, 3) +  ": " + juce::String (m.getAfterTouchValue());
        if (m.isChannelPressure())  return "Channel pressure " + juce::String (m.getChannelPressureValue());
        if (m.isAllNotesOff())      return "All notes off";
        if (m.isAllSoundOff())      return "All sound off";
        if (m.isMetaEvent())        return "Meta event";

        if (m.isController())
        {
            juce::String name (juce::MidiMessage::getControllerName (m.getControllerNumber()));

            if (name.isEmpty())
                name = "[" + juce::String (m.getControllerNumber()) + "]";

            return "Controller " + name + ": " + juce::String (m.getControllerValue());
        }

        return juce::String::toHexString (m.getRawData(), m.getRawDataSize());
    }

   

    /** Starts listening to a MIDI input device, enabling it if necessary. */
    void setMidiInput (int index)
    {
        auto list = juce::MidiInput::getAvailableDevices();

        deviceManager.removeMidiInputDeviceCallback(list[lastInputIndex].identifier, this);

        auto newInput = list[index];

        if (! deviceManager.isMidiInputDeviceEnabled (newInput.identifier))
            deviceManager.setMidiInputDeviceEnabled (newInput.identifier, true);

        deviceManager.addMidiInputDeviceCallback (newInput.identifier, this);
        midiInputList.setSelectedId (index + 1, juce::dontSendNotification);

        lastInputIndex = index;
    }

    // These methods handle callbacks from the midi device + on-screen keyboard..
    void handleIncomingMidiMessage (juce::MidiInput* source, const juce::MidiMessage& message) override
    {
        const juce::ScopedValueSetter<bool> scopedInputFlag (isAddingFromMidiInput, true);
        keyboardState.processNextMidiEvent (message);
        postMessageToList (message, source->getName());
       
    }

    void handleNoteOn (juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override
    {
      
        if (! isAddingFromMidiInput)
        {
            auto m = juce::MidiMessage::noteOn (midiChannel, midiNoteNumber, velocity);
            handleMidiMessage(m, "Software Keyboard");
        }

    }

    void handleNoteOff (juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float /*velocity*/) override
    {
        if (! isAddingFromMidiInput)
        {
            auto m = juce::MidiMessage::noteOff (midiChannel, midiNoteNumber);
            handleMidiMessage(m, "Software Keyboard");
        }
    }

    // This is used to dispach an incoming message to the message thread
    class IncomingMessageCallback   : public juce::CallbackMessage
    {
    public:
        IncomingMessageCallback (PadManAudioProcessorEditor* o, const juce::MidiMessage& m, const juce::String& s)
           : owner (o), message (m), source (s)
        {}

        void messageCallback() override
        {
            if (owner != nullptr)
                owner->handleMidiMessage (message, source);
        }

        Component::SafePointer<PadManAudioProcessorEditor> owner;
        juce::MidiMessage message;
        juce::String source;
    };

    void postMessageToList (const juce::MidiMessage& message, const juce::String& source)
    {
        (new IncomingMessageCallback (this, message, source))->post();
    }

    
    void handleMidiMessage (const juce::MidiMessage& message, const juce::String& source)
    {
        auto time = message.getTimeStamp() - startTime;
        
        auto hours   = ((int) (time / 3600.0)) % 24;
        auto minutes = ((int) (time / 60.0)) % 60;
        auto seconds = ((int) time) % 60;
        auto millis  = ((int) (time * 1000.0)) % 1000;
        
        auto timecode = juce::String::formatted ("%02d:%02d:%02d.%03d",
                                                 hours,
                                                 minutes,
                                                 seconds,
                                                 millis);
        
        auto description = getMidiMessageDescription (message);
        
        juce::String midiMessageString (timecode + "  -  " + description + " (" + source + ")"); // [7]
        //logMessage (midiMessageString);
        //DBG(midiMessageString);
        
        
        //Handling MIDI message
            if(message.isNoteOn())
            {
                addNoteNumberToChord(chord, message.getNoteNumber());
          
            }
            
            if(message.isNoteOff())
            {
                removeNoteNumberFromChord(chord, message.getNoteNumber());
            }
                
            if(message.isSustainPedalOn())
            {
                //isSustainPedalOn = true;
                padDevice.setSustainState(true);
            }
        
            if(message.isSustainPedalOff())
            {
                padDevice.setSustainState(false);

            }
            
            DBG(message.getDescription());
            
            modifyPad();

        
            if(chord.size()==0)
            {
                rootNoteNumber = 128;
            }
            
            
            showChord(chord);
            //DBG(recognizeChord(chord));
            
        
        
    }
    
    
    
    //Find the noteNumber from chord vector
    int findNoteNumber(vector<int> chord, int noteNumber)
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
    
    std::string recognizeChord(vector<int> chord)
    {
        vector<int> degree;
        
        for(int i=0 ; i < chord.size() ; i++)
        {
            int tmpDegree = chord.at(i) % 12;
            
            if(findNoteNumber(degree, tmpDegree)== -1 )
            {
                degree.push_back(tmpDegree);
            }
            
        }
        
        sort(degree.begin(),degree.end());
        
        for(int i=0 ; i < degree.size() ; i++)
        {
           // std::cout << degree.at(i) << ",";
        }
        
        //std::cout << "\n";
        
        std::string chordName = "";
        
        if(degree.size()==1)
        {
            switch(degree.at(0))
            {
                case 0:
                    chordName += "C";
                    return chordName;
                    break;
                case 1:
                    return "C#";
                    break;
                case 2:
                    return "D";
                    break;
                case 3:
                    return "D#";
                    break;
                case 4:
                    return "E";
                    break;
                case 5:
                    return "F";
                    break;
                case 6:
                    return "F#";
                    break;
                case 7:
                    return "G";
                    break;
                case 8:
                    return "G#";
                    break;
                case 9:
                    return "A";
                    break;
                case 10:
                    return "A#";
                    break;
                case 11:
                    return "B";
                    break;
 
                default:
                    return "X";
                    break;
                    
            }
        }
        
        return "Y";
            
       
        
    }
    
    
    void addNoteNumberToChord(vector<int> &chord, int noteNumber){
    
        if(findNoteNumber(chord, noteNumber)== -1)
        {
            //含まれていなければnoteNumberをchordに格納
            chord.push_back(noteNumber);
            
            //低い音から順に並べる
            sort(chord.begin(), chord.end());
            
            //最低音が更新されたらrootNoteNumberを更新する
            if(noteNumber < rootNoteNumber)
            {
                rootNoteNumber = noteNumber;
            }
            
        }
    }
    
    void removeNoteNumberFromChord(vector<int> &chord, int noteNumber)
    {
        int index = findNoteNumber(chord, noteNumber);
        //chordの中にnoteNumberが存在することが確認できたら
        if(index != -1)
        {
            //chordの中の要素が1つ以上あれば
            if(chord.size()>1)
            {
                
                //削除対象がrootNoteNumberだった場合
                if(chord.at(index)== rootNoteNumber)
                {
                    //2番めに低い音をrootNoteNumberとして更新 ：理由 rootNoteNumberが削除されルートがわからなくため
                    rootNoteNumber = chord.at(index+1);
                }
                
            }
            
            //見つかったnoteNumberをchordからeraseする
            chord.erase(std::cbegin(chord) + index);

        }
        
    }
    
    
    
    /// Modify pad lighting following the MIDI input
    void modifyPad()
    {
      
        for(int i=0 ; i < padDevice.getSize() ; i++)
        {
            padDevice.pads.at(i).returnDefaultColour();
            padDevice.pads.at(i).setButtonText("");
            
            /*
             if(!padDevice.getSustainPedalState())
             {
             padDevice.pads.at(i).returnDefaultColour();
             padDevice.pads.at(i).setButtonText("");
             //outDevice->sendMessageNow(padDevice.pads.at(i).getDefaultColourMidiMessage());
             
             }
             */
           
            //コード色判定
            for(int j = 0 ; j < chord.size() ; j++)
            {
                
              
                if(padDevice.pads.at(i).getNoteNumber() == chord.at(j))
                {
                    
                  
                    int degree = padDevice.pads.at(i).getDegree(rootNoteNumber);
                    string padText;
                     
                     //度数判定
                    switch(degree)
                    {
                            
                        case 0://root
                            colour = juce::Colours::red;
                           
                            //outDevice->sendMessageNow({0xF0 ,0x00,0x20, 0x29, 0x02, 0x0E, 0x03, 0x00,  i/8 * 10 + 11 + i%8, 0x48, 0xF7});
                            break;

                        case 1: //Minor
                        case 3:
                        case 8:
                        case 10:
                            colour = juce::Colours::mediumvioletred;
                            //outDevice->sendMessageNow({0xF0 ,0x00,0x20, 0x29, 0x02, 0x0E, 0x03, 0x00,  i/8 * 10 + 11 + i%8, 0x51, 0xF7});
                            break;
                            
                        case 2:
                        case 4: //Major
                        case 9:
                        case 11:
                            colour = juce::Colours::steelblue;
                            //outDevice->sendMessageNow({0xF0 ,0x00,0x20, 0x29, 0x02, 0x0E, 0x03, 0x00,  i/8 * 10 + 11 + i%8, 0x42, 0xF7});
                            break;
                            
                        case 6: //Diminished
                            colour = juce::Colours::blueviolet;
                            //outDevice->sendMessageNow({0xF0 ,0x00,0x20, 0x29, 0x02, 0x0E, 0x03, 0x00,  i/8 * 10 + 11 + i%8, 0x50, 0xF7});
                            break;
                            
                        case 5: // Perfect
                        case 7:
                            //outDevice->sendMessageNow({0xF0 ,0x00,0x20, 0x29, 0x02, 0x0E, 0x03, 0x00,  i/8 * 10 + 11 + i%8, 0x65, 0xF7});
                            colour = juce::Colours::green;
                            break;
                        default :
                            colour = juce::Colours::dimgrey;
                            break;
                            
       
                            
                            // colour = juce::Colours::lime;
                            // padDevice.pads.at(i).setColour(juce::TextButton::ColourIds::buttonColourId, colour);
                    }
                    
                    //度数ごとの色を反映
                    padDevice.pads.at(i).setCurrentColour(colour);
                    
                    //Padに度数をセット
                    padDevice.pads.at(i).setPadText(padDevice.pads.at(i).getDegreeName(rootNoteNumber));
                    
            
                     
                }
            }
        }
    }
    
    

    void showChord(vector<int> chord)
    {
        std::cout << "showChord:";
        for(int i=0 ; i<chord.size(); i++)
        {
            std::cout << chord.at(i) << " ";
        }
        
        std::cout << "\n";
    }
    


    
    //==============================================================================
    juce::AudioDeviceManager deviceManager;           // [1]
    juce::ComboBox midiInputList;                     // [2]
    juce::Label midiInputListLabel;
    int lastInputIndex = 0;                           // [3]
    bool isAddingFromMidiInput = false;               // [4]

    juce::MidiKeyboardState keyboardState;            // [5]
    juce::MidiKeyboardComponent keyboardComponent;    // [6]

    juce::TextEditor midiMessagesBox;
    double startTime;
    
    PadDevice padDevice;
    juce::Colour colour;
    
    bool isSustainPedalOn;
    
    //Input MIDI note vector
    vector<int> chord;
    int rootNoteNumber = 128;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PadManAudioProcessorEditor)
    
    
};
