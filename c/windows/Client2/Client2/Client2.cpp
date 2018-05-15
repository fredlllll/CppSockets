/*

   Example client program for sockets.  

   Adapted from https://msdn.microsoft.com/en-us/library/windows/desktop/ms737591(v=vs.85).aspx
 
   Copyright Simon D. Levy 2018

   MIT License
*/

#include "stdafx.h"

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

static const int BUFLEN = 80;

static const char * HOST = "localhost";
static const char * PORT = "20000";

static const float RATE = 1.0; // updates per second

static void error(const char * fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	char buf[200];
	vsnprintf(buf, 200, fmt, ap);
	puts(buf);
	va_end(ap);

	while (true);
}

static SOCKET connect(void)
{
	SOCKET sock = INVALID_SOCKET;

	// Initialize Winsock
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		error("WSAStartup failed with error: %d\n", iResult);
	}

	struct addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	struct addrinfo *result = NULL;
	iResult = getaddrinfo(HOST, PORT, &hints, &result);
	if (iResult != 0) {
		WSACleanup();
		error("getaddrinfo failed with error: %d\n", iResult);
	}

	// Attempt to connect to an address until one succeeds
	for (struct addrinfo * ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		sock = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (sock == INVALID_SOCKET) {
			WSACleanup();
			error("socket failed with error: %ld\n", WSAGetLastError());
		}

		// Connect to server.
		iResult = connect(sock, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(sock);
			sock = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	return sock;
}

int __cdecl main(int argc, char **argv)
{
    float prevtime = 0;

    SOCKET sock = INVALID_SOCKET;
    bool ready = false;

    while (true) {

        SYSTEMTIME st;
        GetSystemTime(&st);
        float currtime = st.wSecond + st.wMilliseconds / 1000.;

        if ((currtime - prevtime) > 1./RATE) {

            prevtime = currtime;

            // not connected; keep trying
            if (sock == INVALID_SOCKET) {

                printf("Attempting connection to server %s:%s\n", HOST, PORT);

                sock = connect();
            }

            else if (!ready) {
                printf("Connected!\n");
                unsigned long iMode = 1; // non-blocking
                int iResult = ioctlsocket(sock, FIONBIO, &iMode);
                if (iResult != NO_ERROR) {
                    error("ioctlsocket failed with error: %ld\n", iResult);
                }
                else {
                    ready = true;
                }
            }

            else {

                char s[BUFLEN] = "";

                if (recv(sock, s, BUFLEN, 0) > 0) {
                    printf("Server said: %s\n", s);
                }

                // Server exited
                else if (WSAGetLastError() == WSAECONNRESET) {
                    break;
                }

            }
        }
    }

    /*
    // Send an initial buffer
    const char *sendbuf = "this is a test";
    int iResult = send(sock, sendbuf, (int)strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR) {
        closesocket(sock);
        WSACleanup();
        error("send failed with error: %d\n", WSAGetLastError());
    }
    */

    // cleanup
    if (shutdown(sock, SD_SEND) == SOCKET_ERROR) {
        closesocket(sock);
        WSACleanup();
        error("shutdown failed with error: %d\n", WSAGetLastError());
    }
    closesocket(sock);
    WSACleanup();

    return 0;
}

