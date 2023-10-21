#include "opcode.h"
#include <string.h>
#include "word.h"

#define OPCODE_COUNT 212

static const struct OpcodeInfo opcodeTable[OPCODE_COUNT] = {
	{"ADC.A",	0X6D, 3},
	{"ADC.X",	0X7D, 3},
	{"ADC.Y",	0X79, 3},
	{"ADC.I",	0X69, 2},
	{"ADC.Z",	0X65, 2},
	{"ADC.ZXP",	0X61, 2},
	{"ADC.ZX",	0X75, 2},
	{"ADC.ZP",	0X72, 2},
	{"ADC.ZPY",	0X71, 2},
	{"AND.A",	0X2D, 3},
	{"AND.X",	0X3D, 3},
	{"AND.Y",	0X39, 3},
	{"AND.I",	0X29, 2},
	{"AND.Z",	0X25, 2},
	{"AND.ZXP",	0X21, 2},
	{"AND.ZX",	0X35, 2},
	{"AND.ZP",	0X32, 2},
	{"AND.ZPY",	0X31, 2},
	{"ASL.A",	0X0E, 3},
	{"ASL.X",	0X1E, 3},
	{"ASL",		0X0A, 1},
	{"ASL.Z",	0X06, 2},
	{"ASL.ZX",	0X16, 2},
	{"BBR0.A",	0X0F, 2},
	{"BBR1.A",	0X1F, 2},
	{"BBR2.A",	0X2F, 2},
	{"BBR3.A",	0X3F, 2},
	{"BBR4.A",	0X4F, 2},
	{"BBR5.A",	0X5F, 2},
	{"BBR6.A",	0X6F, 2},
	{"BBR7.A",	0X7F, 2},
	{"BBS0.A",	0X8F, 2},
	{"BBS1.A",	0X9F, 2},
	{"BBS2.A",	0XAF, 2},
	{"BBS3.A",	0XBF, 2},
	{"BBS4.A",	0XCF, 2},
	{"BBS5.A",	0XDF, 2},
	{"BBS6.A",	0XEF, 2},
	{"BBS7.A",	0XFF, 2},
	{"BCC.A",	0X90, 2},
	{"BCS.A",	0XB0, 2},
	{"BEQ.A",	0XF0, 2},
	{"BIT.A",	0X2C, 3},
	{"BIT.X",	0X3C, 3},
	{"BIT.I",	0X89, 2},
	{"BIT.Z",	0X24, 2},
	{"BIT.ZX",	0X34, 2},
	{"BMI.A",	0X30, 2},
	{"BNE.A",	0XD0, 2},
	{"BPL.A",	0X10, 2},
	{"BRA.A",	0X80, 2},
	{"BRK",		0X00, 1},
	{"BVC.A",	0X50, 2},
	{"BVS.A",	0X70, 2},
	{"CLC",		0X18, 1},
	{"CLD",		0XD8, 1},
	{"CLI",		0X58, 1},
	{"CLV",		0XB8, 1},
	{"CMP.A",	0XCD, 3},
	{"CMP.X",	0XDD, 3},
	{"CMP.Y",	0XD9, 3},
	{"CMP.I",	0XC9, 2},
	{"CMP.Z",	0XC5, 2},
	{"CMP.ZXP",	0XC1, 2},
	{"CMP.ZX",	0XD5, 2},
	{"CMP.ZP",	0XD2, 2},
	{"CMP.ZPY",	0XD1, 2},
	{"CPX.A",	0XEC, 3},
	{"CPX.I",	0XE0, 2},
	{"CPX.Z",	0XE4, 2},
	{"CPY.A",	0XCC, 3},
	{"CPY.I",	0XC0, 2},
	{"CPY.Z",	0XC4, 2},
	{"DEC.A",	0XCE, 3},
	{"DEC.X",	0XDE, 3},
	{"DEC",		0X3A, 1},
	{"DEC.Z",	0XC6, 2},
	{"DEC.ZX",	0XD6, 2},
	{"DEX",		0XCA, 1},
	{"DEY",		0X88, 1},
	{"EOR.A",	0X4D, 3},
	{"EOR.X",	0X5D, 3},
	{"EOR.Y",	0X59, 3},
	{"EOR.I",	0X49, 2},
	{"EOR.Z",	0X45, 2},
	{"EOR.ZXP",	0X41, 2},
	{"EOR.ZX",	0X55, 2},
	{"EOR.ZP",	0X52, 2},
	{"EOR.ZPY",	0X51, 2},
	{"INC.A",	0XEE, 3},
	{"INC.X",	0XFE, 3},
	{"INC",		0X1A, 1},
	{"INC.Z",	0XE6, 2},
	{"INC.ZX",	0XF6, 2},
	{"INX",		0XE8, 1},
	{"INY",		0XC8, 1},
	{"JMP.A",	0X4C, 3},
	{"JMP.XP",	0X7C, 3},
	{"JMP.P",	0X6C, 3},
	{"JSR.A",	0X20, 3},
	{"LDA.A",	0XAD, 3},
	{"LDA.X",	0XBD, 3},
	{"LDA.Y",	0XB9, 3},
	{"LDA.I",	0XA9, 2},
	{"LDA.Z",	0XA5, 2},
	{"LDA.ZXP",	0XA1, 2},
	{"LDA.ZX",	0XB5, 2},
	{"LDA.ZP",	0XB2, 2},
	{"LDA.ZPY",	0XB1, 2},
	{"LDX.A",	0XAE, 3},
	{"LDX.Y",	0XBE, 3},
	{"LDX.I",	0XA2, 2},
	{"LDX.Z",	0XA6, 2},
	{"LDX.ZY",	0XB6, 2},
	{"LDY.A",	0XAC, 3},
	{"LDY.X",	0XBC, 3},
	{"LDY.I",	0XA0, 2},
	{"LDY.Z",	0XA4, 2},
	{"LDY.ZX",	0XB4, 2},
	{"LSR.A",	0X4E, 3},
	{"LSR.X",	0X5E, 3},
	{"LSR",		0X4A, 1},
	{"LSR.Z",	0X46, 2},
	{"LSR.ZX",	0X56, 2},
	{"NOP",		0XEA, 1},
	{"ORA.A",	0X0D, 3},
	{"ORA.X",	0X1D, 3},
	{"ORA.Y",	0X19, 3},
	{"ORA.I",	0X09, 2},
	{"ORA.Z",	0X05, 2},
	{"ORA.ZXP",	0X01, 2},
	{"ORA.ZX",	0X15, 2},
	{"ORA.ZP",	0X12, 2},
	{"ORA.ZPY",	0X11, 2},
	{"PHA",		0X48, 1},
	{"PHP",		0X08, 1},
	{"PHX",		0XDA, 1},
	{"PHY",		0X5A, 1},
	{"PLA",		0X68, 1},
	{"PLP",		0X28, 1},
	{"PLX",		0XFA, 1},
	{"PLY",		0X7A, 1},
	{"RMB0",	0X07, 2},
	{"RMB1",	0X17, 2},
	{"RMB2",	0X27, 2},
	{"RMB3",	0X37, 2},
	{"RMB4",	0X47, 2},
	{"RMB5",	0X57, 2},
	{"RMB6",	0X67, 2},
	{"RMB7",	0X77, 2},
	{"ROL.A",	0X2E, 3},
	{"ROL.X",	0X3E, 3},
	{"ROL",		0X2A, 1},
	{"ROL.Z",	0X26, 2},
	{"ROL.ZX",	0X36, 2},
	{"ROR.A",	0X6E, 3},
	{"ROR.X",	0X7E, 3},
	{"ROR",		0X6A, 1},
	{"ROR.Z",	0X66, 2},
	{"ROR.ZX",	0X76, 2},
	{"RTI",		0X40, 1},
	{"RTS",		0X60, 1},
	{"SBC.A",	0XED, 3},
	{"SBC.X",	0XFD, 3},
	{"SBC.Y",	0XF9, 3},
	{"SBC.I",	0XE9, 2},
	{"SBC.Z",	0XE5, 2},
	{"SBC.ZXP",	0XE1, 2},
	{"SBC.ZX",	0XF5, 2},
	{"SBC.ZP",	0XF2, 2},
	{"SBC.ZPY",	0XF1, 2},
	{"SEC",		0X38, 1},
	{"SED",		0XF8, 1},
	{"SEI",		0X78, 1},
	{"SMB0",	0X87, 2},
	{"SMB1",	0X97, 2},
	{"SMB2",	0XA7, 2},
	{"SMB3",	0XB7, 2},
	{"SMB4",	0XC7, 2},
	{"SMB5",	0XD7, 2},
	{"SMB6",	0XE7, 2},
	{"SMB7",	0XF7, 2},
	{"STA.A",	0X8D, 3},
	{"STA.X",	0X9D, 3},
	{"STA.Y",	0X99, 3},
	{"STA.Z",	0X85, 2},
	{"STA.ZXP",	0X81, 2},
	{"STA.ZX",	0X95, 2},
	{"STA.ZP",	0X92, 2},
	{"STA.ZPY",	0X91, 2},
	{"STP",		0XDB, 1},
	{"STX.A",	0X8E, 3},
	{"STX.Z",	0X86, 2},
	{"STX.ZY",	0X96, 2},
	{"STY.A",	0X8C, 3},
	{"STY.Z",	0X84, 2},
	{"STY.ZX",	0X94, 2},
	{"STZ.A",	0X9C, 3},
	{"STZ.X",	0X9E, 3},
	{"STZ.Z",	0X64, 2},
	{"STZ.ZX",	0X74, 2},
	{"TAX",		0XAA, 1},
	{"TAY",		0XA8, 1},
	{"TRB.A",	0X1C, 3},
	{"TRB.Z",	0X14, 2},
	{"TSB.A",	0X0C, 3},
	{"TSB.Z",	0X04, 2},
	{"TSX",		0XBA, 1},
	{"TXA",		0X8A, 1},
	{"TXS",		0X9A, 1},
	{"TYA",		0X98, 1},
	{"WAI",		0XCB, 1},
};

