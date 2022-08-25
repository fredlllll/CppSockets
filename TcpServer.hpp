/*
 * General-purpose socket server class
 *
 * Copyright (C) 2019 Simon D. Levy
 *
 * MIT License
 */

#pragma once

#include "TcpClient.hpp"
#include "Socket.hpp"
#include <memory>

namespace CppSockets {

	class TcpServer :public Socket {
		int backlog;
		unsigned short port;

		//prevent socket copying and assignment
		TcpServer(const TcpServer& other) = delete;
		TcpServer& operator=(const TcpServer&) = delete;
	public:

		TcpServer(short port, int backlog = 0x7fffffff)
			:Socket(), port(port), backlog(backlog)
		{}

		void startListening() {
			//convert port to string cause getaddrinfo wants a string
			char _port[6];
			sprintf(_port, "%d", port);

			struct addrinfo hints = { 0 };
			hints.ai_family = AF_INET;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_protocol = IPPROTO_TCP;
			hints.ai_flags = AI_PASSIVE;

			// Resolve the server address and port
			addrinfo* _addressInfo = NULL;
			int result = getaddrinfo(NULL, _port, &hints, &_addressInfo);
			if (result != 0) {
#ifdef _WIN32
				handleWinapiError(result);
#else
				CPPSOCKETS_DEBUG_PRINT_ERROR("getaddrinfo() failed with error: %d", result);
#endif
				return;
			}

			// Create a SOCKET for the server to listen for client connections.
			_sock = socket(_addressInfo->ai_family, _addressInfo->ai_socktype, _addressInfo->ai_protocol);
			if (_sock == INVALID_SOCKET) {
				CPPSOCKETS_DEBUG_PRINT_ERROR("socket() failed");
				freeaddrinfo(_addressInfo);
				return;
			}

			// Setup the TCP listening socket
			result = bind(_sock, _addressInfo->ai_addr, (int)_addressInfo->ai_addrlen);
			if (result == SOCKET_ERROR) {
				CPPSOCKETS_DEBUG_PRINT_ERROR("bind() failed");
				freeaddrinfo(_addressInfo);
				close();
				return;
			}

			freeaddrinfo(_addressInfo);

			result = listen(_sock, backlog);
			if (result == SOCKET_ERROR) {
				CPPSOCKETS_DEBUG_PRINT_ERROR("listen() failed");
				close();
				return;
			}

			//TODO: start thread that runs listenLoop
		}

	private:
		void listenLoop() {
			while (true) {
				socket_t _conn = accept(_sock, (struct sockaddr*)NULL, NULL);
				if (_conn == SOCKET_ERROR) {
					CPPSOCKETS_DEBUG_PRINT_ERROR("accept() failed");
					break;
				}

				std::shared_ptr<TcpClient> client = std::make_shared<TcpClient>(_conn);
				//TODO: call function with client
			}
		}
	};
}