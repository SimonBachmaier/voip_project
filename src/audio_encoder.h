
#ifndef VOIP_AUDIOENCODER_H
#define VOIP_AUDIOENCODER_H

#include "opus.h"

#include <vector>

#include "audio_buffer.h"

class AudioEncoder {
public:
	void start(uint16_t sampleRate, uint8_t numberOfChannels);
	int32_t encodeFloat(const uint8_t* input, uint16_t frameSize, uint8_t* output, uint32_t maxSize);
	void stop();
	bool isRunning();

private:
	OpusEncoder* encoder_ = nullptr;
	bool isRunning_ = false;
};

#endif /* VOIP_AUDIOENCODER_H */
