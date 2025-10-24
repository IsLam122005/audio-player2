#include "PlayerGUI.h"

PlayerGUI::PlayerGUI()
{
    for (auto* btn : { &loadButton, &restartButton , &stopButton, &muteButton,&loopButton, &backwardButton, &forwardButton })
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
    int y = 20;
    int buttonHeight = 40;
    int buttonWidth = 70; // تم تصغير العرض لاستيعاب جميع الأزرار

    // الزر 1: Load (عرض 100)
    loadButton.setBounds(10, y, 100, buttonHeight);

    // الزر 2: Restart (عرض 70)
    restartButton.setBounds(120, y, buttonWidth, buttonHeight);

    // الزر 3: Stop (عرض 70)
    stopButton.setBounds(200, y, buttonWidth, buttonHeight);

    // الزر 4: Loop (عرض 70)
    loopButton.setBounds(280, y, buttonWidth, buttonHeight);

    // الزر 5: << 10s (الجديد)
    backwardButton.setBounds(360, y, buttonWidth, buttonHeight);

    // الزر 6: 10s >> (الجديد)
    forwardButton.setBounds(440, y, buttonWidth, buttonHeight);

    // الزر 7: Mute (عرض 70)
    muteButton.setBounds(520, y, 70, 40);

    volumeSlider.setBounds(20, 100, getWidth() - 40, 30);
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
                }
            });
    }

    if (button == &restartButton)
    {
        playerAudio.play();
    }

    if (button == &stopButton)
    {
        playerAudio.stop();
    }

    // الجديد: منطق القفز للأمام
    if (button == &forwardButton)
    {
        playerAudio.jumpForward(10.0);
    }

    // الجديد: منطق القفز للخلف
    if (button == &backwardButton)
    {
        playerAudio.jumpBackward(10.0);
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