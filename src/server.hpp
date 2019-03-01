#ifndef SERVER_HPP
#define SERVER_HPP

#include <arpa/inet.h>
#include <netinet/in.h>

#include "database.hpp"


class Server {
	Database db;
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
