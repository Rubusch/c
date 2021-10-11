#ifndef DEVKIT_LIB_INET
#define DEVKIT_LIB_INET

/*
  inet_ functions

  ---
  Remarks:

  1.) NO WRAPPER FOR inet_ntoa()

  char *inet_ntoa(struct in_addr in);

  The inet_ntoa() function converts the Internet host address in,
  given in network byte order, to a string in IPv4 dotted-decimal
  notation. The string is returned in a statically allocated buffer,
  which subsequent calls will overwrite.

  -> There is no wrapper, usage would be e.g. printf() call or the
     like from a defined in_addr instance. The in_addr instance is
     expected to be in condition, and in a worst case no string is
     going out which either will be noticed soon or is not
     relevant. Thus keep using inet_ntoa() as is!


  2.) AVOID USING inet_addr(), USE inet_aton()

  in_addr_t inet_addr(const char *cp);

  The inet_addr() function converts the Internet host address cp from
  IPv4 numbers-and-dots notation into binary data in network byte
  order. If the input is invalid, INADDR_NONE (usually -1) is
  returned. Use of this function is problematic because -1 is a valid
  address (255.255.255.255). Avoid its use in favor of inet_aton(),
  inet_pton(3), or getaddrinfo(3) which provide a cleaner way to
  indicate error return.


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
int lothars__inet_aton(const char *, struct in_addr *);
char *lothars__sock_ntop(const struct sockaddr *, socklen_t);
char *lothars__sock_ntop_host(const struct sockaddr *, socklen_t);

#endif /* DEVKIT_LIB_INET */
