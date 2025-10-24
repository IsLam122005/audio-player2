#pragma once
#include <JuceHeader.h>

class PlayerAudio
{
public:
    PlayerAudio();
    ~PlayerAudio();

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();

    bool loadFile(const juce::File& file);
    void play();
    void stop();
    void setGain(float gain);
    void setPosition(double pos);
    void toggleLooping(bool shouldLoop);

    double getPosition() const;
    double getLength() const;
    float toggleMute();
    void jumpForward(double seconds);
    void jumpBackward(double seconds);

private:
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    bool isMuted = false;
    float volumeBeforeMute = 0.5f;
    bool isLooping = false;
};