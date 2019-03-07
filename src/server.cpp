#include "server.hpp"

#include <arpa/inet.h>
#include <unistd.h>
#include <filesystem>
#include <iostream>

#include "util.hpp"

#define BUFFER_SIZE 1024

namespace fs = std::filesystem;


Server::Server() : db(DBFILE) , gr(GOODREADS_DEVKEY) {
	ClientHandler::setDatabase(&db);
}


void Server::init() {
	if ((masterSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		util::fatalError("Failed to open socket.");
	}

	int opt = 1;
	if (setsockopt(masterSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		util::fatalError("Failed to set socket options.");
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	if (bind(masterSocket, (struct sockaddr*) &address, sizeof(address)) < 0) {
		util::fatalError("Failed to bind socket.");
	}

	if (listen(masterSocket, 3) < 0) {
		util::fatalError("Failed to mark socket for listening.");
	}
}


int Server::acceptClient() {
	int clientSocket;

	int addrlen = sizeof(address);
	if ((clientSocket = accept(masterSocket, (struct sockaddr*) &address,
					(socklen_t*) &addrlen)) < 0) {
		util::fatalError("Failed to accept incoming client connection.");
	}
	getpeername(masterSocket, (struct sockaddr*) &address, (socklen_t*) &addrlen);
	std::cout << "New Client connected: " << inet_ntoa(address.sin_addr)
		<< ":" << ntohs(address.sin_port) << std::endl;

	return clientSocket;
}


void Server::backgroundProcessing() {
	for (auto &file : fs::directory_iterator(BOOKS_DIR)) {
		std::optional<struct book> book;
		while (!(book = gr.search(file.path().stem().string()))) {
			sleep(1);
		}
		db.insertBook(*book);
		util::info("Added a new book.");
	}
}


void Server::mainLoop() {
	std::thread bg(&Server::backgroundProcessing, this);

	for (;;) {
		int cSocket = acceptClient();
		bool success = false;
		for (int i = 0 ; i < MAX_CLIENTS && !success; ++i) {
			success = clientHandlers[i].start(cSocket);
		}
		if (!success) {
			close(cSocket);
			util::generalWarning("Client dropped b/c of connection limit.");
		}
	}
}


int main(int /*argc*/, char** /*argv*/) {
	Server s;
	s.init();
	s.mainLoop();
	return EXIT_SUCCESS;
}
