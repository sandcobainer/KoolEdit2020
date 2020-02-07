/*
  ==============================================================================

    AudioProcessingComponent.h
    Created: 7 Feb 2020 3:05:06pm
    Author:  user

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "WaveAudio.h"

//==============================================================================
/*
*/
class AudioProcessingComponent    : public AudioAppComponent, public ChangeListener
{
public:
    AudioProcessingComponent();
    ~AudioProcessingComponent();
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    
    static void openButtonClicked();
    static void playButtonClicked();
    static void stopButtonClicked();
    
private:
    
    enum TransportState
    {
        Stopped,
        Starting,
        Playing,
        Stopping
    };
    
    AudioFormatManager formatManager;
    std::unique_ptr<AudioFormatReaderSource> readerSource;
    AudioTransportSource transportSource;
    TransportState state;
    void changeState(TransportState state);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioProcessingComponent)
};
