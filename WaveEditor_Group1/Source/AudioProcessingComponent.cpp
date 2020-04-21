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
interpolators(nullptr),
sampleRate(0.f),
deviceSampleRate(0.f),
currentPos(0),
markerStartPos(0),
markerEndPos(0)
{
    formatManager.registerBasicFormats();
}

AudioProcessingComponent::~AudioProcessingComponent()  {
    shutdownAudio();
    if (fileLoaded)
    {
        for (int channel=0; channel<getNumChannels(); channel++)
        {
            delete interpolators[channel];
        }
        delete[] interpolators;
        interpolators = nullptr;
    }
}

void AudioProcessingComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate) 
{
    audioBlockBuffer.setSize(getNumChannels(), samplesPerBlockExpected);
    audioBlockBuffer.clear();
    deviceSampleRate = deviceManager.getAudioDeviceSetup().sampleRate;
}

void AudioProcessingComponent::releaseResources() 
{
}

void AudioProcessingComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) 
{
    if (deviceSampleRate == 0)
        return;

    double sampleRateRatio = deviceSampleRate / sampleRate;

    if (state == Playing)
    {
        auto numInputChannels = audioBuffer.getNumChannels();
        auto numOutputChannels = bufferToFill.buffer->getNumChannels();

        auto outputSamplesRemaining = bufferToFill.numSamples;
        auto outputSamplesOffset = bufferToFill.startSample;

        while (outputSamplesRemaining > 0)
        {
            auto bufferSamplesRemaining = markerEndPos - currentPos;
            auto outputSamplesThisTime = jmin (outputSamplesRemaining,
                                               static_cast<int>(bufferSamplesRemaining*sampleRateRatio));
            auto inputSamplesThisTime = static_cast<int>(outputSamplesThisTime / sampleRateRatio);

            for (auto channel = 0; channel < numOutputChannels; channel++)
            {
                interpolators[channel]->process(
                        1/sampleRateRatio,
                        audioBuffer.getReadPointer(channel, currentPos),
                        bufferToFill.buffer->getWritePointer(channel, outputSamplesOffset),
                        outputSamplesThisTime);
            }
            audioBlockBuffer.copyFrom(0, 0, audioBuffer, 0, currentPos, inputSamplesThisTime);
            blockReady.sendChangeMessage();

            outputSamplesRemaining -= outputSamplesThisTime;
            outputSamplesOffset += outputSamplesThisTime;
            currentPos += inputSamplesThisTime;

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
const float* AudioProcessingComponent::getAudioBlockReadPointer(int numChannel, int &numAudioSamples) // public
{
    numAudioSamples = audioBlockBuffer.getNumSamples();
    return audioBlockBuffer.getReadPointer(numChannel);
}

const float* AudioProcessingComponent::getAudioReadPointer(int numChannel, int &numAudioSamples) // public
{
    numAudioSamples = getNumSamples();
    return audioBuffer.getReadPointer(numChannel);
}

float* AudioProcessingComponent::getAudioWritePointer(int numChannel, int &numAudioSamples) // public
{
    numAudioSamples = getNumSamples();
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

int AudioProcessingComponent::getNumSamples()
{
    return audioBuffer.getNumSamples();
}

const AudioBuffer<float>* AudioProcessingComponent::getAudioBuffer()
{
    return &audioBuffer;
}

void AudioProcessingComponent::muteMarkedRegion ()
{
    int numSamples = 0;
    float *writePointer = nullptr;

    auto beforeEditBuffer = getUndoBufferFromMarkedRegion();

    for (int channel=0; channel<getNumChannels(); channel++)
    {
        writePointer = getAudioWritePointer(channel, numSamples);

        for (int i=markerStartPos; i<=markerEndPos; i++)
            writePointer[i] = 0;
    }

    auto afterEditBuffer = getUndoBufferFromMarkedRegion();

    undoStack.pushUndoStackAudioBuffers(beforeEditBuffer, afterEditBuffer);

    audioBufferChanged.sendChangeMessage();
}

void AudioProcessingComponent::undo()
{
    if (!isUndoEnabled())
        return;
    undoStack.undo(audioBuffer);
    audioBufferChanged.sendChangeMessage();
}

void AudioProcessingComponent::redo()
{
    if(!isRedoEnabled())
        return;
    undoStack.redo(audioBuffer);
    audioBufferChanged.sendChangeMessage();
}

bool AudioProcessingComponent::isUndoEnabled()
{
    return undoStack.isUndoEnabled();
}

bool AudioProcessingComponent::isRedoEnabled()
{
    return undoStack.isRedoEnabled();
}

UndoStackAudioBuffer AudioProcessingComponent::getUndoBufferFromSamplesInRange(AudioBuffer<float> &audioBuffer, int startSample,
                                                                int endSample, int startChannel, int numChannels)
{
    int numSamples = endSample - startSample + 1;
    AudioBuffer<float> newAudioBuffer(numChannels, numSamples);
    for (int channel=startChannel; channel<startChannel+numChannels; channel++)
        newAudioBuffer.copyFrom(channel-startChannel, 0, audioBuffer, channel, startSample, numSamples);
    class UndoStackAudioBuffer undoBuffer {newAudioBuffer, startChannel, startSample};
    return undoBuffer;
}

UndoStackAudioBuffer AudioProcessingComponent::getUndoBufferFromMarkedRegion()
{
    auto undoBuffer = getUndoBufferFromSamplesInRange(audioBuffer,
            markerStartPos, markerEndPos, 0, getNumChannels());
    return undoBuffer;
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

//-------------------------------POSITION HANDLING-------------------------------------
void AudioProcessingComponent::setPositionInS(AudioProcessingComponent::PositionType positionType, double newPosition)
{
    int position = static_cast<int>(newPosition * sampleRate);

    // make sure the position is in the range
    if (position < 0)
        position = 0;
    else if (position >= getNumSamples())
        position = getNumSamples() - 1;

    switch (positionType)
    {
        case Cursor:
            currentPos = position;
            break;
        case MarkerStart:
            markerStartPos = position;
            break;
        case MarkerEnd:
            markerEndPos = position;
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
    return getNumSamples() / sampleRate;
}

//-----------------------------BUTTON PRESS HANDLING-------------------------------------------
void AudioProcessingComponent::loadFile(File file)
{
    shutdownAudio();
    auto* reader = formatManager.createReaderFor (file);
    if (reader != nullptr)
    {
        // if there's a file has been loaded, clean the interpolators
        if (fileLoaded)
        {
            for (int channel=0; channel<getNumChannels(); channel++)
            {
                delete interpolators[channel];
            }
            delete[] interpolators;
            interpolators = nullptr;
        }

        // read the entire audio into audioBuffer
        auto numSamples = reader->lengthInSamples;
        auto numChannels = reader->numChannels;
        audioBuffer.setSize(numChannels, numSamples); // TODO: There's a precision losing warning
        reader->read(&audioBuffer, 0, numSamples, 0, true, true);

        // set audio channels
        setAudioChannels(0, numChannels);

        // set sample rate
        sampleRate = reader->sampleRate;

        // create interpolators
        interpolators = new CatmullRomInterpolator*[numChannels];
        for (int channel=0; channel<numChannels; channel++)
            interpolators[channel] = new CatmullRomInterpolator();

        //initialize markers
        markerStartPos = 0;
        markerEndPos = numSamples;

        audioBufferChanged.sendChangeMessage();
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
