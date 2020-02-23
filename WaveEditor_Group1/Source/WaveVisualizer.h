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
class WaveVisualizer    : public Component, public ChangeListener
{
public:
    WaveVisualizer(AudioProcessingComponent& c):apc(c)
    {
        state = apc.getState(); //initialize transport source state
        apc.thumbnail.addChangeListener(this); //listen to changes in the transport source
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
        
        if (apc.thumbnail.getNumChannels() == 0)
            paintIfNoFileLoaded (g, thumbnailBounds);
        else
            paintIfFileLoaded (g, thumbnailBounds);
    }

    void resized() override
    {
        // This method is where you should set the bounds of any child
        // components that your component contains..
        

    }
    
    void changeListenerCallback(ChangeBroadcaster* source) override
    {
        if (source == &apc.thumbnail)
        {
            repaint();
        }
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
        
        apc.thumbnail.drawChannels (g,                                      // [9]
                                thumbnailBounds,
                                0.0,                                    // start time
                                apc.thumbnail.getTotalLength(),             // end time
                                1.0f);                                  // vertical zoom
    }

private:
    //connection to AudioProcessingComponent (passed from parent)
    AudioProcessingComponent& apc;
    String state;                                               //transport state (from apc.getState() function)
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveVisualizer)
};
