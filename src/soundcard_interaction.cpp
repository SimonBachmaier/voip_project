
#include "soundcard_interaction.h"

#include <iostream>


SoundcardInteraction::SoundcardInteraction(SoundcardAudioProcessor* processor) : processor_(processor), initialized_(false), isRunning_(false)
{
}

/**
* Initializes object with passed options and opens RtAudio stream.
*/
bool SoundcardInteraction::init(SoundcardInteraction::Options optionsParam)
{
    options_ = optionsParam;
    bool errorFlag = false;

    validateOptions(errorFlag);
    if (errorFlag) return false;

    initalizeInputOutputParameters();
    updateInputOutputBuffers();

    openStream(errorFlag);
    if (errorFlag) return false;

    initialized_ = true;
    return initialized_;
}

/**
* Validates set options.
*/
void SoundcardInteraction::validateOptions(bool& errorFlag)
{
    if (options_.inputChannels == 0 ||
        options_.outputChannels == 0 ||
        options_.sampleRate == 0 ||
        options_.frameSize == 0 ||
        options_.sampleFormat == AudioBuffer::SampleFormat::UNKNOWN)
        errorFlag = true;

    if (options_.inputDevice < 0)
        options_.inputDevice = rtAudio_.getDefaultInputDevice();

    if (options_.outputDevice < 0)
        options_.outputDevice = rtAudio_.getDefaultOutputDevice();
}

/**
* Initializes the input and output parameters with set options.
*/
void SoundcardInteraction::initalizeInputOutputParameters()
{
    inputParameters_.deviceId = options_.inputDevice;
    inputParameters_.nChannels = options_.inputChannels;
    inputParameters_.firstChannel = 0;

    outputParameters_.deviceId = options_.outputDevice;
    outputParameters_.nChannels = options_.outputChannels;
    outputParameters_.firstChannel = 0;
}

/**
* Updates the options of the input and output buffer with currently set options.
*/
void SoundcardInteraction::updateInputOutputBuffers()
{
    AudioBuffer::Options inputBufferOptions = inputBuffer_.getOptions();
    AudioBuffer::Options outputBufferOptions = outputBuffer_.getOptions();

    inputBufferOptions.sampleRate = outputBufferOptions.sampleRate = options_.sampleRate;
    inputBufferOptions.frameSize = outputBufferOptions.frameSize = options_.frameSize;
    inputBufferOptions.sampleFormat = outputBufferOptions.sampleFormat = options_.sampleFormat;

    inputBufferOptions.numberOfChannels = options_.inputChannels;
    outputBufferOptions.numberOfChannels = options_.outputChannels;

    inputBuffer_.updateOptions(inputBufferOptions);
    outputBuffer_.updateOptions(outputBufferOptions);
}

/**
* Opens the RtAudio stream with the set options and passes callback.
*/
void SoundcardInteraction::openStream(bool& errorFlag)
{
    RtAudioFormat rtaudiofmt = getRtAudioFormat(errorFlag);
    unsigned int expectedFramesize = options_.frameSize;

    try {
        rtAudio_.openStream(&outputParameters_, &inputParameters_, rtaudiofmt, options_.sampleRate, &expectedFramesize, rtAudioCallback, static_cast<void*>(this));
    }
    catch (RtAudioError& e)
    {
        std::cerr << "Error initializing!" << std::endl;
        e.printMessage();
        errorFlag = true;
    }

    adjustForFramesizeDifference(expectedFramesize);
}

/**
* Returns set sample format.
*/
RtAudioFormat SoundcardInteraction::getRtAudioFormat(bool& errorFlag)
{
    RtAudioFormat rtaudiofmt = RTAUDIO_SINT16;

    switch (options_.sampleFormat)
    {
    case AudioBuffer::SampleFormat::INT16 :
        // default value
        break;
    case AudioBuffer::SampleFormat::FLOAT32 :
        rtaudiofmt = RTAUDIO_FLOAT32;
        break;
    default:
        std::cerr << "Unsupported sample format!" << std::endl;
        errorFlag = true;
        break;
    }

    return rtaudiofmt;
}

/**
* Checks if expected frame size differs from set frame size and updates buffers if necessary
*/
void SoundcardInteraction::adjustForFramesizeDifference(unsigned int expectedFramesize)
{
    if (expectedFramesize != options_.frameSize)
    {
        std::cerr << "Warning: requested framesize differs from actual framesize ";
        std::cerr << "(Requested: " << options_.frameSize << " Actual: " << expectedFramesize << ")!" << std::endl;
        std::cerr << "Adjusting framesize to " << expectedFramesize << std::endl;

        options_.frameSize = expectedFramesize;
        updateInputOutputBuffers();
    }
}

