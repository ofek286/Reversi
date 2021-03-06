/***************************************
 * Student Name: Ofek Segal and Natalie Elisha
 * Student ID: 315638288, 209475458
 * Exercise Name: Ex7
 **************************************/

#include "GameTask.h"
#include "Server.h"

/***************************************
 * Function Name: gameThreadMain
 * The Input: a pointer of an argument
 * The Output: NULL
 * The Function Operation: the main
 * function of the game's thread
 **************************************/
void *gameThreadMain(void *arg) {
	//Getting the current game from the arg
	GameInfo *currGame = (GameInfo *) arg;

	//Getting the game set
	GameSet *games = GameSet::getInstance();

	//Getting the client file descriptors
	int firstClient = currGame->getClientA();
	int secondClient = currGame->getClientB();

	bool ok = true;

	//Var to hold the command result
	CommandResult result;

	//Var to hold the current client
	int currClient = firstClient;

	while (ok) {
		result = Task::handleCommand(currClient);
		ok = result.getStatus();
		if (!ok) {
			games->removeGame(currGame);
			return NULL;
		}
		if (currGame->getInterrupt() == true) {
			games->removeGame(currGame);
			return NULL;
		}
		currClient = (currClient == firstClient) ? secondClient : firstClient;
	}

	return NULL;
}

/***************************************
 * Function Name: GameTask
 * The Input: a pointer of an argument
 * The Output: a GameTask instance
 * The Function Operation: creating and
 * initializing a task instance using the
 * given arguments.
 **************************************/
GameTask::GameTask(void *arg): Task(arg) {
	//Nothing right now
}

/***************************************
 * Function Name: execute
 * The Input: no input
 * The Output: no output
 * The Function Operation: executing the
 * gameThreadMain function using the given
 * argument to the GameTask instance
 **************************************/
void GameTask::execute() {
	gameThreadMain(arg);
}
