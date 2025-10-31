#include "PlayerGUI.h"

PlayerGUI::PlayerGUI()
    : thumbnail(512, formatManager, thumbnailCache),
    thumbnailComponent(thumbnail, markers)
{
    formatManager.registerBasicFormats();
    addAndMakeVisible(thumbnailComponent);

    for (auto* btn : { &addFilesButton, &restartButton , &stopButton, &muteButton,&loopButton,
                       &backwardButton, &forwardButton, &playPauseButton, &goToStartButton,
                       &goToEndButton, &setAButton, &setBButton,
                       &addMarkerButton, &nextMarkerButton, &prevMarkerButton, &clearMarkersButton })
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

    addAndMakeVisible(metadataLabel);
    metadataLabel.setText("No file loaded", juce::dontSendNotification);
    metadataLabel.setJustificationType(juce::Justification::centred);
    metadataLabel.setColour(juce::Label::textColourId, juce::Colours::white);

    addAndMakeVisible(playlistBox);
    playlistBox.setModel(this);
    playlistBox.setColour(juce::ListBox::backgroundColourId, juce::Colours::darkgrey.darker(0.5f));

    startTimerHz(30);
    setAButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
    setBButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
    setBButton.setEnabled(false);

    playbackGroup.setText("Playback Controls");
    addAndMakeVisible(playbackGroup);

    loopAndSettingsGroup.setText("Loop & Settings");
    addAndMakeVisible(loopAndSettingsGroup);

    markerGroup.setText("Track Markers");
    addAndMakeVisible(markerGroup);
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
    int playlistWidth = 200;
    int gap = 10;
    int controlsWidth = getWidth() - playlistWidth - (gap * 2);
    int x = gap;
    int y = gap;

    int metadataRowHeight = 25;
    metadataLabel.setBounds(x, y, controlsWidth, metadataRowHeight);
    y += metadataRowHeight + gap;

    int sliderRowHeight = 60;
    int timeLabelWidth = 120;
    timeLabel.setBounds(x + controlsWidth - timeLabelWidth, y, timeLabelWidth, sliderRowHeight);
    thumbnailComponent.setBounds(x, y, controlsWidth - timeLabelWidth - gap, sliderRowHeight);
    positionSlider.setBounds(x, y, controlsWidth - timeLabelWidth - gap, sliderRowHeight);
    y += sliderRowHeight + gap;

    int buttonHeight = 40;
    int groupInternalPadding = 30;
    int groupHeight = buttonHeight + groupInternalPadding + gap;
    playbackGroup.setBounds(x, y, controlsWidth, groupHeight);

    int buttonY = y + groupInternalPadding;
    int xPos = x + gap;
    addFilesButton.setBounds(xPos, buttonY, 100, buttonHeight);
    xPos += 100 + gap;
    goToStartButton.setBounds(xPos, buttonY, 70, buttonHeight);
    xPos += 70 + gap;
    backwardButton.setBounds(xPos, buttonY, 70, buttonHeight);
    xPos += 70 + gap;
    playPauseButton.setBounds(xPos, buttonY, 70, buttonHeight);
    xPos += 70 + gap;
    forwardButton.setBounds(xPos, buttonY, 70, buttonHeight);
    xPos += 70 + gap;
    goToEndButton.setBounds(xPos, buttonY, 70, buttonHeight);
    xPos += 70 + gap;
    stopButton.setBounds(xPos, buttonY, 70, buttonHeight);

    y += groupHeight + gap;
    loopAndSettingsGroup.setBounds(x, y, controlsWidth, groupHeight);

    buttonY = y + groupInternalPadding;
    xPos = x + gap;
    restartButton.setBounds(xPos, buttonY, 80, buttonHeight);
    xPos += 80 + gap;
    loopButton.setBounds(xPos, buttonY, 80, buttonHeight);
    xPos += 80 + gap;
    setAButton.setBounds(xPos, buttonY, 60, buttonHeight);
    xPos += 60 + gap;
    setBButton.setBounds(xPos, buttonY, 60, buttonHeight);
    xPos += 60 + gap;
    muteButton.setBounds(xPos, buttonY, 80, buttonHeight);
    xPos += 80 + gap;
    speedLabel.setBounds(xPos, buttonY, 50, buttonHeight);
    xPos += 50;
    speedSlider.setBounds(xPos, buttonY + 5, 120, 30);
    xPos += 120 + gap;
    volumeSlider.setBounds(xPos, buttonY + 5, controlsWidth - xPos, 30);

    y += groupHeight + gap; 

    markerGroup.setBounds(x, y, controlsWidth, groupHeight);

    buttonY = y + groupInternalPadding;
    xPos = x + gap;
    addMarkerButton.setBounds(xPos, buttonY, 100, buttonHeight);
    xPos += 100 + gap;
    prevMarkerButton.setBounds(xPos, buttonY, 70, buttonHeight);
    xPos += 70 + gap;
    nextMarkerButton.setBounds(xPos, buttonY, 70, buttonHeight);
    xPos += 70 + gap;
    clearMarkersButton.setBounds(xPos, buttonY, 120, buttonHeight);


    playlistBox.setBounds(getWidth() - playlistWidth - gap, gap, playlistWidth, getHeight() - (gap * 2));
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
    if (button == &addFilesButton)
    {
        fileChooser = std::make_unique<juce::FileChooser>(
            "Select audio files...",
            juce::File{},
            "*.wav;*.mp3",
            true);

        fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::canSelectMultipleItems,
            [this](const juce::FileChooser& fc)
            {
                auto files = fc.getResults();
                for (const auto& file : files)
                {
                    if (file.existsAsFile())
                    {
                        playlist.add(file);
                    }
                }
                playlistBox.updateContent();

                if (!playerAudio.isPlaying() && playlist.size() > 0)
                {
                    loadTrack(playlist[0], 0.0);
                    playlistBox.selectRow(0);
                }
            });
    }

    if (button == &restartButton)
    {
        playerAudio.setPosition(0.0);
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

    if (button == &addMarkerButton)
    {
        addMarker();
    }
    if (button == &nextMarkerButton)
    {
        goToNextMarker();
    }
    if (button == &prevMarkerButton)
    {
        goToPrevMarker();
    }
    if (button == &clearMarkersButton)
    {
        clearMarkers();
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

void PlayerGUI::loadTrack(const juce::File& file, double startPosition)
{
    clearMarkers();

    currentLoadedFile = file;

    juce::String metadataText = playerAudio.loadFile(file);
    if (metadataText.isNotEmpty())
    {
        playPauseButton.setButtonText("Pause");
        resetABLoop();
        thumbnail.setSource(new juce::FileInputSource(file));
        metadataLabel.setText(metadataText, juce::dontSendNotification);

        playerAudio.setPosition(startPosition);
    }
    else
    {
        metadataLabel.setText("Failed to load file: " + file.getFileName(), juce::dontSendNotification);
        currentLoadedFile = juce::File{};
    }
}

int PlayerGUI::getNumRows()
{
    return playlist.size();
}

void PlayerGUI::paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected)
{
    if (rowNumber < 0 || rowNumber >= playlist.size())
        return;

    if (rowIsSelected)
    {
        g.fillAll(juce::Colours::lightblue.withAlpha(0.5f));
    }

    g.setColour(juce::Colours::white);
    g.drawText(playlist[rowNumber].getFileName(),
        5, 0, width - 10, height,
        juce::Justification::centredLeft, true);
}

void PlayerGUI::listBoxItemDoubleClicked(int row, const juce::MouseEvent&)
{
    if (row >= 0 && row < playlist.size())
    {
        loadTrack(playlist[row], 0.0);
    }
}

void PlayerGUI::addFileToPlaylist(const juce::File& file)
{
    if (!playlist.contains(file))
    {
        playlist.add(file);
        playlistBox.updateContent();
    }

    for (int i = 0; i < playlist.size(); ++i)
    {
        if (playlist[i] == file)
        {
            playlistBox.selectRow(i);
            break;
        }
    }
}

void PlayerGUI::addMarker()
{
    double currentPos = playerAudio.getPosition();
    if (!markers.contains(currentPos))
    {
        markers.add(currentPos);
        markers.sort();
        thumbnailComponent.repaint();
    }
}

void PlayerGUI::goToNextMarker()
{
    double currentPos = playerAudio.getPosition();
    for (double markerPos : markers)
    {
        if (markerPos > (currentPos + 0.1))
        {
            playerAudio.setPosition(markerPos);
            return;
        }
    }

    if (markers.size() > 0)
    {
        playerAudio.setPosition(markers[0]);
    }
}

void PlayerGUI::goToPrevMarker()
{
    double currentPos = playerAudio.getPosition();
    double prevMarker = -1.0;

    for (double markerPos : markers)
    {
        if (markerPos < (currentPos - 0.1))
        {
            prevMarker = markerPos;
        }
        else
        {
            break;
        }
    }

    if (prevMarker != -1.0)
    {
        playerAudio.setPosition(prevMarker);
    }
    else if (markers.size() > 0)
    {
        playerAudio.setPosition(markers.getLast());
    }
}

void PlayerGUI::clearMarkers()
{
    markers.clear();
    thumbnailComponent.repaint();
}