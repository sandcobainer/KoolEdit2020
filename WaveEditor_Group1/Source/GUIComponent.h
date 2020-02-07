/*
  ==============================================================================

    This file was auto-generated!
    https://docs.juce.com/master/tutorial_playing_sound_files.html

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class GUIComponent : public AudioAppComponent,
                     public ChangeListener
{
public:
    //==============================================================================
    GUIComponent();
    ~GUIComponent() override;

    //==============================================================================
    /*
    \ Called when audio device is started
    \ Or when settings (sample rate, block size, etc.) are changed
    */
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;

    /*
    \ Get next block of audio data
    */
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;

    /*
    \ Called when the audio device stops
    \ Or when restarted due to a setting change
    */
    void releaseResources() override;

    //==============================================================================
    void paint (Graphics& g) override;

    /*
    \ This is called when the MainContentComponent is resized.
    */
    void resized() override;

    void changeListenerCallback(ChangeBroadcaster* source) override;

private:
    //==============================================================================
    enum TransportState //Audio states
    {
        Stopped,
        Starting,
        Playing,
        Pausing,
        Paused,
        Stopping
    };

    /*
    \ Update audio state
    */
    void changeState(TransportState newState);

    /*
    \ When OPEN button is pressed
    \ open file browser
    */
    void openButtonClicked();

    /*
    \ When PLAY button is pressed
    \ change audio state to STARTING or PAUSING
    \ depending on current state
    */
    void playButtonClicked();

    /*
    \ When PAUSE button is pressed
    \ change audio state to PAUSING
    \ if currently playing
    */
    void pauseButtonClicked();

    /*
    \ When STOP button is pressed
    \ change audio state to STOPPED or STOPPING
    \ depending on current state
    */
    void stopButtonClicked();

    //=============================================================================
    //Additional variables...
    //buttons
    TextButton openButton;
    ImageButton playButton;
    ImageButton pauseButton;
    ImageButton stopButton;
    
    //image files
    //all should be located in ../WaveEditor_Group1/
    const String sPlayNormal = "../../../../../play_normal.png";
    const String sPlayOver = "../../../../../play_over.png";
    const String sPlayDown = "../../../../../play_down.png";

    const String sPauseNormal = "../../../../../pause_normal.png";
    const String sPauseOver = "../../../../../pause_over.png";
    const String sPauseDown = "../../../../../pause_down.png";

    const String sStopNormal = "../../../../../stop_normal.png";
    const String sStopOver = "../../../../../stop_over.png";
    const String sStopDown = "../../../../../stop_down.png";

    //image objects
    Image iPlayNormal;
    Image iPlayOver;
    Image iPlayDown;

    Image iPauseNormal;
    Image iPauseOver;
    Image iPauseDown;

    Image iStopNormal;
    Image iStopOver;
    Image iStopDown;

    //image file objects
    ImageFileFormat* fPlayNormal;
    ImageFileFormat* fPlayOver;
    ImageFileFormat* fPlayDown;

    ImageFileFormat* fPauseNormal;
    ImageFileFormat* fPauseOver;
    ImageFileFormat* fPauseDown;

    ImageFileFormat* fStopNormal;
    ImageFileFormat* fStopOver;
    ImageFileFormat* fStopDown;

    //audio aspects
    AudioFormatManager formatManager;
    std::unique_ptr<AudioFormatReaderSource> readerSource;
    AudioTransportSource transportSource;
    TransportState state;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GUIComponent)
};
