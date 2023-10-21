// definition of struct Position and related functions

#ifndef POSITION_H
#define POSITION_H

#include <stdbool.h>
#include "darray.h"

struct Position{
	int fileID;			// ID of the file where the position was found
	int lineNumber;			// line number in the file where the position was found
	int value;			// the value associated with the position that represents a memory address
	int dependencyCount;		// the number of labels in the dependencies DArray
						// -1 indicates that the value has been evaluated / solved and is valid
	struct DArray dependencies;	// contains the IDs of labels whose values are needed to evaluate this position's value
	char* expression;		// pointer to allocated string of the expression to evaluate later
};

// adds a new position object to the global positionArray given information from line
void processPosition(const char* line);

// attempts to evaluate a position's value from its expression and update its value
// returns true if the expression was evaluated successfully or was already evaluated
bool solvePosition(struct Position* pos);

#endif
