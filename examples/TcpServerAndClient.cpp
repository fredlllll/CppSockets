#include "../TcpServer.hpp"
#include <vector>
#include <thread>
#include <stdio.h>
#include <iostream>

std::vector<std::shared_ptr<std::thread>> threads;


void echoLoop(std::shared_ptr<CppSockets::TcpClient> client) {
	const int bufferSize = 1024;
	char buffer[bufferSize + 1];
	buffer[bufferSize] = 0;
	while (true) {
		auto cnt = client->receiveData(buffer, bufferSize);
		if (cnt == 0) {
			break;
		}
		else {
			client->sendData(buffer, cnt);
		}
	}
	printf("echo loop closed\n");
}

void testAccept(std::shared_ptr<CppSockets::TcpClient> client) {
	std::shared_ptr<std::thread> tp = std::make_shared<std::thread>(&echoLoop, client);
	threads.push_back(tp);
}


int main(int argc, char** argv) {
	printf("starting\n");
	CppSockets::cppSocketsInit();
	printf("creating server\n");
	CppSockets::TcpServer server(8989);
	printf("setting callback\n");
	server.acceptCallback = &testAccept;
	printf("starting listening\n");
	server.startListening();

	printf("creating client\n");
	CppSockets::TcpClient client("localhost", 8989);
	printf("sending data\n");
	client.sendData("12345", 5);
	char buffer[10];
	printf("receiving data\n");
	auto cnt = client.receiveData(buffer, 10);
	buffer[cnt] = 0;
	printf("%s\n", buffer);
	client.close();

	printf("waiting\n");
	std::this_thread::sleep_for(std::chrono::seconds(1));
	printf("stopping listening\n");
	server.stopListening();
	printf("stopped listening\n");

	for (auto it = threads.begin(); it != threads.end(); ++it) {
		printf("join a thread");
		(*it)->join();
	}
}