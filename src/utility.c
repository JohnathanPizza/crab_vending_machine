#include "utility.h"
#include <string.h>
#include <stdio.h>

bool strToInt(const char* str, size_t len, int* value){
	int idx = 0;
	int base = 10;
	int v = 0;

	if(len == 0){
		return false;
	}
	if(len >= 2 && str[0] == '0'){
		base = 8;
		++idx;
		if(str[1] == 'B' || str[1] == 'b'){
			base = 2;
			++idx;
		}else if(str[1] == 'X' || str[1] == 'x'){
			base = 16;
			++idx;
		}
	}
	while(idx < len){
		v *= base;
		int digit;
		if(str[idx] >= '0' && str[idx] <= '9'){
			digit = str[idx] - '0';
		}else if(str[idx] >= 'A' && str[idx] <= 'F'){
			digit = 10 + str[idx] - 'A';
		}else if(str[idx] >= 'a' && str[idx] <= 'f'){
			digit = 10 + str[idx] - 'a';
		}else{
			return false;
		}
		if(digit >= base){
			return false;
		}
		v += digit;
		++idx;
	}
	if(value != NULL){
		*value = v;
	}
	return true;
}
