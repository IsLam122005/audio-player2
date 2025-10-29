#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"

class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener,
    public juce::Timer 
    
{
public:
    PlayerGUI();
    ~PlayerGUI() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();

private:
    PlayerAudio playerAudio;

    juce::TextButton loadButton{ "Load File" };
    juce::TextButton restartButton{ "Restart" };
    juce::TextButton stopButton{ "Stop" };
    juce::TextButton loopButton{ "Loop" };
    juce::Slider volumeSlider;
    juce::TextButton muteButton{ "Mute" };
    std::unique_ptr<juce::FileChooser> fileChooser;

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
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};