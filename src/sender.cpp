
#include "sender.h"

/**
* Starts the sender object by starting the encoder and udp socket
*/
void Sender::start(std::string destinationIp, std::string destinationPort, uint16_t sampleRate, uint8_t numberOfChannels) {
    if (isRunning_) return;

    isRunning_ = true;
    encoder_.start(sampleRate, numberOfChannels);
    socket_.setAddress(destinationIp, destinationPort);
    socket_.open();

    nextSequenceNumber_ = createRandom16BitNumber();
    ssrcId = createRandom32BitNumber();
    nextTimestamp_ = createRandom32BitNumber();
}

/**
* Stops the sender object by closing socket an stopping the encoder
*/
void Sender::stop() {
    if (isRunning_ == false) return;

    isRunning_ = false;
    socket_.close();
    encoder_.stop();
}

/**
* Sends the passed raw audio buffer after encoding it and packing it in RTP packet over the network
*/
void Sender::send(AudioBuffer const input) {
    if (isRunning_ == false) return;

    AudioBuffer::Options options = input.getOptions();

    uint8_t* encodedDataBuffer = new uint8_t[input.size() * 2];
    int32_t encodedDataSize = encodeData(options, input, encodedDataBuffer);
    if (encodedDataSize == -99) return;

    std::vector<uint8_t> header = RtpUtils::createRtpHeader(options.numberOfChannels, ssrcId, nextSequenceNumber_, (uint16_t) nextTimestamp_);
    std::vector<uint8_t> data = RtpUtils::createRtpPacket(header, encodedDataBuffer, encodedDataSize);

    uint32_t bytesSent = socket_.send(data);
    std::cout << "Sent RTP packet. Packet size: " << bytesSent << " bytes. Encoded payload size: " << encodedDataSize << " bytes." << std::endl;

    nextTimestamp_ += input.numberOfSamples();
    nextSequenceNumber_++;
}

/**
* Encodes input data and returns encoded buffer with buffer size.
*/
int32_t Sender::encodeData(AudioBuffer::Options& options, const AudioBuffer& input, uint8_t* encodedDataBuffer)
{
    if (options.numberOfChannels == 1)
    {
        float* stereoAudio = AudioMixer::createInterleavedAudioBuffer(options.frameSize, (float*)input.data(), (uint32_t) input.size());
        return encoder_.encodeFloat((uint8_t*)stereoAudio, options.frameSize, encodedDataBuffer, (uint32_t) input.size() * 2);
    }
    else if (options.numberOfChannels == 2)
    {
        return encoder_.encodeFloat(input.data(), options.frameSize, encodedDataBuffer, (uint16_t)input.size());
    }
    else {
        std::cerr << "Only single or dual channel input allowed!" << std::endl;
        return -99;
    }
}

/**
* Creates random 32 bit number
*/
uint32_t Sender::createRandom32BitNumber()
{
    uint32_t randomNumber = 0;
    randomNumber = rand() & 0xff;
    randomNumber |= (rand() & 0xff) << 8;
    randomNumber |= (rand() & 0xff) << 16;
    randomNumber |= (rand() & 0xff) << 24;
    return randomNumber;
}

/**
* Creates random 16 bit number
*/
uint16_t Sender::createRandom16BitNumber()
{
    uint16_t randomNumber = 0;
    randomNumber = rand() & 0xff;
    randomNumber |= (rand() & 0xff) << 8;
    return randomNumber;
}