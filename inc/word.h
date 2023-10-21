// utility functions for navigating strings
// a word is some string of characters delimited by any whitespace including \0

#ifndef WORD_H
#define WORD_H

#include <stddef.h>

// returns the length of the word starting at s
size_t wordLen(const char* s);

// returns a pointer to the start of the next word
// if s is already in a word (*s is not whitespace), go out of it and into the next one
char* wordNext(char* s);

// returns a pointer to the nearest character that is part of a word
// if s is already in a word, s is returned
char* wordFind(char* s);

#endif
