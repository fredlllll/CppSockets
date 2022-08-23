/*
 * Cross-platform compatibility superclass for sockets
 *
 * Copyright (C) 2019 Simon D. Levy
 *
 * MIT License
 */

#pragma once

#ifdef CPPSOCKETS_DEBUG
#define CPPSOCKETS_DEBUG_PRINT(X) fprintf(stdout, X)
#define CPPSOCKETS_DEBUG_PRINT_ERROR(X) fprintf(stderr, X)
#else
#define CPPSOCKETS_DEBUG_PRINT(X)
#define CPPSOCKETS_DEBUG_PRINT_ERROR(X)
#endif

// Windows
#ifdef _WIN32
#pragma comment(lib,"ws2_32.lib")
#define WIN32_LEAN_AND_MEAN
typedef SOCKET socket_t;
#undef TEXT
#include <winsock2.h>
#include <ws2tcpip.h>

// Linux
#else
#define sprintf_s sprintf
typedef int socket_t;
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
static const int INVALID_SOCKET = -1;
static const int SOCKET_ERROR   = -1;
#endif

#include <stdio.h>
#include <stdint.h>

#ifdef _WIN32

void handleWinapiError(int error) {
#ifdef CPPSOCKETS_DEBUG
    LPSTR errorMessagePtr = NULL;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, error, MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT), (LPTSTR)&errorMessagePtr, 0, NULL);
    if (errorMessagePtr) {
        CPPSOCKETS_DEBUG_PRINT_ERROR(errorMessagePtr);
        LocalFree(errorMessagePtr);
    }
#endif
}

bool _cppsockets_initWinsockSuccess = false;
bool initWinsock() {
    if (_cppsockets_initWinsockSuccess) {
        return true;
    }
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        handleWinapiError(result);
        return false;
    }
    _cppsockets_initWinsockSuccess = true;
    return true;
}

bool cleanupWinsock() {
    if (_cppsockets_initWinsockSuccess) {
        _cppsockets_initWinsockSuccess = false;
        int result = WSACleanup();
        if (result != 0) {
            handleWinapiError(result);
            return false;
        }
    }
    return true;
}
#endif

void cppSocketsInit() {
#ifdef _WIN32
    if (!initWinsock()) {
        //honestly no idea
    }
#endif
}

void cppSocketsDeinit() {
#ifdef _WIN32
    if (!cleanupWinsock()) {
        //honestly no idea
    }
#endif
}


class Socket {
    protected:
        socket_t _sock;

        static void inetPton(const char * host, struct sockaddr_in & saddr_in)
        {
#ifdef _WIN32
#ifdef UNICODE
            WCHAR host_[64];
            swprintf_s(host_, L"%S", host);
#else
            const char* host_ = host;
#endif
            InetPton(AF_INET, host_, &(saddr_in.sin_addr.s_addr));
#else
            inet_pton(AF_INET, host, &(saddr_in.sin_addr));
#endif
        }

        void setUdpTimeout(uint32_t msec)
        {
#ifdef _WIN32
            setsockopt(_sock, SOL_SOCKET, SO_RCVTIMEO, (char *) &msec, sizeof(msec));

#else
            struct timeval timeout;
            timeout.tv_sec = msec / 1000;
            timeout.tv_usec = (msec * 1000) % 1000000;
            setsockopt(_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
#endif
        }

    public:

        void closeConnection(void)
        {
#ifdef _WIN32
            closesocket(_sock);
#else
            close(_sock);
#endif
        }
};
