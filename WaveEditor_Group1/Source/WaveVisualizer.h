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
                
        
        waveSelection = new Selection(apc, thumbnail);
        
        addAndMakeVisible (waveSelection);
    }

    ~WaveVisualizer()
    {
//        setLookAndFeel (nullptr);
    }

    void paint (Graphics& g) override
    {
        // TODO: Change this size later
        timelineBounds.setBounds(0,0,getWidth(), 40);
        thumbnailBounds.setBounds(0, 40, getWidth(), getHeight() - 40);
    
        waveSelection->parentDimensions(getWidth(), getHeight());

        if (apc.getNumChannels() == 0)
            paintIfNoFileLoaded (g);
        else
        {
            paintIfFileLoaded (g);
            timelineSlider.setRange (0, apc.getLengthInS(), 0.01);
            timelineSlider.setValue(apc.getPositionInS(AudioProcessingComponent::Cursor));
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
        g.setOpacity (0.3f);
        g.setColour (Colours::black);
        g.fillRect (timelineBounds);
        
        g.setOpacity (0.3f);
        g.setColour (Colours::darkgrey);
        g.fillRect (thumbnailBounds);
        g.setColour (Colours::white);
        g.drawFittedText ("No File Loaded", thumbnailBounds, Justification::centred, 1.0f);
    }
    
    void paintIfFileLoaded (Graphics& g)
    {
        paintTimeline (g);
        //-----------------------------track background--------------------------------------
        g.setColour (Colours::black);
        g.setOpacity (0.2f);
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
        g.drawLine (drawPosition, timelineBounds.getY() + timelineBounds.getHeight()/2, drawPosition,
                    thumbnailBounds.getBottom(), 2.0f);
    }
    
    void setWidth(float waveVisualizerWidth)
    {
        waveWidth = waveVisualizerWidth;
    }
    
    void paintTimeline (Graphics& g)
    {
        g.setOpacity(0.5f);
        g.setColour(Colour(219,218,215));
        g.fillRect (timelineBounds);
        
        auto iy = timelineBounds.getHeight() * 0.3f;
        auto divSizeInS = thumbnail.getTotalLength() * 50 / waveWidth;
        auto textWidth = 20;
        
        int j = 0;
        for (int i = 0; i < waveWidth; i = i + 50)
        {
            g.setColour (Colours::black);
            g.setFont(11.0f);
            
            auto divText = String(j * divSizeInS, 2, false);
            g.drawText (divText, i-textWidth/2, 0, textWidth, iy, Justification::centred, true);
            g.drawLine (i, iy, i, timelineBounds.getBottom());
            j++;
        }
    }

private:
    //connection to AudioProcessingComponent (passed from parent)
    AudioProcessingComponent& apc;
    AudioFormatManager formatManager;
    AudioThumbnailCache thumbnailCache;
    AudioThumbnail thumbnail;
    Rectangle<int> thumbnailBounds;
    Rectangle<int> timelineBounds;
    AudioProcessingComponent::TransportState state;

    
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
