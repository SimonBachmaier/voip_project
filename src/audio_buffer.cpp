
#include "audio_buffer.h"

#include <iostream>

AudioBuffer::AudioBuffer() : options_()
{
}

AudioBuffer::AudioBuffer(Options options) :
	options_(options),
	data_()
{
	data_.resize(options_.frameSize * options_.numberOfChannels * options_.sampleFormat / 8);
}

/**
* Returns the reserved size of the data buffer
*/
size_t AudioBuffer::size() const
{
	return data_.size();
}

/**
* Returns pointer to the buffer containing the audio data
*/
uint8_t* AudioBuffer::data()
{
	return &data_[0];
}

/**
* Returns the number of samples to be stored in this buffer
*/
uint32_t AudioBuffer::numberOfSamples() const
{
	return options_.frameSize * options_.numberOfChannels;
}

/**
* Returns the set number of channels 
*/
uint8_t AudioBuffer::numberOfChannels() const
{
	return options_.numberOfChannels;
}

/**
* Returns all set options
*/
AudioBuffer::Options AudioBuffer::getOptions() const
{
	return options_;
}

/**
* Updates set options, clears buffer and resizes it
*/
void AudioBuffer::updateOptions(Options options)
{
	options_.sampleRate = options.sampleRate;
	options_.frameSize = options.frameSize;
	options_.sampleFormat = options.sampleFormat;
	options_.numberOfChannels = options.numberOfChannels;

	data_.clear();
	uint32_t dataSize = options_.frameSize * options_.numberOfChannels * options_.sampleFormat / 8;
	data_.resize(dataSize);
}
