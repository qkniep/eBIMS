#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

#include <arpa/inet.h>


class Client {
	int serverSocket, valread;
	struct sockaddr_in serverAddress;


public:
	Client();
	void connectToServer(std::string ip, uint16_t port);
};

#endif /* !CLIENT_HPP */
