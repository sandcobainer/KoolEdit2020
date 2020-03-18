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
class AudioProcessingComponent    : public AudioAppComponent, public ChangeBroadcaster, public ChangeListener, private Timer
{
public:
    AudioProcessingComponent();
    ~AudioProcessingComponent();

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;   
    void changeListenerCallback (ChangeBroadcaster* source) override;
    void timerCallback() override;
    
    
    /*! Call from any of GUI components(WaveVisualizer.h) to get FormatManager of APC
     */
    
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
    
    /*! Get a read pointer to one block and one channel of data
        @param numChannel the specific channel need to be fetched
        @param numSamples the sample size
    */
    const float* getAudioBlockReadPointer(int numChannel, int &numBlockSamples);

    /*! Get a read pointer to one channel of audio data
        @param numChannel the specific channel need to be fetched
        @param numSamples the sample size
    */
    const float* getAudioReadPointer(int numChannel, int &numAudioSamples);

    /*! Get a write pointer to one channel of audio data
        @param numChannel the specific channel need to be fetched
        @param numSamples the sample size
    */
    const float* getAudioWritePointer(int numChannel, int &numAudioSamples);

    /*! Returns current audio position
    */
    const double getCurrentPosition();

    /*! Returns number of audio channels in loaded file
    */
    const int getNumChannels();

    /*! Returns total length of audio thumbnail
    */
    const double getThumbnailLength();

    ChangeBroadcaster transportState;           //!< public broadcaster for the transport state    
    ChangeBroadcaster thumbnailChange;          //!< public broadcaster for audio thumbnail

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
    
    /*! Fill the audioSampleBuffer with new coming data.
        @param channelData one channel of data.
        @param numChannel the number of channel that will be filled.
    */
    void fillAudioBlockBuffer(const float* channelData, int numChannel);

    /*! Internal function to change the transport state
    */
    void setState(TransportState state);

    std::unique_ptr<AudioFormatReaderSource> readerSource;
    AudioTransportSource transportSource;
    AudioFormatManager formatManager;
    AudioThumbnailCache thumbnailCache;
    AudioThumbnail thumbnail;

    TransportState state;       //!< enum
    double currentPosition;
    
    // AudioBuffer
    int maxNumChannels;
    int numBlockSamples;
    juce::int64 numAudioSamples;
    AudioBuffer<float> audioBlockBuffer;
    AudioBuffer<float> audioBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioProcessingComponent)
};
