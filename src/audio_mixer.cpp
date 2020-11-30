
#include "audio_mixer.h"

/**
* Takes a single channel input buffer interleaves it and returns pointer to newly created dual channel buffer
*/
float* AudioMixer::createInterleavedAudioBuffer(uint16_t frameSize, float* monoAudio, size_t monoAudioSize)
{
    float* stereoAudio = (float*) new uint8_t[monoAudioSize * 2];
    for (uint16_t i = 0; i < frameSize; i++)
    {
        stereoAudio[i * 2] = monoAudio[i];
        stereoAudio[i * 2 + 1] = monoAudio[i];
    }
    return stereoAudio;
}

/**
* Takes a dual channel input buffer deinterleaves it by copying the left channel and returns pointer to newly created single channel buffer
*/
float* AudioMixer::createDeinterleavedAudioBuffer(uint16_t frameSize, float* stereoAudio, size_t stereoAudioSize)
{
    float* monoAudio = (float*) new uint8_t[stereoAudioSize / 2];
    for (uint16_t i = 0; i < frameSize; i++)
    {
        monoAudio[i] = stereoAudio[i * 2];
    }
    return monoAudio;
}
