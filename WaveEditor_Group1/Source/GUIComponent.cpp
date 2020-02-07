/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "GUIComponent.h"
#include <iostream>

//==============================================================================
GUIComponent::GUIComponent() 
    : state(Stopped)
{
    //-------------------------------GUI images---------------------------------
    //Play button
    iPlayNormal = fPlayNormal->loadFrom(File(sPlayNormal));
    iPlayOver = fPlayOver->loadFrom(File(sPlayOver));
    iPlayDown = fPlayDown->loadFrom(File(sPlayDown));

    //Pause button
    iPauseNormal = fPauseNormal->loadFrom(File(sPauseNormal));
    iPauseOver = fPauseOver->loadFrom(File(sPauseOver));
    iPauseDown = fPauseDown->loadFrom(File(sPauseDown));

    //Stop button
    iStopNormal = fStopNormal->loadFrom(File(sStopNormal));
    iStopOver = fStopOver->loadFrom(File(sStopOver));
    iStopDown = fStopDown->loadFrom(File(sStopDown));

    //-------------------------------GUI buttons--------------------------------
    //Open button
    addAndMakeVisible(&openButton);
    openButton.setButtonText("Open...");
    openButton.onClick = [this] {openButtonClicked(); };

    //Play button
    addAndMakeVisible(&playButton);
    playButton.setImages(false, true, true, iPlayNormal, 1.0, Colours::transparentWhite, iPlayOver, 1.0, Colours::transparentWhite, iPlayDown, 1.0, Colours::transparentWhite);
    playButton.setState(Button::ButtonState::buttonNormal);
    playButton.onClick = [this] {playButtonClicked();  };
    playButton.setEnabled(false);

    //Pause button
    addAndMakeVisible(&pauseButton);
    pauseButton.setImages(false, true, true, iPauseNormal, 1.0, Colours::transparentWhite, iPauseOver, 1.0, Colours::transparentWhite, iPauseDown, 1.0, Colours::transparentWhite);
    pauseButton.setState(Button::ButtonState::buttonNormal);
    pauseButton.onClick = [this] {pauseButtonClicked(); };
    pauseButton.setEnabled(false);

    //Stop button
    addAndMakeVisible(&stopButton);
    stopButton.setImages(false, true, true, iStopNormal, 1.0, Colours::transparentWhite, iStopOver, 1.0, Colours::transparentWhite, iStopDown, 1.0, Colours::transparentWhite);
    stopButton.setState(Button::ButtonState::buttonNormal);
    stopButton.onClick = [this] {stopButtonClicked(); };
    stopButton.setEnabled(false);

    //------------------------------Other------------------------------------
    //Window
    setSize(300, 200);    

    //Audio
    formatManager.registerBasicFormats();
    transportSource.addChangeListener(this);

    setAudioChannels(0, 2); //0 input, 2 output
}

GUIComponent::~GUIComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//--------------------------------Audio Functions-------------------------------
//==============================================================================
void GUIComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void GUIComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    if (readerSource.get() == nullptr) //if no source
    {
        bufferToFill.clearActiveBufferRegion(); //clear the buffer to prevent output of noise
        return;
    }
    transportSource.getNextAudioBlock(bufferToFill); //otherwise get next block
}

void GUIComponent::releaseResources()
{
    // For more details, see the help for AudioProcessor::releaseResources()
    transportSource.releaseResources();
}

//------------------------------Graphics Functions------------------------------
//==============================================================================
void GUIComponent::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
    //manually override button states
    if (state == Playing)
        playButton.setState(Button::ButtonState::buttonDown);
    else if (state == Stopped)
        stopButton.setState(Button::ButtonState::buttonDown);
    else if (state == Paused)
        pauseButton.setState(Button::ButtonState::buttonDown);
}

void GUIComponent::resized()
{
    // If you add any child components, this is where you should
    // update their positions.
    openButton.setBounds(10, 10, getWidth() - 20, 20);
    stopButton.setBounds(67, getHeight() - 32, 30, 30);
    pauseButton.setBounds(35, getHeight() - 32, 30, 30);
    playButton.setBounds(2, getHeight() - 32, 30, 30);
}

void GUIComponent::changeListenerCallback(ChangeBroadcaster* source)
{
    if (source == &transportSource)
    {
        if (transportSource.isPlaying())
            changeState(Playing);
        else if ((state == Stopping) || (state == Playing))
            changeState(Stopped);
        else if (Pausing == state)
            changeState(Paused);
    }
}

//-----------------------Playback State Handling-----------------------------------
//=================================================================================
void GUIComponent::changeState(TransportState newState)
{
    if (state != newState) //if different new state
    {
        state = newState; //change state and perform appropriate actions
        switch (state)
        {
        case Stopped:
            stopButton.setEnabled(false);
            stopButton.setState(Button::ButtonState::buttonDown);
            pauseButton.setEnabled(false);
            pauseButton.setState(Button::ButtonState::buttonNormal);
            playButton.setEnabled(true);
            playButton.setState(Button::ButtonState::buttonNormal);
            transportSource.setPosition(0.0);
            break;
        case Starting:
            transportSource.start();
            break;
        case Playing:
            playButton.setEnabled(false);
            playButton.setState(Button::ButtonState::buttonDown);
            pauseButton.setEnabled(true);
            pauseButton.setState(Button::ButtonState::buttonNormal);
            stopButton.setEnabled(true);
            stopButton.setState(Button::ButtonState::buttonNormal);
            break;
        case Pausing:
            transportSource.stop();
            break;
        case Paused:
            pauseButton.setEnabled(false);
            pauseButton.setState(Button::ButtonState::buttonDown);
            playButton.setEnabled(true);
            playButton.setState(Button::ButtonState::buttonNormal);
            stopButton.setEnabled(true);
            stopButton.setState(Button::ButtonState::buttonNormal);
            break;
        case Stopping:
            transportSource.stop();
            break;
        }
    }
}

void GUIComponent::openButtonClicked()
{
    FileChooser chooser("Select a .wav file to play...", {}, "*.wav"); //open file browser

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

void GUIComponent::playButtonClicked()
{
    if ((state == Stopped) || (state == Paused))
        changeState(Starting);
}

void GUIComponent::pauseButtonClicked()
{
    if (state == Playing)
        changeState(Pausing);
}

void GUIComponent::stopButtonClicked()
{
    if (state == Paused)
        changeState(Stopped);
    else
        changeState(Stopping);
}