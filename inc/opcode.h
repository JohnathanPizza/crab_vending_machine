// struct for holding basic info about opcodes and some related functions

#ifndef OPCODE_H
#define OPCODE_H

#include <stdbool.h>

struct OpcodeInfo{
	const char* name;	// the opcode mnemonic
	unsigned char opcode;	// the actual opcode value
	unsigned char size;	// how many bytes the opcode is in memory
};

// returns a struct containing information about the opcode determined by the instruction at line
// the name pointer in the return value is NULL if no opcode could be matched
struct OpcodeInfo getOpcodeInfo(const char* line);

// returns true if the given opcode operates in PCR mode
bool isPCROpcode(unsigned char opcode);

#endif
