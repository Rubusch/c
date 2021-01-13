#ifndef CONSOLE_INPUT
#define CONSOLE_INPUT


/*
  commandline input functions

  Some wrapper for convenience.

  ---
  References:
  -
*/


/* includes */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>


/* forwards */

void readdigit(unsigned int *, const char *);
int isnumber(const char *, const unsigned int);
void readnumber(unsigned int *, const unsigned int, const char *);
void readlongnumber(unsigned long int *, const unsigned int, const char *);
void readstring(char *, const unsigned int, const char *);


#endif
