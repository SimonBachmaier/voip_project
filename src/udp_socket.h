#ifndef VOIP_UDP_SOCKET_H_
#define VOIP_UDP_SOCKET_H_

#include <string>
#include <vector>
#include <iostream>

#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#define NOMINMAX 1

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else

#include <sys/socket.h>
#include <sys/types.h>

#define SOCKET int

#endif


class SimpleWindowsUpdSocket
{
public:
	SimpleWindowsUpdSocket();

	void setAddress(std::string addr, std::string port_);
	void open();
	uint32_t send(std::vector<uint8_t> data, bool isBlocking = true);
	bool bind();
	uint32_t receive(std::vector<uint8_t>& data, uint32_t maxSize = 0, bool isBlocking = true);
	void close();

private:
	bool isAddressSet_;
	bool isOpen_;
	std::string port_;

	struct addrinfo* addressInfo_ = nullptr;
	SOCKET connectSocket_;
	fd_set socketArray_;
	struct timeval timeout_;
};

#endif // VOIP_UDP_SOCKET_H_