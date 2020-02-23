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


class ToolbarIF : public Component, public ChangeListener
{
public:
    ToolbarIF(AudioProcessingComponent& c) : apc(c)
    {
        setSize(700, 700);
        const String projRootDir = getProjRootDir();
        state = apc.getState(); //initialize transport source state
        apc.transportSource.addChangeListener(this); //listen to changes in the transport source

        //-----------------------GUI Images------------------------------------
        //Play
        iPlayNormal = fPlayNormal->loadFrom(File(projRootDir + sPlayNormal));
        iPlayOver = fPlayOver->loadFrom(File(projRootDir + sPlayOver));
        iPlayDown = fPlayDown->loadFrom(File(projRootDir + sPlayDown));
        
        //Pause
        iPauseNormal = fPauseNormal->loadFrom(File(projRootDir + sPauseNormal));
        iPauseOver = fPauseOver->loadFrom(File(projRootDir + sPauseOver));
        iPauseDown = fPauseDown->loadFrom(File(projRootDir + sPauseDown));

        //Stop
        iStopNormal = fStopNormal->loadFrom(File(projRootDir + sStopNormal));
        iStopOver = fStopOver->loadFrom(File(projRootDir + sStopOver));
        iStopDown = fStopDown->loadFrom(File(projRootDir + sStopDown));

        //-----------------------GUI Buttons-----------------------------------
        //Open
        addAndMakeVisible(&openButton);
        openButton.setButtonText("Open...");
        openButton.onClick = [this] {openButtonClicked(); };

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
        if (source == &apc.transportSource)
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

    void paint(Graphics& g)
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

    void resized()
    {
        //button positioning
        openButton.setBounds(10, 10, getWidth() - 20, 20);
        playButton.setBounds(2, getHeight() - 32, 30, 30);
        pauseButton.setBounds(35, getHeight() - 32, 30, 30);
        stopButton.setBounds(67, getHeight() - 32, 30, 30);
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
    }

    /*! Function to search for root project directory
    \ return root directory as string to be able to
    \ navigate to Assets folder
    */
    const String getProjRootDir()
    {
        File projectRootDir = File::getCurrentWorkingDirectory().getParentDirectory();
        String searchFor;

        #if JUCE_WINDOWS
            searchFor = "..\\..\\WaveEditor_Group1";
        #else
            searchFor = "../../WaveEditor_Group1";
        #endif
        
        while (projectRootDir.getParentDirectory().getRelativePathFrom(projectRootDir) != searchFor)
        {
            projectRootDir = projectRootDir.getParentDirectory();
        }

        return(projectRootDir.getParentDirectory().getFullPathName());
    }

    //==========================================================================
    //Additional variables
    //Buttons
    TextButton openButton;
    ImageButton playButton;
    ImageButton pauseButton;
    ImageButton stopButton;

    //Image files
    //all should be located in ../Source/Assets/
    #if JUCE_WINDOWS
        const String sPlayNormal = "\\Source\\Assets\\play_normal.png";
        const String sPlayOver = "\\Source\\Assets\\play_over.png";
        const String sPlayDown = "\\Source\\Assets\\play_down.png";

        const String sPauseNormal = "\\Source\\Assets\\pause_normal.png";
        const String sPauseOver = "\\Source\\Assets\\pause_over.png";
        const String sPauseDown = "\\Source\\Assets\\pause_down.png";

        const String sStopNormal = "\\Source\\Assets\\stop_normal.png";
        const String sStopOver = "\\Source\\Assets\\stop_over.png";
        const String sStopDown = "\\Source\\Assets\\stop_down.png";
    #else
        const String sPlayNormal = "/Source/Assets/play_normal.png";
        const String sPlayOver = "/Source/Assets/play_over.png";
        const String sPlayDown = "/Source/Assets/play_down.png";

        const String sPauseNormal = "/Source/Assets/pause_normal.png";
        const String sPauseOver = "/Source/Assets/pause_over.png";
        const String sPauseDown = "/Source/Assets/pause_down.png";

        const String sStopNormal = "/Source/Assets/stop_normal.png";
        const String sStopOver = "/Source/Assets/stop_over.png";
        const String sStopDown = "/Source/Assets/stop_down.png";
    #endif

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

    //Image file objects
    ImageFileFormat* fPlayNormal;
    ImageFileFormat* fPlayOver;
    ImageFileFormat* fPlayDown;

    ImageFileFormat* fPauseNormal;
    ImageFileFormat* fPauseOver;
    ImageFileFormat* fPauseDown;

    ImageFileFormat* fStopNormal;
    ImageFileFormat* fStopOver;
    ImageFileFormat* fStopDown;

    //connection to AudioProcessingComponent (passed from parent)
    AudioProcessingComponent& apc;
    String state; //transport state (from apc.getState() function)

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ToolbarIF)
};