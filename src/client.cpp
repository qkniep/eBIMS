#include "client.hpp"

#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <iostream>

#include <termcolor/termcolor.hpp>

#include "client_config.hpp"


Client::Client() : serverSocket(0) {
	if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Socket creation error\n");
		exit(EXIT_FAILURE);
	}
}


void Client::connectToServer(std::string ip, uint16_t port) {
	std::string query;
	char* buffer = {0};

	memset(&serverAddress, '0', sizeof(serverAddress));

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);

	if (inet_pton(AF_INET, ip.c_str(), &serverAddress.sin_addr) <= 0) {
		printf("Invalid address / Address not supported\n");
		exit(EXIT_FAILURE);
	}

	if (connect(serverSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) < 0) {
		printf("Connection to Book Server failed. Try again.\n");
		exit(EXIT_FAILURE);
	}
	std::cin >> query;

	send(serverSocket, query.c_str(), query.length(), 0);
	while ((valread = read(serverSocket, buffer, 1024))) {
		printf("%s\n", buffer);
	}
}


int main() {
	//std::cout << termcolor::yellow << "HELLO" << termcolor::reset << "  WORLD" << std::endl;
	Client c;
	c.connectToServer(SERVER_ADDR, PORT);
	return EXIT_SUCCESS;
}
