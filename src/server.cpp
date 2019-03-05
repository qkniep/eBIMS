#include "server.hpp"

#include <arpa/inet.h>
#include <unistd.h>
#include <filesystem>
#include <iostream>

#include "server_config.hpp"
#include "util.hpp"

#define BUFFER_SIZE 1024

namespace fs = std::filesystem;


Server::Server() : db(DBFILE) , gr(GOODREADS_DEVKEY) {}


void Server::init() {
	if ((masterSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		fatalError("Failed to open socket.");
	}

	int opt = 1;
	if (setsockopt(masterSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		fatalError("Failed to set socket options.");
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	if (bind(masterSocket, (struct sockaddr*) &address, sizeof(address)) < 0) {
		fatalError("Failed to bind socket.");
	}

	if (listen(masterSocket, 3) < 0) {
		fatalError("Failed to mark socket for listening.");
	}
}


int Server::acceptClient() {
	int clientSocket;

	int addrlen = sizeof(address);
	if ((clientSocket = accept(masterSocket, (struct sockaddr*) &address,
					(socklen_t*) &addrlen)) < 0) {
		fatalError("Failed to accept incoming client connection.");
	}
	getpeername(masterSocket, (struct sockaddr*) &address, (socklen_t*) &addrlen);
	std::cout << "New Client connected: " << inet_ntoa(address.sin_addr)
		<< ":" << ntohs(address.sin_port) << std::endl;

	return clientSocket;
}


void Server::handleClient(int clientSocket) {
	for (;;) {
		char buffer[BUFFER_SIZE+1];
		int bytesRead;
		if ((bytesRead = read(clientSocket, buffer, BUFFER_SIZE)) == 0) {
			generalWarning("Client closed connection.");
			close(clientSocket);
			clientSocket = 0;
			return;
		}
		buffer[bytesRead] = '\0';
		db.sendFindBooks(clientSocket, buffer);
	}
}


void Server::backgroundProcessing() {
	for (auto &file : fs::directory_iterator(BOOKS_DIR)) {
		std::optional<struct book> book;
		while (!(book = gr.search(file.path().stem().string()))) {
			sleep(50);
		}
		db.insertBook(*book);
	}
}


void Server::mainLoop() {
	//backgroundProcessing();

	for (;;) {
		int clientSocket = acceptClient();
		handleClient(clientSocket);
	}
}


int main(int /*argc*/, char** /*argv*/) {
	Server s;
	s.init();
	s.mainLoop();
	return EXIT_SUCCESS;
}
