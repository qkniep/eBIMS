#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

#include <arpa/inet.h>


class Client {
	std::string serverIP;
	uint16_t serverPort;
	int serverSocket;
	struct sockaddr_in serverAddress;


public:
	Client(std::string ip, uint16_t port);
	void resetSocket();
	void connectToServer();
	void mainLoop();
};

#endif /* !CLIENT_HPP */
