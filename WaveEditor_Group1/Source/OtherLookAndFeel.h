/*
  ==============================================================================

    OtherLookAndFeel.h
    Created: 26 Apr 2020 8:10:49pm
    Author:  sandcobainer

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class OtherLookAndFeel : public LookAndFeel_V3
{
public:
    
    OtherLookAndFeel()
    {
        setColour (Slider::thumbColourId, Colour(128,255,0));
        setColour (Slider::backgroundColourId, Colour(64,64,64));
    }
    void drawTriangle (Graphics& g, float x1, float y1, float x2, float y2, float x3, float y3, Colour fill, Colour outline)
    {
        Path p;
        p.addTriangle (x1, y1, x2, y2, x3, y3);
        g.setColour (fill);
        g.fillPath (p);

        g.setColour (outline);
        g.strokePath (p, PathStrokeType (0.3f));
    }
    
    void drawLinearSlider (Graphics& g,
                            int x, int y, int w, int h,
                            float sliderPos, float minSliderPos, float maxSliderPos,
                            const Slider::SliderStyle style,
                            Slider& slider) override
    {
         g.fillAll (slider.findColour (Slider::backgroundColourId));

         if (style == Slider::LinearBar)
         {
             g.setColour (slider.findColour (Slider::thumbColourId));
             g.fillRect (x, y, (int) sliderPos - x, h);

             g.setColour (slider.findColour (Slider::textBoxTextColourId).withMultipliedAlpha (0.5f));
             g.drawRect (x, y, (int) sliderPos - x, h);
         }
         else
         {
             drawLinearSliderBackground (g, x, y, w, h, sliderPos, minSliderPos, maxSliderPos, style, slider);
             
             g.setColour (slider.findColour (Slider::trackColourId)
                                .withMultipliedAlpha (slider.isEnabled() ? 1.0f : 0.3f));
             x = x-8;
             w = w+16;

             float alpha = 0.35f;
             if (slider.isEnabled())
                 alpha = slider.isMouseOverOrDragging() ? 1.0f : 0.7f;

             const Colour fill (slider.findColour (Slider::thumbColourId).withAlpha (alpha));
             const Colour outline (Colours::black.withAlpha (slider.isEnabled() ? 0.7f : 0.35f));

             if (style == Slider::LinearHorizontal || style == Slider::ThreeValueHorizontal)
             {
                 drawTriangle (g, sliderPos, y + h * 0.9f,
                               sliderPos - 7.0f, y + h * 0.2f,
                               sliderPos + 7.0f, y + h * 0.2f,
                               fill, outline);
             }
         }
     }
    
    void drawLinearSliderBackground (Graphics& g, int x, int y, int width, int height,
                                     float /*sliderPos*/,
                                     float /*minSliderPos*/,
                                     float /*maxSliderPos*/,
                                     const Slider::SliderStyle /*style*/, Slider& slider) override
    {
        const float sliderRadius = (float) (getSliderThumbRadius (slider) + 20);

        const Colour trackColour (Colour(32,32,32));
        const Colour gradCol1 (trackColour.overlaidWith (Colour (slider.isEnabled() ? 0x13000000 : 0x09000000)));
        const Colour gradCol2 (trackColour.overlaidWith (Colour (0x06000000)));
        Path indent;

        if (slider.isHorizontal())
        {
            auto iy = y + height * 0.5f - sliderRadius * 0.5f;
            g.setGradientFill (ColourGradient::vertical (gradCol1, iy, gradCol2, iy + sliderRadius));
            indent.addRectangle (x - sliderRadius * 0.5f, y, width + sliderRadius, height);
        }
        
        g.fillPath (indent);
        g.setColour (trackColour.contrasting (0.5f));
        g.strokePath (indent, PathStrokeType (0.5f));
        
        auto iy = y + height * 0.3f;
        int j = 0;
        for (int i =0; i < pixelWidth; i = i + divSize)
        {
            g.setColour (Colours::white);
            g.drawLine (x+i, iy ,x+i, height);
            g.setFont(10.0f);
            auto divText = String(j * divSizeInS, 2, false);
            j++;
            g.drawText (divText, x+i-(textWidth/2), y, textWidth, y + height*0.3f, Justification::centred, true);
        }
    }
    
    void setDivSize(float size, float waveWidth, float audioLength)
    {
        pixelWidth = waveWidth;
        divSizeInS = size;
        audioLength = audioLength;
    }

private:
    int pixelWidth = 700;
    int textWidth = 30;
    int divSize = 50;
    float divSizeInS;
    float audioLength;
};
