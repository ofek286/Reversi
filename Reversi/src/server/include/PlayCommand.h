/***************************************
 * Student Name: Ofek Segal and Natalie Elisha
 * Student ID: 315638288, 209475458
 * Exercise Name: Ex5
 **************************************/

#ifndef PLAYCOMMAND_H_
#define PLAYCOMMAND_H_

#include "Command.h"
#include "GameSet.h"

#include <vector>
#include <string>

class PlayCommand: public Command {
public:
	PlayCommand(GameSet& info);
	virtual ~PlayCommand();

	virtual void execute(int sender, std::vector<std::string> args);

private:
	GameSet& info;
};

#endif /* PLAYCOMMAND_H_ */
