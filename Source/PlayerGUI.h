#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"

class WaveformDisplay : public juce::Component,
    public juce::ChangeListener
{
public:
    WaveformDisplay(juce::AudioThumbnail& thumb)
        : thumbnail(thumb)
    {
        thumbnail.addChangeListener(this);
    }

    ~WaveformDisplay() override
    {
        thumbnail.removeChangeListener(this);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::darkgrey.darker());
        g.setColour(juce::Colours::lightgreen);

        if (thumbnail.getTotalLength() > 0.0)
        {
            
            thumbnail.drawChannels(g,
                getLocalBounds(),
                0.0, 
                thumbnail.getTotalLength(), 
                1.0f); 
        }
        else
        {
            g.setColour(juce::Colours::white);
            g.drawText("No file loaded", getLocalBounds(), juce::Justification::centred);
        }
    }

    void changeListenerCallback(juce::ChangeBroadcaster* source) override
    {
        if (source == &thumbnail)
        {
            repaint();
        }
    }

private:
    juce::AudioThumbnail& thumbnail;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformDisplay)
};

class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener,
    public juce::Timer,
    public juce::ListBoxModel
{   
public:

    PlayerGUI();
    ~PlayerGUI() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();
    int getNumRows() override;
    void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    void listBoxItemDoubleClicked(int row, const juce::MouseEvent&) override;
private:
    PlayerAudio playerAudio;
    juce::TextButton addFilesButton{ "Add Files" }; 
    juce::TextButton restartButton{ "Restart" };
    juce::TextButton stopButton{ "Stop" };
    juce::TextButton loopButton{ "Loop" };
    juce::Slider volumeSlider;
    juce::TextButton muteButton{ "Mute" };
    std::unique_ptr<juce::FileChooser> fileChooser;
    juce::Label metadataLabel;
    juce::Array<juce::File> playlist;
    juce::ListBox playlistBox;

    void loadTrack(const juce::File& file); 

    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;
    juce::TextButton backwardButton{ "<< 10s" };
    juce::TextButton forwardButton{ "10s >>" };
    juce::TextButton playPauseButton{ "Play" }; 
    juce::TextButton goToStartButton{ "Start" };
    juce::TextButton goToEndButton{ "End" }; 
    juce::Slider positionSlider;
    juce::Label timeLabel;  
    juce::String formatTime(double seconds);
    juce::TextButton setAButton{ "Set A" };
    juce::TextButton setBButton{ "Set B" };
    double loopPointA = -1.0; 
    double loopPointB = -1.0;
    void resetABLoop();
    void timerCallback() override;
    
    juce::AudioFormatManager formatManager;
    juce::AudioThumbnailCache thumbnailCache{ 5 };
    juce::AudioThumbnail thumbnail;
    WaveformDisplay thumbnailComponent;

    juce::Slider speedSlider;
    juce::Label speedLabel;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};