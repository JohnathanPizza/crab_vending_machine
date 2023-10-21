#include <stdbool.h>
#include <stddef.h>
#include "darray.h"

#define getFilename(idx) *((char**)getDArray(&filenameArray, idx))

bool strToInt(const char* str, size_t len, int* value);
