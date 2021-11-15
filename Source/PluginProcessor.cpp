/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "LFO.h"
#include <math.h>

//==============================================================================
LofijuicerAudioProcessor::LofijuicerAudioProcessor()
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

LofijuicerAudioProcessor::~LofijuicerAudioProcessor()
{
}

//==============================================================================
const juce::String LofijuicerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool LofijuicerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool LofijuicerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool LofijuicerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double LofijuicerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int LofijuicerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int LofijuicerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void LofijuicerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String LofijuicerAudioProcessor::getProgramName (int index)
{
    return {};
}

void LofijuicerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void LofijuicerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    const int numInputChannels = getTotalNumInputChannels();
    
    // set local variables
    fs = sampleRate;
    
    /* initialize the filter */
    
    // set up filter
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = fs;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getMainBusNumOutputChannels();
    
    stateVariableFilter.reset();             // get rid of potential garbage values
    stateVariableFilter.prepare(spec);
    
    
    /* initialize the delay buffer */
    
    // allow access to audio being played 4 second ago
    const int delayBufferSize = 4 * (sampleRate + samplesPerBlock);
    // set the delay buffer size
    thisDelayBuffer.setSize(numInputChannels, delayBufferSize);
    thisDelayBuffer.clear();                // get rid of potential garbage values
    
    // initialize LFO
    lfo.initLFO(lfoRate, fs);
    
}

void LofijuicerAudioProcessor::updateFilter()
{
    stateVariableFilter.state->type = juce::dsp::StateVariableFilter::Parameters<float>::Type::lowPass;
    stateVariableFilter.state->setCutOffFrequency(fs, cutoff, resonance);
}

void LofijuicerAudioProcessor::updateParameters()
{
    
}

void LofijuicerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool LofijuicerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void LofijuicerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    /* Low-pass Filter Processing */
    
    // define the audio block for the processor to duplicate the mono channel to stereo
    juce::dsp::AudioBlock<float> block (buffer);
    updateFilter();
    stateVariableFilter.process(juce::dsp::ProcessContextReplacing<float>(block));
    
    
    /* Chorus Filter Processing */
    
    const int bufferLength = buffer.getNumSamples();
    const int delayBufferLength = thisDelayBuffer.getNumSamples();

    // Begins writing the original singal into the delay buffer
    for (int channel = 0; channel < totalNumInputChannels; channel++)
    {
        const float* bufferData = buffer.getReadPointer(channel);
        const float* delayBufferData = thisDelayBuffer.getReadPointer(channel);

        // copy the delayed signal to the delay buffer
        writeDelayBuffer(channel, bufferLength, delayBufferLength, bufferData);

        // read the delayed buffer and add it to the main buffer
        getFromDelayBuffer(buffer, channel, bufferLength, delayBufferLength, delayBufferData);
    }

    // update the buffer and advance the position
    thisWritePosition += bufferLength;
    thisWritePosition %= delayBufferLength;

    // update the LFO
    lfo.phase += lfo.phase_inc / 4.0;
    lfo.output = sin(lfo.phase);
}

//==============================================================================

/* Copy the data from the main buffer to the delay buffer */
void LofijuicerAudioProcessor::writeDelayBuffer(int channel, const int bufferLength, const int delayBufferLength, const float* bufferData)
{
    
    if (delayBufferLength > bufferLength + thisWritePosition)
    {
        thisDelayBuffer.copyFromWithRamp(channel, thisWritePosition, bufferData, bufferLength, 0.75, 0.75);
    }
    else
    {
        // fill up the remaining values
        const int bufferRemaining = delayBufferLength - thisWritePosition;
        thisDelayBuffer.copyFromWithRamp(channel, thisWritePosition, bufferData, bufferRemaining, 0.75, 0.75);
        // prepare to go to the next buffer (and thus back to 0)
        thisDelayBuffer.copyFromWithRamp(channel, 0, bufferData + bufferRemaining, bufferLength - bufferRemaining, 0.75, 0.75);
    }
}

/* Add the delay buffer delayed by t milliseconds to the main buffer */
void LofijuicerAudioProcessor::getFromDelayBuffer(juce::AudioBuffer<float>& buffer, int channel, const int bufferLength, const int delayBufferLength, const float* delayBufferData)
{
    // delay time in ms
    float t_left = delayTime + ( depth * lfo.output );
    float t_right = delayTime - ( depth * lfo.output );
    float t;
    
    // create different delay time in L/R
    if (channel == 0) {
        t = t_left;
    }
    else
    {
        t = t_right;
    }

    // next lowest integer (floor and ceil)
    auto readPosition = juce::roundToInt(delayBufferLength + thisWritePosition - (fs * t / 1000.0)) % delayBufferLength;
    
//    if (readPosition < 0)
//        readPosition += delayBufferLength;
    
    // add delay buffer
    if (delayBufferLength > bufferLength + readPosition)
    {
        // buffer.addFrom(channel, 0, delayBufferData + readPosition, bufferLength);
        buffer.addFromWithRamp(channel, 0, delayBufferData + readPosition, bufferLength, 0.8, 0.8);
    }
    else
    {
        const int bufferRemaining = delayBufferLength - readPosition;
        buffer.addFromWithRamp(channel, 0, delayBufferData + readPosition, bufferRemaining, 0.8, 0.8);
        buffer.addFromWithRamp(channel, bufferRemaining, delayBufferData, bufferLength - bufferRemaining, 0.8, 0.8);
    }
}

//==============================================================================
bool LofijuicerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* LofijuicerAudioProcessor::createEditor()
{
    return new LofijuicerAudioProcessorEditor (*this);
}

//==============================================================================
void LofijuicerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void LofijuicerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LofijuicerAudioProcessor();
}
