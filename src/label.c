#include "label.h"
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "word.h"
#include "error.h"
#include "global.h"
#include "utility.h"
#include "position.h"
#include "expression.h"

bool validLabelName(const char* name){
	// valid chars are A-Z, 0-9, _
	// cant start with number
	if(wordLen(name) == 0 || isdigit(name[0])){
		return false;
	}
	for(int idx = 0; idx < wordLen(name); ++idx){
		if(!isalnum(name[idx]) && name[idx] != '_'){
			return false;
		}
	}
	return true;
}

void processImplicitLabel(const char* line){
	if(!validLabelName(line)){
		printErrorHeader();
		printError(ERROR_LABEL_NAME, wordLen(line), line);
		exit(1);
	}

	// create a new label to go into the array
	struct Label label;
	label.fileID = currentFileID;
	label.lineNumber = currentLine;
	label.value = virtualPosition;
	label.posDependencyID = positionCount - 1;
	label.defined = true;
	label.dependencyCount = 0;
	label.expression = NULL;
	label.name = malloc(wordLen(line) + 1);
	strncpy(label.name, line, wordLen(line));
	label.name[wordLen(line)] = '\0';

	// try to find an existing label with the same name and replace it
	for(size_t idx = 0; idx < labelCount; ++idx){
		struct Label* lp = getDArray(&labelArray, idx);
		if(strlen(label.name) == strlen(lp->name) && strcmp(label.name, lp->name) == 0){
			// if the label name was found but the label is already defined, print error and exit
			if(lp->defined){
				printErrorHeader();
				printError(ERROR_LABEL_DUPLICATE, lp->name, getFilename(lp->fileID), lp->lineNumber);
				exit(1);
			}
			free(lp->name);
			free(lp->expression);
			*lp = label;
			return;
		}
	}
	
	// otherwise add the label to the array
	addDArray(&labelArray, &label, labelCount++);
}

void processExplicitLabel(const char* line){
	if(!validLabelName(line)){
		printErrorHeader();
		printError(ERROR_LABEL_NAME, wordLen(line), line);
		exit(1);
	}

	// create a new label to go into the array
	struct Label label;
	label.fileID = currentFileID;
	label.lineNumber = currentLine;
	label.value = 0;
	label.posDependencyID = -1;
	label.defined = true;
	label.dependencyCount = 0;
	label.name = malloc(wordLen(line) + 1);
	strncpy(label.name, line, wordLen(line));
	label.name[wordLen(line)] = '\0';
	line = wordNext((char*)line);
	label.expression = malloc(strlen(line) + 1);
	strcpy(label.expression, line);
	label.expression[strlen(line)] = '\0';
	label.dependencies = newDArray(0, sizeof(size_t));

	while(line[0] != '\0'){
		// if the word is not a symbol and not a number, then it is a label
		// add it to the dependencies of this label
		if(!isSymbol(line) && !strToInt(line, wordLen((char*)line), NULL)){
			bool wasFound = false;
			for(size_t idx = 0; idx < labelCount; ++idx){
				struct Label* lp = getDArray(&labelArray, idx);
				if(wordLen(line) == strlen(lp->name) && strcmp(line, lp->name) == 0){
					// names are exactly equal, label already exists in array
					// add it as a dependency
					addDArray(&label.dependencies, &idx, label.dependencyCount++);
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
				addDArray(&label.dependencies, &idx, label.dependencyCount++);
			}
		}
		line = wordNext((char*)line);
	}

	// try to find an existing label with the same name and replace it
	for(size_t idx = 0; idx < labelCount; ++idx){
		struct Label* lp = getDArray(&labelArray, idx);
		if(strlen(label.name) == strlen(lp->name) && strcmp(label.name, lp->name) == 0){
			// if the label name was found but the label is already defined, print error and exit
			if(lp->defined){
				printErrorHeader();
				printError(ERROR_LABEL_DUPLICATE, lp->name, getFilename(lp->fileID), lp->lineNumber);
				exit(1);
			}
			free(lp->name);
			free(lp->expression);
			*lp = label;
			return;
		}
	}
	
	// otherwise add the label to the array
	addDArray(&labelArray, &label, labelCount++);
}

bool solveLabel(struct Label* lab){
	currentFileID = lab->fileID;
	currentLine = lab->lineNumber;

	if(lab->dependencyCount == -1){
		return true;
	}
	if(lab->expression == NULL){
		// implicit label
		if(lab->posDependencyID == -1){
			lab->dependencyCount = -1;
			return true;
		}
		struct Position* dp = getDArray(&positionArray, lab->posDependencyID);
		if(dp->dependencyCount != -1){
			return false;
		}
		lab->value += dp->value;
		lab->dependencyCount = -1;
	}else{
		// explicit label
		for(int dependencyIdx = 0; dependencyIdx < lab->dependencyCount; ++dependencyIdx){
			size_t depID = *((size_t*)getDArray(&lab->dependencies, dependencyIdx));
			struct Label* dp = getDArray(&labelArray, depID);
			if(dp->dependencyCount != -1){
				return false;
			}
		}
		if(!evalExpression(lab->expression, &lab->value)){
			printErrorHeader();
			printError(ERROR_EXPRESSION_EVALUATION);
			exit(1);
		}
		lab->dependencyCount = -1;
	}
	return true;
}
