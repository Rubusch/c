// console_input.h
/*
 * @author: Lothar Rubusch
 * @email: L.Rubusch@gmx.ch
 * @license: GPLv3
 *
 * console input functions
//*/

#ifndef CONSOLE_INPUT
#define CONSOLE_INPUT

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void readdigit(unsigned int *, const char *);
int isnumber(const char *, const unsigned int);
void readnumber(unsigned int *, const unsigned int, const char *);
void readlongnumber(unsigned long int *, const unsigned int, const char *);
void readstring(char *, const unsigned int, const char *);

#endif
