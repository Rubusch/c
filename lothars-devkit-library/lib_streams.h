#ifndef DEVKIT_LIB_STREAMS
#define DEVKIT_LIB_STREAMS


/* includes */

//#define _XOPEN_SOURCE 600

#include <stdlib.h>
#include <stdio.h>

#include <fcntl.h> /* fcntl(), F_GETFL */

#include "lib_error.h"


// forwards

void lothars__fclose(FILE *);
FILE* lothars__fdopen(int, const char *);
char* lothars__fgets(char *, int, FILE *);
FILE* lothars__fopen(const char *, const char *);
void lothars__fputs(const char *, FILE *);


#endif /* DEVKIT_LIB_STREAMS */
