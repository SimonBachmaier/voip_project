
#ifndef VOIP_AUDIO_MIXER_H_
#define VOIP_AUDIO_MIXER_H_

#include <cstdint>

class AudioMixer
{
public:
	static float* createInterleavedAudioBuffer(uint16_t frameSize, float* monoAudio, size_t monoAudioSize);
	static float* createDeinterleavedAudioBuffer(uint16_t frameSize, float* stereoAudio, size_t stereoAudioSize);

private:
	AudioMixer() {};
};


#endif // !VOIP_AUDIO_MIXER_H_