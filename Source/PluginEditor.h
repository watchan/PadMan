/*
 ==============================================================================
 
 This file contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Chord.h"

using namespace std;

//==============================================================================
/**
 */
class PadManAudioProcessorEditor  : public AudioProcessorEditor,
private MidiInputCallback,
private MidiKeyboardStateListener
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
            chord.addNoteToChord(message.getNoteNumber());
            
        }
        
        if(message.isNoteOff())
        {
            chord.removeNoteNumberFromChord(message.getNoteNumber());
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
        
    
        if(chord.getNumberOfNotes()==0)
        {
            chord.setRootNoteNumber(128);
        }
        
       chord.showChord();
        
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
            for(int j = 0 ; j < chord.getNumberOfNotes() ; j++)
            {
      
                if(padDevice.pads.at(i).getNoteNumber() == chord.getNotes().at(j))
                {
                    
                    
                    int degree = padDevice.pads.at(i).getDegree(chord.getRootNoteNumber());
                    string padText;
                    
                    //度数判定
                    switch(degree)
                    {
                            
                        case 0://root
                            colour = juce::Colours::red;
                            
                            //outDevice->sendMessageNow({0xF0 ,0x00,0x20, 0x29, 0x02, 0x0E, 0x03, 0x00,  i/8 * 10 + 11 + i%8, 0x48, 0xF7});
                            break;
                            
                            //Minor
                        case 1: //m2
                            colour = juce::Colours::deepskyblue;
                            break;
                        case 3: //m3
                            colour = juce::Colours::dodgerblue;
                            break;
                        case 8: //m6
                            colour = juce::Colours::royalblue;
                            break;
                            
                        case 10://7
                            colour = juce::Colours::blue;
                            //outDevice->sendMessageNow({0xF0 ,0x00,0x20, 0x29, 0x02, 0x0E, 0x03, 0x00,  i/8 * 10 + 11 + i%8, 0x51, 0xF7});
                            break;
                            
                            //Major
                        case 2: //m2
                            colour = juce::Colours::darksalmon;
                            break;
                            
                        case 4: //M3
                            colour = juce::Colours::orangered;
                            break;
                        case 9: //M6
                            colour = juce::Colours::indianred;
                            break;
                            
                        case 11://M7
                            colour = juce::Colours::darkorange;
                            //outDevice->sendMessageNow({0xF0 ,0x00,0x20, 0x29, 0x02, 0x0E, 0x03, 0x00,  i/8 * 10 + 11 + i%8, 0x42, 0xF7});
                            break;
                            
                        case 6: //Diminished
                            colour = juce::Colours::blueviolet;
                            //outDevice->sendMessageNow({0xF0 ,0x00,0x20, 0x29, 0x02, 0x0E, 0x03, 0x00,  i/8 * 10 + 11 + i%8, 0x50, 0xF7});
                            break;
                            
                            //Perfect
                        case 5: // P4
                            colour = juce::Colours::mediumseagreen;
                            break;
                        case 7: // P5
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
                    padDevice.pads.at(i).setPadText(padDevice.pads.at(i).getDegreeName(chord.getRootNoteNumber()));
                    
                    
                    
                }
            }
        }
    }
    
    
    
    //==============================================================================
    AudioDeviceManager deviceManager;           // [1]
    ComboBox midiInputList;                     // [2]
    Label midiInputListLabel;
    int lastInputIndex = 0;                           // [3]
    bool isAddingFromMidiInput = false;               // [4]
    
    MidiKeyboardState keyboardState;            // [5]
    MidiKeyboardComponent keyboardComponent;    // [6]
    
    TextEditor midiMessagesBox;
    double startTime;
    
    PadDevice padDevice;
    Colour colour;
    
    bool isSustainPedalOn;
    
    //Chord
    Chord chord;

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PadManAudioProcessorEditor)
    
    
    
    
};
