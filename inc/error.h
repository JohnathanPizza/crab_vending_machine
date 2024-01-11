// defines error and warning codes to be used with the printError and printWarning functions in main code

#ifndef ERROR_H
#define ERROR_H

#define ERROR_FILE_OPEN 0
#define ERROR_LINE_LENGTH_MAX 1
#define ERROR_INSTRUCTION_UNKNOWN 2
#define ERROR_EXPRESSION_DEADLOCK 3
#define ERROR_EXPRESSION_EVALUATION 4
#define WARNING_BRANCH_LIMIT 5
#define WARNING_INSTRUCTION_ZP_TRUNCATION 6
#define WARNING_INSTRUCTION_ABS_TRUNCATION 7
#define ERROR_LABEL_NAME 8
#define ERROR_RESERVE 9
#define ERROR_LABEL_DUPLICATE 10
#define ERROR_STRING_ESCAPE 11
#define WARNING_POSITION_NEGATIVE 12
#define ERROR_LABEL_UNDEFINED 13

// prints the current file name and line to stderr
void printErrorHeader(void);

// prints the message related to the code with any format arguments to stderr
void printError(int code, ...);

// prints the warning related to the code with any format arguments to stderr, prepends "WARNING, " to the message
void printWarning(int code, ...);

#endif