/**
* Static function passed as callback to RtAudio with the openStream function
*/
int SoundcardInteraction::rtAudioCallback(void* outBuf, void* inBuf, unsigned int nFrames, double streamTime, RtAudioStreamStatus status, void* userData)
{
    SoundcardInteraction *instance = static_cast<SoundcardInteraction*>(userData);
    return instance->processRtAudioCallback(outBuf, inBuf, nFrames, streamTime, status);
}

/**
* Non static function called from the rtAudioCallback function to check audio data, copy input and output buffers and call the process function
*/
int SoundcardInteraction::processRtAudioCallback(void* outBuf, void* inBuf, unsigned int nFrames, double streamTime, RtAudioStreamStatus status)
{
    adjustForFramesizeDifference(nFrames);

    ::memcpy(inputBuffer_.data(), inBuf, inputBuffer_.size());
    int retval = processor_->process(outputBuffer_, inputBuffer_);
    ::memcpy(outBuf, outputBuffer_.data(), outputBuffer_.size());

    return retval;
}

/**
* Starts the already opened RtAudio stream
*/
bool SoundcardInteraction::start()
{
    if (initialized_ == false)
        return false;

    try {
        rtAudio_.startStream();
    }
    catch (RtAudioError& e)
    {
        std::cerr << "Error starting stream!" << std::endl;
        e.printMessage();
        return false;
    }

    isRunning_ = true;
    return isRunning_;
}

/**
* Stops and closes current RtAudio stream if it is opened
*/
bool SoundcardInteraction::stop()
{
    if (isRunning_ == false)
        return false;

    try
    {
        rtAudio_.stopStream();
        if (rtAudio_.isStreamOpen())
        {
            rtAudio_.closeStream();
            initialized_ = false;
        }
    }
    catch (RtAudioError& e)
    {
        std::cerr << "Error stopping stream!" << std::endl;
        e.printMessage();
    }

    isRunning_ = false;
    return true;
}

/**
* Returns the set options
*/
SoundcardInteraction::Options SoundcardInteraction::getOptions()
{
    return options_;
}

/**
* Helper function used in list devices to display the supported sample formats
*/
static std::string supportedSampleTypes(RtAudio::DeviceInfo const& info)
{
    std::stringstream s;
    s << ((info.nativeFormats & RTAUDIO_SINT8) ? "SINT8 " : "");
    s << ((info.nativeFormats & RTAUDIO_SINT16) ? "SINT16 " : "");
    s << ((info.nativeFormats & RTAUDIO_SINT24) ? "SINT24 " : "");
    s << ((info.nativeFormats & RTAUDIO_SINT32) ? "SINT32 " : "");
    s << ((info.nativeFormats & RTAUDIO_FLOAT32) ? "FLOAT32 " : "");
    s << ((info.nativeFormats & RTAUDIO_FLOAT64) ? "FLOAT64 " : "");
    return s.str();
}
/**
* Prints all audio devices on this machine.
*/
void SoundcardInteraction::listDevices()
{
    RtAudio audio;

    // Disable display of warning messages
    audio.showWarnings(false);

    const unsigned int nDevs = audio.getDeviceCount();

    RtAudio::DeviceInfo info;
    for (unsigned int i = 0; i < nDevs; ++i)
    {
        info = audio.getDeviceInfo(i);
        if (info.probed)
        {
            std::cerr << std::endl;
            std::cerr << "************************************************************" << std::endl;
            std::cerr << "Device ID = " << i << std::endl;
            std::cerr << "Device Name = " << info.name << std::endl;
            std::cerr << "Supported Channels (in/out/duplex) = ";
            std::cerr << info.inputChannels << "/" << info.outputChannels << "/" << info.duplexChannels << std::endl;
            std::cerr << "Native sample rates = ";
            for (unsigned int i = 0; i < info.sampleRates.size(); ++i)
            {
                std::cerr << info.sampleRates[i] << "Hz" << (i == info.sampleRates.size() - 1 ? " " : ", ");
            }
            std::cerr << std::endl;
            std::cerr << "Native sample types = " << supportedSampleTypes(info) << std::endl;

            std::cerr << "Is default input/output device = " << (info.isDefaultInput ? "YES" : "NO") << " / ";
            std::cerr << (info.isDefaultOutput ? "YES" : "NO") << std::endl;
        }
    }
    std::cerr << "************************************************************" << std::endl << std::endl;
}