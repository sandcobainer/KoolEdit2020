/*
  ==============================================================================

    Selection.h
    Created: 13 Apr 2020 9:29:12pm
    Author:  sophi

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "AudioProcessingComponent.h"
#include "WaveVisualizer.h"

class Timeline : public Component
{
public:
    
    Timeline(AudioProcessingComponent& c, AudioThumbnail& t) :
        apc(c),
        thumb(t),
        timelineWidth(0),
        timelineHeight(0),
        thresholdInPixels(5),
        waveVisWidthRatio(1),
        selectionBounds(0,0,0,0),
        timelineBounds(0,0,0,0)
    {
    }

    ~Timeline()
    {
    }

    void paint(Graphics& g) override
    {
        //std::cout << selectionBounds.getWidth() << " " << selectionBounds.getHeight() << std::endl;

        if (apc.getNumChannels() == 0)
            paintIfNoFileLoaded(g);
        else
            paintIfFileLoaded(g);
    }

    void paintIfNoFileLoaded(Graphics& g)
    {
        
        timelineBounds.setBounds(0,50,getWidth(), timelineHeight);
        g.setColour(Colours::green);
        g.setOpacity(0.6);
        g.fillRect(timelineBounds);
    }

    void paintIfFileLoaded(Graphics& g)
    {
        timelineBounds.setBounds(0,50,getWidth(), timelineHeight);
        g.setColour(Colours::green);
        g.setOpacity(0.6);
        g.fillRect(timelineBounds);
        //do not paint if markers are at start and end of track
//        if (selectionBounds.getWidth() == 0)
//            return;
//        //paint color according to loop on/off
//        if (apc.isLoopEnabled() == false)
//            g.setColour(Colours::palevioletred);
//        else
//            g.setColour(Colours::aquamarine);
//        g.setOpacity(0.4);
//        g.fillRect(selectionBounds);
    }

    void resized() override
    {

    }

    /*! Called by TrackVisualizer to pass component dimensions
    */
    void parentDimensions(int width, int height)
    {
        timelineWidth = width;
        timelineHeight = height;
        setSize(timelineWidth, timelineHeight);
        setSelectionSize();
        setTimelineSize();
    }

    void resetBounds()
    {
        selectionBounds.setBounds(0, 0, 0, 0);
        repaint();
    }

    /*! Gets start stop markers from APC
     \   and resizes this component
     \   also called in timer callback in WaveVisualizer
     */
    void setSelectionSize()
    {
        auto audioLength(thumb.getTotalLength());
        auto audioStart(apc.getPositionInS(AudioProcessingComponent::MarkerStart));
        auto audioEnd(apc.getPositionInS(AudioProcessingComponent::MarkerEnd));

        if (audioStart == 0 && (abs(audioEnd - apc.getLengthInS()) < 0.001))
        {
            resetBounds();
        }
        else
        {
            auto selectStart = (audioStart / audioLength) * timelineWidth;
            auto selectEnd = (audioEnd / audioLength) * timelineWidth;

            selectionBounds.setBounds(selectStart , 0, selectEnd - selectStart, timelineHeight);
            repaint();
        }
    }
    
    void setTimelineSize()
    {
        timelineBounds.setBounds(0 , 100, getWidth(), timelineHeight);
        repaint();
    }

private:

    AudioProcessingComponent& apc;
    AudioThumbnail& thumb;

    int timelineWidth;
    int timelineHeight;
    int thresholdInPixels;     //threshold to fix tiny selection bug
    
    float waveVisWidthRatio;
    Rectangle<int> selectionBounds; //member that gets painted
    Rectangle<int> timelineBounds;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Timeline)
};
