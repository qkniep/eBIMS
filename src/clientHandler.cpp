#include "clientHandler.hpp"

#include <unistd.h>

#include "util.hpp"

#define BUFFER_SIZE 1024


Database* ClientHandler::db = nullptr;


ClientHandler::ClientHandler() {}


void ClientHandler::setDatabase(Database* _db) {
	db = _db;
}


bool ClientHandler::start(int sock) {
	if (active) return false;
	active = true;
	socket = sock;
	mainThread = std::thread(&ClientHandler::handleRequests, this);
	//mainThread = std::thread([this] {this->handleRequests()});
	mainThread.detach();
	return true;
}


bool ClientHandler::stop() {
	if (!active) return false;
	active = false;
	close(socket);
	socket = 0;
	mainThread = std::thread();
	return true;
}


bool ClientHandler::isActive() {
	return active;
}


void ClientHandler::handleRequests() {
	for (;;) {
		char buffer[BUFFER_SIZE+1];
		int bytesRead;
		if ((bytesRead = read(socket, buffer, BUFFER_SIZE)) == 0) {
			util::generalWarning("Client closed connection.");
			stop();
			return;
		}
		buffer[bytesRead] = '\0';
		db->sendFindBooks(socket, buffer);
	}
}
