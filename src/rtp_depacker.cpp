
#include "rtp_depacker.h"

/**
* Returns the payload type information from the passed RTP header
*/
uint8_t RtpDepacker::getPayloadType(std::vector<uint8_t>& header)
{
	if (header.size() >= 12) {
		return header[1] & 127; // // gets payload type pt: 0|000 0000|
	}
	return 0;
}

/**
* Splits RTP header from payload
*/
void RtpDepacker::unpackRtpPacket(std::vector<uint8_t>& packet, std::vector<uint8_t>& header, std::vector<uint8_t>& payload)
{
	uint16_t headerByteSize = 12; // minimum size
	
	uint8_t csrcCount = packet[0] & 15; // get csrc count field: 0000 |0000|
	headerByteSize += (4 * csrcCount);	// each csrc identifier is 4 bytes big

	bool extensionHeaderSet = (packet[0] & 16) == 16; // check if extension header bit is set: 000|0| 0000
	if (extensionHeaderSet)
	{
		uint16_t extensionLength = 0;
		extensionLength = packet[headerByteSize + 2] << 8; // first half of extension length field
		extensionLength = extensionLength | packet[headerByteSize + 3]; // second half of extension length field

		headerByteSize += 4 + extensionLength * 4; // extension length defines number of following 32 bit words
	}

	header = std::vector<uint8_t>(packet.begin(), packet.begin() + headerByteSize);
	payload = std::vector<uint8_t>(packet.begin() + headerByteSize, packet.end());
}
