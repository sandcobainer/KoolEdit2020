/*
  ==============================================================================

    OtherLookAndFeel.h
    Created: 26 Apr 2020 8:10:49pm
    Author:  sancobainer

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class OtherLookAndFeel : public LookAndFeel_V4
{
public:
//    OtherLookAndFeel()
//    {
//        setColour (Slider::thumbColourId, Colours::green);
////        drawLinearSliderBackground(Graphics &g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const Slider::SliderStyle style, Slider &);
//    }
    void drawLinearSliderBackground    (Graphics &  g,
                                    int     x,
                                    int     y,
                                    int     width,
                                    int     height,
                                    float     sliderPos,
                                    float     minSliderPos,
                                    float     maxSliderPos,
                                    const Slider::SliderStyle     style,
                                    Slider &
                                        ) override
    
    {
        g.setColour (Colours::red);
        g.fillRect (x,y,width,height);
    }
};
