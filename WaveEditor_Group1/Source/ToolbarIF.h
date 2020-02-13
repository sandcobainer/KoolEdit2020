/*
  ==============================================================================

    ToolbarIF.h
    Created: 7 Feb 2020 2:28:55pm
    Author:  sophi

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class ToolbarIF : public Component
{
public:
    ToolbarIF()
    {
        setSize(500, 500);
        const String projRootDir = getProjRootDir();

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
    void paint(Graphics& g)
    {
        g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
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
        //call appropriate function of AudioProcessingComponent
        //to open file and set up transport source

        //once AudioProcessingComponent returns...
        playButton.setEnabled(true);
    }

    void playButtonClicked()
    {

    }

    void pauseButtonClicked()
    {

    }

    void stopButtonClicked()
    {

    }

    /*! Function to search for root project directory
    \ return root directory as string to be able to
    \ navigate to Assets folder
    */
    const String getProjRootDir()
    {
        File projectRootDir = File::getCurrentWorkingDirectory().getParentDirectory();
        
        while (projectRootDir.getParentDirectory().getRelativePathFrom(projectRootDir) != "..\\..\\WaveEditor_Group1")
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
    const String sPlayNormal = "\\Source\\Assets\\play_normal.png";
    const String sPlayOver = "\\Source\\Assets\\play_over.png";
    const String sPlayDown = "\\Source\\Assets\\play_down.png";

    const String sPauseNormal = "\\Source\\Assets\\pause_normal.png";
    const String sPauseOver = "\\Source\\Assets\\pause_over.png";
    const String sPauseDown = "\\Source\\Assets\\pause_down.png";

    const String sStopNormal = "\\Source\\Assets\\stop_normal.png";
    const String sStopOver = "\\Source\\Assets\\stop_over.png";
    const String sStopDown = "\\Source\\Assets\\stop_down.png";

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


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ToolbarIF)
};