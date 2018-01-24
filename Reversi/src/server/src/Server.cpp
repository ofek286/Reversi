/***************************************
 * Student Name: Ofek Segal and Natalie Elisha
 * Student ID: 315638288, 209475458
 * Exercise Name: Ex7
 **************************************/

#include "Server.h"
using namespace std;

//void *gameThreadMain(void *arg);
//void *clientCommunicationThreadMain(void *arg);
void *exitThreadMain(void *arg);

/***************************************
 * Function Name: Server (Constructor)
 * The Input: nothing
 * The Output: the server instance
 * The Function Operation: initializing
 *  the server from config file
 **************************************/
Server::Server(): serverSocket(0) {
	//Reading the config file and applying the config
	ifstream serverConfig("server_port.txt");
	if (!serverConfig.is_open()) {
		throw runtime_error("File not opening");
	}
	port = 0;
	serverConfig >> port;
	//Closing the file
	serverConfig.close();

	VerboseController::initialize();
	this->verbose = VerboseController::getInstance(false);

	CommandsManager::initialize();
	this->cmdManager = CommandsManager::getInstance();

	ThreadPool::initialize();
	this->threadPool = ThreadPool::getInstance(MAX_THREADS);

	ExitController::initialize();
	this->exitController = ExitController::getInstance();
}

/***************************************
 * Function Name: Server (Constructor)
 * The Input: verbose flag
 * The Output: the server instance
 * The Function Operation: initializing
 *  the server from config file
 **************************************/
Server::Server(bool verbose): serverSocket(0){
	//Reading the config file and applying the config
	ifstream serverConfig("server_port.txt");
	if (!serverConfig.is_open()) {
		throw runtime_error("File not opening");
	}
	port = 0;
	serverConfig >> port;
	//Closing the file
	serverConfig.close();

	VerboseController::initialize();
	this->verbose = VerboseController::getInstance(verbose);

	CommandsManager::initialize();
	this->cmdManager = CommandsManager::getInstance();

	ThreadPool::initialize();
	this->threadPool = ThreadPool::getInstance(MAX_THREADS);

	ExitController::initialize();
	this->exitController = ExitController::getInstance();
}

/***************************************
 * Function Name: Server (Constructor)
 * The Input: the port
 * The Output: the server instance
 * The Function Operation: initializing
 *  the server from port input
 **************************************/
Server::Server(int port): port(port), serverSocket(0){

	VerboseController::initialize();
	this->verbose = VerboseController::getInstance(false);

	CommandsManager::initialize();
	this->cmdManager = CommandsManager::getInstance();

	ThreadPool::initialize();
	this->threadPool = ThreadPool::getInstance(MAX_THREADS);

	ExitController::initialize();
	this->exitController = ExitController::getInstance();
}

/***************************************
 * Function Name: Server (Constructor)
 * The Input: the port and verbose flag
 * The Output: the server instance
 * The Function Operation: initializing
 *  the server from port input
 **************************************/
Server::Server(int port, bool verbose): port(port), serverSocket(0){

	VerboseController::initialize();
	this->verbose = VerboseController::getInstance(verbose);

	CommandsManager::initialize();
	this->cmdManager = CommandsManager::getInstance();

	ThreadPool::initialize();
	this->threadPool = ThreadPool::getInstance(MAX_THREADS);

	ExitController::initialize();
	this->exitController = ExitController::getInstance();
}

/***************************************
 * Function Name: ~Server (Destructor)
 * The Input: no input
 * The Output: no output
 * The Function Operation: nothing
 **************************************/
Server::~Server() {
//	for (vector<pthread_t *>::iterator it = gameThreads.begin(); it != gameThreads.end(); ++it) {
//		delete (*it);
//	}
//
//	for (vector<pthread_t *>::iterator it = clientCommunicationThreads.begin(); it != clientCommunicationThreads.end(); ++it) {
//		delete (*it);
//	}
	VerboseController::deleteInstance();
	CommandsManager::deleteInstance();
	ExitController::deleteInstance();

	cout << "All done. All communications are closed!" << endl;
}


