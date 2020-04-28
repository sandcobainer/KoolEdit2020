/*
  ==============================================================================

    WaveVisualizer.h
    Created: 7 Feb 2020 3:15:20pm
    Author:  sandcobainer

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "AudioProcessingComponent.h"
#include "Selection.h"
#include "OtherLookAndFeel.h"

//==============================================================================
/*
*/
class WaveVisualizer    : public Component,
                          public ChangeListener,
                          private Timer
{
public:
    WaveVisualizer(AudioProcessingComponent& c):
    apc(c),
    thumbnailCache (5),
    thumbnail (512, formatManager, thumbnailCache),
    thumbnailBounds(0, 0, 0, 0),
    timelineBounds(0, 0, 0, 0)
    {
        state = apc.getState(); //initialize transport source state
        apc.audioBufferChanged.addChangeListener(this);
        startTimerHz (60); // refresh the visualizer 30 times per second
                
        timelineSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
        timelineSlider.setSliderStyle (Slider::LinearHorizontal);
        timelineSlider.setRange (0, 10, 0.1);
        timelineSlider.setLookAndFeel (&otherLookAndFeel);
        
        waveSelection = new Selection(apc, thumbnail);
        
        addAndMakeVisible (waveSelection);
        addAndMakeVisible (timelineSlider);
    }

    ~WaveVisualizer()
    {
        setLookAndFeel (nullptr);
    }

    void paint (Graphics& g) override
    {
        // TODO: Change this size later
        timelineSlider.setBounds(0, 0, getWidth(), getHeight()-210);
        thumbnailBounds.setBounds(8, 40, getWidth()-16, getHeight() - 40);
        
        waveSelection->setThumbnailBounds(thumbnailBounds);
        waveSelection->parentDimensions(getWidth()-16, getHeight());

        if (apc.getNumChannels() == 0)
            paintIfNoFileLoaded (g);
        else
        {
            paintIfFileLoaded (g);
            timelineSlider.setRange (0, apc.getLengthInS(), 0.01);
            timelineSlider.setValue(apc.getPositionInS(AudioProcessingComponent::Cursor));
            
            auto audioLength (thumbnail.getTotalLength());
            otherLookAndFeel.setDivSize(audioLength *50 / waveWidth, waveWidth, audioLength);
        }
        
    }

    void resized() override
    {
        waveSelection->parentDimensions(getWidth(), getHeight());
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
        waveSelection->setSelectionSize();
    }

    void paintIfNoFileLoaded (Graphics& g)
    {
        g.setColour (Colours::darkgrey);
        g.fillRect (thumbnailBounds);
        g.setColour (Colours::white);
        g.drawFittedText ("No File Loaded", thumbnailBounds, Justification::centred, 1.0f);
    }
    
    void paintIfFileLoaded (Graphics& g)
    {
        //-----------------------------track background--------------------------------------
        g.setColour (Colours::dimgrey);
        g.fillRect (thumbnailBounds);
        
        //---------------------------------waveform------------------------------------------
        g.setColour (Colour(153,255,255));                                     // [8]
        thumbnail.drawChannels (g,                                 // [9]
                                thumbnailBounds,
                                0.0,                                    // start time
                                thumbnail.getTotalLength(),             // end time
                                1.0f);                                  // vertical zoom
        
        //-------------------------------play marker----------------------------------------
        g.setColour (Colour(128,255,0));
        auto audioLength (thumbnail.getTotalLength());
        auto audioPosition (apc.getPositionInS(AudioProcessingComponent::Cursor));
        auto drawPosition ((audioPosition / audioLength) * thumbnailBounds.getWidth()
                           + thumbnailBounds.getX());                                        // [13]
        g.drawLine (drawPosition, thumbnailBounds.getY()-5, drawPosition,
                    thumbnailBounds.getBottom(), 2.0f);
    }
    
    void setWidth(float waveVisualizerWidth)
    {
        auto audioLength (thumbnail.getTotalLength());
        waveWidth = waveVisualizerWidth;
        otherLookAndFeel.setDivSize(audioLength * 50 / waveWidth, waveWidth, audioLength);
    }
    

private:
    //connection to AudioProcessingComponent (passed from parent)
    AudioProcessingComponent& apc;
    AudioFormatManager formatManager;
    AudioThumbnailCache thumbnailCache;
    AudioThumbnail thumbnail;
    Rectangle<int> thumbnailBounds;
    Rectangle<int> timelineBounds;
    AudioProcessingComponent::TransportState state;                                //transport state (from apc.getState() function)

    
    OtherLookAndFeel otherLookAndFeel;
    Slider timelineSlider;
    Selection *waveSelection;
    float waveWidth;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveVisualizer)
};

//-----------------------------------------------------------------------------------------------

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
            waveVis.setWidth(waveVisualizerWidth);
            isInitialized = true;
        }
        waveVisualizerWidthRatio = incDecSlider.getValue();
        waveVis.setBounds(0, 0, waveVisualizerWidthRatio*waveVisualizerWidth, getHeight()-50);
        trackViewport.setViewedComponent(&waveVis);
        trackViewport.setBounds(0, 50, getWidth(), getHeight()-50);
        incDecSlider.setBounds(getWidth()-100, 0, 100, 50);

        waveVis.repaint();
    }

    void sliderValueChanged(Slider* slider) override
    {
        waveVis.setWidth(incDecSlider.getValue() * waveVisualizerWidth);
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
