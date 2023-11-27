/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"


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

    void logMessage (const juce::String& m)
    {
        midiMessagesBox.moveCaretToEnd();
        midiMessagesBox.insertTextAtCaret (m + juce::newLine);
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
        DBG("handleIncomingMidiMessage");
    }

    void handleNoteOn (juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override
    {
      
        if (! isAddingFromMidiInput)
        {
            auto m = juce::MidiMessage::noteOn (midiChannel, midiNoteNumber, velocity);
            m.setTimeStamp (juce::Time::getMillisecondCounterHiRes() * 0.001);
            postMessageToList (m, "On-Screen Keyboard");
            
      
        }
        
    
  
    }

    void handleNoteOff (juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float /*velocity*/) override
    {
        if (! isAddingFromMidiInput)
        {
            auto m = juce::MidiMessage::noteOff (midiChannel, midiNoteNumber);
            m.setTimeStamp (juce::Time::getMillisecondCounterHiRes() * 0.001);
            postMessageToList (m, "On-Screen Keyboard");
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
                owner->addMessageToList (message, source);
        }

        Component::SafePointer<PadManAudioProcessorEditor> owner;
        juce::MidiMessage message;
        juce::String source;
    };

    void postMessageToList (const juce::MidiMessage& message, const juce::String& source)
    {
        (new IncomingMessageCallback (this, message, source))->post();
    }

    void addMessageToList (const juce::MidiMessage& message, const juce::String& source)
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
        logMessage (midiMessageString);
        DBG(midiMessageString);
      
        
    }
    
    
    
    
    void modifyPadDevice()
    {
       
        for(int i=0 ; i < padDevice.getSize() ; i++)
        {
           
           /*
            if(!padDevice.getSustainPedalState())
            {
                padDevice.pads.at(i).returnOrgColour();
                padDevice.pads.at(i).setButtonText("");
                //outDevice->sendMessageNow(padDevice.pads.at(i).getOrgColourMidiMessage());
       
            }
            
            //コード色判定
            for(int j = 0 ; j < chord.size() ; j++)
            {
                if(padDevice.pads.at(i).getNoteNumber() == chord.at(j))
                {
                    Colour colour;
                    int degree = padDevice.pads.at(i).getDegree(rootNoteNumber);
                     

                    //度数ごとの色設定
                    switch(degree)
                    {
                           
                        case 0://root
                            colour = juce::Colours::red;
                            padDevice.pads.at(i).setColour(juce::TextButton::buttonColourId, colour);
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
                            
                    }
             */
                    
                    //padDevice.pads.at(i).setButtonText(padDevice.pads.at(i).getDegreeName(rootNoteNumber));
            
            
            
        }
        

    
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
    
    juce::TextButton tb;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PadManAudioProcessorEditor)
    
    
};
