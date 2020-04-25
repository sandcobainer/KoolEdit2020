/*
  ==============================================================================

    SpectrogramProcessor.h
    Created: 7 Feb 2020 2:29:30pm
    Author:  sophi

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "AudioProcessingComponent.h"

class SpectrogramProcessor : public ChangeListener
{
public:
    SpectrogramProcessor(AudioProcessingComponent& c):
    apc(c),
    forwardFFT (fftOrder),
    matrixRawPointer(nullptr),
    numRows(0),
    numColumns(0),
    isMatrixInitialized(false)
    {
        apc.audioBufferChanged.addChangeListener(this);
    }
    ~SpectrogramProcessor ()
    {
        if (isMatrixInitialized)
            releaseMemForMatrix();
    }

    enum
    {
        fftOrder = 10,
        fftSize  = 1 << fftOrder
    };

    int getFftSize()
    {
        return static_cast<int>(fftSize);
    }

    const float* getReadPointer()
    {
        return const_cast<float*> (matrixRawPointer);
    }

    int getNumRows()
    {
        return numRows;
    }

    int getNumColumns()
    {
        return numColumns;
    }

    bool isSpectrogramReady()
    {
        return isMatrixInitialized;
    }

    void changeListenerCallback (ChangeBroadcaster* source) override
    {
        int numSamples;
        int cols = fftSize * 2;
        // TODO: it only takes channel 0
        auto readPointer = apc.getAudioReadPointer(0, numSamples);
        releaseMemForMatrix();
        // the size of each row must be fftSize*2
        allocMemForMatrix(static_cast<int>(numSamples/fftSize), cols);
        zeromem(matrixRawPointer, sizeof(float)*getNumRows()*getNumColumns());
        for (int row=0; row<getNumRows(); row++)
        {
            // [----------]  <- [*****]
            // [*****-----]
            memcpy(matrixRawPointer+row*cols, readPointer+row*fftSize, sizeof(float)*fftSize);
            forwardFFT.performFrequencyOnlyForwardTransform(matrixRawPointer+row*cols);
        }
        spectrogramReady.sendChangeMessage();
    }

    ChangeBroadcaster spectrogramReady;

private:
    void allocMemForMatrix(int numRows, int numColumns)
    {
        matrixRawPointer = new float[numRows*numColumns];
        this->numRows = numRows;
        this->numColumns = numColumns;
        isMatrixInitialized = true;
    }

    void releaseMemForMatrix()
    {
        delete  matrixRawPointer;
        matrixRawPointer = nullptr;
        numRows = 0;
        numColumns = 0;
        isMatrixInitialized = false;
    }

    AudioProcessingComponent& apc;
    dsp::FFT forwardFFT;

    float* matrixRawPointer; // spectrogram matrix, raw*col, each raw represents a block
    int numRows;
    int numColumns;
    bool isMatrixInitialized;
};
