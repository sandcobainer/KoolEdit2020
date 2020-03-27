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
#include "actionMessages.h"

//==============================================================================
/*
*/
class AudioProcessingComponent    : public AudioAppComponent,
                                    public ChangeBroadcaster,
                                    public ChangeListener,
                                    public ActionBroadcaster
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
    const float* getAudioWritePointer(int numChannel, int &numAudioSamples);

    /*! Returns a pointer to the current audio buffer
    */
    const AudioBuffer<float>* getAudioBuffer();

    /*! Returns the number of channels of audio data
    */
    int getNumChannels();

    /*! Returns the current sample rate
    */
    double getSampleRate();

    /*! Returns current audio position
    */
    const double getCurrentPosition();

    /*! Returns number of audio channels in loaded file
    */
    //const int getNumChannels();

    ChangeBroadcaster transportState;           //!< public broadcaster for the transport state

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

    TransportState state;       //!< enum
    double currentPosition;
    
    // AudioBuffer
    int maxNumChannels;
    int numBlockSamples;
    double sampleRate;
    juce::int64 numAudioSamples;
    AudioBuffer<float> audioBlockBuffer;
    AudioBuffer<float> audioBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioProcessingComponent)
};
