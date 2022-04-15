
#ifndef VOIP_PLAYOUT_BUFFER_H_
#define VOIP_PLAYOUT_BUFFER_H_

#include "audio_buffer.h"
#include <cstring>

class PlayoutBuffer {
public:
	PlayoutBuffer() = delete;
	PlayoutBuffer(uint16_t capacity);

	AudioBuffer* getNext();
	void removeNext();
	void add(AudioBuffer& audioBuffer);
	uint16_t capacity();
	uint16_t size();

private:
	uint16_t capacity_;
	uint16_t size_;
	uint16_t tail_;
	uint16_t head_;
	AudioBuffer* buffer_;
};

#endif /* VOIP_PLAYOUT_BUFFER_H_ */
