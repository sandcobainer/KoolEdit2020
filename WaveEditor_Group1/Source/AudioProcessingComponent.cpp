/*
  ==============================================================================

    AudioProcessingComponent.cpp
    Created: 7 Feb 2020 3:05:06pm
    Author:  user

  ==============================================================================
*/

#include <JuceHeader.h>
#include "AudioProcessingComponent.h"

//==============================================================================
AudioProcessingComponent::AudioProcessingComponent():state(Stopped)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    formatManager.registerBasicFormats();       // [1]
    transportSource.addChangeListener (this);   // [2]
    
    setAudioChannels (0, 2);
}

AudioProcessingComponent::~AudioProcessingComponent()
{
    shutdownAudio();
}

void prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay (samplesPerBlockExpected, sampleRate);
}

void releaseResources() {
    
}

void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) {

}


void changeState (TransportState newState)
{
    if (state != newState)
    {
        state = newState;
        
        switch (state)
        {
            case Stopped:                           // [3]
                stopButton.setEnabled (false);
                playButton.setEnabled (true);
                transportSource.setPosition (0.0);
                break;
                
            case Starting:                          // [4]
                playButton.setEnabled (false);
                transportSource.start();
                break;
                
            case Playing:                           // [5]
                stopButton.setEnabled (true);
                break;
                
            case Stopping:                          // [6]
                transportSource.stop();
                break;
        }
    }
}

void openButtonClicked()
{
    FileChooser chooser ("Select a Wave file to play...",
                         {},
                         "*.wav");                                        // [7]
    
    if (chooser.browseForFileToOpen())                                    // [8]
    {
        auto file = chooser.getResult();                                  // [9]
        auto* reader = formatManager.createReaderFor (file);              // [10]
        
        if (reader != nullptr)
        {
            std::unique_ptr<AudioFormatReaderSource> newSource (new AudioFormatReaderSource (reader, true)); // [11]
            transportSource.setSource (newSource.get(), 0, nullptr, reader->sampleRate);                     // [12]
            playButton.setEnabled (true);                                                                    // [13]
            readerSource.reset (newSource.release());                                                        // [14]
        }
    }
}

void playButtonClicked()
{
    changeState (Starting);
}

void stopButtonClicked()
{
    changeState (Stopping);
}
