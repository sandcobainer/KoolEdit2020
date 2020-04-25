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

class UndoStack
{
public:
    UndoStack(int maxUndoTimes = 5):
    stackPos(-1),
    mode(RedoMode),
    maxUndoTimes(0)
    {
        this->maxUndoTimes = maxUndoTimes;
    }
    ~UndoStack(){}

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
        if (!isEmpty()) // clear the operations that don't need to be stored
        {
            if (mode == RedoMode && !isPosAtTop()) // if it's in redo mode, the current record will be remained
            {
                for (int i=getLength()-1; i>stackPos; i--)
                    undoStack.pop_back();
            }
            else if (mode == UndoMode) // if it's in undo mode, the current record should be removed
            {
                stackPos--;
                for (int i=getLength()-1; i>stackPos; i--)
                    undoStack.pop_back();
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
