#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include "utility.h"
#include "darray.h"
#include "label.h"
#include "word.h"
#include "position.h"
#include "opcode.h"
#include "instruction.h"
#include "error.h"
#include "expression.h"

#define LINE_LENGTH_MAX 200
#define DEFAULT_POSITION 0 // default value to use for resetting the virtual position var

// globals
struct DArray filenameArray;
struct DArray labelArray;
struct DArray positionArray;
size_t fileCount = 0;
size_t labelCount = 0;
size_t positionCount = 0;
unsigned virtualPosition = 0;
int currentFileID = 0;
int currentLine = 0;

static unsigned memoryBase = 0;
static unsigned memorySize = 0;
static unsigned char* memoryChunk = NULL;
static unsigned memoryEnd = 0;

static bool generateOutput = true;
static bool verboseEnabled = false;
static const char* outputName = "out.mb";

static void processOptions(int argCount, char* argValues[]);
static char* formatLine(char* line);
static void addToMemory(unsigned addr, unsigned char v);

int main(int argc, char* argv[]){
	// process user options
	processOptions(argc, argv);

	// read in filenames from args
	filenameArray = newDArray(0, sizeof(char*));
	for(; optind < argc; ++optind){
		addDArray(&filenameArray, &argv[optind], fileCount++);
	}

	// init vars
	labelArray = newDArray(0, sizeof(struct Label));
	positionArray = newDArray(0, sizeof(struct Position));

	memoryChunk = malloc(memorySize);
	if(memoryChunk == NULL){
		printError(ERROR_RESERVE, strerror(errno));
		exit(EXIT_FAILURE);
	}
	memoryEnd = memoryBase + memorySize;

	// preprocess all files for labels and positions

	virtualPosition = DEFAULT_POSITION; // reset position to default
	char lineBuffer[LINE_LENGTH_MAX + 1];
	int quit = 0;
	for(int fileID = 0; fileID < fileCount; ++fileID){
		currentFileID = fileID;
		currentLine = 0;

		// open file and read lines until EOF
		FILE* filePointer = fopen(getFilename(currentFileID), "r");
		if(filePointer == NULL){
			printError(ERROR_FILE_OPEN, getFilename(currentFileID), strerror(errno));
			exit(EXIT_FAILURE);
		}
		while(fgets(lineBuffer, LINE_LENGTH_MAX + 1, filePointer) == lineBuffer){
			++currentLine; // inc line
			
			// max line length check
			if(strlen(lineBuffer) > 0 && lineBuffer[strlen(lineBuffer) - 1] != '\n'){
				printErrorHeader();
				printError(ERROR_LINE_LENGTH_MAX, LINE_LENGTH_MAX);
				exit(EXIT_FAILURE);
			}

			char* start = formatLine(lineBuffer);
			if(start == NULL){
				// NULL indicates the line can be skipped
				continue;
			}
		
			// check the first character of the line to see what to do with the line
			switch(start[0]){
			case '>': // implied label line
				processImplicitLabel(wordFind(start + 1));
				break;
			case '=': // explicit label line
				processExplicitLabel(wordFind(start + 1));
				break;
			case '@': // position line
				processPosition(wordFind(start + 1));
				break;
			case '!': // byte line
				virtualPosition += 1;
				break;
			case '$': // double byte line
				virtualPosition += 2;
				break;
			case '"': // string line
				++start;
				while(*start != '\0'){
					if(*start == '\\'){
						++start;
						if(*start == '\0'){
							printErrorHeader();
							printError(ERROR_STRING_ESCAPE);
							exit(EXIT_FAILURE);
						}
						if(virtualPosition >= memoryBase && virtualPosition < memoryEnd){
							switch(*start){
							case '\\':
							case 'n':
							case '0':
								break;
							default:
								printErrorHeader();
								printError(ERROR_STRING_ESCAPE);
								exit(EXIT_FAILURE);
							}
						}
					}
					++virtualPosition;
					++start;
				}
				break;
			default:
				;
				// instruction line
				struct OpcodeInfo oi = getOpcodeInfo(start);
				if(oi.name != NULL){
					virtualPosition += oi.size;
				}else{
					printErrorHeader();
					printError(ERROR_INSTRUCTION_UNKNOWN);
					quit = 1;
				}
				break;
			}
		}
		fclose(filePointer);
	}

	if(quit != 0){
		exit(EXIT_FAILURE);
	}

	// quick search for any undefined labels
	for(size_t labelIdx = 0; labelIdx < labelCount; ++labelIdx){
		struct Label* lp = getDArray(&labelArray, labelIdx);
		if(!lp->defined){
			printError(ERROR_LABEL_UNDEFINED, wordLen(lp->name), lp->name);
			exit(1);
		}
	}

	// eval attempts now
	size_t remaining = labelCount + positionCount;
	while(1){
		size_t unsolved = 0;
		// try solving positions first
		for(size_t positionIdx = 0; positionIdx < positionCount; ++positionIdx){
			bool res = solvePosition(getDArray(&positionArray, positionIdx));
			if(!res){
				++unsolved;
			}
		}
		// then labels
		for(int labelIdx = 0; labelIdx < labelCount; ++labelIdx){
			bool res = solveLabel(getDArray(&labelArray, labelIdx));
			if(!res){
				++unsolved;
			}
		}
		if(unsolved == 0){
			break;
		}
		if(remaining == unsolved){
			printError(ERROR_EXPRESSION_DEADLOCK);
			for(size_t positionIdx = 0; positionIdx < positionCount; ++positionIdx){
				struct Position* p = getDArray(&positionArray, positionIdx);
				if(p->dependencyCount != -1){
					currentFileID = p->fileID;
					currentLine = p->lineNumber;
					printErrorHeader();
					fprintf(stderr, "%s\n", p->expression);
				}
			}
			for(int labelIdx = 0; labelIdx < labelCount; ++labelIdx){
				struct Label* l = getDArray(&labelArray, labelIdx);
				if(l->dependencyCount != -1 && l->expression != NULL){
					currentFileID = l->fileID;
					currentLine = l->lineNumber;
					printErrorHeader();
					fprintf(stderr, "%s\n", l->expression);
				}
			}
			exit(EXIT_FAILURE);
		}
		remaining = unsolved;
	}

	virtualPosition = 0;
	struct DArray instructions = newDArray(0, sizeof(struct Instruction));
	size_t instructionCount = 0;
	for(int fileID = 0; fileID < fileCount; ++fileID){
		currentFileID = fileID;
		currentLine = 0;

		// open file and read lines until EOF
		FILE* filePointer = fopen(getFilename(currentFileID), "r");
		if(filePointer == NULL){
			printError(ERROR_FILE_OPEN, getFilename(currentFileID), strerror(errno));
			exit(EXIT_FAILURE);
		}
		while(fgets(lineBuffer, LINE_LENGTH_MAX + 1, filePointer) == lineBuffer){
			++currentLine; // inc line
			char* start = formatLine(lineBuffer);
			if(start == NULL){
				continue;
			}

			int tempv;
			switch(start[0]){
			case '@':
				// if a position line is found then get its value
				for(int positionIdx = 0; positionIdx < positionCount; ++positionIdx){
					struct Position* pp = getDArray(&positionArray, positionIdx);
					if(pp->fileID == currentFileID && pp->lineNumber == currentLine){
						virtualPosition = pp->value;
						break;
					}
				}
				break;
			case '!':
				if(!evalExpression(wordFind(start + 1), &tempv)){
					printErrorHeader();
					printError(ERROR_EXPRESSION_EVALUATION);
					exit(EXIT_FAILURE);
				}
				addToMemory(virtualPosition, tempv);
				virtualPosition += 1;
				break;
			case '$':
				if(!evalExpression(wordFind(start + 1), &tempv)){
					printErrorHeader();
					printError(ERROR_EXPRESSION_EVALUATION);
					exit(EXIT_FAILURE);
				}
				addToMemory(virtualPosition, tempv);
				addToMemory(virtualPosition + 1, tempv >> 8);
				virtualPosition += 2;
				break;
			case '"':
				++start;
				while(*start != '\0'){
					if(*start == '\\'){
						++start;
						switch(*start){
						case '\\':
							addToMemory(virtualPosition, '\\');
							break;
						case 'n':
							addToMemory(virtualPosition, '\n');
							break;
						case '0':
							addToMemory(virtualPosition, '\0');
							break;
						}
					}else{
						addToMemory(virtualPosition, *start);
					}
					++virtualPosition;
					++start;
				}
				break;
			case '=':
			case '>':
				// dont need to process these lines anymore so skip them
				break;
			default:
				;
				// add the instruction to the array
				struct OpcodeInfo oi = getOpcodeInfo(start);
				if(oi.name != NULL){
					struct Instruction i;
					i.info = oi;
					i.fileID = currentFileID;
					i.lineNumber = currentLine;
					i.position = virtualPosition;
					start = wordNext(start);
					if(!evalExpression(start, &i.value)){
						printErrorHeader();
						printError(ERROR_EXPRESSION_EVALUATION);
						exit(EXIT_FAILURE);
					}
					addDArray(&instructions, &i, instructionCount++);
					virtualPosition += oi.size;
				}
				break;
			}
		}
	}
	
	for(int instructionIdx = 0; instructionIdx < instructionCount; ++instructionIdx){
		struct Instruction* ip = getDArray(&instructions, instructionIdx);
		currentFileID = ip->fileID;
		currentLine = ip->lineNumber;
		int pos = ip->position;
		addToMemory(pos, ip->info.opcode);
		if(isPCROpcode(ip->info.opcode)){
			// calculate for pcrs
			int pcr = pos + 2;
			int v = ip->value - pcr;
			if(v < -128 || v > 127){
				printErrorHeader();
				printWarning(WARNING_BRANCH_LIMIT);
			}
			addToMemory(pos + 1, v);
		}else{
			if(ip->info.size == 2){
				if((ip->value > 0xFF || ip->value < 0) && strstr(ip->info.name, ".Z") != NULL){
					printErrorHeader();
					printWarning(WARNING_INSTRUCTION_ZP_TRUNCATION);
				}
				addToMemory(pos + 1, ip->value);
			}else if(ip->info.size == 3){
				if(ip->value < 0 || ip->value > 0xFFFF){
					printErrorHeader();
					printWarning(WARNING_INSTRUCTION_ABS_TRUNCATION);
				}
				addToMemory(pos + 1, ip->value);
				addToMemory(pos + 2, ip->value >> 8);
			}
		}
	}

	if(generateOutput){
		FILE* filePointer = fopen(outputName, "w");
		if(filePointer == NULL){
			fprintf(stderr, "couldn't open output file: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		fwrite(memoryChunk, 1, memorySize, filePointer);
		fclose(filePointer);
	}
	free(memoryChunk);
	
	if(verboseEnabled == false){
		return EXIT_SUCCESS;
	}
	
	for(size_t idx = 0; idx < labelCount; ++idx){
		struct Label* lp = getDArray(&labelArray, idx);
		printf("LABEL %s @ %4d, %s %X\n", getFilename(lp->fileID), lp->lineNumber, lp->name, lp->value);
	}
	for(size_t idx = 0; idx < positionCount; ++idx){
		struct Position* pp = getDArray(&positionArray, idx);
		printf("POSITION %s @ %4d, %X\n", getFilename(pp->fileID), pp->lineNumber, pp->value);
	}
	for(size_t idx = 0; idx < instructionCount; ++idx){
		struct Instruction* ip = getDArray(&instructions, idx);
		printf("%s %4d, %-7s %02X %dB @%04X =%4X\n", getFilename(ip->fileID), ip->lineNumber, ip->info.name, ip->info.opcode, ip->info.size, ip->position, ip->value);
	}
	
	return EXIT_SUCCESS;
}

// process user options
static void processOptions(int argCount, char* argValues[]){
	optind = 1;
	opterr = 0;
	int base;
	int size;
	while(1){
		switch(getopt(argCount, argValues, ":o:b:s:vxh")){
		case 'b':
			if(!strToInt(optarg, strlen(optarg), &base)){
				fprintf(stderr, "couldn't make base from arg\n");
				exit(EXIT_FAILURE);
			}
			memoryBase = base >= 0 ? base : 0;
			break;
		case 's':
			if(!strToInt(optarg, strlen(optarg), &size)){
				fprintf(stderr, "couldn't make length from arg\n");
				exit(EXIT_FAILURE);
			}
			memorySize = size >= 0 ? size : 0;
			break;
		case 'o':
			outputName = optarg;
			break;
		case 'v':
			verboseEnabled = true;
			break;
		case 'x':
			generateOutput = false;
			break;
		case 'h':
			puts("available options:\n"
				"-h, print help menu\n"
				"-x, dont' create output file\n"
				"-v, print verbose output\n"
				"-o NAME, change output file name to NAME\n"
				"-b BASE, specify the base address of the output\n"
				"-s SIZE, specify the size of the output in bytes\n"
			);
			exit(EXIT_SUCCESS);
			break;
		case ':':
			fprintf(stderr, "%c is missing its argument\n", optopt);
			exit(EXIT_FAILURE);
		case '?':
			fprintf(stderr, "%c is not an argument\n", optopt);
			exit(EXIT_FAILURE);
		case -1:
			return;
		}
	}
}

// return null on skippable line
// make sure line ends with nul
static char* formatLine(char* line){
	// strip leading whitespace
	while(isspace(*line)){
		++line;
	}
	if(strlen(line) == 0){
		return NULL; // if the line is empty then return
	}

	// do nothing if the line is a string line
	if(line[0] == '"'){
		line[strlen(line) - 1] = '\0';
		return line;
	}
	
	if(strchr(line, ';') != NULL){
		*strchr(line, ';') = '\0'; // effectively remove the comment from the line
	}
	if(strlen(line) == 0){
		return NULL; // if the line is empty then return
	}

	// strip trailing whitespace
	char* end = strchr(line, '\0') - 1; // points to the last char in the line
	while(isspace(*end)){
		*end = '\0';
		if(end == line){
			break;
		}
		--end;
	}
	
	if(strlen(line) == 0){
		return NULL; // if the line is empty then return
	}

	// convert everything in the line to uppercase until a possible " is found
	for(int idx = 0; idx < strlen(line); ++idx){
		line[idx] = toupper(line[idx]);
	}
	return line;
}

static void addToMemory(unsigned addr, unsigned char v){
	if(addr >= memoryBase && addr < memoryEnd){
		memoryChunk[addr - memoryBase] = v;
	}
}
