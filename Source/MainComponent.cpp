#include "MainComponent.h"

MainComponent::MainComponent()
{
    juce::PropertiesFile::Options options;
    options.applicationName = "Simple Audio Player";
    options.filenameSuffix = "settings";
    options.osxLibrarySubFolder = "Application Support";
    options.folderName = "SimpleAudioPlayer";
    appProperties.setStorageParameters(options);

    addAndMakeVisible(player1);

    setSize(800, 500);

    setAudioChannels(0, 2);

    loadLastSession();
}

MainComponent::~MainComponent()
{
    saveLastSession();

    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    player1.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    player1.releaseResources();
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void MainComponent::resized()
{
    player1.setBounds(getLocalBounds());
}


void MainComponent::saveLastSession()
{
    if (auto* settings = appProperties.getUserSettings())
    {
        juce::File currentFile = player1.getCurrentFile();
        if (currentFile.existsAsFile())
        {
            settings->setValue("lastFile", currentFile.getFullPathName());
            settings->setValue("lastPosition", player1.getCurrentPosition());
        }
        else
        {
            settings->removeValue("lastFile");
            settings->removeValue("lastPosition");
        }
    }
}

void MainComponent::loadLastSession()
{
    if (auto* settings = appProperties.getUserSettings())
    {
        juce::String lastFilePath = settings->getValue("lastFile");
        if (lastFilePath.isNotEmpty())
        {
            juce::File lastFile(lastFilePath);
            if (lastFile.existsAsFile())
            {
                double lastPosition = settings->getDoubleValue("lastPosition", 0.0);

                player1.addFileToPlaylist(lastFile);
                player1.loadTrack(lastFile, lastPosition);
            }
        }
    }
}