/***************************************
 * Function Name: start
 * The Input: no input
 * The Output: no output
 * The Function Operation: starting the
 *  server
 **************************************/
void Server::start() {
	pthread_t exitThread;

	//Creating a socket pointer
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1) {
		//The socket wasn't created, so we crash
		throw "Error opening socket";
	}
	//Assigning a local address to the socket
	bzero((void *)&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);
	//Binding the port
	if (bind(serverSocket, (struct sockaddr *)&server, sizeof(server)) < 0) {
		//The binding failed, so we crash
		throw "Error on binding";
	}
	//Setting a timeout - to be able to shutdown gracefully (almost) anytime
	struct timeval tv;
	tv.tv_sec = SOCKET_TIMEOUT;
	tv.tv_usec = 0;

	if (setsockopt(serverSocket, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tv, sizeof(struct timeval)) < 0) {
		//Timeout setting failed, so we crash
		throw "Error on setting timeout";
	}

	//Starting listening to incoming connections
	listen(serverSocket, MAX_CONNECTED_CLIENTS);

	int exitThreadResult = pthread_create(&exitThread, NULL, exitThreadMain, (void *) this);
	if (exitThreadResult) {
		cout << "Exit thread creation failed, exiting" << endl;
		return;
	} else {
		cout << "Enter 'exit' to close the server" << endl;
	}

	//Waiting for the clients to connect
	verbose->lockCout();
	cout << "Waiting for client connections..." << endl;
	verbose->unlockCout();

	while (true) {
		if (getExit()) {
			break;
		}
		int currClientSocket;
		struct sockaddr_in currClient;
		socklen_t clientLen = sizeof(struct sockaddr_in);
		//Cleaning the client's info structure
		bzero((void *)&currClient, sizeof(currClient));

		//Connecting the current client
		currClientSocket = accept(serverSocket, (struct sockaddr *)&currClient, (socklen_t*)&clientLen);

		bool exitNow = false;
		while (currClientSocket < 0 && !exitNow) {
			//Timeout (or failure), so we check for exit and get beck to accepting
			if (getExit()) {
				exitNow = true;
				break;
			}
			if (getVerbose()) {
				verbose->lockCout();
				cout << "Client accept timeout, retrying.." << endl;
				verbose->unlockCout();
			}
			//Cleaning and retrying the accept
			bzero((void *)&currClient, sizeof(currClient));
			currClientSocket = accept(serverSocket, (struct sockaddr *)&currClient, (socklen_t*)&clientLen);
		}
		if (exitNow) {
			break;
		}
		if (getExit()) {
			close(currClientSocket);
			break;
		}
		verbose->lockCout();
		cout << "New Client is connected" << endl;
		verbose->unlockCout();

//		pthread_t *newThread = new pthread_t();

//		clientCommunicationThreads.push_back(newThread);
//
//		lastUsedClient = currClientSocket;

//		int threadCreateResult = pthread_create(newThread, NULL, clientCommunicationThreadMain, (void *) this);
//
//		if (threadCreateResult) {
//			verbose->lockCout();
//			cout << "Error, thread creating failed" << endl;
//			verbose->unlockCout();
//
//			removeFromVector(&clientCommunicationThreads, newThread);
//		}

		long socketAsLong = (long) currClientSocket;
		threadPool->addTask(new ClientRequestsTask((void *) socketAsLong));
	}

//	for (vector<pthread_t *>::iterator it = clientCommunicationThreads.begin(); it != clientCommunicationThreads.end(); ++it) {
//		pthread_join(*(*it), NULL);
//	}
//
//	for (vector<pthread_t *>::iterator it = gameThreads.begin(); it != gameThreads.end(); ++it) {
//		pthread_join(*(*it), NULL);
//	}
	pthread_join(exitThread, NULL);
}

/***************************************
 * Function Name: stop
 * The Input: no input
 * The Output: no output
 * The Function Operation: closing the
 *  socket
 **************************************/
