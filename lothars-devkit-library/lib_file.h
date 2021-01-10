#ifndef FILEOPERATIONS
#define FILEOPERATIONS

#ifdef __unix__

#define _GNU_SOURCE
#include <stddef.h>

#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// FILE pointer to a stream
FILE* lothars__fopen(FILE **fp, char *path, const char *mode)
//int get_read_write_file_pointer(FILE **, char[]); // TODO rm
int lothars__fopen_rw(FILE**, char[]);
//int get_read_file_pointer(FILE **, char[]); // TODO rm
int lothars__fopen_r(FILE **, char[]);
//int get_write_file_pointer(FILE **, char[]); // TODO rm
int lothars__fopen_w(FILE **, char[]);
//int get_append_file_pointer(FILE **, char[]); // TODO rm
int lothars__fopen_a(File **, char[]);

// close a FILE pointer to a stream
//int close_stream(FILE **); // TODO rm
int lothars__fclose_null(FILE **);

// reading from a file stream
int read_char(FILE *, char **, unsigned long int *);
int read_without_spaces(FILE *, char **, unsigned long int *);
int read_linewise(FILE *, char **, unsigned long int *);
int read_blockwise(FILE *, char *, const unsigned int);

// write to a file stream
int write_char(FILE *, char *, const unsigned long int);
int write_formated(FILE *, char *); // no size due to formatating opts
int write_linewise(FILE *, char *, const unsigned long int);
// int write_blockwise(FILE*, struct Data); // de-comment when writing from a
// struct - DEMO

// file options
int create_file(const char *, const unsigned long int);
int remove_file(const char *);
int shred_file(const char *);
int rename_file(const char *, const char *);

// copying
int copy_characterwise_unbuffered(const char *, const char *);
int copy_characterwise_buffered(const char *, const char *,
                                const unsigned long int);

// temporary files
unsigned int number_of_tempfiles();
unsigned int number_of_characters_in_static_temp();
// int create_tmp(FILE**, char[]);
// int close_tmp(FILE**);

// checking files
int filesize(FILE *, unsigned long int *);
int check_eof(FILE *);
int check_error(FILE *);
int get_bufsize();

// reading out of a file stream
int read_without_eof(FILE *, char *, const unsigned int);
int read_nth_line(FILE *, char *, const unsigned long int,
                  const unsigned long int);
int read_lines(FILE *, char *, const unsigned long int, const unsigned long int,
               const unsigned long int);
int read_lines_with_pattern(FILE *, char *, const unsigned long int, char *);

// reallocation
int get_more_space(char **, unsigned long int *, const unsigned long int);

#endif
