#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

#include <arpa/inet.h>


class Client {
	int serverSocket, valrecv;
	struct sockaddr_in serverAddress;


public:
	Client();
	void connectToServer(std::string ip, uint16_t port);
	void mainLoop();
};

#endif /* !CLIENT_HPP */
