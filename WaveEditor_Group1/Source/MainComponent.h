/*
  ==============================================================================

    MainComponent.h
    Created: 7 Feb 2020 3:03:43pm
    Author:  user

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GUIComponent.h"
#include "AudioProcessingComponent.h"

//==============================================================================
/*
*/
class MainComponent    : public Component
{
public:
    MainComponent();
    ~MainComponent();

    void paint (Graphics&) override;
    void resized() override;

private:
    GUIComponent gui;
    AudioProcessingComponent cap;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