struct OpcodeInfo getOpcodeInfo(const char* line){
	bool hasValue = ((line + wordLen(line))[0] != '\0'); // if the first char of the next word is NOT nul char, then part of an expression exists
	bool hasMode = (strchr(line, '.') != NULL && strchr(line, '.') - line < wordLen(line)); // if there is a . character within the instruciton name, it has a mode
	
	// search each opcode in the table for matching names
	for(int opcodeIdx = 0; opcodeIdx < OPCODE_COUNT; ++opcodeIdx){
		const char* opname = opcodeTable[opcodeIdx].name;
		if(hasMode){
			// if it has a mode and there is an exact name match, then this must be the right opcode
			if(strlen(opname) == wordLen(line) && strncmp(opname, line, strlen(opname)) == 0){
				return opcodeTable[opcodeIdx];
			}
			continue;
		}
		// no mode
		if(strncmp(opname, line, wordLen(line)) == 0){
			// partial match for an instruction name found
			if(hasValue){
				// if the instruction has a value, then it is a shorthand absolute
				// the first entry in the table must have a name ending with .A or else no .A exists for that ins
				if(strstr(opname, ".A") != NULL){
					// absoulte mode exists
					return opcodeTable[opcodeIdx];
				}else{
					// else no mode is suitable
					return (struct OpcodeInfo){.name = NULL};
				}
			}
			if(strlen(opname) == wordLen(line)){
				// there is an exact name match without values so an implied form is found
				return opcodeTable[opcodeIdx];
			}
		}
	}
	
	// no match was ever found so return null
	return (struct OpcodeInfo){.name = NULL};
}

bool isPCROpcode(unsigned char v){
	static const unsigned char pcrInstructions[] = {
		0X0F,
		0X1F,
		0X2F,
		0X3F,
		0X4F,
		0X5F,
		0X6F,
		0X7F,
		0X8F,
		0X9F,
		0XAF,
		0XBF,
		0XCF,
		0XDF,
		0XEF,
		0XFF,
		0X90,
		0XB0,
		0XF0,
		0X30,
		0XD0,
		0X10,
		0X80,
		0X50,
		0X70,
	};

	for(size_t idx = 0; idx < sizeof(pcrInstructions); ++idx){
		if(pcrInstructions[idx] == v){
			return true;
		}
	}
	return false;
}
