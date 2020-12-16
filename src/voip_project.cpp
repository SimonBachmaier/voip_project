
#include "voip_project.h"

VoipProject::VoipProject() : soundcardInteraction_(this), isRunning_(false) {}

/**
* Entry point of the voip project. 
* Parses arguments, starts and stops the application on receiving an "enter" key 
*/
int VoipProject::run(int argc, char* argv[])
{
    VoipProject::Options* options = new VoipProject::Options();
    if (processArguments(argc, argv, options) == false)
    {
        std::cerr << "Error processing arguments!" << std::endl;
        return -1;
    }

    start(options);

    // Stop on enter
    char input;
    std::cin.get(input);

    stop();

    return 0;
}

/**
* Sets up the application by initializing soundcard interaction, starting the sender starting and receiver thread.
*/
void VoipProject::start(VoipProject::Options* options)
{
    initSoundcard(options);
    //receiver_.start(options->destinationIp, options->remotePort, options->sampleRate, options->numberOfOutputChannels, options->frameSize);
    //sender_.start(options->destinationIp, options->remotePort, options->sampleRate, options->numberOfInputChannels);
    // Set to 2 for testing
    receiver_.start(options->destinationIp, options->remotePort, options->sampleRate, 2, options->frameSize, options->isUsingOpus);
    sender_.start(options->destinationIp, options->remotePort, options->sampleRate, 2, options->isUsingOpus);
    soundcardInteraction_.start(); 
    isRunning_ = true;
}

/**
* Initializes soundcard interaction by passing specific options.
*/
void VoipProject::initSoundcard(VoipProject::Options* options)
{
    SoundcardInteraction::Options soundcardOptions;
    soundcardOptions.inputDevice = options->inputDevice;
    soundcardOptions.outputDevice = options->outputDevice;
    soundcardOptions.inputChannels = options->numberOfInputChannels;
    soundcardOptions.outputChannels = options->numberOfOutputChannels;
    soundcardOptions.sampleRate = options->sampleRate;
    soundcardOptions.frameSize = options->frameSize;
    soundcardOptions.sampleFormat = AudioBuffer::SampleFormat::FLOAT32;
    soundcardInteraction_.init(soundcardOptions);
}

/**
* Stops the application.
*/
void VoipProject::stop()
{
    isRunning_ = false;
    sender_.stop();
    receiver_.stop();
    soundcardInteraction_.stop();
}

/**
* Parses command line arguments and sets options.
*/
bool VoipProject::processArguments(int argc, char *argv[], VoipProject::Options* options)
{
  try {
    TCLAP::CmdLine cmd("VoIP Project using Opus, RtAudio and Sockets", ' ', "0.1");

    // This is the only command line argument currently used
    TCLAP::SwitchArg listDevices("l", "list-devices", "List audio devices", cmd, false);

    // Configuration options
    TCLAP::ValueArg<int> inputDevice("i", "input-device", "Select input device", false, -1, "int", cmd);
    TCLAP::ValueArg<int> outputDevice("o", "output-device", "Select output device", false, -1, "int", cmd);
    TCLAP::ValueArg<unsigned int> inputChannels("",  "inCh", "Number of input channels (default: 1)", false, 1, "unsigned int", cmd);
    TCLAP::ValueArg<unsigned int> outputChannels("",  "outCh", "Number of output channels (default 2)", false, 2, "unsigned int", cmd);
    TCLAP::ValueArg<unsigned int> frameSize("f", "Framesize", "Framesize (default: 480)", false, 480, "unsigned int", cmd);
    TCLAP::ValueArg<unsigned int> sampleRate("s", "samplerate", "Samplerate (default: 48000)", false, 48000, "unsigned int", cmd);
    TCLAP::ValueArg<std::string> remotePort("", "rPort", "Remote Port (default: 1976)", false, "1976", "std::string", cmd);
    TCLAP::ValueArg<std::string> localPort("", "lPort", "Local Port (default: 1976)", false, "1976", "std::string", cmd);
    TCLAP::ValueArg<bool> isUsingOpus("", "useOpus", "useOpus encoding", false, true, "bool", cmd);
    TCLAP::UnlabeledValueArg<std::string> destinationIp("destIp", "Destination IP address", false, "127.0.0.1", "std::string", cmd);

    cmd.parse(argc, argv);

    /* Add argument processing here */
    if (listDevices.getValue())
    {
      this->listDevices();
      exit(0);
    }

    // if -l is not specified, the IP is mandatory
    // in order to establish an endpoint connection
    if (destinationIp.getValue() == "")
    {
      TCLAP::StdOutput().usage(cmd);
      exit(-1);
    }

    options->isUsingOpus = isUsingOpus.getValue();
    options->inputDevice = inputDevice.getValue();
    options->outputDevice = outputDevice.getValue();
    options->numberOfInputChannels = inputChannels.getValue();
    options->numberOfOutputChannels = outputChannels.getValue();
    options->frameSize = frameSize.getValue();
    options->sampleRate= sampleRate.getValue();
    options->remotePort= remotePort.getValue();
    options->localPort = localPort.getValue();
    options->destinationIp = destinationIp.getValue();

  } catch (TCLAP::ArgException& argEx)
  {
    std::cerr << "Error parsing command line arguments: " << argEx.error() << " for argument " << argEx.argId() << std::endl;
    return false;
  }

  return true;
}

/**
* Prints list of audio devices conntected to this machine.
*/
void VoipProject::listDevices()
{
    SoundcardInteraction::listDevices();
}

/**
* Callback function used by RtAudio to get audio input or pass audio output
*/
int VoipProject::process(AudioBuffer& output, AudioBuffer const& input)
{
    sender_.send(input);
    playNextFrame(output);

    // Check if application is still running to prevent noise when stopping the application
    if (isRunning_ == false)
    {
        ::memset(output.data(), 0, output.size());
    }

    return 0;
}

/**
* Copies next audio buffer of the receiver objects playout buffer to the passed output buffer
*/
void VoipProject::playNextFrame(AudioBuffer& output)
{
    std::lock_guard<std::mutex> lck2{ receiver_.mtxPlayoutBuffer };
    if (receiver_.playoutBuffer.size() != 0)
    {
        AudioBuffer* toPlayout = receiver_.playoutBuffer.getNext();

        output.updateOptions(toPlayout->getOptions());
        ::memcpy(output.data(), toPlayout->data(), output.size());

        receiver_.playoutBuffer.removeNext();
    }
}
