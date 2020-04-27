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
#include "Selection.h"
#include "Timeline.h"
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
    thumbnailBounds(0, 0, 0, 0)
//    timelineBounds(0, 0, 0, 0)
    {
        state = apc.getState(); //initialize transport source state
        //apc.addActionListener(this);
        apc.audioBufferChanged.addChangeListener(this);
        startTimerHz (60); // refresh the visualizer 30 times per second
        
        
//        timelineSlider.setTextBoxStyle (Slider::TextBoxAbove, true, 50,15 );
//        timelineSlider.setSliderStyle (Slider::LinearHorizontal);
//        timelineSlider.setRange (0, 10, 0.1);
//        timelineSlider.setLookAndFeel (&otherLookAndFeel);
//
//        timelineLabel.setFont (10.0f);
//        timelineLabel.setText ("Playback Time", NotificationType::dontSendNotification);
//        timelineLabel.setJustificationType (Justification::centredTop);
//        timelineLabel.attachToComponent (&timelineSlider, false);
        
        timeline = new Timeline(apc, thumbnail);
        waveSelection = new Selection(apc, thumbnail);
        
        addAndMakeVisible (waveSelection);
        addAndMakeVisible (timeline);
//        addAndMakeVisible (timelineSlider);
    }

    ~WaveVisualizer()
    {
    }

    void paint (Graphics& g) override
    {
        // TODO: Change this size later
//        timelineSlider.setBounds(0, 0, getWidth(), getHeight()-195);
        thumbnailBounds.setBounds(0, 100, getWidth(), getHeight()-100);
        waveSelection->parentDimensions(getWidth(), getHeight());
        timeline->parentDimensions(getWidth(), getHeight()-250);
        
        if (apc.getNumChannels() == 0)
            paintIfNoFileLoaded (g);
        else
        {
            paintIfFileLoaded (g);
//            timelineSlider.setRange (0, apc.getLengthInS(), 0.01);
//        timelineSlider.setMinValue(apc.getPositionInS(AudioProcessingComponent::MarkerStart));
//        timelineSlider.setMaxValue(apc.getPositionInS(AudioProcessingComponent::MarkerEnd));
//        timelineSlider.setValue(apc.getPositionInS(AudioProcessingComponent::Cursor));
        }
        
    }

    void resized() override
    {
        waveSelection->parentDimensions(getWidth(), getHeight());
        timeline->parentDimensions(getWidth(), getHeight()-250);
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
        g.setColour (Colours::white);                                     // [8]
        thumbnail.drawChannels (g,                                 // [9]
                                thumbnailBounds,
                                0.0,                                    // start time
                                thumbnail.getTotalLength(),             // end time
                                1.0f);                                  // vertical zoom
        
        //-------------------------------play marker----------------------------------------
        g.setColour (Colours::red);
        auto audioLength (thumbnail.getTotalLength());
        auto audioPosition (apc.getPositionInS(AudioProcessingComponent::Cursor));
        auto drawPosition ((audioPosition / audioLength) * thumbnailBounds.getWidth()
                           + thumbnailBounds.getX());                                        // [13]
        g.drawLine (drawPosition, thumbnailBounds.getY()-5, drawPosition,
                    thumbnailBounds.getBottom(), 2.0f);
    }

private:
    //connection to AudioProcessingComponent (passed from parent)
    AudioProcessingComponent& apc;
    AudioFormatManager formatManager;
    AudioThumbnailCache thumbnailCache;
    AudioThumbnail thumbnail;
    Rectangle<int> thumbnailBounds;
//    Rectangle<int> timelineBounds;
    AudioProcessingComponent::TransportState state;                                //transport state (from apc.getState() function)

    OtherLookAndFeel otherLookAndFeel;
    Selection *waveSelection;
    Timeline *timeline;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveVisualizer)
};

//-----------------------------------------------------------------------------------------------

class TrackVisualizer: public Component,
                       public Slider::Listener
{
public:
    TrackVisualizer (AudioProcessingComponent &apc):
    waveVis(apc),
    thumbnailCache (5),
    thumbnail (512, formatManager, thumbnailCache),
    waveVisualizerWidthRatio(1.f),
    waveVisualizerWidth(0),
    isInitialized(false)
    {
        incDecSlider.addListener(this);
        incDecSlider.setRange(0.1, 10.0, 0.1);
        incDecSlider.setValue(1.0);
        
//        timeline = new Timeline(apc, thumbnail);
        
        addAndMakeVisible(incDecSlider);
        addAndMakeVisible(trackViewport);
//        addAndMakeVisible(timeline);
        
        
    }
    ~TrackVisualizer()
    {
    }

    void paint (Graphics& g) override
    {
//        timeline->parentDimensions(getWidth(), getHeight()-250);
    }

    void resized() override
    {
        if (!isInitialized)
        {
            waveVisualizerWidth = getWidth();
            isInitialized = true;
        }
        waveVisualizerWidthRatio = incDecSlider.getValue();
//        timeline->setWidthRatio(waveVisualizerWidthRatio);
        incDecSlider.setBounds(getWidth()-100, 0, 100, 50);
        trackViewport.setViewedComponent(&waveVis);
        trackViewport.setBounds(0, 0, getWidth(), getHeight());
        
//        timeline->parentDimensions(getWidth(), getHeight()-250);
        
        waveVis.setBounds(0, 0, waveVisualizerWidthRatio*waveVisualizerWidth, getHeight());
        
//      timeline->repaint();
        waveVis.repaint();
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
//    Timeline *timeline;
    AudioFormatManager formatManager;
    AudioThumbnailCache thumbnailCache;
    AudioThumbnail thumbnail;
    
    Slider incDecSlider { Slider::IncDecButtons, Slider::TextBoxBelow };

    float waveVisualizerWidthRatio;
    int waveVisualizerWidth;
    bool isInitialized;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackVisualizer)
};
