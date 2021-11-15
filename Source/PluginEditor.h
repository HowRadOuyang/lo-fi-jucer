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
class LofijuicerAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                        public juce::Slider::Listener
{
public:
    LofijuicerAudioProcessorEditor (LofijuicerAudioProcessor&);
    ~LofijuicerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged (juce::Slider* slider) override;

private:
    juce::Slider cutOff;
    juce::Slider res;
    
    juce::Slider depth;
    juce::Slider delayTime;
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    LofijuicerAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LofijuicerAudioProcessorEditor)
};
