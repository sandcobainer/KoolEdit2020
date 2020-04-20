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
        thresholdInPixels(5),
        selectionBounds(0,0,0,0)
    {
        popupMenu.addItem("Mute", [this]() {apc.muteMarkedRegion(); });
    }

    ~Selection()
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
        selectionBounds.setBounds(0, 0, 0, 0);
    }

    void paintIfFileLoaded(Graphics& g)
    {
        if (apc.isLoopEnabled() == false)
            g.setColour(Colours::palevioletred);
        else
            g.setColour(Colours::aquamarine);
        g.setOpacity(0.4);
        g.fillRect(selectionBounds);
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
        setSize(waveVisWidth, waveVisHeight);
        setSelectionSize();
    }

    /*! Called when single mouse press on waveform
    \   zeros out selection bounds
    */
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

        if (audioStart == 0 && audioEnd == apc.getLengthInS())
        {
            resetBounds();
        }
        else
        {
            auto selectStart = (audioStart / audioLength) * waveVisWidth + 0;
            auto selectEnd = (audioEnd / audioLength) * waveVisWidth + 0;

            selectionBounds.setBounds(selectStart, 0, selectEnd - selectStart, waveVisHeight);
            repaint();
        }
    }

private:

    /*! Called when new selection is being created
    \   Sets markers in APC
    \   calls setSelectionSize to redefine selectionBounds
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

    /*! Called when existing selection is being moved
    \   Sets markers in APC
    \   calls setSelectionSize to redefine selectionBounds
    */
    void slideBounds(const MouseEvent& event)
    {
        //std::cout << selectionBounds.getX() << " " << selectionBounds.getRight() << std::endl;
        //if click is inside original selection bounds
        int mousePos = event.getMouseDownX() + event.getDistanceFromDragStartX();
        if ((mousePos >= selectionBounds.getX()) && (mousePos <= selectionBounds.getRight()))
        {
            //coordinates from mouse event
            float start = float(event.getMouseDownX());
            float dragDist = float(event.getDistanceFromDragStartX());
            //std::cout << dragDist << std::endl;

            //start and end positions in seconds
            float startPos = apc.getPositionInS(AudioProcessingComponent::MarkerStart);
            float endPos = apc.getPositionInS(AudioProcessingComponent::MarkerEnd);

            if (dragDist > thresholdInPixels)
            {
                //startPos = (start / waveVisWidth) * apc.getLengthInS();
                startPos = startPos + ((float(dragDist) / float(getWidth())) * apc.getLengthInS());
                //endPos = ((start + dragDist) / waveVisWidth) * apc.getLengthInS();
                endPos = endPos + ((float(dragDist) / float(getWidth())) * apc.getLengthInS());
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
        else
            std::cout << "else" << std::endl;
    }

    void mouseDown(const MouseEvent& event) override
    {
        //calculate apc cursor position if left click inside selection
        //with cursor mouse
        if (event.mods.isLeftButtonDown())
        {
            if (!apc.isMouseNormal())
            {
                float ratio = float(event.getMouseDownX()) / float(getWidth());
                apc.setPositionInS(AudioProcessingComponent::Cursor, ratio * apc.getLengthInS());
                apc.setPositionInS(AudioProcessingComponent::MarkerStart, 0);
                apc.setPositionInS(AudioProcessingComponent::MarkerEnd, apc.getLengthInS());

                resetBounds();
                repaint();
            }
        }
        //only trigger left click menu if clicked inside the selection
        else if (event.mods.isRightButtonDown())
        {
            popupMenu.show();
        }
    }

    /*! Click and drag inside the selection component will
    \   keep the size constant but move it around if mouse normal
    \   otherwise will redraw a new selection
    */
    void mouseDrag(const MouseEvent& event) override
    {
        if (apc.isMouseNormal())
            slideBounds(event);
        else
            createBounds(event);

        repaint();
    }

    void mouseEnter(const MouseEvent& event) override
    {
        //automatically change the cursor to IBeam style when over the waveform
        //and if mouse icon is not clicked
        if (apc.isMouseNormal())
            setMouseCursor(juce::MouseCursor::NormalCursor);
        else
            setMouseCursor(juce::MouseCursor::IBeamCursor);
    }

    void mouseExit(const MouseEvent& event) override
    {
        //automatically change the cursor to back to normal when not over the waveform
        setMouseCursor(juce::MouseCursor::NormalCursor);
    }

    AudioProcessingComponent& apc;
    AudioThumbnail& thumb;
    PopupMenu popupMenu;

    int waveVisWidth;
    int waveVisHeight;
    int thresholdInPixels;     //threshold to fix tiny selection bug
    Rectangle<int> selectionBounds; //member that gets painted

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Selection)
};
