#include "../TcpServer.hpp"
#include <stdio.h>
#include <iostream>

void testAccept(std::shared_ptr<CppSockets::TcpClient> client) {
	char data[] = "HTTP/1.1 200 OK\n\nHello World";
	while (true) {
		char buffer[1025];
		buffer[1024] = 0;
		auto cnt = client->receiveData(buffer, 1024);
		if (cnt < 1024) {
			//printf("%s\n", buffer);
			break;
		}
		else {
			//printf(buffer);
		}
	}
	client->sendData(data, strlen(data));
	printf("sent stuff\n");
	client->close();
}

int main(int argc, char** argv) {
	printf("starting\n");
	CppSockets::cppSocketsInit();
	printf("creating server");
	CppSockets::TcpServer server(80);
	printf("setting callback\n");
	server.acceptCallback = &testAccept;
	printf("starting listening\n");
	server.startListening();
	printf("waiting\n");
	std::this_thread::sleep_for(std::chrono::seconds(15));
	printf("stopping listening\n");
	server.stopListening();
	printf("stopped listening\n");
}