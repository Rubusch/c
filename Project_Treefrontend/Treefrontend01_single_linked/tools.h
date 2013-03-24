// tools.h
/*
  console input functions
//*/

#ifndef TREETOOLS
#define TREETOOLS

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// painting and string converting
#define DATA_DIGITS 3

// return the greatest element
#define MAX(x,y) ((x > y) ? (x) : (y))

// return the smallest element
#define MIN(x,y) ((x < y) ? (x) : (y))

// console io
void readdigit(unsigned int*, const char*);
int isnumber(const char*, const unsigned int);
void readnumber(unsigned int*, const unsigned int, const char*);
void readlongnumber(unsigned long int*, const unsigned int, const char*);
void readstring(char*, const unsigned int, const char*);

// file io
int get_write_file_pointer(FILE **, char[FILENAME_MAX]);
int write_linewise(FILE*, char*, const unsigned long int);
int close_stream(FILE**);

// general
int data_to_string(char*, const unsigned int, const unsigned int);

#endif
