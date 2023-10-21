#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <stdbool.h>

// tries to evaluate the expression and store the result in value
// if successful, returns true and updates value, otherwise value is not changed
bool evalExpression(const char* exp, int* value);

// returns true if the character at s is a symbol used for expression logic
bool isSymbol(const char* s);

#endif
