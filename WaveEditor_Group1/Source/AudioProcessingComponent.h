/*
  ==============================================================================

    AudioProcessingComponent.h
    Created: 7 Feb 2020 3:05:06pm
    Author:  user

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class AudioProcessingComponent    : public Component
{
public:
    AudioProcessingComponent();
    ~AudioProcessingComponent();

    void paint (Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioProcessingComponent)
};
