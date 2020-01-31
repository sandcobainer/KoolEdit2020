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
    void changeState(TransportState newState)
    {
        if (state != newState) //if different new state
        {
            state = newState; //change state and perform appropriate actions
            switch (state)
            {
                case Stopped:
                    stopButton.setEnabled(false);
                    pauseButton.setEnabled(false);
                    playButton.setEnabled(true);
                    transportSource.setPosition(0.0);
                    break;
                case Starting:
                    transportSource.start();
                    break;
                case Playing:
                    playButton.setEnabled(false);
                    pauseButton.setEnabled(true);
                    stopButton.setEnabled(true);
                    break;
                case Pausing:
                    transportSource.stop();
                    break;
                case Paused:
                    pauseButton.setEnabled(false);
                    playButton.setEnabled(true);
                    stopButton.setEnabled(true);
                    break;
                case Stopping:
                    transportSource.stop();
                    break;
            }
        }
    }

    /*
    \ When OPEN button is pressed
    \ open file browser
    */
    void openButtonClicked()
    {
        FileChooser chooser ("Select a .wav file to play...", {}, "*.wav"); //open file browser

        if (chooser.browseForFileToOpen())
        {
            auto file = chooser.getResult();
            auto* reader = formatManager.createReaderFor(file);

            if (reader != nullptr)
            {
                std::unique_ptr<AudioFormatReaderSource> newSource(new AudioFormatReaderSource(reader, true));
                transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
                playButton.setEnabled(true);
                readerSource.reset(newSource.release());
            }
        }
    }

    /*
    \ When PLAY button is pressed
    \ change audio state to STARTING or PAUSING
    \ depending on current state
    */
    void playButtonClicked()
    {
        if ((state == Stopped) || (state == Paused))
            changeState(Starting);
    }

    void pauseButtonClicked()
    {
        if (state == Playing)
            changeState(Pausing);
    }

    /*
    \ When STOP button is pressed
    \ change audio state to STOPPED or STOPPING
    \ depending on current state
    */
    void stopButtonClicked()
    {
        if (state == Paused)
            changeState(Stopped);
        else
            changeState(Stopping);
    }

    //=============================================================================
    //Additional variables...
    //buttons
    TextButton openButton;
    TextButton playButton;
    TextButton pauseButton;
    TextButton stopButton;

    AudioFormatManager formatManager;
    std::unique_ptr<AudioFormatReaderSource> readerSource;
    AudioTransportSource transportSource;
    TransportState state;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GUIComponent)
};
