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
AudioProcessingComponent::AudioProcessingComponent():state(Stopped),
                                                     thumbnailCache (5),
                                                     thumbnail (512, formatManager, thumbnailCache)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    formatManager.registerBasicFormats();
    transportSource.addChangeListener (this);
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

//-------------------------------TRANSPORT STATE HANDLING-------------------------------------
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
    if (source == &thumbnail)
    {
        thumbnail.sendChangeMessage();
    }
}

const String AudioProcessingComponent::getState () 
{
    String sState;
    switch (state)
    {
        case Stopped:
            sState = "Stopped";
            break;
        case Playing:
            sState = "Playing";
            break;
        case Paused:
            sState = "Paused";
            break;
    }
    
    return sState;
}

void AudioProcessingComponent::setState (TransportState newState)
{
    if (state != newState)
    {

        switch (newState)
        {
            case Stopped:                           
                currentPosition = 0.0;
                transportSource.setPosition(0.0);
                break;
                
            case Starting:                          
                transportSource.setPosition(currentPosition);
                transportSource.start();
                break;
                
            case Playing: //THIS CASE IS MAINLY FOR GUI/TOOLBARIF INFORMATION ***KEEP***                          
                break;
                
            case Stopping:                          
                currentPosition = 0.0;
                transportSource.stop();
                break;
            
            case Pausing:
                currentPosition = transportSource.getCurrentPosition();
                transportSource.stop();
                break;

            case Paused: //THIS CASE IS MAINLY FOR GUI/TOOLBARIF INFORMATION ***KEEP***
                break;
        }
        
        state = newState;
        transportSource.sendChangeMessage(); //send notification of state change
    }
}

//-----------------------------BUTTON PRESS HANDLING-------------------------------------------
void AudioProcessingComponent::loadFile(File file)
{
        auto* reader = formatManager.createReaderFor (file);              
        if (reader != nullptr)
        {
            std::unique_ptr<AudioFormatReaderSource> newSource (new AudioFormatReaderSource (reader, true)); 
            transportSource.setSource (newSource.get(), 0, nullptr, reader->sampleRate);
            thumbnail.setSource (new FileInputSource (file)); 
            readerSource.reset (newSource.release());                                                        
        }
}

void AudioProcessingComponent::playRequested()
{
    if((state == Stopped) || (state == Paused))
        setState (Starting); //start transport
}

void AudioProcessingComponent::stopRequested()
{
    if (state == Paused)
        setState(Stopped); //reset transport position to 0
    else
        setState(Stopping); //stop transport
}

void AudioProcessingComponent::pauseRequested()
{
    if (state == Playing)
        setState(Pausing); //stop transport, save current position
}
