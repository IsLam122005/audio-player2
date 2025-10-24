#include "PlayerGUI.h"

PlayerGUI::PlayerGUI()
{
    for (auto* btn : { &loadButton, &restartButton , &stopButton, &muteButton,&loopButton, &backwardButton, &forwardButton, &playPauseButton, &goToStartButton, &goToEndButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);
}

PlayerGUI::~PlayerGUI()
{
}

void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void PlayerGUI::resized()
{
    int y_row1 = 20;   
    int y_row2 = 80;  
    int buttonHeight = 40;
    int gap = 10;      
    int x = gap;

    loadButton.setBounds(x, y_row1, 100, buttonHeight);
    x += 100 + gap;

    goToStartButton.setBounds(x, y_row1, 70, buttonHeight);
    x += 70 + gap; 

    backwardButton.setBounds(x, y_row1, 70, buttonHeight);
    x += 70 + gap; 

    playPauseButton.setBounds(x, y_row1, 70, buttonHeight);
    x += 70 + gap;

    forwardButton.setBounds(x, y_row1, 70, buttonHeight);
    x += 70 + gap; 

    goToEndButton.setBounds(x, y_row1, 70, buttonHeight);
    x += 70 + gap;

    stopButton.setBounds(x, y_row1, 70, buttonHeight);
    x = gap; 

    restartButton.setBounds(x, y_row2, 80, buttonHeight);
    x += 80 + gap;

    loopButton.setBounds(x, y_row2, 80, buttonHeight);
    x += 80 + gap;

    muteButton.setBounds(x, y_row2, 80, buttonHeight);
    x += 80 + gap;

    volumeSlider.setBounds(x, y_row2 + 5, getWidth() - x - gap, 30);
}

void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        fileChooser = std::make_unique<juce::FileChooser>(
            "Select an audio file...",
            juce::File{},
            "*.wav;*.mp3");

        fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                if (file.existsAsFile())
                {
                    playerAudio.loadFile(file);
                    playPauseButton.setButtonText("Pause");
                }
            });
    }

    if (button == &restartButton)
    {
        playerAudio.play();
        playPauseButton.setButtonText("Pause");
    }

    if (button == &stopButton)
    {
        playerAudio.stop();
        playPauseButton.setButtonText("Play"); 
    }

    if (button == &forwardButton)
    {
        playerAudio.jumpForward(10.0);
    }
    if (button == &backwardButton)
    {
        playerAudio.jumpBackward(10.0);
    }


    if (button == &playPauseButton)
    {
        playerAudio.togglePlayPause();
        if (playerAudio.isPlaying())
        {
            playPauseButton.setButtonText("Pause");
        }
        else
        {
            playPauseButton.setButtonText("Play");
        }
    }

    if (button == &goToStartButton)
    {
        playerAudio.setPosition(0.0);
    }

    if (button == &goToEndButton)
    {
        double trackLength = playerAudio.getLength();
        if (trackLength > 0.0)
            playerAudio.setPosition(trackLength - 0.01);
    }


    if (button == &muteButton)
    {
        float newGain = playerAudio.toggleMute();
        volumeSlider.setValue(newGain);

        if (newGain == 0.0f)
            muteButton.setButtonText("Unmute");
        else
            muteButton.setButtonText("Mute");
    }
    if (button == &loopButton)
    {

        bool newState = !button->getToggleState();
        button->setToggleState(newState, juce::dontSendNotification);


        playerAudio.toggleLooping(newState);


        if (newState)
            button->setColour(juce::TextButton::buttonColourId, juce::Colours::green.darker(0.3f));
        else
            button->setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
    }
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
    {
        playerAudio.setGain((float)slider->getValue());

     
        muteButton.setButtonText("Mute");
    }
}

void PlayerGUI::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    playerAudio.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerGUI::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    playerAudio.getNextAudioBlock(bufferToFill);
}

void PlayerGUI::releaseResources()
{
    playerAudio.releaseResources();
}


