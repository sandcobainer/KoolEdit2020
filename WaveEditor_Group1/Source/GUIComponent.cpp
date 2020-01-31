/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "GUIComponent.h"

//==============================================================================
GUIComponent::GUIComponent() 
    : state(Stopped)
{
    //GUI buttons
    addAndMakeVisible(&openButton);
    openButton.setButtonText("Open...");
    openButton.onClick = [this] {openButtonClicked(); };

    addAndMakeVisible(&playButton);
    playButton.setButtonText("Play");
    playButton.onClick = [this] {playButtonClicked(); };
    playButton.setColour(TextButton::buttonColourId, Colours::green);
    playButton.setEnabled(false);

    addAndMakeVisible(&pauseButton);
    pauseButton.setButtonText("Pause");
    pauseButton.onClick = [this] {pauseButtonClicked(); };
    pauseButton.setColour(TextButton::buttonColourId, Colours::yellow);
    pauseButton.setEnabled(false);

    addAndMakeVisible(&stopButton);
    stopButton.setButtonText("Stop");
    stopButton.onClick = [this] {stopButtonClicked(); };
    stopButton.setColour(TextButton::buttonColourId, Colours::red);
    stopButton.setEnabled(false);

    setSize(300, 200);

    formatManager.registerBasicFormats();
    transportSource.addChangeListener(this);

    setAudioChannels(0, 2); //0 input, 2 output
}

GUIComponent::~GUIComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

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

//==============================================================================
void GUIComponent::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void GUIComponent::resized()
{
    // If you add any child components, this is where you should
    // update their positions.
    openButton.setBounds(10, 10, getWidth() - 20, 20);
    stopButton.setBounds(90, getHeight() - 25, 30, 20);
    pauseButton.setBounds(44, getHeight() - 25, 40, 20);
    playButton.setBounds(8, getHeight() - 25, 30, 20);
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