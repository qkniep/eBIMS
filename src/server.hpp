#ifndef SERVER_HPP
#define SERVER_HPP

#include <netinet/in.h>

#include "database.hpp"
#include "goodreads.hpp"


class Server {
	Database db;
	Goodreads gr;
	int masterSocket;
	struct sockaddr_in address;


public:
	Server();
	void init();
	void mainLoop();

private:
	void backgroundProcessing();
	void handleClient();
	void printBookTable();
};

#endif /* !SERVER_HPP */
