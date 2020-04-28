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
#include "UndoStack.h"

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

    /*! Takes juce::File object passed from ToolbarIF.h
    \   saves the current buffer as a file
    */
    void saveFile(File);

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

    /*! Called from ToolbarIF.h when user clicks loop button
    \   sets loopEnabled to TRUE
    */
    void loopOnRequested();

    /*! Called from ToolbarIF.h when user clicks loop button
    \   sets loopEnabled to FALSE
    */
    void loopOffRequested();

    /*! Returns true if loop is enabled
    */
    const bool isLoopEnabled();

    /*! Called from ToolbarIF.h when user clicks mouse icon
    \   Toggles mouse state between cursor for wave selection
    \   and normal mouse state for clicking and moving selection
    */
    void setMouseState(bool);

    /*! Returns true if mouse state is normal (not selection cursor)
    */
    const bool isMouseNormal();

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

    /*! Returns the number of samples of audio data
    */
    int getNumSamples();

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

    /*! Fades in the audio in the marked region
    */
    void fadeInMarkedRegion();

    /*! Fades out the audio in the marked region
    */
    void fadeOutMarkedRegion();

    /*! Normalizes out the audio in the marked region
    */
    void normalizeMarkedRegion();

    /*! Adds gain to the audio in the marked region
        @param float the gain value
    */
    void gainMarkedRegion(float gainValue);

    /*! Copies the audio in the marked region
    */
    void copyMarkedRegion();

    /*! Cuts the audio in the marked region
    */
    void cutMarkedRegion();

    /*! Delete the audio in the marked region
    */
    void deleteMarkedRegion();

    /*! Pastes the audio from the current cursor position
    */
    void pasteFromCursor();

    /*! Insert the audio from the current cursor position
    */
    void insertFromCursor();

    bool isPasteEnabled();

    /*! Undo the last operation
    */
    void undo();

    /*! Redo the last operation
    */
    void redo();

    bool isUndoEnabled();

    bool isRedoEnabled();

    ChangeBroadcaster transportState;
    ChangeBroadcaster audioBufferChanged;
    ChangeBroadcaster blockReady;
    ChangeBroadcaster audioCopied;

private:

    /*! Internal function to change the transport state
    */
    void setState(TransportState state);

    /*! Makes sure the markers are not out of range. It should be used in
     * any operation that might change the audio buffer size
    */
    void boundPositions();

    void inplaceOperate(const std::function<void(float*, int, int)>&, int startSample, int numSamples);
    void inplaceOperateMarkedRegion(const std::function<void(float*, int, int)>&);

    AudioFormatManager formatManager;
    TransportState state;
    bool fileLoaded;  // indicates if a file is loaded
    CatmullRomInterpolator** interpolators;
    UndoStack undoStack;

    //// AudioBuffer
    // buffer definitions
    AudioBuffer<float> audioBlockBuffer;
    AudioBuffer<float> audioBuffer;
    AudioBuffer<float> audioCopyBuffer;
    // meta info
    double sampleRate;
    double deviceSampleRate;
    // position info (the unit is always in sample)
    int currentPos;
    int markerStartPos;
    int markerEndPos;

    bool loopEnabled;
    bool mouseNormal;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioProcessingComponent)
};
