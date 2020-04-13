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
class AudioProcessingComponent    : public AudioAppComponent
{
public:
    AudioProcessingComponent();
    ~AudioProcessingComponent();

    enum TransportState
    {
        Stopped,  //initial state, position 0
        Pausing,  //stopped, last position saved
        Playing,
        Paused,
        Starting, //starts transport
        Stopping  //stops transport
    };

    enum PositionType
    {
        Cursor,
        MarkerStart,
        MarkerEnd
    };

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;   

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
    TransportState getState();
    
    /*! Returns a read pointer to one block and one channel of data
        @param numChannel the specific channel need to be fetched
        @param numSamples the sample size
    */
    const float* getAudioBlockReadPointer(int numChannel, int &numBlockSamples);

    /*! Returns a read pointer to one channel of audio data
        @param numChannel the specific channel need to be fetched
        @param numSamples the sample size
    */
    const float* getAudioReadPointer(int numChannel, int &numAudioSamples);

    /*! Returns a write pointer to one channel of audio data
        @param numChannel the specific channel need to be fetched
        @param numSamples the sample size
    */
    float* getAudioWritePointer(int numChannel, int &numAudioSamples);

    /*! Returns a pointer to the current audio buffer
    */
    const AudioBuffer<float>* getAudioBuffer();

    /*! Returns the number of channels of audio data
    */
    int getNumChannels();

    /*! Returns the current sample rate
    */
    double getSampleRate();

    /*! Sets the cursor position or marker position in seconds
        @param PositionType the position type
        @param newPosition the new playback position in seconds
    */
    void setPositionInS(PositionType positionType, double position);

    /*! Gets the cursor position or marker position in seconds
        @param PositionType the position type
    */
    double getPositionInS(PositionType positionType);

    /*! Returns the stream's length in seconds.
    */
    double getLengthInS();

    /*! Set all the samples in the selected region to be zero
    */
    void muteMarkedRegion();

    ChangeBroadcaster transportState;           //!< public broadcaster for the transport state
    ChangeBroadcaster fileLoaded;               //!< public broadcaster for the transport state
    ChangeBroadcaster blockReady;               //!< public broadcaster for the transport state

private:
    

    /*! Fill the audioSampleBuffer with new coming data.
        @param channelData one channel of data.
        @param numChannel the number of channel that will be filled.
    */
    void fillAudioBlockBuffer(const float* channelData, int numChannel);

    /*! Internal function to change the transport state
    */
    void setState(TransportState state);

    AudioFormatManager formatManager;

    TransportState state;       //!< enum

    //// AudioBuffer
    // buffer definitions
    AudioBuffer<float> audioBlockBuffer;
    AudioBuffer<float> audioBuffer;
    // meta info
    unsigned int numChannels;
    int numBlockSamples;
    double sampleRate;
    juce::int64 numAudioSamples;
    // position info (the unit is always in sample)
    int currentPos;
    int markerStartPos;
    int markerEndPos;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioProcessingComponent)
};
