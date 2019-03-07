#ifndef CLIENTHANDLER_HPP
#define CLIENTHANDLER_HPP

#include <netinet/in.h>
#include <thread>

#include "database.hpp"


class ClientHandler {
	static Database* db;
	bool active;
	int socket;
	std::thread mainThread;


public:
	ClientHandler();
	static void setDatabase(Database* db);
	bool start(int sock);
	bool stop();
	bool isActive();

private:
	void handleRequests();
};

#endif /* !CLIENTHANDLER_HPP */
