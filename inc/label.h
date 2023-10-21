// contains struct Label definition and all functions related to it

#ifndef LABEL_H
#define LABEL_H

#include <stdbool.h>
#include <stddef.h>
#include "darray.h"

/* 
 * label definition is when a label is actuall given a value somewhere
 * using a label name in an expression is not defintion
 * >label and =value 100 are examples of label defintion
*/

struct Label{
	int fileID;			// ID of the file where the label was defined
	int lineNumber;			// line number in the file where the label was defined
	int value;			// the value associated with the label that is used in expressions
	int posDependencyID;		// the ID of the position statement that is immediately above the label
						// if -1 then no position statement exists behind the label
	bool defined;			// if the label was defined at all anywhere
	int dependencyCount;		// for an explicit label, the number of labels in the dependencies DArray
						// for implicit and explicit labels, -1 indicates that the value has been evaluated / solved and is valid
	struct DArray dependencies;	// for an explicit label, contains the IDs of other labels whose values are needed to evaluate this label
	char* expression;		// for explicit labels, pointer to allocated string of the expression to evaluate later
	char* name;			// pointer to allocated string of the label name
};

// returns true if the word at name contains only allowably characters for a label name
bool validLabelName(const char* name);

// adds a new label object to the global labelArray given information from line
void processImplicitLabel(const char* line);
void processExplicitLabel(const char* line);

// attempts to evaluate a label's value from its expression and update its value
// returns true if the expression was evaluated successfully or was already evaluated
bool solveLabel(struct Label* lab);

#endif
