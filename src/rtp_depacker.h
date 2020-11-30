
#ifndef VOIP_RTP_DEPACKER_H
#define VOIP_RTP_DEPACKER_H

#include <vector>

class RtpDepacker {
public:
	static uint8_t getPayloadType(std::vector<uint8_t>& header);
	static void unpackRtpPacket(std::vector<uint8_t>& packet, std::vector<uint8_t>& header, std::vector<uint8_t>& payload);

private:
	RtpDepacker() {}
};

#endif /* VOIP_RTP_DEPACKER_H */
