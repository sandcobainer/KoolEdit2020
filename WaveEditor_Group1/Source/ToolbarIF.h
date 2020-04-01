/*
  ==============================================================================

    ToolbarIF.h
    Created: 7 Feb 2020 2:28:55pm
    Author:  sophi

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "AudioProcessingComponent.h"
#include "buttonAssets.h"


class ToolbarIF : public Component, public ChangeListener
{
public:
    ToolbarIF(AudioProcessingComponent& c) : apc(c)
    {
        state = apc.getState(); //initialize transport source state
        //apc.transportSource.addChangeListener(this); //listen to changes in the transport source
        apc.transportState.addChangeListener(this);

        //-----------------------GUI Images------------------------------------
        //Play
        iPlayNormal = ImageFileFormat::loadFrom(buttonAssets::play_normal_png, (size_t)buttonAssets::play_normal_pngSize);
        iPlayOver = ImageFileFormat::loadFrom(buttonAssets::play_over_png, (size_t)buttonAssets::play_over_pngSize);
        iPlayDown = ImageFileFormat::loadFrom(buttonAssets::play_down_png, (size_t)buttonAssets::play_down_pngSize);
        
        //Pause
        iPauseNormal = ImageFileFormat::loadFrom(buttonAssets::pause_normal_png, (size_t)buttonAssets::pause_normal_pngSize);
        iPauseOver = ImageFileFormat::loadFrom(buttonAssets::pause_over_png, (size_t)buttonAssets::pause_over_pngSize);
        iPauseDown = ImageFileFormat::loadFrom(buttonAssets::pause_down_png, (size_t)buttonAssets::pause_down_pngSize);

        //Stop
        iStopNormal = ImageFileFormat::loadFrom(buttonAssets::stop_normal_png, (size_t)buttonAssets::stop_normal_pngSize);
        iStopOver = ImageFileFormat::loadFrom(buttonAssets::stop_over_png, (size_t)buttonAssets::stop_over_pngSize);
        iStopDown = ImageFileFormat::loadFrom(buttonAssets::stop_down_png, (size_t)buttonAssets::stop_down_pngSize);

        //Open
        iOpenNormal = ImageFileFormat::loadFrom(buttonAssets::openfile_normal_png, (size_t)buttonAssets::openfile_normal_pngSize);
        iOpenOver = ImageFileFormat::loadFrom(buttonAssets::openfile_over_png, (size_t)buttonAssets::openfile_over_pngSize);
        iOpenDown = ImageFileFormat::loadFrom(buttonAssets::openfile_down_png, (size_t)buttonAssets::openfile_down_pngSize);

        //Save
        iSaveNormal = ImageFileFormat::loadFrom(buttonAssets::savefile_normal_png, (size_t)buttonAssets::savefile_normal_pngSize);
        iSaveOver = ImageFileFormat::loadFrom(buttonAssets::savefile_over_png, (size_t)buttonAssets::savefile_over_pngSize);
        iSaveDown = ImageFileFormat::loadFrom(buttonAssets::savefile_down_png, (size_t)buttonAssets::savefile_down_pngSize);

        //Fastforward
        iFFWDNormal = ImageFileFormat::loadFrom(buttonAssets::ffwd_normal_png, (size_t)buttonAssets::ffwd_normal_pngSize);
        iFFWDOver = ImageFileFormat::loadFrom(buttonAssets::ffwd_over_png, (size_t)buttonAssets::ffwd_over_pngSize);
        iFFWDDown = ImageFileFormat::loadFrom(buttonAssets::ffwd_down_png, (size_t)buttonAssets::ffwd_down_pngSize);

        //Rewind
        iRewindNormal = ImageFileFormat::loadFrom(buttonAssets::rewind_normal_png, (size_t)buttonAssets::rewind_normal_pngSize);
        iRewindOver = ImageFileFormat::loadFrom(buttonAssets::rewind_over_png, (size_t)buttonAssets::rewind_over_pngSize);
        iRewindDown = ImageFileFormat::loadFrom(buttonAssets::rewind_down_png, (size_t)buttonAssets::rewind_down_pngSize);

        //Loop
        iLoopNormal = ImageFileFormat::loadFrom(buttonAssets::loop_normal_png, (size_t)buttonAssets::loop_normal_pngSize);
        iLoopOver = ImageFileFormat::loadFrom(buttonAssets::loop_over_png, (size_t)buttonAssets::loop_over_pngSize);
        iLoopDown = ImageFileFormat::loadFrom(buttonAssets::loop_down_png, (size_t)buttonAssets::loop_down_pngSize);

        //-----------------------GUI Buttons-----------------------------------
        //Open
        addAndMakeVisible(&openButton);
        openButton.setImages(false, true, true, iOpenNormal, 1.0, Colours::transparentWhite, iOpenOver, 1.0, Colours::transparentWhite, iOpenDown, 1.0, Colours::transparentWhite);
        openButton.setState(Button::ButtonState::buttonNormal);
        openButton.onClick = [this] {openButtonClicked(); };
        openButton.setEnabled(true);

        //Save
        addAndMakeVisible(&saveButton);
        saveButton.setImages(false, true, true, iSaveNormal, 1.0, Colours::transparentWhite, iSaveOver, 1.0, Colours::transparentWhite, iSaveDown, 1.0, Colours::transparentWhite);
        saveButton.setState(Button::ButtonState::buttonNormal);
        saveButton.onClick = [this] {saveButtonClicked(); };
        saveButton.setEnabled(false);

        //Play
        addAndMakeVisible(&playButton);
        playButton.setImages(false, true, true, iPlayNormal, 1.0, Colours::transparentWhite, iPlayOver, 1.0, Colours::transparentWhite, iPlayDown, 1.0, Colours::transparentWhite);
        playButton.setState(Button::ButtonState::buttonNormal);
        playButton.onClick = [this] {playButtonClicked(); };
        playButton.setEnabled(false);

        //Pause
        addAndMakeVisible(&pauseButton);
        pauseButton.setImages(false, true, true, iPauseNormal, 1.0, Colours::transparentWhite, iPauseOver, 1.0, Colours::transparentWhite, iPauseDown, 1.0, Colours::transparentWhite);
        pauseButton.setState(Button::ButtonState::buttonNormal);
        pauseButton.onClick = [this] {pauseButtonClicked(); };
        pauseButton.setEnabled(false);

        //Stop
        addAndMakeVisible(&stopButton);
        stopButton.setImages(false, true, true, iStopNormal, 1.0, Colours::transparentWhite, iStopOver, 1.0, Colours::transparentWhite, iStopDown, 1.0, Colours::transparentWhite);
        stopButton.setState(Button::ButtonState::buttonNormal);
        stopButton.onClick = [this] {stopButtonClicked(); };
        stopButton.setEnabled(false);

        //Fastforward
        addAndMakeVisible(&ffwdButton);
        ffwdButton.setImages(false, true, true, iFFWDNormal, 1.0, Colours::transparentWhite, iFFWDOver, 1.0, Colours::transparentWhite, iFFWDDown, 1.0, Colours::transparentWhite);
        ffwdButton.setState(Button::ButtonState::buttonNormal);
        ffwdButton.onClick = [this] {ffwdButtonClicked(); };
        ffwdButton.setEnabled(false);

        //Rewind
        addAndMakeVisible(&rewindButton);
        rewindButton.setImages(false, true, true, iRewindNormal, 1.0, Colours::transparentWhite, iRewindOver, 1.0, Colours::transparentWhite, iRewindDown, 1.0, Colours::transparentWhite);
        rewindButton.setState(Button::ButtonState::buttonNormal);
        rewindButton.onClick = [this] {rewindButtonClicked(); };
        rewindButton.setEnabled(false);

        //Loop
        addAndMakeVisible(&loopButton);
        loopButton.setImages(false, true, true, iLoopNormal, 1.0, Colours::transparentWhite, iLoopOver, 1.0, Colours::transparentWhite, iLoopDown, 1.0, Colours::transparentWhite);
        loopButton.setState(Button::ButtonState::buttonNormal);
        loopButton.onClick = [this] {loopButtonClicked(); };
        loopButton.setEnabled(false);
    }

    ~ToolbarIF()
    {

    }

    //==========================================================================
    /*! Listen to changes in the transport state 
    \   Set buttons accordingly
    */
    void changeListenerCallback(ChangeBroadcaster* source) override
    {
        if (source == &apc.transportState)
        {
            if (apc.getState() == "Playing")
            {
                playButton.setEnabled(false);
                playButton.setState(Button::ButtonState::buttonDown);
                pauseButton.setEnabled(true);
                pauseButton.setState(Button::ButtonState::buttonNormal);
                stopButton.setEnabled(true);
                stopButton.setState(Button::ButtonState::buttonNormal);
            }
            else if (apc.getState() == "Stopped")
            {
                stopButton.setEnabled(false);
                stopButton.setState(Button::ButtonState::buttonDown);
                pauseButton.setEnabled(false);
                pauseButton.setState(Button::ButtonState::buttonNormal);
                playButton.setEnabled(true);
                playButton.setState(Button::ButtonState::buttonNormal);
            }
            else if (apc.getState() == "Paused")
            {
                pauseButton.setEnabled(false);
                pauseButton.setState(Button::ButtonState::buttonDown);
                playButton.setEnabled(true);
                playButton.setState(Button::ButtonState::buttonNormal);
                stopButton.setEnabled(true);
                stopButton.setState(Button::ButtonState::buttonNormal);
            }
        }
    }

    void paint(Graphics& g) override
    {
        g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
        
        //paint buttons based on current audio state
        state = apc.getState();
        if (state == "Playing")
            playButton.setState(Button::ButtonState::buttonDown);
        else if (state == "Stopped")
            stopButton.setState(Button::ButtonState::buttonDown);
        else if (state == "Paused")
            pauseButton.setState(Button::ButtonState::buttonDown);
    }

    void resized() override
    {
        //button positioning
        openButton.setBounds(2, 10, 30, 30);
        saveButton.setBounds(35, 10, 30, 30);
        playButton.setBounds(100, 10, 30, 30);
        pauseButton.setBounds(133, 10, 30, 30);
        stopButton.setBounds(166, 10, 30, 30);
        rewindButton.setBounds(199, 10, 30, 30);
        ffwdButton.setBounds(232, 10, 30, 30);
        loopButton.setBounds(getWidth()-40, 10, 30, 30);
    }

    //==========================================================================

