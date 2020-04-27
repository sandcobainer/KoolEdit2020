/*
  ==============================================================================

    OtherLookAndFeel.h
    Created: 26 Apr 2020 8:10:49pm
    Author:  sancobainer

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class OtherLookAndFeel : public LookAndFeel_V3
{
public:
    
    OtherLookAndFeel()
    {
        setColour (Slider::thumbColourId, Colours::green);
//        setColour (Slider::backgroundColourId, Colours::red);
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
             g.setColour (slider.findColour (Slider::trackColourId)
                                .withMultipliedAlpha (slider.isEnabled() ? 1.0f : 0.3f));
             x = x-8;
             w = w+16;
             
             if (slider.isHorizontal())
             {
                 g.fillRect (x, y + roundToInt (h * 0.6f),
                             w, roundToInt (h * 0.2f));
             }
             else
             {
                 g.fillRect (x + roundToInt (w * 0.5f - jmin (3.0f, w * 0.1f)), y,
                             jmin (4, roundToInt (w * 0.2f)), h);
             }

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
        DBG('hi');
        const float sliderRadius = (float) (getSliderThumbRadius (slider) + 20);

        const Colour trackColour (Colours:: red);
//        (slider.findColour (Slider::trackColourId));
        const Colour gradCol1 (trackColour.overlaidWith (Colour (slider.isEnabled() ? 0x13000000 : 0x09000000)));
        const Colour gradCol2 (trackColour.overlaidWith (Colour (0x06000000)));
        Path indent;

        if (slider.isHorizontal())
        {
            auto iy = y + height * 0.5f - sliderRadius * 0.5f;

            g.setGradientFill (ColourGradient::vertical (gradCol1, iy, gradCol2, iy + sliderRadius));

            indent.addRoundedRectangle (x - sliderRadius * 0.5f, iy, width + sliderRadius, sliderRadius, 5.0f);
        }
        else
        {
            auto ix = x + width * 0.5f - sliderRadius * 0.5f;

            g.setGradientFill (ColourGradient::horizontal (gradCol1, ix, gradCol2, ix + sliderRadius));

            indent.addRoundedRectangle (ix, y - sliderRadius * 0.5f, sliderRadius, height + sliderRadius, 5.0f);
        }

        g.fillPath (indent);

        g.setColour (trackColour.contrasting (0.5f));
        g.strokePath (indent, PathStrokeType (0.5f));
    }
    
};
