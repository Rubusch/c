// console_input.h
/*
  console input functions
//*/

#ifndef CONSOLE_INPUT
#define CONSOLE_INPUT

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

void readdigit(unsigned int*, const char*); // used!
int isnumber(const char*, const unsigned int);
void readnumber(unsigned int*, const unsigned int, const char*);
void readstring(char*, const unsigned int, const char*); // used

#endif
