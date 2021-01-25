#ifndef DEVKIT_LIB_READ_WRITE
#define DEVKIT_LIB_READ_WRITE


/*
  read/write functions

  ---
  References:
  Unix Network Programming, Stevens (1996)
*/


/* includes */

//#define _XOPEN_SOURCE 600

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> /* write(), read() */

#include <fcntl.h> /* fcntl() */
#include <sys/socket.h> /* SOL_SOCKET, sendmsg(), recvmesg(),... */

#include "lib_error.h"


/* forwards */

ssize_t lothars__read(int, void *, size_t);
ssize_t lothars__readline_fd(int, void *, size_t);
ssize_t lothars__readn(int, void *, size_t);
ssize_t lothars__read_fd(int, void*, size_t, int *);
void lothars__write(int, void *, size_t);
void lothars__writen(int, void *, size_t);
ssize_t lothars__write_fd(int, void*, size_t, int);


#endif /* DEVKIT_LIB_READ_WRITE */
