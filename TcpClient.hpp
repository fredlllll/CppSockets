/*
 * Cross-platform compatibility superclass for sockets
 *
 * Copyright (C) 2019 Simon D. Levy
 *
 * MIT License
 */

#pragma once

#include "Socket.hpp"

namespace CppSockets {
	class TcpClient : public Socket {
	private:
		//prevent socket copying and assignment
		TcpClient(const TcpClient& other) = delete;
		TcpClient& operator=(const TcpClient&) = delete;
	public:

		TcpClient(socket_t sock) :
			Socket(sock) {}

		TcpClient(const char* host, const unsigned short port) :
			Socket()
		{
			//convert port to string cause getaddrinfo wants a string
			char _port[6];
			sprintf(_port, "%d", port);

			// Set up client address info
			struct addrinfo hints = { 0 };
			hints.ai_family = AF_INET;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_protocol = IPPROTO_TCP;

			// Resolve the server address and port, returning on failure
			addrinfo* _addressInfo = NULL;
			int result = getaddrinfo(host, _port, &hints, &_addressInfo);
			if (result != 0) {
#ifdef _WIN32
				handleWinapiError(result);
#else
				CPPSOCKETS_DEBUG_PRINT_ERROR("getaddrinfo() failed with error: %d", result);
#endif
				return;
			}

			// Create a SOCKET for connecting to server, returning on failure
			_sock = socket(_addressInfo->ai_family, _addressInfo->ai_socktype, _addressInfo->ai_protocol);
			if (_sock == INVALID_SOCKET) {
				CPPSOCKETS_DEBUG_PRINT_ERROR("socket() failed");
				freeaddrinfo(_addressInfo);
				return;
			}

			result = connect(_sock, _addressInfo->ai_addr, (int)_addressInfo->ai_addrlen);
			if (result == SOCKET_ERROR) {
				close();
			}
			freeaddrinfo(_addressInfo);
		}

		int sendData(void* buf, size_t len)
		{
			return send(_sock, (const char*)buf, len, 0) == len;
		}


		int receiveData(void* buf, size_t len)
		{
			return recv(_sock, (char*)buf, len, 0) == len;
		}
	};
}

