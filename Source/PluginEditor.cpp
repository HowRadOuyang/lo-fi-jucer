/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#define LOFIJUICER_W        200
#define LOFIJUICER_H        300
#define MIN_DEPTH           0.1
#define MAX_DEPTH           1.0
#define MIN_DEL             10
#define MAX_DEL             100
#define MIN_CUTOFF          50
#define MAX_CUTOFF          1000
#define MIN_RES             1.0
#define MAX_RES             10.0
#define INIT_DEPTH          0.5
#define INIT_DEL            25
#define INIT_CUTOFF         250
#define INIT_RES            2.5
#define INC_DEPTH           0.01
#define INC_DEL             1
#define INC_CUTOFF          1
#define INC_RES             0.1

//==============================================================================
LofijuicerAudioProcessorEditor::LofijuicerAudioProcessorEditor (LofijuicerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (LOFIJUICER_W, LOFIJUICER_H);
    
    // define "CutOff" dial properties
    cutOff.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    cutOff.setRange(MIN_CUTOFF, MAX_CUTOFF, INC_CUTOFF);
    cutOff.setValue(INIT_CUTOFF);
    cutOff.setTextBoxStyle(juce::Slider::NoTextBox, true, 0 , 0);
    cutOff.setPopupDisplayEnabled(true, true, this);
    addAndMakeVisible(&cutOff);
    
    // define "Resonance" dial properties
    res.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    res.setRange(MIN_RES, MAX_RES, INC_RES);
    res.setValue(INIT_RES);
    res.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    res.setPopupDisplayEnabled(true, true, this);
    addAndMakeVisible(&res);
    
    // define "Depth" dial properties
    depth.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    depth.setRange(MIN_DEPTH, MAX_DEPTH, INC_DEPTH);
    depth.setValue(INIT_DEPTH);
    depth.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    depth.setPopupDisplayEnabled(true, true, this);
    addAndMakeVisible(&depth);
    
    // define "Delay Time" dial properties
    delayTime.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    delayTime.setRange(MIN_DEL, MAX_DEL, INC_DEL);
    delayTime.setValue(INIT_DEL);
    delayTime.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    depth.setPopupDisplayEnabled(true, true, this);
    addAndMakeVisible(&delayTime);
    
    cutOff.addListener(this);
    res.addListener(this);
    depth.addListener(this);
    delayTime.addListener(this);
}

LofijuicerAudioProcessorEditor::~LofijuicerAudioProcessorEditor()
{
}

//==============================================================================
void LofijuicerAudioProcessorEditor::paint (juce::Graphics& g)
{
    // UI title area
    juce::Rectangle<int> titleArea(0, 10, LOFIJUICER_W, 20);
    
    // background image
    // g.fillAll (juce::Colours::aquamarine);
    juce::Image background = juce::ImageCache::getFromMemory(BinaryData::juice_jpeg, BinaryData::juice_jpegSize);
    g.drawImageAt(background, 0, 0);
    
    // title text
    g.setColour(juce::Colours::black);
    g.setFont (juce::Font("Avenir", 30.0, juce::Font::bold));
    g.drawText("lo-fi JUICER", titleArea, juce::Justification::centredTop);
    
    // lower-fi text
    g.setFont (juce::Font("Avenir", 15.0, juce::Font::plain));
    g.drawText("lower-fi", 32.5, 130, 50, 25, juce::Justification::centredLeft);
    
    // broken text
    g.setFont (juce::Font("Avenir", 15.0, juce::Font::plain));
    g.drawText("broken", 132.5, 130, 50, 25, juce::Justification::centredLeft);
    
    // cutoff text
    g.setFont (juce::Font("Avenir", 15.0, juce::Font::plain));
    g.drawText("mood", 35, 180, 50, 25, juce::Justification::centredLeft);
    
    // res text
    g.setFont (juce::Font("Avenir", 15.0, juce::Font::plain));
    g.drawText("spice", 135, 180, 50, 25, juce::Justification::centredLeft);
    
}

void LofijuicerAudioProcessorEditor::resized()
{
    cutOff.setBounds(0, 180, LOFIJUICER_W / 2, LOFIJUICER_H / 2);
    res.setBounds(LOFIJUICER_W / 2, 180, LOFIJUICER_W / 2, LOFIJUICER_H / 2);
    depth.setBounds(0, 20, LOFIJUICER_W / 2, LOFIJUICER_H / 2);
    delayTime.setBounds(LOFIJUICER_W / 2, 20, LOFIJUICER_W / 2, LOFIJUICER_H / 2);
}

void LofijuicerAudioProcessorEditor::sliderValueChanged (juce::Slider* slider)
{
    if (slider == &depth)
    {
        audioProcessor.depth = depth.getValue();
    }
    
    if (slider == &delayTime)
    {
        audioProcessor.delayTime = delayTime.getValue();
    }
    
    if (slider == &cutOff)
    {
        audioProcessor.cutoff = cutOff.getValue();
    }
    
    if (slider == &res)
    {
        audioProcessor.resonance = res.getValue();
    }
}
