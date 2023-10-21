// definition for struct Instruction

#ifndef INSTRUCTION_H
#define INSTRUCTION_H

struct Instruction{
	struct OpcodeInfo info;
	int fileID;
	int lineNumber;
	int value;
	unsigned position;
};

#endif
