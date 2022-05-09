#ifndef DEVKIT_LIB_INET
#define DEVKIT_LIB_INET

/*
  inet_ functions

  ---
  References:
  Unix Network Programming, Stevens (1996)
  Developer Manpages
*/

/* includes */

//#define _XOPEN_SOURCE 600

#include <stdlib.h>
#include <stdio.h>

#include <fcntl.h> /* fcntl(), F_GETFL */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h> /* struct sockaddr_un */

#include "lib_error.h"

/* forwards */

const char *lothars__inet_ntop(int, const void *, char *, size_t);
void lothars__inet_pton(int, const char *, void *);
//int lothars__inet_aton(const char *, struct in_addr *); // DEPRECATED replace by inet_pton()
char *lothars__sock_ntop(const struct sockaddr *, socklen_t);
char *lothars__sock_ntop_host(const struct sockaddr *, socklen_t);

#endif /* DEVKIT_LIB_INET */
