
#ifndef VOIP_AUDIODECODER_H
#define VOIP_AUDIODECODER_H


#include "opus.h"

#include "audio_buffer.h"

class AudioDecoder {
public:
	void start(uint16_t sampleRate, uint8_t numberOfChannels);
	int32_t decodeFloat(std::vector<uint8_t>& input, uint16_t frameSize, uint8_t* output);
	void stop();

private:
	OpusDecoder* decoder_ = nullptr;
	bool isRunning_ = false;
};

#endif /* VOIP_AUDIODECODER_H */
