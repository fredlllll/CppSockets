#include "../TcpServer.hpp"
#include <stdio.h>
#include <iostream>
#include <mutex>
#include <vector>

struct chatClient {
	std::shared_ptr<std::thread> receiveThread;
	std::shared_ptr<CppSockets::TcpClient> client;
};

struct messageHeader {
	unsigned char opcode;
};

const int opMessage = 1;

std::mutex serverMutex;
std::shared_ptr<CppSockets::TcpServer> server;
std::vector<chatClient> clients;

void broadcastMessage(char* message, unsigned int len) {
	LOCK_GUARD(serverMutex);
	std::for_each(clients.begin(), clients.end(), [](auto cc) {
		cc.client->sendData((void*)&opMessage, sizeof(int));
	});
}

void clientLoop(std::shared_ptr<CppSockets::TcpClient> client) {
	char buffer[1024];
	while (true) {
		int cnt = client->receiveData(buffer, sizeof(messageHeader));
		if (cnt == 0) {
			break;
		}
		messageHeader header = (*(messageHeader*)buffer);
		switch (header.opcode) {
		case opMessage:
			client->receiveData(buffer, sizeof(unsigned int));
			unsigned int messageLength = (*(unsigned int*)buffer);
			char* messageBuffer = new char[messageLength];
			client->receiveData(messageBuffer, messageLength);
			broadcastMessage(messageBuffer, messageLength);
			delete[] messageBuffer;
		}
	}
	{
		LOCK_GUARD(serverMutex);
		auto it = clients.begin();
		while (it->client != client) {
			it++;
			if (it == clients.end()) {
				printf("couldnt remove client??");
				return;
			}
		}
		clients.erase(it);
	}
}

void serverAccept(std::shared_ptr<CppSockets::TcpClient> client) {
	LOCK_GUARD(serverMutex);
	chatClient cc;
	cc.client = client;
	std::shared_ptr<std::thread> tp = std::make_shared<std::thread>(&clientLoop, client);
	cc.receiveThread = tp;
	clients.push_back(cc);
}

int main(int argc, char** argv) {
	printf("starting\n");
	CppSockets::cppSocketsInit();
	printf("creating server\n");
	server = std::make_shared<CppSockets::TcpServer>(8989);
	printf("setting callback\n");
	server->acceptCallback = &serverAccept;
	printf("starting listening\n");
	server->startListening();
	printf("waiting\n");
	//TODO: wait for key input
	std::this_thread::sleep_for(std::chrono::seconds(15));
	printf("stopping listening\n");
	server->stopListening();
	printf("stopped listening\n");

	std::for_each(clients.begin(), clients.end(), [](auto cc) {
		cc.client->close();
		cc.receiveThread->join();
	});
}