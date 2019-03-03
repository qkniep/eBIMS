#include "client.hpp"

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <iostream>
#include <sstream>

#include <termcolor/termcolor.hpp>

#include "client_config.hpp"


Client::Client() : serverSocket(0) {
	if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Socket creation error\n");
		exit(EXIT_FAILURE);
	}
}


void Client::connectToServer(std::string ip, uint16_t port) {
	memset(&serverAddress, '0', sizeof(serverAddress));

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);

	if (inet_pton(AF_INET, ip.c_str(), &serverAddress.sin_addr) <= 0) {
		std::cout << termcolor::red << "ERROR" << termcolor::reset
			<< " Invalid address: " << ip << std::endl;
		exit(EXIT_FAILURE);
	}

	if (connect(serverSocket, (struct sockaddr*) &serverAddress,
				sizeof(serverAddress)) < 0) {
		std::cout << termcolor::red << "ERROR" << termcolor::reset
			<< " Connection to Book Server failed. Try again." << std::endl;
		exit(EXIT_FAILURE);
	}
}


void Client::mainLoop() {
	for (;;) {
		std::string query;
		char buffer[1024] = {0};

		std::cout << "> ";
		std::cin >> query;
		send(serverSocket, query.c_str(), query.length(), 0);

		recv(serverSocket, buffer, 1024, 0);
		int content_len = atoi(buffer);
		memset(buffer, 0, 1024);
		std::stringstream ss;
		while (content_len > 0 && (valrecv = recv(serverSocket, buffer, 1024, 0))) {
			content_len -= valrecv;
			ss << std::string(buffer);
			memset(buffer, 0, 1024);
		}
		if (valrecv == 0) {
			printf("Book Server closed the connection.\n");
			exit(EXIT_FAILURE);
		}
		std::cout << ss.str();
		fflush(stdout);
	}
}


int main() {
	Client c;
	c.connectToServer(SERVER_ADDR, PORT);
	c.mainLoop();
	return EXIT_SUCCESS;
}
