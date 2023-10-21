#include "word.h"
#include <ctype.h>

size_t wordLen(const char* s){
	size_t len = 0;
	while(*s != '\0' && !isspace(*s)){
		++len;
		++s;
	}
	return len;
}

char* wordNext(char* s){
	s += wordLen(s);
	while(isspace(*s)){
		++s;
	}
	return s;
}

char* wordFind(char* s){
	while(isspace(*s)){
		++s;
	}
	return s;
}
