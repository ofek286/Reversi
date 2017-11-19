/***************************************
 * Student Name: Ofek Segal
 * Student ID: 315638288
 * Exercise Name: Ex2
 **************************************/

#ifndef HUMANPLAYER_H_
#define HUMANPLAYER_H_

#include <vector>
#include <iostream>
#include <stdio.h>
#include "Tools.h"
#include "Player.h"

class HumanPlayer: public Player {
public:
	HumanPlayer(Board::Cell type);
	~HumanPlayer();
	virtual Point makeMove(std::vector<Point> *options);
};

#endif /* HUMANPLAYER_H_ */
