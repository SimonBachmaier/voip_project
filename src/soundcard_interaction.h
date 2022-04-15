
#ifndef VOIP_SOUNDCARD_INTERACTION_H_
#define VOIP_SOUNDCARD_INTERACTION_H_

#include "RtAudio.h"

#include "audio_buffer.h"
#include <cstring>

// TODO after AudioBuffer is coded
class SoundcardAudioProcessor {
public:
	virtual int process(AudioBuffer& output, AudioBuffer const& input) = 0;
};

class SoundcardInteraction
{
public:
	static void listDevices();

public:
	struct Options {
		int inputDevice = -1;
		int outputDevice = -1;
		unsigned int inputChannels = 2;
		unsigned int outputChannels = 2;
		unsigned int sampleRate = 44100;
		unsigned int frameSize = 512;
		AudioBuffer::SampleFormat sampleFormat = AudioBuffer::SampleFormat::UNKNOWN;
	};

	SoundcardInteraction(SoundcardAudioProcessor* processor);

	bool init(Options options);
	bool start();
	bool stop();
	SoundcardInteraction::Options getOptions();

private:
	bool initialized_;
	bool isRunning_;

	RtAudio rtAudio_;
	RtAudio::StreamParameters inputParameters_;
	RtAudio::StreamParameters outputParameters_;
	AudioBuffer inputBuffer_;
	AudioBuffer outputBuffer_;
	SoundcardAudioProcessor* processor_;
	SoundcardInteraction::Options options_;

	void validateOptions(bool& errorFlag);
	void initalizeInputOutputParameters();
	void updateInputOutputBuffers();
	RtAudioFormat getRtAudioFormat(bool& errorFlag);
	void adjustForFramesizeDifference(unsigned int expectedFramesize);
	void openStream(bool& errorFlag);
	int processRtAudioCallback(void* outBuf, void* inBuf, unsigned int nFrames, double streamTime, RtAudioStreamStatus status);
	static int rtAudioCallback(void* outBuf, void* inBuf, unsigned int nFrames, double streamTime, RtAudioStreamStatus status, void* userData);
};

#endif // VOIP_SOUNDCARD_INTERACTION_H_