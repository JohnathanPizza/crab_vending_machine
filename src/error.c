#include "error.h"
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include "global.h"
#include "utility.h"

static const char* const errorStrings[] = {
	[ERROR_FILE_OPEN] = "couldn't open file \"%s\": %s\n", // filename, strerror
	[ERROR_LINE_LENGTH_MAX] = "line exceeds maximum set line length (max = %d including newline)\n", // line max length
	[ERROR_INSTRUCTION_UNKNOWN] = "unknown instruction\n",
	[ERROR_EXPRESSION_DEADLOCK] = "unsolvable state exists in label or position expressions\n",
	[ERROR_EXPRESSION_EVALUATION] = "expression evaluation failure\n",
	[WARNING_BRANCH_LIMIT] = "branch instruction exceeds range limit\n",
	[WARNING_INSTRUCTION_ZP_TRUNCATION] = "zero page value truncated\n",
	[WARNING_INSTRUCTION_ABS_TRUNCATION] = "absolute value truncated\n",
	[ERROR_LABEL_NAME] = "\"%.*s\" is not a valid label name\n", // label name
	[ERROR_RESERVE] = "error reserving space for malloc: %s\n", // strerror
	[ERROR_LABEL_DUPLICATE] = "label \"%s\" already defined (found also in %s @ %d)\n", // label name, alterante file name, alternate file line
	[ERROR_STRING_ESCAPE] = "string escape sequence not valid\n",
	[WARNING_POSITION_NEGATIVE] = "position value is negative\n",
};

void printErrorHeader(void){
	fprintf(stderr, "%s @ %d: ", getFilename(currentFileID), currentLine);
}

void printError(int code, ...){
	va_list list;
	va_start(list, code);
	vfprintf(stderr, errorStrings[code], list);
	va_end(list);
}

void printWarning(int code, ...){
	fprintf(stderr, "WARNING, ");
	va_list list;
	va_start(list, code);
	vfprintf(stderr, errorStrings[code], list);
	va_end(list);
}
