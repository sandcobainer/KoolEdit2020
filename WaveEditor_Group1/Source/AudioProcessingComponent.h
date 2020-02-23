/*
  ==============================================================================

    AudioProcessingComponent.h
    Created: 7 Feb 2020 3:05:06pm
    Author:  user

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "WaveAudio.h"

//==============================================================================
/*
*/
class AudioProcessingComponent    : public AudioAppComponent, public ChangeBroadcaster, public ChangeListener
{
public:
    AudioProcessingComponent();
    ~AudioProcessingComponent();

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;   
    void changeListenerCallback (ChangeBroadcaster* source) override;
    
    /*! Call from any of GUI components(WaveVisualizer.h) to get FormatManager of APC
     */
    AudioThumbnail getThumbnail();
    
    /*! Takes juce::File object passed from ToolbarIF.h
    \   reads user chosen audio file and sets up transport source
    \   transport state should still be STOPPED
    */
    void loadFile(File);

    /*! Called from ToolbarIF.h when user clicks play button
    \   evaluates current transport states and determines action
    */
    void playRequested();

    /*! Called from ToolbarIF.h when user clicks stop button
    \   evaluates current transport states and determines action
    */
    void stopRequested();

    /*! Called from ToolbarIF.h when user clicks pause button
    \   evaluate current transport states and determines action
    */
    void pauseRequested();

    /*! Called by ToolbarIF.h to get current transport state info
    */
    const String getState();

    AudioTransportSource transportSource;
    AudioFormatManager formatManager;
    AudioThumbnailCache thumbnailCache;                  // [1]
    AudioThumbnail thumbnail;                            // [2]
    

private:
    
    enum TransportState
    {
        Stopped,  //initial state, position 0 
        Pausing,  //stopped, last position saved
        Playing,
        Paused,
        Starting, //starts transport
        Stopping  //stops transport
    };
    
    std::unique_ptr<AudioFormatReaderSource> readerSource;

    TransportState state;
    double currentPosition;
    
    /*! Internal function to change the transport state
    */
    void setState(TransportState state);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioProcessingComponent)
};
