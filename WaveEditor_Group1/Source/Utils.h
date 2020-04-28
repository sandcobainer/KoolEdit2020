/*
  ==============================================================================

    Utils.h
    Created: 24 Apr 2020 11:43:35am
    Author:  Yilin Zhang

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

template <typename Type>
class AudioBufferUtils
{
public:
    static void deleteRegion (AudioBuffer<Type> &audioBuffer, int startSample, int deleteLength)
    {
        Type* writePointer = nullptr;
        int oldLength = audioBuffer.getNumSamples();
        for (int channel=0; channel<audioBuffer.getNumChannels(); channel++)
        {
            writePointer = audioBuffer.getWritePointer(channel);
            for (int i=startSample; i<oldLength-deleteLength; i++)
                writePointer[i] = writePointer[i+deleteLength];
        }
        audioBuffer.setSize(audioBuffer.getNumChannels(), oldLength-deleteLength, true);
    }

    static void insertRegion (AudioBuffer<Type>& audioBuffer, const AudioBuffer<Type>& insertBuffer, int startSample)
    {
        Type* writePointer = nullptr;
        const Type* readPointer = nullptr;
        int oldLength = audioBuffer.getNumSamples();
        int insertLength = insertBuffer.getNumSamples();
        audioBuffer.setSize(audioBuffer.getNumChannels(), oldLength+insertLength, true);
        for (int channel=0; channel<audioBuffer.getNumChannels(); channel++)
        {
            writePointer = audioBuffer.getWritePointer(channel);
            readPointer = insertBuffer.getReadPointer(channel);
            for (int i=oldLength+insertLength-1; i>=startSample+insertLength; i--)
                writePointer[i] = writePointer[i-insertLength];
            for (int i=startSample; i<startSample+insertLength; i++)
                writePointer[i] = readPointer[i-startSample];
        }
    }

    static void replaceRegion (AudioBuffer<Type>& audioBuffer, const AudioBuffer<Type>& replaceBuffer, int startSample, int replaceLength)
    {
        // TODO: this part can be optimized
        deleteRegion(audioBuffer, startSample, replaceLength);
        insertRegion(audioBuffer, replaceBuffer, startSample);
    }

private:
    AudioBufferUtils(){};
    ~AudioBufferUtils(){};
};

class AudioProcessingUtils
{
public:
    static void mute (float* bufferWritePointer, int startSample, int numSamples)
    {
        for (int i=0; i<numSamples; i++)
            bufferWritePointer[startSample+i] *= 0;
    }

    static void fadeIn (float* bufferWritePointer, int startSample, int numSamples)
    {
        for (int i=0; i<numSamples; i++)
            bufferWritePointer[startSample+i] *= static_cast<float>(i) / static_cast<float>(numSamples-1);
    }

    static void fadeOut (float* bufferWritePointer, int startSample, int numSamples)
    {
        for (int i=0; i<numSamples; i++)
            bufferWritePointer[startSample+i] *= (1- static_cast<float>(i) / static_cast<float>(numSamples-1));
    }

    static void gain (float* bufferWritePointer, int startSample, int numSamples, float gainValue)
    {
        for (int i=0; i<numSamples; i++)
            bufferWritePointer[startSample+i] *= gainValue;
    }

    static std::function<void(float*, int, int)> getGainFunc (float gainValue)
    {
        return [gainValue](float* bufferWritePointer, int startSample, int numSamples) {
            gain(bufferWritePointer, startSample, numSamples, gainValue);
        };
    }
};