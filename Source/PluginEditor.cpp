/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//Padの初期値
#define MIDI_INPUT_LIST 50
#define KEY_HEIGHT 80
#define PAD_ROW 8
#define PAD_COL 8
#define PAD_KEY 0
#define PAD_START_NOTENUMBER 48
#define PAD_OCTAVE 0

#define MARGIN 10
#define BUTTON_SIZE 50

//==============================================================================
PadManAudioProcessorEditor::PadManAudioProcessorEditor (PadManAudioProcessor& p)
: AudioProcessorEditor (&p), audioProcessor (p),keyboardComponent(keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard),startTime (juce::Time::getMillisecondCounterHiRes() * 0.001)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    //padDevice.init(PAD_ROW, PAD_COL, PAD_KEY, PAD_START_NOTENUMBER, PAD_OCTAVE);
    padDevice.init(PAD_ROW, PAD_COL, PAD_KEY, PAD_START_NOTENUMBER, PAD_OCTAVE);
    
    /* LaunchPadMk3のライティング制御
    //LaunchPadProMk3を探す
    auto midiOutputs = juce::MidiOutput::getAvailableDevices();
    
    for(auto output:midiOutputs)
    {
        if(isLaunchPadProMk3(output.name))
        {
            std::cout << "Found LaunchPadProMk3.\n";
            //outDeviceにLaunchPadProMk3をセットする
            audioProcessor.outDevice = juce::MidiOutput::openDevice(output.identifier);
            //プログラマモードに遷移する
            audioProcessor.outDevice->sendMessageNow(LaunchPadMk3::Mode::programmer);
            foundLaunchPadProMk3 = true;
        }
    }
    
    
    if(!foundLaunchPadProMk3)
    {
        std::cout << "Coudn't find LaunchPadProMk3.\n";
    }
    
    */
    
    
    //ウィンドウサイズ
   setSize ( padDevice.getMargin() + (padDevice.getMargin() + padDevice.getButtonSize()) * padDevice.getCol(), (padDevice.getMargin() + padDevice.getButtonSize()) * padDevice.getRow() + KEY_HEIGHT + MIDI_INPUT_LIST + padDevice.getMargin() * 2);
    

    
    addAndMakeVisible (midiInputListLabel);
    midiInputListLabel.setText ("MIDI Input:", juce::dontSendNotification);
    midiInputListLabel.attachToComponent (&midiInputList, true);

    addAndMakeVisible (midiInputList);
    midiInputList.setTextWhenNoChoicesAvailable ("No MIDI Inputs Enabled");
    auto midiInputs = juce::MidiInput::getAvailableDevices();

    juce::StringArray midiInputNames;

    for (auto input : midiInputs)
        midiInputNames.add (input.name);

    midiInputList.addItemList (midiInputNames, 1);
    midiInputList.onChange = [this] { setMidiInput (midiInputList.getSelectedItemIndex()); };

    // find the first enabled device and use that by default
    for (auto input : midiInputs)
    {
        if (deviceManager.isMidiInputDeviceEnabled (input.identifier))
        {
            setMidiInput (midiInputs.indexOf (input));
            break;
        }
    }

    // if no enabled devices were found just use the first one in the list
    if (midiInputList.getSelectedId() == 0)
        setMidiInput (0);

    addAndMakeVisible (keyboardComponent);
    keyboardState.addListener (this);

    addAndMakeVisible (midiMessagesBox);
    midiMessagesBox.setMultiLine (true);
    midiMessagesBox.setReturnKeyStartsNewLine (true);
    midiMessagesBox.setReadOnly (true);
    midiMessagesBox.setScrollbarsShown (true);
    midiMessagesBox.setCaretVisible (false);
    midiMessagesBox.setPopupMenuEnabled (true);
    midiMessagesBox.setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x32ffffff));
    midiMessagesBox.setColour (juce::TextEditor::outlineColourId, juce::Colour (0x1c000000));
    midiMessagesBox.setColour (juce::TextEditor::shadowColourId, juce::Colour (0x16000000));


    
  
    //Editorに各PadをaddAndMakeVisibleする
    for(int i=0,padnum=0; i < padDevice.getRow() ; i++)
    {
        for(int j=0 ; j < padDevice.getCol() ; j++)
        {
            addAndMakeVisible(padDevice.pads.at(padnum));
            padnum++;
        }
    
    }
 
}

PadManAudioProcessorEditor::~PadManAudioProcessorEditor()
{
}

