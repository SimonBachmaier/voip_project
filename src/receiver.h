
#ifndef VOIP_RECEIVER_H
#define VOIP_RECEIVER_H

#include <string>
#include <thread>
#include <iostream>
#include <vector>
#include <mutex>

#include "rtp_utils.h"
#include "udp_socket.h"
#include "audio_buffer.h"
#include "audio_decoder.h"
#include "playout_buffer.h"

class Receiver {
public:
	Receiver();
	~Receiver();

	PlayoutBuffer playoutBuffer;
	std::mutex mtxPlayoutBuffer;

	void start(std::string ip, std::string port, uint16_t sampleRate, uint8_t numberOfChannels, uint16_t frameSize);
	void stop();

private:
	bool isRunning_;
	uint16_t frameSize_;
	std::string port_;
	uint16_t sampleRate_;
	uint8_t numberOfChannels_;
	std::vector<uint32_t> senderIds_;
	std::thread self_;

	SimpleWindowsUpdSocket socket_;
	AudioDecoder decoder_;

	void receive(std::string ip, std::string port, uint16_t sampleRate, uint8_t numberOfChannels, uint16_t frameSize);
	void handleReceivedData(std::vector<uint8_t>& header, std::vector<uint8_t>& payload);
	void handlePayload(const uint8_t& payloadType, std::vector<uint8_t>& payload);
	uint32_t recordSenderId(std::vector<uint8_t>& header);
	void initializeReceiver(std::string ip, std::string port, uint16_t sampleRate, uint8_t numberOfChannels, uint16_t frameSize);
};

#endif /* VOIP_RECEIVER_H */
