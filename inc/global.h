// contains symbols for all used global variables
// all global symbols are defined in main

#ifndef GLOBAL_H
#define GLOBAL_H

struct DArray;

extern struct DArray filenameArray;
extern struct DArray labelArray;
extern struct DArray positionArray;
extern size_t fileCount;
extern size_t labelCount;
extern size_t positionCount;
extern unsigned virtualPosition;
extern int currentFileID;
extern int currentLine;

#endif
