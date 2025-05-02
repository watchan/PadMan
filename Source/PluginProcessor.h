/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PadDevice.h"
//==============================================================================
/**
*/
class PadManAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    PadManAudioProcessor();
    ~PadManAudioProcessor() override;

    //=PadMan=======================================================================
    PadDevice padDevice;
    
    
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    //==============================================================================
    
    static BusesProperties getBusesLayout()
    {
        // Reference of MIDILogger Plug-in
        // Ableton Live doesn't like to load midi-only plugins, so we add an audio input and output.
        const PluginHostType host;
      
        return host.isAbletonLive() ? BusesProperties().withInput("input", AudioChannelSet::stereo()).withOutput("out", AudioChannelSet::stereo())
                                                     : BusesProperties();
  
    }

    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PadManAudioProcessor)
};
