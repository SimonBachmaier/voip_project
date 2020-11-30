
#include "audio_decoder.h"

#include <iostream>

/**
* Starts the decoder by creating new opus decoder for float audio data with passed settings
*/
void AudioDecoder::start(uint16_t sampleRate, uint8_t numberOfChannels)
{
	if (isRunning_ == true) return;

	int error = 0;
	decoder_ = opus_decoder_create(sampleRate, numberOfChannels, &error);

	if (error == 0) isRunning_ = true;
	else {
		std::cerr << "Error when creating opus decoder: " << error << std::endl;
	}
}

/**
* Decodes input buffer and returns decoded buffer in output paramater
* Function returns the number of decoded samples or an opus error code if something went wrong
*/
int32_t AudioDecoder::decodeFloat(std::vector<uint8_t>& input, uint16_t frameSize, uint8_t* output)
{
	if (isRunning_)
	{
		return opus_decode_float(decoder_, input.data(), (opus_int32) input.size(), (float*)output, frameSize, 0);
	}
	return 0;
}

/**
* Stops the decoder
*/
void AudioDecoder::stop()
{
	if (isRunning_ == false) return;

	opus_decoder_destroy(decoder_);
	isRunning_ = false;
}
