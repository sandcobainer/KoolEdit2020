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
fileLoaded(false),
numChannels(0),
numBlockSamples(0),
sampleRate(0.f),
numAudioSamples(0),
currentPos(0),
markerStartPos(0),
markerEndPos(0)
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
            auto bufferSamplesRemaining = markerEndPos - currentPos;
            auto samplesThisTime = jmin (outputSamplesRemaining, bufferSamplesRemaining);

            for (auto channel = 0; channel < numOutputChannels; ++channel)
            {
                bufferToFill.buffer->copyFrom (channel,
                                               outputSamplesOffset,
                                               audioBuffer,
                                               channel % numInputChannels,
                                               currentPos,
                                               samplesThisTime);
                audioBlockBuffer.copyFrom(0, 0, *(bufferToFill.buffer), 0, 0, samplesThisTime);
                blockReady.sendChangeMessage();
            }

            outputSamplesRemaining -= samplesThisTime;
            outputSamplesOffset += samplesThisTime;
            currentPos += samplesThisTime;

            if (currentPos == markerEndPos)
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
AudioProcessingComponent::TransportState AudioProcessingComponent::getState ()
{
    return state;
}

void AudioProcessingComponent::setState (TransportState newState)
{
    if (state != newState)
    {
        switch (newState)
        {
            case Stopped:                           
                currentPos = markerStartPos;
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
                currentPos = markerStartPos;
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

void AudioProcessingComponent::setPositionInS(AudioProcessingComponent::PositionType positionType, double newPosition)
{
    switch (positionType)
    {
        case Cursor:
            currentPos = static_cast<int>(newPosition * sampleRate);
            break;
        case MarkerStart:
            markerStartPos = static_cast<int>(newPosition * sampleRate);
            break;
        case MarkerEnd:
            markerEndPos = static_cast<int>(newPosition * sampleRate);
            break;
    }
}

double AudioProcessingComponent::getPositionInS(AudioProcessingComponent::PositionType positionType)
{

    switch (positionType)
    {
        case Cursor:
            return currentPos / sampleRate;
        case MarkerStart:
            return markerStartPos / sampleRate;
        case MarkerEnd:
            return markerEndPos / sampleRate;
        default:
            return 0.0f;
    }
}

double AudioProcessingComponent::getLengthInS()
{
    return numAudioSamples / sampleRate;
}

void AudioProcessingComponent::muteMarkedRegion ()
{
    int numSamples = 0;
    float *writePointer = nullptr;
    for (int c=0; c<numChannels; c++)
    {
        writePointer = getAudioWritePointer(c, numSamples);

        for (int i=markerStartPos; i<=markerEndPos; i++)
            writePointer[i] = 0;
    }
    audioBufferChanged.sendChangeMessage();
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
        audioBufferChanged.sendChangeMessage();

        //initialize markers
        markerStartPos = 0;
        markerEndPos = numAudioSamples;

        fileLoaded = true;
    }
}

void AudioProcessingComponent::saveFile(File file)
{
    // doesn't do anything if no file is loaded
    if (!fileLoaded)
        return;

    WavAudioFormat format;
    std::unique_ptr<AudioFormatWriter> writer;
    writer.reset (format.createWriterFor (new FileOutputStream (file),
                                          sampleRate,
                                          audioBuffer.getNumChannels(),
                                          24,
                                          {},
                                          0));
    if (writer != nullptr)
        writer->writeFromAudioSampleBuffer (audioBuffer, 0, audioBuffer.getNumSamples());
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
