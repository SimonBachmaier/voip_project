
#ifndef VOIP_AUDIO_BUFFER_H_
#define VOIP_AUDIO_BUFFER_H_

#include <vector>
#include <cstdint>

class AudioBuffer
{
public:
	typedef enum sampleformat_ {
		UNKNOWN = 0,
		INT16 = 16,
		FLOAT32 = 32,
	} SampleFormat;
	struct Options {
		uint16_t frameSize = 0;
		uint8_t numberOfChannels = 0;
		uint16_t sampleRate = 0;
		SampleFormat sampleFormat = SampleFormat::UNKNOWN;
	};

	AudioBuffer();
	AudioBuffer(Options options);

	size_t size() const;
	uint8_t* data();
	const uint8_t* data() const {
		return &data_[0];
	}
	uint32_t numberOfSamples() const;
	uint8_t numberOfChannels() const;
	struct Options getOptions() const;
	void updateOptions(Options options);

private:
	std::vector<uint8_t> data_;
	Options options_;
};


#endif // !VOIP_AUDIO_BUFFER_H_