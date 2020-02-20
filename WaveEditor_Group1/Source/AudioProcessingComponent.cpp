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
    currentPosition = 0.0;
    
    setAudioChannels (0, 2);
}

AudioProcessingComponent::~AudioProcessingComponent()  {
    shutdownAudio();
}

void AudioProcessingComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate) 
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void AudioProcessingComponent::releaseResources() 
{
    transportSource.releaseResources();    
}

void AudioProcessingComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) 
{
    if (readerSource.get() == nullptr)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    transportSource.getNextAudioBlock(bufferToFill);
}

void AudioProcessingComponent::changeListenerCallback (ChangeBroadcaster* source) 
{
    if (source == &transportSource)
    {
        if (transportSource.isPlaying())
            setState(Playing);
        else if ((state == Stopping) || (state == Playing))
            setState(Stopped);
        else if (state == Pausing)
            setState(Paused);
    }
}

void AudioProcessingComponent::getState (TransportState newState) {
    
}

void AudioProcessingComponent::setState (TransportState newState)
{
    if (state != newState)
    {

        switch (newState)
        {
            case Stopped:                           // [3]
                transportSource.setPosition (0.0);
                break;
                
            case Starting:                          // [4]
                transportSource.setPosition(currentPosition);
                transportSource.start();
                break;
                
            case Playing:                           // [5]
                break;
                
            case Stopping:                          // [6]
                transportSource.stop();
                break;
            
            case Pausing:
                currentPosition = transportSource.getCurrentPosition();
                transportSource.stop();
                break;

            case Paused:
                break;
        }
        
        state = newState;
    }
}

void AudioProcessingComponent::openButtonClicked()
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
            readerSource.reset (newSource.release());                                                        // [14]
        }
    }
}

void AudioProcessingComponent::playButtonClicked()
{
    if((state == Stopped) || (state == Paused))
        setState (Starting);
}

void AudioProcessingComponent::stopButtonClicked()
{
    if (state == Paused)
        setState(Stopped);
    else
        setState(Stopping);
}

void AudioProcessingComponent::pauseButtonClicked()
{
    if (state == Playing)
        setState(Pausing);
}