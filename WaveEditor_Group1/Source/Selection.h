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

class Selection : public Component
{
public:
    Selection(AudioProcessingComponent& c) : 
        apc(c),
        selectionBounds(0, 0, 0, 0)
    {

    }

    ~Selection()
    {

    }

    void paint(Graphics& g) override
    {

    }

    void paintIfNoFileLoaded(Graphics& g)
    {

    }

    void paintIfFileLoaded(Graphics& g)
    {
        auto audioLength(thumbnail.getTotalLength());

        auto audioStart(apc.getPositionInS(AudioProcessingComponent::MarkerStart));
        auto audioEnd(apc.getPositionInS(AudioProcessingComponent::MarkerEnd));

        auto selectStart = (audioStart / audioLength) * thumbnailBounds.getWidth() + thumbnailBounds.getX();
        auto selectEnd = (audioEnd / audioLength) * thumbnailBounds.getWidth() + thumbnailBounds.getX();

        selectionBounds.setBounds(selectStart, 0, selectEnd - selectStart, getHeight());

        g.setColour(Colours::palevioletred);
        //if there is no actual selection (start = beginning, end = track end) AND loop is off, don't paint
        if (audioStart == 0 && audioEnd == apc.getLengthInS())
            g.setOpacity(0);
        else
            g.setOpacity(0.4);
        g.fillRect(selectionBounds);
    }

    void resized() override
    {

    }

    /*! Called when single mouse press on waveform
    \   zeros out selection bounds
    */
    void resetBounds()
    {
        repaint();
    }

    void createBounds(const MouseEvent& event)
    {
        //coordinates from mouse event
        float start = float(event.getMouseDownX());
        float dragDist = float(event.getDistanceFromDragStartX());

        //start and end positions in seconds
        float startPos = 0;
        float endPos = 0;

        if (dragDist > 0)
        {
            startPos = (start / getWidth()) * apc.getLengthInS();
            endPos = ((start + dragDist) / getWidth()) * apc.getLengthInS();
        }
        else if (dragDist < 0)
        {
            startPos = ((start + dragDist) / getWidth()) * apc.getLengthInS();
            endPos = (start / getWidth()) * apc.getLengthInS();
        }

        apc.setPositionInS(AudioProcessingComponent::MarkerStart, startPos);
        apc.setPositionInS(AudioProcessingComponent::MarkerEnd, endPos);
        apc.setPositionInS(AudioProcessingComponent::Cursor, startPos);
        repaint();
    }

private:
    AudioProcessingComponent& apc;
    Rectangle<float> selectionBounds;              //rectangle drawn when user clicks and drags

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Selection)
};