void Server::stop() {
	ThreadPool::deleteInstance();

	for (vector<int>::iterator it = connectedClients.begin(); it != connectedClients.end(); ++it) {
		close(*it);
	}

	close(serverSocket);
}

///***************************************
// * Function Name: handleCommand
// * The Input: socket id of a client
// * The Output: boolean value (true or false)
// * The Function Operation: handling the client's
// * command and returning if the operation succeeded
// **************************************/
//bool Server::handleCommand(int client) {
//	//Creating a buffer
//	char buffer[BUFFER_SIZE] = {0};
//	//Keeping the message
//	string msg = "";
//	//Creating vars to keep the sockets' responses
//	int readSize;
//	while (true) {
//		//Reading the message
//		readSize = recv(client, buffer, BUFFER_SIZE, RECV_FLAGS);
//		while (readSize == -1) {
//			//Re-reading the message (timeout)
//			readSize = recv(client, buffer, BUFFER_SIZE, RECV_FLAGS);
//		}
//		if (readSize <= 0) {
//			//Client disconnected
//			pthread_mutex_lock(&coutMutex);
//			cout << "Client " << client << " disconnected" << endl;
//			pthread_mutex_unlock(&coutMutex);
//			return false;
//		}
//
//		msg.append(buffer);
//		memset(buffer, 0, BUFFER_SIZE);
//
//		if (readSize != BUFFER_SIZE) {
//			break;
//		}
//	}
//	if (getVerbose()) {
//		pthread_mutex_lock(&coutMutex);
//		cout << client << ": " << msg << endl;
//		pthread_mutex_unlock(&coutMutex);
//	}
//	pair<string, vector<string> > cmd = extractCommand(msg);
//	cmdManager.executeCommand(client, cmd.first, cmd.second);
//	return true;
//}

/***************************************
 * Function Name: sendMessageToClient
 * The Input: socket id of a client and a reference
 * to a message
 * The Output: boolean value
 * The Function Operation: sending a message
 * to the given client and returning true if succeeded,
 * otherwise false
 **************************************/
bool Server::sendMessageToClient(int client, string& msg) {
	int writeSize;
	writeSize = send(client, msg.c_str(), msg.length(), SEND_FLAGS);
	if (writeSize < 0) {
		//Error in writing
		verbose->lockCout();
		cout << "Error writing to client " << client << endl;
		verbose->unlockCout();
		return false;
	}
	return true;
}

///***************************************
// * Function Name: addThread
// * The Input: socket id of a client
// * The Output: no output
// * The Function Operation: adding a thread for
// * the given client
// **************************************/
//void Server::addThread(int client) {
//	pthread_t *newThread = new pthread_t();
//
//	gameThreads.push_back(newThread);
//
//	GameInfo *currGame = games.getGameInfo(client);
//
//	lastUsedGame = currGame;
//
//	int threadCreateResult = pthread_create(newThread, NULL, gameThreadMain, (void *) this);
//
//	if (threadCreateResult) {
//		verbose->lockCout();
//		cout << "Error, thread creating failed" << endl;
//		verbose->unlockCout();
//
//		removeFromVector(&gameThreads, newThread);
//		int other = currGame->getOtherClient(client);
//		string endGame = string("END");
//		sendMessageToClient(client, endGame);
//		sendMessageToClient(other, endGame);
//	}
//}


/***************************************
 * Function Name: getVerbose
 * The Input: no input
 * The Output: boolean value
 * The Function Operation: returning
 * the value of the verbose
 **************************************/
bool Server::getVerbose() {
	return verbose->getVerbose();
}

/***************************************
 * Function Name: getExit
 * The Input: no input
 * The Output: boolean value
 * The Function Operation: returning the
 * value of the exit boolean member
 **************************************/
bool Server::getExit() {
//	pthread_mutex_lock(&serverExitMutex);
//	bool toReturn = serverExit;
//	pthread_mutex_unlock(&serverExitMutex);
//	return toReturn;
	return exitController->getExit();
}

