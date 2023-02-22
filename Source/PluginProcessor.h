/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class EQEQnoMiAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    EQEQnoMiAudioProcessor();
    ~EQEQnoMiAudioProcessor() override;

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

    //<ano_1

    static juce::AudioProcessorValueTreeState::ParameterLayout 
        createParameterLayout();

    juce::AudioProcessorValueTreeState apvts{

        *this, nullptr,
        "Parameters",
        createParameterLayout()
    
    };

    //ano_1>


private:

    //<ano_3
        
    using Filter = juce::dsp::IIR::Filter<float>;

    //low and high cut 4 step filter (db/oct)
    using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;

    //signal path <lowcut, pb1, pb2, pb3, highcut>
    using MonoChain = juce::dsp::ProcessorChain<CutFilter,
                                                Filter, Filter, Filter,
                                                CutFilter>;

    MonoChain leftChain, rightChain;


    //ano_3>

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQEQnoMiAudioProcessor)
};
