/***************************************
 * Student Name: Ofek Segal and Natalie Elisha
 * Student ID: 315638288, 209475458
 * Exercise Name: Ex4
 **************************************/

#ifndef SERVER_H_
#define SERVER_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <utility>

#include "CommandsManager.h"
#include "GameInfo.h"

#define MAX_CONNECTED_CLIENTS 10

#define BUFFER_SIZE 2000

#define RECV_FLAGS 0
#define SEND_FLAGS 0

#define MAX_GAMES 5

class Server {
public:
	Server();
	Server(bool verbose);
	Server(int port);
	Server(int port, bool verbose);
	~Server();

	void start();
	void stop();

	bool sendMessageToClient(int client, std::string& msg);
private:
	bool verbose;

	int port;
	int serverSocket;
	struct sockaddr_in server;

	int clientASock;
	int clientBSock;
	struct sockaddr_in clientA;
	struct sockaddr_in clientB;
	socklen_t clientALen;
	socklen_t clientBLen;

	bool handleClient(int clientA, char curr, int clientB);
};


std::pair<std::string, std::vector<std::string> > extractCommand(std::string& msg);
#endif /* SERVER_H_ */
