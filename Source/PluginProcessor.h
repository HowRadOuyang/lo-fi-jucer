/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "LFO.h"
#define pi                     3.14159265358979323846


//==============================================================================
/**
*/
class LofijuicerAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    LofijuicerAudioProcessor();
    ~LofijuicerAudioProcessor() override;

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
    
    //==============================================================================
    void updateFilter();
    void updateParameters();
    
    //==============================================================================
    void writeDelayBuffer(int channel, const int bufferLength, const int delayBufferLength, const float* bufferData);
    void getFromDelayBuffer(juce::AudioBuffer<float>& buffer, int channel, const int bufferLength, const int delayBufferLength, const float* delayBufferData);
    
    //==============================================================================
    Lfo lfo;
    // LFO values accessible with knobs
    float delayTime { 30.0 };                   // in ms
    float lfoRate { 2000.0 };                   // in Hz
    float depth { 0.5 };
    // filter values accessible with knobs
    float cutoff = 600.0;
    float resonance = 2.5;

private:
    juce::AudioBuffer<float> thisDelayBuffer;
    juce::dsp::ProcessorDuplicator<juce::dsp::StateVariableFilter::Filter<float>, juce::dsp::StateVariableFilter::Parameters<float>> stateVariableFilter;
    
    int thisWritePosition { 0 };
    
    int fs { 48000 };
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LofijuicerAudioProcessor)
};
