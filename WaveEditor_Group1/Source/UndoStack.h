/*
  ==============================================================================

    UndoStack.h
    Created: 19 Apr 2020 10:52:01am
    Author:  Yilin Zhang

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Utils.h"

class UndoRecord
{
public:
    UndoRecord(AudioBuffer<float> bufferBeforeOperation, AudioBuffer<float> bufferAfterOperation, int startChannel, int startSample)
    {
        this->bufferBeforeOperation = bufferBeforeOperation;
        this->bufferAfterOperation = bufferAfterOperation;
        this->startChannel = startChannel;
        this->startSample = startSample;
    }
    ~UndoRecord(){};

    enum BufferType
    {
        UndoBuffer,
        RedoBuffer
    };

    int getNumSamples(BufferType bufferType)
    {
        switch (bufferType)
        {
            case UndoBuffer:
                return bufferBeforeOperation.getNumSamples();
            case RedoBuffer:
                return bufferAfterOperation.getNumSamples();
            default:
                return 0;
        }
    }

    int getNumChannels(BufferType bufferType)
    {
        switch (bufferType)
        {
            case UndoBuffer:
                return bufferBeforeOperation.getNumChannels();
            case RedoBuffer:
                return bufferAfterOperation.getNumChannels();
            default:
                return 0;
        }
    }

    int getStartChannel()
    {
        return startChannel;
    }

    int getStartSample()
    {
        return startSample;
    }

    AudioBuffer<float>* getAudioBuffer(BufferType bufferType)
    {
        switch (bufferType)
        {
            case UndoBuffer:
                return &bufferBeforeOperation;
            case RedoBuffer:
                return &bufferAfterOperation;
            default:
                return nullptr;
        }
    }

private:
    AudioBuffer<float> bufferBeforeOperation;
    AudioBuffer<float> bufferAfterOperation;
    int startChannel;
    int startSample;
};

class NewUndoStack
{
public:
    NewUndoStack(int maxUndoTimes = 5):
    stackPos(-1),
    mode(RedoMode),
    maxUndoTimes(0)
    {
        this->maxUndoTimes = maxUndoTimes;
    }
    ~NewUndoStack(){}

    enum Mode
    {
        UndoMode,
        RedoMode
    };


    void reset()
    {
        stackPos = -1;
        maxUndoTimes = 0;
        mode = RedoMode;
        undoStack.clear();
    }

    int getLength()
    {
        return static_cast<int>(undoStack.size());
    }

    int getStackPos()
    {
        return stackPos;
    }

    int getMaxUndoTimes()
    {
        return maxUndoTimes;
    }

    void setMaxUndoTimes(int newMaxUndoTimes)
    {
        maxUndoTimes = newMaxUndoTimes;
    }

    bool isEmpty()
    {
        return undoStack.empty();
    }

    bool isPosAtTop()
    {
        return stackPos == (getLength() - 1);
    }

    bool isUndoEnabled()
    {
         return !isEmpty() && ((mode == RedoMode) || (mode == UndoMode && getStackPos()>0));
    }

    bool isRedoEnabled()
    {
        return !isEmpty() && ((mode == UndoMode) || (mode == RedoMode && !isPosAtTop()));
    }

    void addRecord(UndoRecord undoRecord)
    {
        if (!isEmpty() && !isPosAtTop()) // clear the operations that don't need to be stored
        {
            if (mode == RedoMode) // if it's in redo mode, the current record will be remained
            {
                for (int i=getLength()-1; i>stackPos; i--)
                    undoStack.pop_back();
            }
            else if (mode == UndoMode) // if it's in undo mode, the current record should be removed
            {
                for (int i=getLength()-1; i>stackPos-1; i--)
                    undoStack.pop_back();
                stackPos--;
            }
        }
        if (getLength() >= maxUndoTimes) // remove the record at bottom to keep the max length
        {
            auto oldLength = getLength();
            std::reverse(undoStack.begin(),undoStack.end());
            for (int i=oldLength; i>=maxUndoTimes; i--)
            {
                undoStack.pop_back();
                stackPos--;
            }
            std::reverse(undoStack.begin(),undoStack.end());
        }
        undoStack.push_back(undoRecord);
        mode = RedoMode;
        stackPos++;
    }

    void undo(AudioBuffer<float>& audioBuffer)
    {
        if(!isUndoEnabled())
            return;

        if (mode == UndoMode)
            stackPos--;

        auto record = getRecord(stackPos);
        auto undoBuffer = record->getAudioBuffer(UndoRecord::UndoBuffer);
        auto startSample = record->getStartSample();
        auto replaceNumSamples = record->getNumSamples(UndoRecord::RedoBuffer);
        AudioBufferUtils<float>::replaceRegion(audioBuffer, *undoBuffer, startSample, replaceNumSamples);

        mode = UndoMode;
    }

    void redo(AudioBuffer<float>& audioBuffer)
    {
        if(!isRedoEnabled())
            return;

        if (mode == RedoMode)
            stackPos++;

        auto record = getRecord(stackPos);
        auto redoBuffer = record->getAudioBuffer(UndoRecord::RedoBuffer);
        auto startSample = record->getStartSample();
        auto replaceNumSamples = record->getNumSamples(UndoRecord::UndoBuffer);
        AudioBufferUtils<float>::replaceRegion(audioBuffer, *redoBuffer, startSample, replaceNumSamples);

        mode = RedoMode;
    }


private:

    UndoRecord* getRecord(int position)
    {
        return &undoStack[position];
    }

    int stackPos;
    Mode mode;
    int maxUndoTimes;
    std::vector<UndoRecord> undoStack;
};

///////////////////////////////////////////////////////////////
// Deprecated classes

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
