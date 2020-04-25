/*
  ==============================================================================

    SpectrogramVisualizer.h
    Created: 7 Feb 2020 2:28:33pm
    Author:  sophi

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "AudioProcessingComponent.h"
#include "SpectrogramProcessor.h"

class SpectrogramVisualizer : public Component,
                              public ChangeListener
{
public:
    SpectrogramVisualizer(AudioProcessingComponent& c) :
    spectrogramImage (Image::RGB, 512, 512, true),
    spectrogramProcessor(c)
    {
        setOpaque (true);
        setSize (700, 500);
        setBounds(0, 100, 700, 500);
        spectrogramProcessor.spectrogramReady.addChangeListener(this);
    }

    ~SpectrogramVisualizer() override
    {
    }

    //==============================================================================
    void changeListenerCallback (ChangeBroadcaster* source) override
    {
        repaint();
    }
    
    //==============================================================================
    void paint (Graphics& g) override
    {
        g.fillAll (Colours::black);
        drawNextLineOfSpectrogram();
        g.setOpacity (1.0f);
        g.drawImage (spectrogramImage, getLocalBounds().toFloat());
    }

    void drawNextLineOfSpectrogram()
    {
        if (!spectrogramProcessor.isSpectrogramReady())
            return;

        auto rightHandEdge = spectrogramImage.getWidth() - 1;
        auto imageHeight   = spectrogramImage.getHeight();

        auto matrixRawPointer = spectrogramProcessor.getReadPointer();
        auto fftSize = spectrogramProcessor.getFftSize();
        auto matrixRows = spectrogramProcessor.getNumRows();
        auto matrixCols = spectrogramProcessor.getNumColumns();
        auto maxLevel = FloatVectorOperations::findMinAndMax (matrixRawPointer, matrixCols*matrixRows);

        float ratio = static_cast<float>(matrixRows) / static_cast<float>(rightHandEdge);

        for (auto x = 0; x < rightHandEdge; ++x)
        {
            for (auto y = 1; y < imageHeight; ++y)
            {
                auto skewedProportionY = 1.0f - std::exp (std::log (y / (float) imageHeight) * 0.2f);
                auto fftDataIndex = jlimit (0, fftSize / 2, (int) (skewedProportionY * (int) fftSize / 2));
                auto level = jmap (
                        matrixRawPointer[static_cast<int>(x*ratio)*matrixCols+fftDataIndex],
                        0.0f,
                        jmax (maxLevel.getEnd(), 1e-5f),
                        0.0f,
                        1.0f);
                spectrogramImage.setPixelAt (x, y, Colour::fromHSV (level, 1.0f, level, 1.0f));
            }
        }
    }

private:
    Image spectrogramImage;
    SpectrogramProcessor spectrogramProcessor;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectrogramVisualizer)
};
