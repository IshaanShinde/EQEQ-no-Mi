/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
EQEQnoMiAudioProcessor::EQEQnoMiAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

EQEQnoMiAudioProcessor::~EQEQnoMiAudioProcessor()
{
}

//==============================================================================
const juce::String EQEQnoMiAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool EQEQnoMiAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool EQEQnoMiAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool EQEQnoMiAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double EQEQnoMiAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int EQEQnoMiAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int EQEQnoMiAudioProcessor::getCurrentProgram()
{
    return 0;
}

void EQEQnoMiAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String EQEQnoMiAudioProcessor::getProgramName (int index)
{
    return {};
}

void EQEQnoMiAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void EQEQnoMiAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    //<ano_4
    
    juce::dsp::ProcessSpec spec;
    
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels      = 1;
    spec.sampleRate       = sampleRate;

    leftChain.prepare(spec);
    rightChain.prepare(spec);

    //ano_4>
}

void EQEQnoMiAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool EQEQnoMiAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void EQEQnoMiAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    /* This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
    */

    //<ano_5
        
    juce::dsp::AudioBlock<float> block(buffer);

    //blocks representing individual channels
    auto leftBlock  = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);

    //context providing wrapper around the block that the chain can use
    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

    //passing context to the mono filter chain
    leftChain.process(leftContext);
    rightChain.process(rightContext);

    //ano_5>
}

//==============================================================================
bool EQEQnoMiAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* EQEQnoMiAudioProcessor::createEditor()
{
    //return new EQEQnoMiAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void EQEQnoMiAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void EQEQnoMiAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}


//<ano_2

juce::AudioProcessorValueTreeState::ParameterLayout
    EQEQnoMiAudioProcessor::createParameterLayout(){
        
        juce::AudioProcessorValueTreeState::ParameterLayout layout;

            layout.add(std::make_unique<juce::AudioParameterFloat>(
                "LowCut",
                "LowCut",
                juce::NormalisableRange<float>(20.f,20000.f, 1.f, 1.f),
                20.f
            ));

            layout.add(std::make_unique<juce::AudioParameterFloat>(
                "HighCut",
                "HighCut",
                juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 1.f),
                20000.f
                ));

            //Parametric Band 1 - PB_1
            layout.add(std::make_unique<juce::AudioParameterFloat>(
                "PB_1_freq",
                "PB_1_freq",
                juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 1.f),
                250.f
                ));

            layout.add(std::make_unique<juce::AudioParameterFloat>(
                "PB_1_gain",
                "PB_1_gain",
                juce::NormalisableRange<float>(-24.f, 24.f, 0.1f, 1.f),
                0.0f
                ));

            layout.add(std::make_unique<juce::AudioParameterFloat>(
                "PB_1_q",
                "PB_1_q",
                juce::NormalisableRange<float>(0.0f, 10.f, 0.1f, 1.f),
                1.f
                ));

            //Parametric Band 2 - PB_2
            layout.add(std::make_unique<juce::AudioParameterFloat>(
                "PB_2_freq",
                "PB_2_freq",
                juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 1.f),
                1000.f
                ));

            layout.add(std::make_unique<juce::AudioParameterFloat>(
                "PB_2_gain",
                "PB_2_gain",
                juce::NormalisableRange<float>(-24.f, 24.f, 0.1f, 1.f),
                0.0f
                ));

            layout.add(std::make_unique<juce::AudioParameterFloat>(
                "PB_2_q", 
                "PB_2_q",
                juce::NormalisableRange<float>(0.0f, 10.f, 0.1f, 1.f),
                1.f
                ));

            //Parametric Band 3 - PB_3
            layout.add(std::make_unique<juce::AudioParameterFloat>(
                "PB_3_freq",
                "PB_3_freq",
                juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 1.f),
                2500.f
                ));

            layout.add(std::make_unique<juce::AudioParameterFloat>(
                "PB_3_gain",
                "PB_3_gain",
                juce::NormalisableRange<float>(-24.f, 24.f, 0.1f, 1.f),
                0.0f
                ));

            layout.add(std::make_unique<juce::AudioParameterFloat>(
                "PB_3_q",
                "PB_3_q",
                juce::NormalisableRange<float>(0.0f, 10.f, 0.1f, 1.f),
                1.f
                ));

        juce::StringArray stringArray;
        for (int i = 0; i < 4; ++i) {
            juce::String str;
            str << (12 + 12*i);
            str << "db/oct";
            stringArray.add(str);
        }

            layout.add(std::make_unique<juce::AudioParameterChoice>(
                "LowCut Slope",
                "LowCut Slope",
                stringArray,
                0
            ));
            layout.add(std::make_unique<juce::AudioParameterChoice>(
                "HighCut Slope",
                "HighCut Slope",
                stringArray,
                0
            ));

        return layout;
    }

//ano_2>


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new EQEQnoMiAudioProcessor();
}
