/*
  ==============================================================================

    UndoStack.h
    Created: 19 Apr 2020 10:52:01am
    Author:  Yilin Zhang

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class UndoStackAudioBuffer
{
public:
    UndoStackAudioBuffer(AudioBuffer<float> audioBuffer, int startChannel, int startSample)
    {
        this->audioBuffer = audioBuffer;
        this->startChannel = startChannel;
        this->startSample = startSample;
    }

    const AudioBuffer<float>* getAudioBuffer()
    {
        return &audioBuffer;
    }

    int getStartSample()
    {
        return startSample;
    }

    int getStartChannel()
    {
        return startChannel;
    }

private:
    AudioBuffer<float> audioBuffer;
    int startSample;
    int startChannel;
};

/*
 * Audio buffers should always be pushed to stack in pairs (push the buffer before editing and after editing).
 * It only supports inplace operations for now.
 */

class UndoStack
{
public:
    UndoStack():stackPos(-1), isJustUndid(false), isJustRedid(false), maxUndoTimes(0) {}
    ~UndoStack(){}

    void setMaxUndoTimes(int maxUndoTimes)
    {
        if (maxUndoTimes < 0)
            return;
        this->maxUndoTimes = maxUndoTimes;
    }

    /*! Push undo-stack audio buffer into stack
        @param UndoStackAudioBuffer the audio buffer before editing
        @param UndoStackAudioBuffer the audio buffer after editing
    */
    void pushUndoStackAudioBuffers(UndoStackAudioBuffer beforeEditBuffer, UndoStackAudioBuffer afterEditBuffer)
    {
        if (!isEmpty() && stackPos < getLength()-1) // clear the operations that don't need to be stored
        {
            if (stackPos % 2 == 0)
                stackPos--;
            for (int i=getLength()-1; i>stackPos; i--)
                undoStack.pop_back();
        }
        undoStack.push_back(beforeEditBuffer);
        undoStack.push_back(afterEditBuffer);
        if (getLength() > maxUndoTimes*2) // remove the buffers at the bottom if the length is longer than the limit.
        {
            std::reverse(undoStack.begin(),undoStack.end());
            undoStack.pop_back();
            undoStack.pop_back();
            std::reverse(undoStack.begin(),undoStack.end());
        }
        else
            stackPos += 2;
        isJustUndid = false;
        isJustRedid = false;
    }

    /*! Undo an operation
        @param AudioBuffer<float>& a reference to the audio buffer
    */
    void undo (AudioBuffer<float>& audioBuffer)
    {
        if (!isUndoEnabled())
            return;

        int newStackPos;
        if (isJustUndid)
            newStackPos = stackPos - 2;
        else
            newStackPos = stackPos - 1;

        UndoStackAudioBuffer* stackElem = getUndoStackAudioBuffer(newStackPos);
        const AudioBuffer<float>* undoAudioBuffer = stackElem->getAudioBuffer();
        int startSample = stackElem->getStartSample();
        int startChannel = stackElem->getStartChannel();

        for (int channel=startChannel; channel<undoAudioBuffer->getNumChannels(); channel++)
            audioBuffer.copyFrom(channel, startSample, *undoAudioBuffer, channel-startChannel, 0, undoAudioBuffer->getNumSamples());

        isJustUndid = true;
        isJustRedid = false;
        stackPos = newStackPos;
    }

    /*! Redo an operation
        @param AudioBuffer<float>& a reference to the audio buffer
    */
    void redo (AudioBuffer<float>& audioBuffer)
    {
        if (!isRedoEnabled())
            return;

        int newStackPos;
        if (isJustRedid)
            newStackPos = stackPos + 2;
        else
            newStackPos = stackPos + 1;

        UndoStackAudioBuffer* stackElem = getUndoStackAudioBuffer(newStackPos);
        const AudioBuffer<float>* undoAudioBuffer = stackElem->getAudioBuffer();
        int startSample = stackElem->getStartSample();
        int startChannel = stackElem->getStartChannel();

        for (int channel=startChannel; channel<undoAudioBuffer->getNumChannels(); channel++)
            audioBuffer.copyFrom(channel, startSample, *undoAudioBuffer, channel-startChannel, 0, undoAudioBuffer->getNumSamples());

        isJustRedid = true;
        isJustUndid = false;
        stackPos = newStackPos;
    }

    void reset()
    {
        stackPos = -1;
        isJustUndid = false;
        isJustRedid = false;
        undoStack.clear();
    }

    bool isEmpty()
    {
        return undoStack.empty();
    }

    int getLength()
    {
        return static_cast<int>(undoStack.size());
    }

    int getStackPos()
    {
        return stackPos;
    }

    bool isUndoEnabled()
    {
        return (!isJustUndid && getStackPos() >= 1) || (isJustUndid && getStackPos() >= 2);
    }

    bool isRedoEnabled()
    {
        return (!isJustRedid && (getLength() - 1 - stackPos) >= 1) || (isJustRedid && (getLength() - 1 - stackPos) >= 2);
    }
private:
    UndoStackAudioBuffer* getUndoStackAudioBuffer(int position)
    {
        return &undoStack[position];
    }

    std::vector<UndoStackAudioBuffer> undoStack;
    int stackPos;
    bool isJustUndid;
    bool isJustRedid;
    int maxUndoTimes;
};
