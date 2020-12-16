
#ifndef VOIP_SENDER_H
#define VOIP_SENDER_H

#include <string>
#include <iostream>

#include "rtp_utils.h"
#include "udp_socket.h"
#include "audio_buffer.h"
#include "audio_encoder.h"
#include "audio_mixer.h"


class Sender {

public:
	void start(std::string ip, std::string port_, uint16_t sampleRate, uint8_t numberOfChannels, bool isUsingOpus);
	void stop();
	void send(AudioBuffer const input);

private:
	bool isRunning_ = false;
	bool isUsingOpus_;
	uint16_t nextSequenceNumber_;
	uint32_t nextTimestamp_;
	uint32_t ssrcId;

	SimpleWindowsUpdSocket socket_;
	AudioEncoder encoder_;

	uint32_t createRandom32BitNumber();
	uint16_t createRandom16BitNumber();
	int32_t encodeData(AudioBuffer::Options& options, const AudioBuffer& input, uint8_t* encodedDataBuffer);
};

#endif /* VOIP_SENDER_H */
