
#include "playout_buffer.h"

PlayoutBuffer::PlayoutBuffer(uint16_t capacity) : capacity_(capacity), size_(0), head_(0), tail_(0), buffer_(new AudioBuffer[capacity])
{
}

/**
* Returns pointer to next audio buffer
* Doesn't check if buffer is empty
*/
AudioBuffer* PlayoutBuffer::getNext()
{
	return &buffer_[tail_];
}

/**
* Removes the next (oldest) buffer entry
*/
void PlayoutBuffer::removeNext()
{
	if (size_ == 0) return;

	memset(&buffer_[tail_], 0, sizeof(AudioBuffer));

	size_--;
	if (tail_ != head_)
	{
		if (tail_ == (capacity_ - 1))
			tail_ = 0;
		else
			tail_++;
	}
}

/**
* Adds new audio buffer to the playout buffer
*/
void PlayoutBuffer::add(AudioBuffer& audioBuffer)
{
	if (size_ == capacity_) return;

	buffer_[head_] = audioBuffer;

	size_++;
	if (head_ == (capacity_ - 1))
		head_ = 0;
	else 
		head_++;
}

/**
* Returns the capacity of the playout buffer object aka the maximum number of entries
*/
uint16_t PlayoutBuffer::capacity()
{
	return capacity_;
}

/**
* Returns the number of entries in the playout buffer
*/
uint16_t PlayoutBuffer::size()
{
	return size_;
}
