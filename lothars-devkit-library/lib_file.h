#ifndef FILEOPERATIONS
#define FILEOPERATIONS

/*
  file operations

  implementation of some common file operations

  Lothar Rubusch, 2006

  ---
  References:
  various?!

TODO needs revision, refac, and testing
TODO check references
TODO remove debugging
*/


#define _GNU_SOURCE
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>

#include "lib_error.h"


/* forwards */

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
void lothars__fputs(const char *, FILE *);
int lothars__fopen(FILE **fp, char *path, const char *mode);
int lothars__fopen_rw(FILE**, char[]);
int lothars__fopen_r(FILE **, char[]);
int lothars__fopen_w(FILE **, char[]);
int lothars__fopen_a(FILE **, char[]);
int read_char(FILE *, char **, unsigned long int *);
int read_without_spaces(FILE *, char **, unsigned long int *);
int read_linewise(FILE *, char **, unsigned long int *);
int read_blockwise(FILE *, char *, const unsigned int);
int write_char(FILE *, char *, const unsigned long int);
int write_formated(FILE *, char *); // no size due to formatating opts
int write_linewise(FILE *, char *, const unsigned long int);
// int write_blockwise(FILE*, struct Data); // de-comment when writing from a struct - DEMO
int create_file(const char *, const unsigned long int);
int remove_file(const char *);
int shred_file(const char *);
int rename_file(const char *, const char *);
int copy_characterwise_unbuffered(const char *, const char *);
int copy_characterwise_buffered(const char *, const char *, const unsigned long int);
unsigned int number_of_tempfiles();
unsigned int number_of_characters_in_static_temp();
// int create_tmp(FILE**, char[]);
// int close_tmp(FILE**);
int filesize(FILE *, unsigned long int *);
int check_eof(FILE *);
int check_error(FILE *);
int get_bufsize();
int read_without_eof(FILE *, char *, const unsigned int);
int read_nth_line(FILE *, char *, const unsigned long int, const unsigned long int);
int read_lines(FILE *, char *, const unsigned long int, const unsigned long int, const unsigned long int);
int read_lines_with_pattern(FILE *, char *, const unsigned long int, char *);
int get_more_space(char **, unsigned long int *, const unsigned long int);


#endif
