/***************************************
 * Student Name: Ofek Segal and Natalie Elisha
 * Student ID: 315638288, 209475458
 * Exercise Name: Ex5
 **************************************/

#ifndef TOOLSFORSTRINGS_H_
#define TOOLSFORSTRINGS_H_

#include <string>
#include <utility>
#include <sstream>
#include <vector>

char *myToString(int number);

bool replaceStringWithString(std::string& replacee,
							 const std::string& toReplace,
							 const std::string& replaceWith);

std::pair<int, int> extractPairFromString(std::string& theString);

std::pair<std::string, std::vector<std::string> > extractCommand(std::string& msg);

#endif /* TOOLSFORSTRINGS_H_ */
