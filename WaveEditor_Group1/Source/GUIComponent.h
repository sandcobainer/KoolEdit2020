/*
  ==============================================================================

    GUIComponent.h
    Created: 7 Feb 2020 3:04:33pm
    Author:  user

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ToolbarIF.h"

//==============================================================================
/*
*/
class GUIComponent    : public Component
{
public:
    GUIComponent(AudioProcessingComponent&);
    ~GUIComponent();

    void paint (Graphics&) override;
    void resized() override;

private:
    //connection to AudioProcessingComponent (passed from parent)
    AudioProcessingComponent& cap;
    ToolbarIF *toolbar;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GUIComponent)
};
