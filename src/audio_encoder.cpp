
#include "audio_encoder.h"

#include <iostream>

/**
* Starts the encoder by creating new opus encoder for float audio data with passed settings
*/
void AudioEncoder::start(uint16_t sampleRate, uint8_t numberOfChannels)
{
	if (isRunning_ == true) return;

	int error = 0;
	encoder_ = opus_encoder_create(sampleRate, numberOfChannels, OPUS_APPLICATION_VOIP, &error);
	
	if (error == 0) isRunning_ = true;
	else {
		std::cerr << "Error when creating opus encoder: " << error << std::endl;
	}
}

/**
* Encodes input buffer and returns encoded buffer in output paramater
* Function returns the size of the encoded output buffer or an opus error code if something went wrong
*/
int32_t AudioEncoder::encodeFloat(const uint8_t* input, uint16_t frameSize, uint8_t* output, uint32_t maxSize)
{
	if (isRunning_) {
		return opus_encode_float(encoder_, (float*)input, frameSize, output, maxSize);
	}
	return 0;
}

/**
* Stops the encoder
*/
void AudioEncoder::stop()
{
	if (isRunning_ == false) return;

	opus_encoder_destroy(encoder_);
	isRunning_ = false;
}

/**
* Returns true if the encoder is running
*/
bool AudioEncoder::isRunning()
{
	return isRunning_;
}
