#include "server.hpp"

#include <sys/socket.h>
#include <unistd.h>
#include <filesystem>
#include <iostream>

#include "server_config.hpp"


//namespace fs = boost::filesystem;
namespace fs = std::filesystem;


Server::Server()
	: db(DBFILE)
	, gr(GOODREADS_DEVKEY) {}


void Server::init() {
	int opt = 1;
	if ((masterSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	if (setsockopt(masterSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		perror("setsockopt failed");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	if (bind(masterSocket, (struct sockaddr*) &address, sizeof(address)) < 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
}


void Server::handleClient() {
	int clientSocket;

	if (listen(masterSocket, 3) < 0) {
		perror("listen failed");
		exit(EXIT_FAILURE);
	}
	int addrlen = sizeof(address);
	if ((clientSocket = accept(masterSocket, (struct sockaddr*) &address, (socklen_t*) &addrlen)) < 0) {
		perror("accept failed");
		exit(EXIT_FAILURE);
	}
	std::cout << "New Client connected." << std::endl;

	for (;;) {
		char buffer[1024] = {0};
		if (recv(clientSocket, buffer, 1024, 0) == 0) {
			std::cout << "Client closed connection." << std::endl;
			return;
		}
		db.sendFindBooks(clientSocket, buffer);
	}
}


void Server::backgroundProcessing() {
	for (auto &file : fs::directory_iterator("Books/")) {
		std::optional<struct book> book;
		while (!(book = gr.search(file.path().stem().string()))) {
			sleep(50);
		}
		db.insertBook(*book);
		//db.printBookTable();
	}
}


void Server::mainLoop() {
	//backgroundProcessing();

	for (;;) {
		handleClient();
	}
}


int main(int /*argc*/, char** /*argv*/) {
	Server s;
	s.init();
	s.mainLoop();
	return EXIT_SUCCESS;
}
