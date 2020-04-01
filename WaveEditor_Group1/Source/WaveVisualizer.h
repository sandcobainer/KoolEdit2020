/*
  ==============================================================================

    WaveVisualizer.h
    Created: 7 Feb 2020 3:15:20pm
    Author:  user

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "AudioProcessingComponent.h"

//==============================================================================
/*
*/
class WaveVisualizer    : public Component,
                          //public ActionListener,
                          public ChangeListener,
                          private Timer
{
public:
    WaveVisualizer(AudioProcessingComponent& c):
    apc(c),
    thumbnailCache (5),
    thumbnail (512, formatManager, thumbnailCache)
    {
        state = apc.getState(); //initialize transport source state
        //apc.addActionListener(this);
        apc.fileLoaded.addChangeListener(this);
        startTimerHz (60); // refresh the visualizer 30 times per second
    }

    ~WaveVisualizer()
    {
    }

    void paint (Graphics& g) override
    {
        // TODO: Change this size later
        Rectangle<int> thumbnailBounds (0,0, getWidth(), getHeight());
        
        if (apc.getNumChannels() == 0)
            paintIfNoFileLoaded (g, thumbnailBounds);
        else
            paintIfFileLoaded (g, thumbnailBounds);
    }

    void resized() override
    {
    }

    void changeListenerCallback (ChangeBroadcaster* source) override
    {
        auto audioBuffer = apc.getAudioBuffer();
        thumbnail.reset(apc.getNumChannels(), apc.getSampleRate(), audioBuffer->getNumSamples());
        thumbnail.addBlock(0, *audioBuffer, 0, audioBuffer->getNumSamples());
        repaint();
    }

    void timerCallback() override
    {
        repaint();
    }

    void paintIfNoFileLoaded (Graphics& g, const Rectangle<int>& thumbnailBounds)
    {
        g.setColour (Colours::darkgrey);
        g.fillRect (thumbnailBounds);
        g.setColour (Colours::white);
        g.drawFittedText ("No File Loaded", thumbnailBounds, Justification::centred, 1.0f);
    }
    
    void paintIfFileLoaded (Graphics& g, const Rectangle<int>& thumbnailBounds)
    {
        g.setColour (Colours::dimgrey);
        g.fillRect (thumbnailBounds);
        
        g.setColour (Colours::white);                                     // [8]
        thumbnail.drawChannels (g,                                 // [9]
                                thumbnailBounds,
                                0.0,                                    // start time
                                thumbnail.getTotalLength(),             // end time
                                1.0f);                                  // vertical zoom
        
        
        g.setColour (Colours::red);
        auto audioLength (thumbnail.getTotalLength());
        auto audioPosition (apc.getCurrentPositionInS());
        auto drawPosition ((audioPosition / audioLength) * thumbnailBounds.getWidth()
                           + thumbnailBounds.getX());                                        // [13]
        g.drawLine (drawPosition, thumbnailBounds.getY(), drawPosition,
                    thumbnailBounds.getBottom(), 2.0f);
    }

    void mouseDown (const MouseEvent &event)
    {
        float ratio = float(event.getMouseDownX()) / float(getWidth());
        apc.setPositionInS(ratio * apc.getLengthInS());
        repaint();
    }

    void mouseDrag(const MouseEvent& event)
    {

    }

    void mouseEnter(const MouseEvent& event)
    {
        //automatically change the cursor to IBeam style when over the waveform
        setMouseCursor(juce::MouseCursor::IBeamCursor);
    }

    void mouseExit(const MouseEvent& event)
    {
        //automatically change the cursor to back to normal when not over the waveform
        setMouseCursor(juce::MouseCursor::NormalCursor);
    }

private:
    //connection to AudioProcessingComponent (passed from parent)
    AudioProcessingComponent& apc;
    AudioFormatManager formatManager;
    AudioThumbnailCache thumbnailCache;
    AudioThumbnail thumbnail;
    String state;                                               //transport state (from apc.getState() function)
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveVisualizer)
};

class TrackVisualizer: public Component,
                       public Slider::Listener
{
public:
    TrackVisualizer (AudioProcessingComponent &c):
    waveVis(c),
    waveVisualizerWidthRatio(1.f),
    waveVisualizerWidth(0),
    isInitialized(false)
    {
        incDecSlider.addListener(this);
        incDecSlider.setRange(0.1, 10.0, 0.1);
        incDecSlider.setValue(1.0);
        addAndMakeVisible(incDecSlider);
        addAndMakeVisible(trackViewport);
    }
    ~TrackVisualizer()
    {
    }

    void paint (Graphics& g) override
    {
    }

    void resized() override
    {
        if (!isInitialized)
        {
            waveVisualizerWidth = getWidth();
            isInitialized = true;
        }
        waveVisualizerWidthRatio = incDecSlider.getValue();
        waveVis.setBounds(0, 0, waveVisualizerWidthRatio*waveVisualizerWidth, getHeight()-50);
        trackViewport.setViewedComponent(&waveVis);
        trackViewport.setBounds(0, 50, getWidth(), getHeight()-50);
        incDecSlider.setBounds(getWidth()-100, 0, 100, 50);
    }

    void sliderValueChanged(Slider* slider) override
    {
        resized();
    }

    void changeWaveVisualizerWidthRatio(float ratio)
    {
        waveVisualizerWidthRatio = ratio;
        resized();
    }
private:
    Viewport trackViewport;
    WaveVisualizer waveVis;
    Slider incDecSlider { Slider::IncDecButtons, Slider::TextBoxBelow };

    float waveVisualizerWidthRatio;
    int waveVisualizerWidth;
    bool isInitialized;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackVisualizer)
};