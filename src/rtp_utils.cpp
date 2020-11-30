
#include "rtp_utils.h"

/**
* Returns RTP packet buffer from RTP header and payload buffers.
*/
std::vector<uint8_t> RtpUtils::createRtpPacket(std::vector<uint8_t>& header, uint8_t* payload, uint16_t payloadSize)
{
	std::vector<uint8_t> package;

	package.resize(header.size() + payloadSize);
	std::move(header.begin(), header.end(), package.begin());
	std::move(payload, payload + payloadSize, package.begin() + header.size());

	return package;
}

/**
* Creates RTP header from passed arguments. Payload type is fixed at 99 for this application.
*/
std::vector<uint8_t> RtpUtils::createRtpHeader(uint8_t numberOfChannels, uint16_t sequenceNumber, uint32_t timestamp, uint32_t ssrcId)
{
	const uint8_t numberOfHeaderBytes = 12;
	std::vector<uint8_t> header(numberOfHeaderBytes, 0);

	header[0] = header[0] | 192; // set version: |00|00 0000
	header[0] = header[0] | 0; // set padding p: 00|0|0 0000
	header[0] = header[0] | 0; // set extension x: 000|0| 0000
	header[0] = header[0] | 0; // set csrc count cc: 0000 |0000|

	header[1] = header[1] | 0; // set marker m: |0|000 0000
	header[1] = header[1] | 99; // set payload type pt: 0|000 0000|

	header[2] = sequenceNumber >> 8; // set first part of sequence number: |0000 0000
	header[3] = (uint8_t) sequenceNumber; // set second part of sequence number: 0000 0000|

	header[4] = timestamp >> 24; // set first part of timestamp: |0000 0000
	header[5] = timestamp >> 16; // set second part of timestamp: 0000 0000
	header[6] = timestamp >> 8; // set third part of timestamp: 0000 0000
	header[7] = timestamp; // set fourth part of timestamp: 0000 0000|

	header[8] = ssrcId >> 24; // set first part of ssrc identifier: |0000 0000
	header[9] = ssrcId >> 16; // set second part of ssrc identifier: 0000 0000
	header[10] = ssrcId >> 8; // set third part of ssrc identifier: 0000 0000
	header[11] = ssrcId; // set fourth part of ssrc identifier: 0000 0000|

	return header;
}


/**
* Returns the payload type information from the passed RTP header
*/
uint8_t RtpUtils::getPayloadType(std::vector<uint8_t>& header)
{
	if (header.size() >= 12) {
		return header[1] & 127; // // gets payload type pt: 0|000 0000|
	}
	return 0;
}

/**
* Splits RTP header from payload
*/
void RtpUtils::unpackRtpPacket(std::vector<uint8_t>& packet, std::vector<uint8_t>& header, std::vector<uint8_t>& payload)
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
