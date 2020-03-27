/*
  ==============================================================================

    GUIComponent.cpp
    Created: 7 Feb 2020 3:04:33pm
    Author:  user

  ==============================================================================
*/

#include <JuceHeader.h>
#include "GUIComponent.h"

//==============================================================================
GUIComponent::GUIComponent(AudioProcessingComponent& c) : apc(c)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    tlbar = new ToolbarIF(apc);
    addAndMakeVisible(tlbar);

    specvis = new SpectrogramVisualizer(apc);
    addAndMakeVisible(specvis);

    trackVis = new TrackVisualizer(apc);
    addAndMakeVisible(trackVis);
}

GUIComponent::~GUIComponent()
{
    delete tlbar;
    tlbar = nullptr;

    delete specvis;
    specvis = nullptr;

    delete trackVis;
    trackVis = nullptr;
}

void GUIComponent::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (Colours::white);
    g.setFont (14.0f);
    g.drawText ("GUIComponent", getLocalBounds(),
                Justification::centred, true);   // draw some placeholder text
}

void GUIComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    tlbar->setBounds(0, 0, getWidth(), 100);
    trackVis->setBounds(0,100,getWidth(),(getHeight()-100)/2);
    specvis->setBounds(0, 100+(getHeight()-100)/2, getWidth(), (getHeight()-100)/2);
}