private:
    void openButtonClicked()
    {
        //open file browser
        FileChooser chooser("Select a Wave file to play...",
            {},
            "*.wav");                                        

        if (chooser.browseForFileToOpen())                                   
        {
            auto file = chooser.getResult();               
            apc.loadFile(file);             //go to audio thread
        }

        //once AudioProcessingComponent returns...
        playButton.setEnabled(true);
        saveButton.setEnabled(true);
        ffwdButton.setEnabled(true);
        rewindButton.setEnabled(true);
        loopButton.setEnabled(true);
    }

    void saveButtonClicked()
    {
    }

    void playButtonClicked()
    {
        //call appropriate function of AudioProcessingComponent
        //to start transport source from beginning
        apc.playRequested();

        //once AudioProcessingComponent returns...
        playButton.setEnabled(false);
        pauseButton.setEnabled(true);
        stopButton.setEnabled(true);
        ffwdButton.setEnabled(false);
        rewindButton.setEnabled(false);
    }

    void pauseButtonClicked()
    {
        //call appropriate function of AudioProcessingComponent
        //to stop transport source and store position
        apc.pauseRequested();

        //once AudioProcessingComponent returns...
        pauseButton.setEnabled(false);
        playButton.setEnabled(true);
        stopButton.setEnabled(true);
        ffwdButton.setEnabled(true);
        rewindButton.setEnabled(true);
    }

    void stopButtonClicked()
    {
        //call appropriate function of AudioProcessingComponent
        //to stop transport source and set position to beginning
        apc.stopRequested();

        //once AudioProcessingComponent returns...
        stopButton.setEnabled(false);
        playButton.setEnabled(true);
        pauseButton.setEnabled(false);
        ffwdButton.setEnabled(true);
        rewindButton.setEnabled(true);
    }

    void ffwdButtonClicked()
    {

    }

    void rewindButtonClicked()
    {

    }

    void loopButtonClicked()
    {

    }
    
    //==========================================================================
    //Additional variables
    //Buttons
    ImageButton openButton;
    ImageButton saveButton;
    ImageButton playButton;
    ImageButton pauseButton;
    ImageButton stopButton;
    ImageButton ffwdButton;
    ImageButton rewindButton;
    ImageButton loopButton;

    //Image objects
    Image iPlayNormal;
    Image iPlayOver;
    Image iPlayDown;

    Image iPauseNormal;
    Image iPauseOver;
    Image iPauseDown;

    Image iStopNormal;
    Image iStopOver;
    Image iStopDown;

    Image iOpenNormal;
    Image iOpenOver;
    Image iOpenDown;

    Image iSaveNormal;
    Image iSaveOver;
    Image iSaveDown;

    Image iFFWDNormal;
    Image iFFWDOver;
    Image iFFWDDown;

    Image iRewindNormal;
    Image iRewindOver;
    Image iRewindDown;

    Image iLoopNormal;
    Image iLoopOver;
    Image iLoopDown;

    //connection to AudioProcessingComponent (passed from parent)
    AudioProcessingComponent& apc;
    String state; //transport state (from apc.getState() function)

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ToolbarIF)
};
