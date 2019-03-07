#ifndef SERVER_HPP
#define SERVER_HPP

#include <netinet/in.h>

#include "clientHandler.hpp"
#include "database.hpp"
#include "goodreads.hpp"
#include "server_config.hpp"


class Server {
	Database db;
	Goodreads gr;
	int masterSocket;
	struct sockaddr_in address;
	ClientHandler clientHandlers[MAX_CLIENTS];


public:
	Server();
	void init();
	void mainLoop();

private:
	void backgroundProcessing();
	int acceptClient();
	void printBookTable();
};

#endif /* !SERVER_HPP */
