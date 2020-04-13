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
numChannels(0),
numBlockSamples(0),
sampleRate(0.f),
numAudioSamples(0),
currentPosition(0),
startPos(0),
endPos(0)
{
    formatManager.registerBasicFormats();
}

AudioProcessingComponent::~AudioProcessingComponent()  {
    shutdownAudio();
}

void AudioProcessingComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate) 
{
    // allocate memory for JUCE audio buffer
    numBlockSamples = samplesPerBlockExpected;
    audioBlockBuffer.setSize(numChannels, samplesPerBlockExpected);
    audioBlockBuffer.clear();
    // TODO: the playback speed is not correct if the device sample rate doesn't match the audio sample rate
    //auto audioDeviceSetup = deviceManager.getAudioDeviceSetup();
    //audioDeviceSetup.sampleRate = sampleRate;
    //deviceManager.setAudioDeviceSetup(audioDeviceSetup, true);
}

void AudioProcessingComponent::releaseResources() 
{
}

void AudioProcessingComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) 
{
    if (state == Playing)
    {
        auto numInputChannels = audioBuffer.getNumChannels();
        auto numOutputChannels = bufferToFill.buffer->getNumChannels();

        auto outputSamplesRemaining = bufferToFill.numSamples;
        auto outputSamplesOffset = bufferToFill.startSample;

        while (outputSamplesRemaining > 0)
        {
            auto bufferSamplesRemaining = endPos - currentPosition;
            auto samplesThisTime = jmin (outputSamplesRemaining, bufferSamplesRemaining);

            for (auto channel = 0; channel < numOutputChannels; ++channel)
            {
                bufferToFill.buffer->copyFrom (channel,
                                               outputSamplesOffset,
                                               audioBuffer,
                                               channel % numInputChannels,
                                               currentPosition,
                                               samplesThisTime);
                audioBlockBuffer.copyFrom(0, 0, *(bufferToFill.buffer), 0, 0, samplesThisTime);
                blockReady.sendChangeMessage();
            }

            outputSamplesRemaining -= samplesThisTime;
            outputSamplesOffset += samplesThisTime;
            currentPosition += samplesThisTime;

            if (currentPosition == endPos)
            {
                bufferToFill.clearActiveBufferRegion();
                stopRequested();
                break;
            }
        }
    }
}

//---------------------------------AUDIO BUFFER HANDLING--------------------------------------

void AudioProcessingComponent::fillAudioBlockBuffer(const float* channelData, int numChannel)
{
    float* writePointer = audioBlockBuffer.getWritePointer(numChannel);
    for (auto i = 0; i < numBlockSamples; ++i)
        writePointer[i] = channelData[i];
}

const float* AudioProcessingComponent::getAudioBlockReadPointer(int numChannel, int &numAudioSamples) // public
{
    numAudioSamples = this->numBlockSamples;
    return audioBlockBuffer.getReadPointer(numChannel);
}

const float* AudioProcessingComponent::getAudioReadPointer(int numChannel, int &numAudioSamples) // public
{
    numAudioSamples = this->numAudioSamples;
    return audioBuffer.getReadPointer(numChannel);
}

float* AudioProcessingComponent::getAudioWritePointer(int numChannel, int &numAudioSamples) // public
{
    numAudioSamples = this->numAudioSamples;
    return audioBuffer.getWritePointer(numChannel);
}

double AudioProcessingComponent::getSampleRate()
{
    return sampleRate;
}

int AudioProcessingComponent::getNumChannels()
{
    return audioBuffer.getNumChannels();
}

const AudioBuffer<float>* AudioProcessingComponent::getAudioBuffer()
{
    return &audioBuffer;
}

//-------------------------------TRANSPORT STATE HANDLING-------------------------------------
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
                currentPosition = startPos;
                state = Stopped;
                break;
                
            case Starting:                          
                prepareToPlay(512, sampleRate); // TODO: block size is hard coded here
                state = Playing;
                break;
                
            case Playing: //THIS CASE IS MAINLY FOR GUI/TOOLBARIF INFORMATION ***KEEP***
                state = Playing;
                break;
                
            case Stopping:                          
                releaseResources();
                currentPosition = startPos;
                state = Stopped;
                break;
            
            case Pausing:
                releaseResources();
                state = Paused;
                break;

            case Paused: //THIS CASE IS MAINLY FOR GUI/TOOLBARIF INFORMATION ***KEEP***
                state = Paused;
                break;
        }
        
        transportState.sendChangeMessage(); //send notification of state change
    }
}

double AudioProcessingComponent::getCurrentPositionInS()
{
    return currentPosition / sampleRate;
}

void AudioProcessingComponent::setPositionInS(double newPosition)
{
    currentPosition = (int) (newPosition * sampleRate);
}

double AudioProcessingComponent::getLengthInS()
{
    return numAudioSamples / sampleRate;
}

void AudioProcessingComponent::setMarkersInS(double startMarker, double endMarker)
{
    startPos = (int)(startMarker * sampleRate);
    endPos = (int)(endMarker * sampleRate);
}

const double AudioProcessingComponent::getMarkerInS(String marker)
{
    if (marker == "start")
        return startPos / sampleRate;
    else if (marker == "end")
        return endPos / sampleRate;
    else
        return -1;
}

void AudioProcessingComponent::muteMarkedRegion ()
{
    int numSamples = 0;
    float *writePointer = nullptr;
    for (int c=0; c<numChannels; c++)
    {
        writePointer = getAudioWritePointer(c, numSamples);

        for (int i=startPos; i<=endPos; i++)
            writePointer[i] = 0;
    }
    fileLoaded.sendChangeMessage();
}

//-----------------------------BUTTON PRESS HANDLING-------------------------------------------
void AudioProcessingComponent::loadFile(File file)
{
    shutdownAudio();
    auto* reader = formatManager.createReaderFor (file);
    if (reader != nullptr)
    {
        // read the entire audio into audioBuffer
        numAudioSamples = reader->lengthInSamples;
        audioBuffer.setSize(reader->numChannels, numAudioSamples); // TODO: There's a precision losing warning
        reader->read(&audioBuffer, 0, reader->lengthInSamples, 0, true, true);

        // set audio channels
        numChannels = reader->numChannels;
        setAudioChannels(0, reader->numChannels);

        // set sample rate
        sampleRate = reader->sampleRate;
        fileLoaded.sendChangeMessage();

        //initialize markers
        startPos = 0;
        endPos = numAudioSamples;
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
