
#include "receiver.h"

Receiver::Receiver()
  : self_(),
    isRunning_(true),
    senderIds_(),
    playoutBuffer(PlayoutBuffer(100)),
    frameSize_(),
    sampleRate_(),
    numberOfChannels_()
{}

Receiver::~Receiver() {
  if (isRunning_) {
    stop();
  }
}

/**
* Starts the receiver thread
*/
void Receiver::start(std::string ip, std::string port, uint16_t sampleRate, uint8_t numberOfChannels, uint16_t frameSize) {
    self_ = std::thread([=] { receive(ip, port, sampleRate, numberOfChannels, frameSize); });
}

/**
* Stops the receiver and waits for the receiver thread to stop
*/
void Receiver::stop() {
  isRunning_ = false;
  self_.join();
}

/**
* Checks the receiver socket continuously for incoming messages
*/
void Receiver::receive(std::string ip, std::string port, uint16_t sampleRate, uint8_t numberOfChannels, uint16_t frameSize) {
    static bool once = true;

    while (isRunning_) {
        if (once) {
            once = false;
            initializeReceiver(ip, port, sampleRate, numberOfChannels, frameSize);
        }

        uint16_t maxSize = 1024;
        std::vector<uint8_t> packet(maxSize, 0);
        uint32_t bytesReceived = socket_.receive(packet, maxSize);

        if (bytesReceived > 12) {
            std::vector<uint8_t> header, payload;
            RtpUtils::unpackRtpPacket(packet, header, payload);
            std::cout << "Received RTP packet. Packet size: " << packet.size() << " bytes. ";
            std::cout << "Header size : " << header.size() << " bytes. Encoded payload size: " << payload.size() << " bytes.\n" << std::endl;

            handleReceivedData(header, payload);
        }
    }
    socket_.close();
    decoder_.stop();
}

/**
* Checks RTP header and passes payload to payload handler 
*/
void Receiver::handleReceivedData(std::vector<uint8_t>& header, std::vector<uint8_t>& payload)
{
    recordSenderId(header);
    uint8_t payloadType = RtpUtils::getPayloadType(header);
    handlePayload(payloadType, payload);
}

/**
* Handles payload. Depending on the type it decodes the payload and adds the decoded audio buffer to the playout buffer
*/
void Receiver::handlePayload(const uint8_t& payloadType, std::vector<uint8_t>& payload)
{
    std::lock_guard<std::mutex> lck2{ mtxPlayoutBuffer };
    if (payloadType == 99)
    {
        AudioBuffer::Options options;
        options.sampleFormat = AudioBuffer::SampleFormat::FLOAT32;
        options.sampleRate = sampleRate_;
        options.frameSize = frameSize_;
        options.numberOfChannels = numberOfChannels_;
        AudioBuffer audioBuffer(options);

        uint8_t* decodedPayload = new uint8_t[audioBuffer.size()];
        decoder_.decodeFloat(payload, options.frameSize, audioBuffer.data());

        playoutBuffer.add(audioBuffer);
    }
}

/**
* Extracts the sender id from the RTP header and stores the sender id if it is new
*/
uint32_t Receiver::recordSenderId(std::vector<uint8_t>& header)
{
    uint32_t ssrcId = 0;
    ssrcId |= header[8] << 24; // set first part of ssrc identifier: |0000 0000
    ssrcId |= header[9] << 16; // set second part of ssrc identifier: 0000 0000
    ssrcId |= header[10] << 8; // set third part of ssrc identifier: 0000 0000
    ssrcId |= header[11]; // set fourth part of ssrc identifier: 0000 0000|

    if (std::count(senderIds_.begin(), senderIds_.end(), ssrcId))
    {
        senderIds_.push_back(ssrcId);
    }

    return ssrcId;
}

/**
* Initializes the receiver object by starting the socket an decoder
*/
void Receiver::initializeReceiver(std::string ip, std::string port, uint16_t sampleRate, uint8_t numberOfChannels, uint16_t frameSize)
{
    frameSize_ = frameSize;
    sampleRate_ = sampleRate;
    numberOfChannels_ = numberOfChannels;

    socket_.setAddress(ip, port);
    socket_.open();
    socket_.bind();

    decoder_.start(sampleRate, numberOfChannels);
}
