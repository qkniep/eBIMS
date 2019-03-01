#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <iostream>

#include <termcolor/termcolor.hpp>

#include "client_config.hpp"


int main() {
	//std::cout << termcolor::yellow << "HELLO" << termcolor::reset << "  WORLD" << std::endl;

	int sock = 0, valread;
	struct sockaddr_in serv_addr;
	std::string query;
	char buffer[1024] = {0};

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Socket creation error\n");
		return EXIT_FAILURE;
	}

	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	if (inet_pton(AF_INET, SERVER_ADDR, &serv_addr.sin_addr) <= 0) {
		printf("Invalid address / Address not supported\n");
		return EXIT_FAILURE;
	}

	if (connect(sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
		printf("Connection to Book Server failed. Try again.\n");
		return EXIT_FAILURE;
	}
	std::cin >> query;

	send(sock, query.c_str(), query.length(), 0);
	while ((valread = read(sock, buffer, 1024))) {
		printf("%s\n", buffer);
	}
	return EXIT_SUCCESS;
}
