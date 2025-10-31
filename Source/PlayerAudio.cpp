#include "PlayerAudio.h"

PlayerAudio::PlayerAudio()
    : storedSamplesPerBlock(0), storedSampleRate(0.0)
{
    formatManager.registerBasicFormats();
}

PlayerAudio::~PlayerAudio()
{
    releaseResources();
}

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    storedSamplesPerBlock = samplesPerBlockExpected;
    storedSampleRate = sampleRate;

    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);

    if (resamplingSource != nullptr)
    {
        resamplingSource->prepareToPlay(samplesPerBlockExpected, sampleRate);
    }
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (resamplingSource != nullptr)
    {
        resamplingSource->getNextAudioBlock(bufferToFill);
    }
    else
    {
        bufferToFill.clearActiveBufferRegion();
    }
}

void PlayerAudio::releaseResources()
{
    resamplingSource.reset();
    transportSource.releaseResources();
    readerSource.reset();
}
void PlayerAudio::toggleLooping(bool shouldLoop)
{
    isLooping = shouldLoop;
    if (readerSource != nullptr)
    {
        readerSource->setLooping(isLooping);
    }
}

bool PlayerAudio::loadFile(const juce::File& file)
{
    if (auto* reader = formatManager.createReaderFor(file))
    {
        transportSource.stop();
        transportSource.setSource(nullptr);
        resamplingSource.reset();
        readerSource.reset();

        readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, isLooping);

        transportSource.setSource(readerSource.get(), 0, nullptr, reader->sampleRate);

        resamplingSource = std::make_unique<juce::ResamplingAudioSource>(&transportSource, false, 2);

        if (storedSampleRate > 0)
        {
            resamplingSource->prepareToPlay(storedSamplesPerBlock, storedSampleRate);
        }

        resamplingSource->setResamplingRatio(1.0);
        transportSource.start();
        return true;
    }
    return false;
}

void PlayerAudio::play()
{
    transportSource.start();
}

void PlayerAudio::stop()
{
    transportSource.stop();
    transportSource.setPosition(0.0);
}

void PlayerAudio::setGain(float gain)
{
    transportSource.setGain(gain);
    isMuted = false;
    if (gain > 0.0f)
        volumeBeforeMute = gain;
}

double PlayerAudio::getPosition() const
{
    return transportSource.getCurrentPosition();
}

double PlayerAudio::getLength() const
{
    if (readerSource != nullptr)
        return readerSource->getTotalLength() / readerSource->getAudioFormatReader()->sampleRate;
    return 0.0;
}

float PlayerAudio::toggleMute()
{
    isMuted = !isMuted;
    if (isMuted)
    {
        float currentGain = transportSource.getGain();
        if (currentGain > 0.0f)
            volumeBeforeMute = currentGain;

        transportSource.setGain(0.0f);
        return 0.0f;
    }
    else
    {
        transportSource.setGain(volumeBeforeMute);
        return volumeBeforeMute;
    }
} 


void PlayerAudio::setPosition(double pos)
{
    transportSource.setPosition(pos);
}

void PlayerAudio::jumpForward(double seconds)
{
    double newPosition = transportSource.getCurrentPosition() + seconds;
    double trackLength = getLength();

    if (trackLength > 0.0)
    {
        if (newPosition > trackLength)
            newPosition = trackLength - 0.01;

        transportSource.setPosition(newPosition);
    }
}

void PlayerAudio::jumpBackward(double seconds)
{
    double newPosition = transportSource.getCurrentPosition() - seconds;

    if (newPosition < 0.0)
        newPosition = 0.0;

    transportSource.setPosition(newPosition);
}
void PlayerAudio::togglePlayPause()
{
    if (transportSource.isPlaying())
    {
        transportSource.stop();
    }
    else
    {
        transportSource.start();
    }
}
bool PlayerAudio::isPlaying() const
{
    return transportSource.isPlaying();
}
void PlayerAudio::setSpeed(double speed)
{
    if (resamplingSource != nullptr)
    {
        resamplingSource->setResamplingRatio(speed);
    }
}