
#ifndef VOIP_RTP_PACKER_H
#define VOIP_RTP_PACKER_H

#include <vector>
#include <iostream>
#include <bitset>

#include "audio_buffer.h"

class RtpUtils {
public:
	static std::vector<uint8_t> createRtpHeader(uint8_t payloadType, uint16_t sequenceNumber, uint32_t timestamp, uint32_t ssrcId);
	static std::vector<uint8_t> createRtpPacket(std::vector<uint8_t>& header, uint8_t* payload, uint16_t payloadSize);
	static uint8_t getPayloadType(std::vector<uint8_t>& header);
	static void unpackRtpPacket(std::vector<uint8_t>& packet, std::vector<uint8_t>& header, std::vector<uint8_t>& payload);

private:
	RtpUtils() {}
};

#endif /* VOIP_RTP_PACKER_H */
