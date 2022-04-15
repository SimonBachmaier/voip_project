
#include "udp_socket.h"

#include <netdb.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <cstring>
static const int INVALID_SOCKET = -1;
static const int SOCKET_ERROR   = -1;

#define ioctlsocket ioctl

SimpleWindowsUpdSocket::SimpleWindowsUpdSocket() : connectSocket_(INVALID_SOCKET), addressInfo_(), isOpen_(false), isAddressSet_(false)
{
    // connectSocket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    // WSADATA wsaData;
	// int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    // if (iResult != 0)
    // {
    //     std::cerr << "WSAStartup failed: " << iResult << std::endl;
    // }
    
}

/**
* Initializes addressInfo_ property with passed ipV4 address and port
*/
void SimpleWindowsUpdSocket::setAddress(std::string address, std::string port)
{
    if (isOpen_) close();

    port_ = port;
    struct addrinfo hints;


    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    int iResult = getaddrinfo(address.c_str(), port.c_str(), &hints, &addressInfo_);
    if (iResult != 0)
    {
        std::cerr << "getaddrinfo failed failed: " << iResult << std::endl;
    }
    else
    {
        if (addressInfo_->ai_canonname)
            std::cerr << "Canonical name of socket: " << addressInfo_->ai_canonname << std::endl;

        if (addressInfo_->ai_family != AF_INET)
        {
            addressInfo_ = nullptr;
            isAddressSet_ = true;
            std::cerr << "SocketAddress::setAddress(): Socket addresses other than ipv4 not supported!" << std::endl;
        }
    }
    isAddressSet_ = true;
}

/**
* Initializes connectSocket_ with address information form addressInfo_
*/
void SimpleWindowsUpdSocket::open()
{
    if (isAddressSet_ == false) return;

    connectSocket_ = socket(addressInfo_->ai_family, addressInfo_->ai_socktype, addressInfo_->ai_protocol);

    timeout_.tv_sec = 0;
    timeout_.tv_usec = 100000;
    // Set up the file descriptor set for timeout check


    FD_ZERO(&socketArray_);
    FD_SET(connectSocket_, &socketArray_);

    if (connectSocket_ == INVALID_SOCKET)
    {   
        std::cerr << "Error at socket(): " << strerror(errno) << std::endl;
        freeaddrinfo(addressInfo_);
        // WSACleanup();
    }

    isOpen_ = true;

    int32_t reuseaddr = 1;
    if (::setsockopt(connectSocket_, SOL_SOCKET, SO_REUSEADDR, (char*)&reuseaddr, sizeof(reuseaddr)))
    {
        std::cerr << "Error setting reuse option on socket!" << std::endl;
    }
}

/**
* Closes the socket.
*/
void SimpleWindowsUpdSocket::close()
{
    if (isOpen_ == false) return;

    isOpen_ = false;
    ::close(connectSocket_);
}

/**
* Sends the data buffer to the previously set address
*/
uint32_t SimpleWindowsUpdSocket::send(std::vector<uint8_t> data, bool isBlocking)
{
    if (isOpen_ == false) return 0;

    u_long nonblocking = 0;
    if (isBlocking == false)
    {
        nonblocking = 1;
        if (ioctlsocket(connectSocket_, FIONBIO, &nonblocking) != 0)
        {
            std::cerr << "Could not set non-blocking mode on socket!" << std::endl;
        }
    }

    struct sockaddr* address = nullptr;
    address = (struct sockaddr*)addressInfo_->ai_addr;


    int32_t bytes_sent = ::sendto(connectSocket_, (char*) &data[0], (int) data.size(), 0, address, sizeof(sockaddr_in));
    if (bytes_sent == -1)
    {
        int e = errno;
      
        std::cerr << "Error in sendto: " << e << std::endl;
    }

    if (isBlocking == false)
    {
        nonblocking = 0;
        if (ioctlsocket(connectSocket_, FIONBIO, &nonblocking) != 0)
        {
            std::cerr << "Could not set blocking mode on socket!" << std::endl;
        }
    }

    return bytes_sent;
}

/**
* Binds the previously set address as own address to prepare socket for incoming messages.
*/
bool SimpleWindowsUpdSocket::bind()
{
    if (isOpen_ == false)
    {
        std::cerr << "Error: trying to bind a non open socket!" << std::endl;
        return false;
    }

    struct sockaddr* address = nullptr;
    address = (struct sockaddr*)addressInfo_->ai_addr;

    if (::bind(connectSocket_, address, sizeof(sockaddr_in)) != 0)
    {
        int e = errno;
        std::cerr << "Error binding socket: " << e << std::endl;
        return false;
    }

    return true;
}

/**
* Checks socket connection for incoming messages and stores incoming message in data buffer.
*/
uint32_t SimpleWindowsUpdSocket::receive(std::vector<uint8_t>& data, uint32_t maxSize, bool isBlocking)
{
    if (isOpen_ == false) return 0;

    struct sockaddr *a;
    socklen_t a_len;

    sockaddr_in addrIn;
    addrIn.sin_port = 0;
    addrIn.sin_family = AF_INET;
    addrIn.sin_addr.s_addr = htonl(INADDR_ANY);

    a = (struct sockaddr*)&addrIn;
    a_len = sizeof(sockaddr_in);

    u_long nonblocking = 0;
    if (isBlocking == false)
    {
        nonblocking = 1;
        if (ioctlsocket(connectSocket_, FIONBIO, &nonblocking) != 0)
        {
            std::cerr << "Could not set non-blocking mode on socket!" << std::endl;
        }
    }

    uint32_t toread = maxSize ? maxSize : (uint32_t) data.capacity();
    if (maxSize > data.capacity())
    {
        std::cerr << "Data to read over buffer capacity!" << std::endl;
        return 0;
    }

    // Check for timeout, used to terminate thread on application close.
    if (select((int) connectSocket_, &socketArray_, NULL, NULL, &timeout_) == 0)
    {
        return 0;
    }

    int32_t read_bytes = ::recvfrom(connectSocket_, (char*)&data[0], toread, 0, a, &a_len);

    int32_t e = errno;
    if (read_bytes == -1 || read_bytes == 0)
    {
        std::cerr << "Error in recvfrom: " << e << std::endl;
        data.clear();
        return 0;
    }

    if (isBlocking == false)
    {
        nonblocking = 0;
        if (ioctlsocket(connectSocket_, FIONBIO, &nonblocking) != 0)
        {
            std::cerr << "Could not set blocking mode on socket!" << std::endl;
        }
    }

    data.resize(std::max(read_bytes, 0));

    return read_bytes;
}
