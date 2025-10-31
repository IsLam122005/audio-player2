#include "PlayerGUI.h"

PlayerGUI::PlayerGUI()
    : thumbnail(512, formatManager, thumbnailCache),
    thumbnailComponent(thumbnail)
{
    formatManager.registerBasicFormats();
    addAndMakeVisible(thumbnailComponent);

    for (auto* btn : { &loadButton, &restartButton , &stopButton, &muteButton,&loopButton, &backwardButton, &forwardButton, &playPauseButton, &goToStartButton, &goToEndButton, &setAButton, &setBButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    addAndMakeVisible(positionSlider);
    positionSlider.addListener(this);
    positionSlider.setRange(0.0, 1.0);
    positionSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    positionSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    positionSlider.setColour(juce::Slider::trackColourId, juce::Colours::transparentBlack);
    positionSlider.setColour(juce::Slider::thumbColourId, juce::Colours::red);

    addAndMakeVisible(timeLabel);
    timeLabel.setText("00:00 / 00:00", juce::dontSendNotification);
    timeLabel.setJustificationType(juce::Justification::centredRight);

    speedSlider.setRange(0.5, 2.0, 0.1);
    speedSlider.setValue(1.0);
    speedSlider.addListener(this);
    addAndMakeVisible(speedSlider);
    speedLabel.setText("Speed:", juce::dontSendNotification);
    addAndMakeVisible(speedLabel);

    startTimerHz(30);
    setAButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
    setBButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
    setBButton.setEnabled(false);
}
juce::String PlayerGUI::formatTime(double seconds)
{
    int totalSeconds = static_cast<int>(std::round(seconds));
    int mins = totalSeconds / 60;
    int secs = totalSeconds % 60;
    return juce::String::formatted("%02d:%02d", mins, secs);
}

PlayerGUI::~PlayerGUI()
{
    stopTimer();
}

void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}
void PlayerGUI::resized()
{
    int sliderRowHeight = 60;
    int buttonHeight = 40;
    int gap = 10;
    int x = gap;


    int y_slider_row = 20;
    int timeLabelWidth = 120;
    timeLabel.setBounds(getWidth() - timeLabelWidth - gap, y_slider_row, timeLabelWidth, sliderRowHeight);

    thumbnailComponent.setBounds(x, y_slider_row, getWidth() - timeLabelWidth - (gap * 3), sliderRowHeight);
    positionSlider.setBounds(x, y_slider_row, getWidth() - timeLabelWidth - (gap * 3), sliderRowHeight);


    int y_row1 = y_slider_row + sliderRowHeight + gap;
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


    int y_row2 = y_row1 + buttonHeight + gap;
    restartButton.setBounds(x, y_row2, 80, buttonHeight);
    x += 80 + gap;

    loopButton.setBounds(x, y_row2, 80, buttonHeight);
    x += 80 + gap;


    setAButton.setBounds(x, y_row2, 60, buttonHeight);
    x += 60 + gap;

    setBButton.setBounds(x, y_row2, 60, buttonHeight);
    x += 60 + gap;

    muteButton.setBounds(x, y_row2, 80, buttonHeight);
    x += 80 + gap;
    speedLabel.setBounds(x, y_row2, 50, buttonHeight);
    x += 50;
    speedSlider.setBounds(x, y_row2 + 5, 120, 30);
    x += 120 + gap;
    volumeSlider.setBounds(x, y_row2 + 5, getWidth() - x - gap, 30);
}
 void PlayerGUI::resetABLoop()
 {
     loopPointA = -1.0;
     loopPointB = -1.0;
     setAButton.setButtonText("Set A");
     setBButton.setButtonText("Set B");
     setBButton.setEnabled(false); 
     setAButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
     setBButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
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
                    resetABLoop();
                    thumbnail.setSource(new juce::FileInputSource(file));
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
        volumeSlider.setValue(newGain, juce::dontSendNotification);
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
    if (button == &setAButton)
    {
        if (loopPointA == -1.0) 
        {
            loopPointA = playerAudio.getPosition(); 
            setAButton.setButtonText("A Set");
            setAButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green.darker(0.3f));
            setBButton.setEnabled(true); 
        }
        else 
        {
            resetABLoop(); 
        }
    }

    if (button == &setBButton)
    {
        if (loopPointB == -1.0) 
        {
            double currentPos = playerAudio.getPosition();
            if (currentPos > loopPointA) 
            {
                loopPointB = currentPos; 
                setBButton.setButtonText("B Set");
                setBButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red.darker(0.3f));
            }
            
        }
        else 
        {
            loopPointB = -1.0; 
            setBButton.setButtonText("Set B");
            setBButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
        }
    }
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
    {
        playerAudio.setGain((float)slider->getValue());
        muteButton.setButtonText("Mute");
    }
    if (slider == &positionSlider)
    {
        if (slider->isMouseButtonDown())
        {
            playerAudio.setPosition(slider->getValue());
        }
    }
    if (slider == &speedSlider)
    {
        playerAudio.setSpeed(slider->getValue());
    }
}
void PlayerGUI::timerCallback()
{
    double trackLength = playerAudio.getLength();
    double currentPos = playerAudio.getPosition();

    positionSlider.setRange(0.0, trackLength, juce::dontSendNotification);
    if (loopPointA != -1.0 && loopPointB != -1.0) 
    {
        
        if (currentPos >= loopPointB)
        {
            
            playerAudio.setPosition(loopPointA);
            
            currentPos = loopPointA;
        }
    }
    positionSlider.setValue(currentPos, juce::dontSendNotification);

    timeLabel.setText(formatTime(currentPos) + " / " + formatTime(trackLength),
        juce::dontSendNotification);
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