//==============================================================================
void PadManAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    
    /*
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
     
     */
}

void PadManAudioProcessorEditor::resized()
{
    
    auto area = getLocalBounds();

//    tb.setSize(50, 50);
//    //tb.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::limegreen);
//    tb               .setBounds (area.removeFromTop(80).reduced(8));
    midiInputList    .setBounds (area.removeFromTop (36).removeFromRight (getWidth() - 150).reduced (8));
    keyboardComponent.setBounds (area.removeFromTop (80).reduced(8));
    //midiMessagesBox  .setBounds (area.reduced (8));
    
   
    //Padのインデックス
    int padnum=0;
    
    //Padの情報に合わせて配置を決める
    auto bounds = getLocalBounds();
    int bottom = bounds.getHeight() -  padDevice.getButtonSize() -  padDevice.getMargin();
    int left = bounds.getX() +  padDevice.getMargin();
        
    //Padを配置する
    for(int i=0 ; i <  padDevice.getRow() ; i++)
    {
       
        
        for(int j=0 ; j <  padDevice.getCol() ; j++)
        {
           // pad.pads.at(cellnum).setButtonText(to_string(cellnum));
            padDevice.pads.at(padnum).setBounds(left +  padDevice.getMargin() * j +  padDevice.getButtonSize() * j, bottom - ( padDevice.getMargin() * i +  padDevice.getButtonSize() * i),  padDevice.getButtonSize() ,  padDevice.getButtonSize());
            
         
            
            int offset;
            
            if( padDevice.getCol() > 12)
            {
                offset =  padDevice.getFinger() - 1;
            }else{
                offset =  padDevice.getFinger() -  padDevice.getCol();
            }
            switch((padnum + offset * i +  padDevice.getStartNote())%12){
                case 0://Root
                  
                    padDevice.pads.at(padnum).setCurrentColour(juce::Colours::violet);
                    padDevice.pads.at(padnum).setDefaultColour();
                    
                    //padDevice.pads.at(padnum).setCurrentColourMidiMessage({0xF0 ,0x00,0x20, 0x29, 0x02, 0x0E, 0x03, 0x00,  j + 10 * i + 11, 0x3B, 0xF7});
                    //padDevice.pads.at(padnum).setDefaultColour();
                    //padDevice.pads.at(padnum).setDefaultColourMidiMessage();

                    //audioProcessor.outDevice->sendMessageNow({0xF0 ,0x00,0x20, 0x29, 0x02, 0x0E, 0x03, 0x00,  j + 10 * i + 11, 0x3B, 0xF7});
                    
                    break;
                case 1://m2
                case 3://m3
                case 6://a4,d5
                case 8://m6
                case 10://m7
                    padDevice.pads.at(padnum).setCurrentColour(juce::Colours::darkslategrey);
                    padDevice.pads.at(padnum).setDefaultColour();
 
                    //padDevice.pads.at(padnum).setCurrentColourMidiMessage({0xF0 ,0x00,0x20, 0x29, 0x02, 0x0E, 0x03, 0x00,   j + 10 * i + 11, 0, 0xF7});
                    //padDevice.pads.at(padnum).setOrgColour();
                    //padDevice.pads.at(padnum).setDefaultColourMidiMessage();
                    
                    //audioProcessor.outDevice->sendMessageNow({0xF0 ,0x00,0x20, 0x29, 0x02, 0x0E, 0x03, 0x00,   j + 10 * i + 11, 0, 0xF7});

                   break;
                case 2://M2
                case 4://M3
                case 5://P4
                case 7://P5
                case 9://M6
                case 11://M7
                    padDevice.pads.at(padnum).setCurrentColour(juce::Colours::lightskyblue);
                    padDevice.pads.at(padnum).setDefaultColour();
 
                    //padDevice.pads.at(padnum).setCurrentColourMidiMessage({0xF0 ,0x00,0x20, 0x29, 0x02, 0x0E, 0x03, 0x00,   j + 10 * i + 11, 0x01, 0xF7});
                    //padDevice.pads.at(padnum).setOrgColour();
                    //padDevice.pads.at(padnum).setDefaultColourMidiMessage();
                    //audioProcessor.outDevice->sendMessageNow({0xF0 ,0x00,0x20, 0x29, 0x02, 0x0E, 0x03, 0x00,   j + 10 * i + 11, 0x01, 0xF7});
                     break;
                    
                default :
                    break;
                
            }
            
            padnum++;
               
            
            
        }

    }
    
    modifyPad();
     
   
    
}
