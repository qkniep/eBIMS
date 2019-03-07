#include "client.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

#include "client_config.hpp"
#include "util.hpp"

#define BUFFER_SIZE 1024


Client::Client(std::string ip, uint16_t port)
		: serverIP(ip), serverPort(port), serverSocket(0) {
	resetSocket();
	connectToServer();
}


void Client::resetSocket() {
	if (serverSocket) close(serverSocket);

	if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		util::fatalError("Failed to open socket.");
	}
}


void Client::connectToServer() {
	std::cout << "Trying to establish connection with: "
		<< serverIP << ":" << serverPort << std::endl;

	memset(&serverAddress, '0', sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(serverPort);

	if (inet_pton(AF_INET, serverIP.c_str(), &serverAddress.sin_addr) <= 0) {
		util::fatalError("Invalid server IP address.");
	}

	if (connect(serverSocket, (struct sockaddr*) &serverAddress,
				sizeof(serverAddress)) < 0) {
		util::fatalError("Connection to Book Server failed.");
	}

	util::successMessage("Connection Successful!");
}


void Client::mainLoop() {
	std::string query;
	char buffer[BUFFER_SIZE+1];
	int bytesRead, contentLength;

	for (;;) {
		std::cout << "> ";
		std::cin >> query;
		write(serverSocket, query.c_str(), query.length());

		bytesRead = read(serverSocket, buffer, BUFFER_SIZE);
		buffer[bytesRead] = '\0';
		contentLength = atoi(buffer);

		while (contentLength > 0
				&& (bytesRead = read(serverSocket, buffer, BUFFER_SIZE))) {
			buffer[bytesRead] = '\0';
			contentLength -= bytesRead;
			std::cout << std::string(buffer);
		}
		if (bytesRead == 0) {
			util::generalWarning("Book Server closed the connection.");
			resetSocket();
			connectToServer();
		}
	}
}


int main(int /*argc*/, char** /*argv*/) {
	Client c(SERVER_ADDR, PORT);
	c.mainLoop();
	return EXIT_SUCCESS;
}
