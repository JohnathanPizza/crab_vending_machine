#include "position.h"
#include <stdlib.h>
#include <string.h>
#include "label.h"
#include "global.h"
#include "error.h"
#include "word.h"
#include "expression.h"
#include "utility.h"

void processPosition(const char* line){
	virtualPosition = 0; // reset at each position line during preprocessing for relative offsets for labels

	// create a new position to go into the array
	struct Position position;
	position.fileID = currentFileID;
	position.lineNumber = currentLine;
	position.dependencyCount = 0;
	position.expression = malloc(strlen(line) + 1);
	strcpy(position.expression, line);
	position.expression[strlen(line)] = '\0';
	position.dependencies = newDArray(0, sizeof(size_t));

	while(line[0] != '\0'){
		// if the word is not a symbol and not a number, then it is a label
		// add it to the dependencies of this position
		if(!isSymbol(line) && !strToInt(line, wordLen(line), NULL)){
			bool wasFound = false;
			for(size_t idx = 0; idx < labelCount; ++idx){
				struct Label* lp = getDArray(&labelArray, idx);
				if(wordLen(line) == strlen(lp->name) && strcmp(line, lp->name) == 0){
					// names are exactly equal, label already exists in array
					// add it as a dependency
					addDArray(&position.dependencies, &idx, position.dependencyCount++);
					wasFound = true;
					break;
				}
			}
			// if no label with the name was found, add it as undefined
			if(!wasFound){
				if(!validLabelName(line)){
					printErrorHeader();
					printError(ERROR_LABEL_NAME, wordLen(line), line);
					exit(1);
				}
				struct Label l;
				l.defined = false;
				l.name = malloc(wordLen(line) + 1);
				strncpy(l.name, line, wordLen(line));
				l.name[wordLen(line)] = '\0';
				l.expression = NULL;
				addDArray(&labelArray, &l, labelCount);
				size_t idx = labelCount++;
				addDArray(&position.dependencies, &idx, position.dependencyCount++);
			}
		}
		line = wordNext((char*)line);
	}
	
	// otherwise add the label to the array
	addDArray(&positionArray, &position, positionCount++);
}

bool solvePosition(struct Position* pos){
	currentFileID = pos->fileID;
	currentLine = pos->lineNumber;
	if(pos->dependencyCount == -1){
		return true;
	}
	// for each position, if it has deps, check each and if each is found then compute the text expression
	for(int dependencyIdx = 0; dependencyIdx < pos->dependencyCount; ++dependencyIdx){
		size_t depID = *((size_t*)getDArray(&pos->dependencies, dependencyIdx));
		struct Label* lp = getDArray(&labelArray, depID);
		if(lp->dependencyCount != -1){
			return false;
		}
	}
	if(!evalExpression(pos->expression, &pos->value)){
		printErrorHeader();
		printError(ERROR_EXPRESSION_EVALUATION);
		exit(1);
	}
	if(pos->value < 0){
		printErrorHeader();
		printError(WARNING_POSITION_NEGATIVE);
	}
	pos->dependencyCount = -1;
	return true;
}
