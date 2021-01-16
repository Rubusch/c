#ifndef DEVKIT_LIB_STREAMS
#define DEVKIT_LIB_STREAMS


/*
  Stream functions

  Some wrapper for convenience.

  ---
  References:
  Unix Network Programming, Stevens (1996)
*/


/* includes */

//#define _XOPEN_SOURCE 600

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> /* sync() */

#include <fcntl.h> /* fcntl(), F_GETFL */

#include "lib_error.h"


/* forwards */

void lothars__fclose(FILE **);
FILE* lothars__fdopen(int, const char *);
char* lothars__fgets(char *, int, FILE *);
FILE* lothars__fopen(const char *, const char *);
void lothars__fputs(const char *, FILE *);


#endif /* DEVKIT_LIB_STREAMS */