/***************************************
 * Function Name: setExit
 * The Input: boolean value
 * The Output: no output
 * The Function Operation: updating the
 * value of the exit boolean member
 **************************************/
void Server::setExit() {
	exitController->setExit();
	threadPool->terminate();
}

//Outsider Functions


///***************************************
// * Function Name: extractCommand
// * The Input: a reference to a message
// * The Output: a pair of a command and
// * a suitable vector of arguments
// * The Function Operation: extracting a command
// * based on the given message
// **************************************/
//pair<string, vector<string> > extractCommand(string& msg) {
//	string command;
//	vector<string> args;
//	string currentWord = "";
//	bool foundCommand = false;
//
//	if (msg.find(" ") == string::npos) {
//		return make_pair(msg, args);
//	}
//
//	size_t index = 0;
//	while ((index = msg.find(" ")) != string::npos) {
//		currentWord = msg.substr(0, index);
//		if (!foundCommand) {
//			command = currentWord;
//			foundCommand = true;
//		} else {
//			args.push_back(currentWord);
//		}
//		msg = msg.substr(index + 1);
//	}
//	args.push_back(msg);
//
//	return make_pair(command, args);
//}

///***************************************
// * Function Name: gameThreadMain
// * The Input: a pointer of an argument
// * The Output: NULL
// * The Function Operation: the main
// * function of the game's thread
// **************************************/
//void *gameThreadMain(void *arg) {
//	Server *theServer = (Server *) arg;
//	GameInfo *currGame = theServer->lastUsedGame;
//
//	int firstClient = currGame->getClientA();
//	int secondClient = currGame->getClientB();
//
//	bool ok = true;
//
//	int currClient = firstClient;
//
//	while (ok) {
//		bool ok = theServer->handleCommand(currClient);
//		if (!ok) {
//			theServer->games.removeGame(currGame);
//			return NULL;
//		}
//		if (currGame->getInterrupt() == true) {
//			theServer->games.removeGame(currGame);
//			return NULL;
//		}
//		currClient = (currClient == firstClient) ? secondClient : firstClient;
//	}
//
//	return NULL;
//}
//
///***************************************
// * Function Name: clientCommunicationThreadMain
// * The Input: a pointer of an argument
// * The Output: NULL
// * The Function Operation: the main function
// * of the client's communication thread
// **************************************/
//void *clientCommunicationThreadMain(void *arg) {
//	Server *theServer = (Server *) arg;
//	int currClientSocket = theServer->lastUsedClient;
//
//	//Handling the requests
//	bool ok = true;
//	while (ok) {
//		if (theServer->getExit()) {
//			close(currClientSocket);
//			return NULL;
//		}
//		ok = theServer->handleCommand(currClientSocket);
//		if (!ok || (theServer->games.getLastCommand() == GameSet::Start &&
//					theServer->games.getLastCommandResult() == NO_ERROR_RESULT)) {
//			ok = false;
//		} else if (theServer->games.getLastCommand() == GameSet::Join &&
//				   theServer->games.getLastCommandResult() == NO_ERROR_RESULT) {
//			theServer->addThread(currClientSocket);
//			ok = false;
//		} else if (theServer->getExit()) {
//			ok = false;
//			close(currClientSocket);
//		}
//	}
//
//	return NULL;
//}

/***************************************
 * Function Name: exitThreadMain
 * The Input: a pointer of an argument
 * The Output: NULL
 * The Function Operation: the main function
 * of the "exit" thread
 **************************************/
void *exitThreadMain(void *arg) {
	Server *theServer = (Server *) arg;

	string input = "";
	string toCompare = "exit";
	cin >> input;
	while (input != toCompare) {
		cin >> input;
	}
	theServer->setExit();
	theServer->verbose->lockCout();
	cout << "Closing all communications, please wait." << endl;
	cout << "It may take up to " << SOCKET_TIMEOUT << " seconds" << endl;
	theServer->verbose->unlockCout();

	return NULL;
}
