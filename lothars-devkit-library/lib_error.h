#ifndef DEVKIT_LIB_ERROR
#define DEVKIT_LIB_ERROR

/*
  Error functions

  Some wrapper to read out and display errno information.

  ---
  References:
  Unix Network Programming, Stevens (1996)
*/

/* includes */

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdarg.h> /* va_list, va_start,... */
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

/* constants */

#define MAXLINE 4096 /* max text line length */

/* forwards */

void err_dump(const char *, ...);
void err_msg(const char *, ...);
void err_quit(const char *, ...);
void err_ret(const char *, ...);
void err_sys(const char *, ...);

#endif /* DEVKIT_LIB_ERROR */
