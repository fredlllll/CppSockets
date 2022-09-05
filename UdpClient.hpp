/*
 * Class for UDP sockets
 *
 * Copyright (C) 2019 Simon D. Levy
 *
 * MIT License
 */

#pragma once

#include "Socket.hpp"

namespace CppSockets {

    class UdpClient : public Socket {

    protected:

        struct sockaddr_in _si_other;
        socklen_t _slen = sizeof(_si_other);

        void setupTimeout(uint32_t msec)
        {
            if (msec > 0) {
                setUdpTimeout(msec);
            }
        }

        void setUdpTimeout(uint32_t msec)
        {
#ifdef _WIN32
            setsockopt(_sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&msec, sizeof(msec));
#else
            struct timeval timeout;
            timeout.tv_sec = msec / 1000;
            timeout.tv_usec = (msec * 1000) % 1000000;
            setsockopt(_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
#endif
        }

    public:

        void sendData(void* buf, size_t len)
        {
            sendto(_sock, (const char*)buf, (int)len, 0, (struct sockaddr*)&_si_other, (int)_slen);

        }

        bool receiveData(void* buf, size_t len)
        {
            return recvfrom(_sock, (char*)buf, (int)len, 0, (struct sockaddr*)&_si_other, &_slen) == _slen;
        }
    };
}