/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//Padの初期値
#define KEY_HEIGHT 50
#define PAD_ROW 8
#define PAD_COL 8
#define PAD_KEY 0
#define PAD_START_NOTENUMBER 48
#define PAD_OCTAVE 0

#define MARGIN 10
#define BUTTON_SIZE 50

//==============================================================================
PadManAudioProcessorEditor::PadManAudioProcessorEditor (PadManAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    audioProcessor.padDevice.init(PAD_ROW, PAD_COL, PAD_KEY, PAD_START_NOTENUMBER, PAD_OCTAVE);
    
    
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
    setSize ( audioProcessor.padDevice.getMargin() + (audioProcessor.padDevice.getMargin() + audioProcessor.padDevice.getButtonSize()) * audioProcessor.padDevice.getCol(), (audioProcessor.padDevice.getMargin() + audioProcessor.padDevice.getButtonSize()) * audioProcessor.padDevice.getRow() + KEY_HEIGHT + audioProcessor.padDevice.getMargin() * 2);
    
    //EditorにPadをaddAndMakeVisibleする
    int padnum = 0;
    for(int i=0 ; i < audioProcessor.padDevice.getRow() ; i++)
    {
        for(int j=0 ; j < audioProcessor.padDevice.getCol() ; j++)
        {
            addAndMakeVisible(audioProcessor.padDevice.pads.at(padnum));
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
    //Padのインデックス
    int padnum=0;
    
    //Padの情報に合わせて配置を決める
    auto bounds = getLocalBounds();
    int bottom = bounds.getHeight() -  audioProcessor.padDevice.getButtonSize() -  audioProcessor.padDevice.getMargin();
    int left = bounds.getX() +  audioProcessor.padDevice.getMargin();
        
    //Padを配置する
    for(int i=0 ; i <  audioProcessor.padDevice.getRow() ; i++)
    {
        for(int j=0 ; j <  audioProcessor.padDevice.getCol() ; j++)
        {
           // pad.pads.at(cellnum).setButtonText(to_string(cellnum));
            audioProcessor.padDevice.pads.at(padnum).setBounds(left +  audioProcessor.padDevice.getMargin() * j +  audioProcessor.padDevice.getButtonSize() * j, bottom - ( audioProcessor.padDevice.getMargin() * i +  audioProcessor.padDevice.getButtonSize() * i),  audioProcessor.padDevice.getButtonSize() ,  audioProcessor.padDevice.getButtonSize());
            
         
            
            int offset;
            
            if( audioProcessor.padDevice.getCol() > 12)
            {
                offset =  audioProcessor.padDevice.getFinger() - 1;
            }else{
                offset =  audioProcessor.padDevice.getFinger() -  audioProcessor.padDevice.getCol();
            }
            switch((padnum + offset * i +  audioProcessor.padDevice.getStartNote())%12){
                case 0://Root

                    audioProcessor.padDevice.pads.at(padnum).setCurrentColour(juce::Colours::violet);
                    
                    //audioProcessor.padDevice.pads.at(padnum).setCurrentColourMidiMessage({0xF0 ,0x00,0x20, 0x29, 0x02, 0x0E, 0x03, 0x00,  j + 10 * i + 11, 0x3B, 0xF7});
                    audioProcessor.padDevice.pads.at(padnum).setOrgColour();
                    audioProcessor.padDevice.pads.at(padnum).setOrgColourMidiMessage();

                    //audioProcessor.outDevice->sendMessageNow({0xF0 ,0x00,0x20, 0x29, 0x02, 0x0E, 0x03, 0x00,  j + 10 * i + 11, 0x3B, 0xF7});
                    
                    break;
                case 1://m2
                case 3://m3
                case 6://a4,d5
                case 8://m6
                case 10://m7
                    audioProcessor.padDevice.pads.at(padnum).setCurrentColour(juce::Colours::darkslategrey);
                    //audioProcessor.padDevice.pads.at(padnum).setCurrentColourMidiMessage({0xF0 ,0x00,0x20, 0x29, 0x02, 0x0E, 0x03, 0x00,   j + 10 * i + 11, 0, 0xF7});
                    audioProcessor.padDevice.pads.at(padnum).setOrgColour();
                    audioProcessor.padDevice.pads.at(padnum).setOrgColourMidiMessage();
                    
                    //audioProcessor.outDevice->sendMessageNow({0xF0 ,0x00,0x20, 0x29, 0x02, 0x0E, 0x03, 0x00,   j + 10 * i + 11, 0, 0xF7});

                   break;
                case 2://M2
                case 4://M3
                case 5://P4
                case 7://P5
                case 9://M6
                case 11://M7
                    audioProcessor.padDevice.pads.at(padnum).setCurrentColour(juce::Colours::lightskyblue);
                    //audioProcessor.padDevice.pads.at(padnum).setCurrentColourMidiMessage({0xF0 ,0x00,0x20, 0x29, 0x02, 0x0E, 0x03, 0x00,   j + 10 * i + 11, 0x01, 0xF7});
                    audioProcessor.padDevice.pads.at(padnum).setOrgColour();
                    audioProcessor.padDevice.pads.at(padnum).setOrgColourMidiMessage();
                    //audioProcessor.outDevice->sendMessageNow({0xF0 ,0x00,0x20, 0x29, 0x02, 0x0E, 0x03, 0x00,   j + 10 * i + 11, 0x01, 0xF7});
                     break;
                    
                default :
                    break;
                
            }
            
            padnum++;
               
            
            
        }

    }
    
    //modifyPadLighting();
    
}
