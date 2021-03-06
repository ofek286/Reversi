/***************************************
 * Student Name: Ofek Segal and Natalie Elisha
 * Student ID: 315638288, 209475458
 * Exercise Name: Ex5
 **************************************/

#ifndef JOINCOMMAND_H_
#define JOINCOMMAND_H_

#include "Command.h"
#include "GameSet.h"

#include <vector>
#include <string>

class JoinCommand: public Command {
public:
	JoinCommand();
	virtual ~JoinCommand();

	virtual CommandResult execute(int sender, std::vector<std::string> args);

private:
	GameSet *games;
};

#endif /* JOINCOMMAND_H_ */
