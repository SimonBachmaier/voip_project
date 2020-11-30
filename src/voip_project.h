
#ifndef VOIP_PROJECT_H
#define VOIP_PROJECT_H

#include <iostream>
#include <string>

#include "tclap/CmdLine.h"

#include "sender.h"
#include "receiver.h"
#include "soundcard_interaction.h"
#include "audio_buffer.h"

class VoipProject : public SoundcardAudioProcessor {
public:
	struct Options {
		int inputDevice = 0;
		int outputDevice = 0;
		unsigned int numberOfInputChannels = 0;
		unsigned int numberOfOutputChannels = 0;
		unsigned int frameSize = 0;
		unsigned int sampleRate = 0;
		std::string remotePort = "";
		std::string localPort = "";
		std::string destinationIp = "";
	};

	VoipProject();

	int run(int argc, char *argv[]);
	int process(AudioBuffer& output, AudioBuffer const& input);

	void playNextFrame(AudioBuffer& output);

private:
	bool isRunning_;

	SoundcardInteraction soundcardInteraction_;
	Sender sender_;
	Receiver receiver_;

	void start(VoipProject::Options* options);
	void initSoundcard(VoipProject::Options* options);
	void stop();
	bool processArguments(int argc, char* argv[], VoipProject::Options* options);
	void listDevices();
};

#endif /* VOIP_PROJECT_H */
