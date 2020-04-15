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

class Selection : public Component
{
public:
    Selection(AudioProcessingComponent& c, AudioThumbnail& t) :
        apc(c),
        thumb(t),
        waveVisWidth(0),
        waveVisHeight(0),
        thresholdInPixels(5)
    {
        setSize(0, 0);
        popupMenu.addItem("Mute", [this]() {apc.muteMarkedRegion(); });
    }

    ~Selection()
    {

    }

    void paint(Graphics& g) override
    {
        if (apc.getNumChannels() == 0)
            paintIfNoFileLoaded(g);
        else
            paintIfFileLoaded(g);
    }

    void paintIfNoFileLoaded(Graphics& g)
    {
        setSize(0, 0);
    }

    void paintIfFileLoaded(Graphics& g)
    {
        g.setColour(Colours::palevioletred);
        g.setOpacity(0.4);
        g.fillAll();
    }

    void resized() override
    {

    }

    /*! Called by WaveVisualizer to pass component dimensions
    */
    void parentDimensions(int width, int height)
    {
        waveVisWidth = width;
        waveVisHeight = height;
        setSelectionSize();
    }

    /*! Called when single mouse press on waveform
    \   zeros out selection bounds
    */
    void resetBounds()
    {
        setSize(0, 0);
        repaint();
    }

    /*! Called on mouseDrag in WaveVisualizer
    \   Sets markers in APC
    \   calls setSelectionSize to redefine this component size
    */
    void createBounds(const MouseEvent& event)
    {
        //coordinates from mouse event
        float start = float(event.getMouseDownX());
        float dragDist = float(event.getDistanceFromDragStartX());

        //start and end positions in seconds
        float startPos = 0;
        float endPos = 0;

        if (dragDist > thresholdInPixels)
        {
            startPos = (start / waveVisWidth) * apc.getLengthInS();
            endPos = ((start + dragDist) / waveVisWidth) * apc.getLengthInS();
        }
        else if (dragDist < -thresholdInPixels)
        {
            startPos = ((start + dragDist) / waveVisWidth) * apc.getLengthInS();
            endPos = (start / waveVisWidth) * apc.getLengthInS();
        }
        else
            return; //if threshold is not met, do nothing

        //otherwise set markers in apc and draw
        apc.setPositionInS(AudioProcessingComponent::MarkerStart, startPos);
        apc.setPositionInS(AudioProcessingComponent::MarkerEnd, endPos);
        apc.setPositionInS(AudioProcessingComponent::Cursor, startPos);

        setSelectionSize();
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

        if (audioStart == 0 && audioEnd == apc.getLengthInS())
        {
            resetBounds();
        }
        else
        {
            auto selectStart = (audioStart / audioLength) * waveVisWidth + 0;
            auto selectEnd = (audioEnd / audioLength) * waveVisWidth + 0;

            setSize(selectEnd - selectStart, waveVisHeight);
            setBounds(selectStart, 0, selectEnd - selectStart, waveVisHeight);
            repaint();
        }
    }

private:
    void mouseDown(const MouseEvent& event) override
    {
        //calculate apc cursor position if left click inside selection
        if (event.mods.isLeftButtonDown())
        {
            float ratio = float(event.getMouseDownX()) / float(getWidth());
            auto selectionStart(apc.getPositionInS(AudioProcessingComponent::MarkerStart));
            auto selectionEnd(apc.getPositionInS(AudioProcessingComponent::MarkerEnd));

            float selectionLengthInS = selectionEnd - selectionStart;

            apc.setPositionInS(AudioProcessingComponent::Cursor, (ratio * selectionLengthInS) + selectionStart);
            apc.setPositionInS(AudioProcessingComponent::MarkerStart, 0);
            apc.setPositionInS(AudioProcessingComponent::MarkerEnd, apc.getLengthInS());

            resetBounds();
            repaint();
        }
        //only trigger left click menu if clicked inside the selection
        else if (event.mods.isRightButtonDown())
        {
            popupMenu.show();
        }
    }

    /*! Click and drag inside the selection component will
    \   keep the size constant but move it around
    */
    void mouseDrag(const MouseEvent& event) override
    {

    }

    void mouseEnter(const MouseEvent& event) override
    {
        //automatically change the cursor to IBeam style when over the waveform
        setMouseCursor(juce::MouseCursor::IBeamCursor);
    }

    AudioProcessingComponent& apc;
    AudioThumbnail& thumb;
    PopupMenu popupMenu;

    int waveVisWidth;
    int waveVisHeight;
    int thresholdInPixels;     //threshold to fix tiny selection bug

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Selection)
};
