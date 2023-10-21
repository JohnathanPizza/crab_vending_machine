#include "expression.h"
#include <stddef.h>
#include <string.h>
#include "word.h"
#include "utility.h"
#include "global.h"
#include "darray.h"
#include "label.h"

enum{
	ADD = '+',
	SUB = '-',
	LS = '<',
	RS = '>',
	MUL = '*',
	DIV = '/'
} static mode;

static const char symbols[] = {
	ADD, SUB, LS, RS, MUL, DIV
};

bool evalExpression(const char* exp, int* value){
	mode = ADD; // default mode is adding
	int v = 0; // temp value

	exp = wordFind((char*)exp);
	while(exp[0] != '\0'){		// go until no more words left
		if(isSymbol(exp)){	// if it is a symbol, switch mode and go to next word
			mode = exp[0];
			++exp;
			exp = wordFind((char*)exp);
			continue;
		}

		int r; // temp to hold strtoint result
		if(!strToInt(exp, wordLen(exp), &r)){
			// maybe a label, try to find it and get its value
			bool found = false;
			for(size_t idx = 0; idx < labelCount; ++idx){
				struct Label* lp = getDArray(&labelArray, idx);
				if(wordLen(lp->name) == wordLen(exp) && strncmp(lp->name, exp, wordLen(exp)) == 0){
					if(lp->dependencyCount != -1){
						return false;
					}
					r = lp->value;
					found = true;
					break;
				}
			}
			if(found == false){
				return false;
			}
		}

		// at this point there must be some value to work with in r
		switch(mode){
		case ADD:
			v += r;
			break;
		case SUB:
			v -= r;
			break;
		case LS:
			v <<= r;
			break;
		case RS:
			v >>= r;
			break;
		case MUL:
			v *= r;
			break;
		case DIV:
			v /= r;
			break;
		}
		exp = wordNext((char*)exp);
	}
	*value = v;
	return true;
}

bool isSymbol(const char* s){
	if(wordLen(s) != 1){
		return false;
	}
	for(size_t idx = 0; idx < sizeof(symbols); ++idx){
		if(symbols[idx] == s[0]){
			return true;
		}
	}
	return false;
}
