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
AudioProcessingComponent::AudioProcessingComponent():
state(Stopped),
maxNumChannels(2), // TODO: The channel number is fixed here
numSamples(0),
thumbnailCache (5),
thumbnail (512, formatManager, thumbnailCache)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    formatManager.registerBasicFormats();
    transportSource.addChangeListener (this);
    currentPosition = 0.0;
    
    setAudioChannels (0, maxNumChannels);
    startTimer (40);
}

AudioProcessingComponent::~AudioProcessingComponent()  {
    shutdownAudio();
}

void AudioProcessingComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate) 
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    // allocate memory for JUCE audio buffer
    numSamples = samplesPerBlockExpected;
    audioBlockBuffer.setSize(maxNumChannels, samplesPerBlockExpected);
    //audioBlockBuffer = new AudioBuffer<float>(maxNumChannels, samplesPerBlockExpected);
    //audioBlockBuffer->clear(); // initialize the buffer with 0 values
}

void AudioProcessingComponent::releaseResources() 
{
    transportSource.releaseResources();
}

void AudioProcessingComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) 
{
    transportSource.getNextAudioBlock(bufferToFill);
    int numChannels = 0;
    const float* channelData = nullptr;
    if (readerSource.get() != nullptr)
    {
        numChannels = bufferToFill.buffer->getNumChannels();
        for (auto c = 0; c < numChannels; ++c)
        {
            channelData = bufferToFill.buffer->getReadPointer (0, bufferToFill.startSample);
            numSamples = bufferToFill.buffer->getNumSamples();
            //fillAudioSampleBuffer(channelData, c, numSamples);
            fillAudioSampleBuffer(audioBlockBuffer.getWritePointer(c), channelData, numSamples);
        }

        sendChangeMessage(); // TODO: maybe try send messages instead
    } else {
        bufferToFill.clearActiveBufferRegion();
    }
}

//---------------------------------AUDIO BUFFER HANDLING--------------------------------------

void AudioProcessingComponent::fillAudioSampleBuffer(float* bufferWritePointer, const float* channelData, int numSamples)
{
    for (auto i = 0; i < numSamples; ++i)
        bufferWritePointer[i] = channelData[i];
}

const float* AudioProcessingComponent::getAudioBlockBuffer(int numChannel, int &numSamples)
{
    numSamples = this->numSamples;
    return audioBlockBuffer.getReadPointer(numChannel);
}

void AudioProcessingComponent::timerCallback()
{
    thumbnail.sendChangeMessage();
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
