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
                          public ActionListener,
                          private Timer
{
public:
    WaveVisualizer(AudioProcessingComponent& c):
    apc(c),
    thumbnailCache (5),
    thumbnail (512, formatManager, thumbnailCache)
    {
        state = apc.getState(); //initialize transport source state
        apc.addActionListener(this);
        startTimerHz (30); // refresh the visualizer 30 times per second
    }

    ~WaveVisualizer()
    {
    }

    void paint (Graphics& g) override
    {
        /* This demo code just fills the component's background and
           draws some placeholder text to get you started.

           You should replace everything in this method with your own
           drawing code..
        */
        Rectangle<int> thumbnailBounds (0,0, getWidth(), getHeight());
        
        if (apc.getNumChannels() == 0)
            paintIfNoFileLoaded (g, thumbnailBounds);
        else
            paintIfFileLoaded (g, thumbnailBounds);
    }

    void resized() override
    {
        // This method is where you should set the bounds of any child
        // components that your component contains..
        

    }

    void actionListenerCallback (const String &message) override
    {
        if (message == MSG_FILE_LOADED)
        {
            int numAudioSamples;
            auto audioBuffer = apc.getAudioBuffer();
            thumbnail.reset(apc.getNumChannels(), apc.getSampleRate(), audioBuffer->getNumSamples());

            thumbnail.addBlock(0, *audioBuffer, 0, audioBuffer->getNumSamples());

            repaint();
        }
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
        g.setColour (Colours::white);
        g.fillRect (thumbnailBounds);
        
        g.setColour (Colours::red);                                     // [8]
        
        //auto thumbnailRef = apc.getThumbnail();
        thumbnail.drawChannels (g,                                 // [9]
                                thumbnailBounds,
                                0.0,                                    // start time
                                thumbnail.getTotalLength(),             // end time
                                1.0f);                                  // vertical zoom
        
        
        g.setColour (Colours::green);
        
        auto audioLength (thumbnail.getTotalLength());
        auto audioPosition (apc.getCurrentPosition());
        auto drawPosition ((audioPosition / audioLength) * thumbnailBounds.getWidth()
                           + thumbnailBounds.getX());                                        // [13]
        g.drawLine (drawPosition, thumbnailBounds.getY(), drawPosition,
                    thumbnailBounds.getBottom(), 2.0f);
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
