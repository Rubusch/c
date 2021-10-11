#ifndef DEVKIT_LIB_SOCKET
#define DEVKIT_LIB_SOCKET

/*
  Socket functions

  Some wrapper for convenience.

  ---
  References:
  Unix Network Programming, Stevens (1996)
*/

/* includes */

//#define _XOPEN_SOURCE 600

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h> /* fcntl(), F_GETFL */

#include <sys/poll.h> /* struct pollfd */
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h> /* select(), FD_SET,... */
#include <netdb.h> /* struct sockaddr */
#include <sys/un.h> /* struct sockaddr_un */

#include "lib_error.h"

/* constants */

#define LISTENQ                                                                \
	1024 /* the listen queue - serving as backlog for listen, can also be provided as env var */

/* forwards */

int lothars__accept(int, struct sockaddr *, socklen_t *);
void lothars__bind(int, const struct sockaddr *, socklen_t);
void lothars__connect(int, const struct sockaddr *, socklen_t);
void lothars__getpeername(int, struct sockaddr *, socklen_t *);
void lothars__getsockname(int, struct sockaddr *, socklen_t *);
void lothars__getsockopt(int, int, int, void *, socklen_t *);
void lothars__listen(int, int);
int lothars__poll(struct pollfd *, unsigned long, int);
ssize_t lothars__recv(int, void *, size_t, int);
ssize_t lothars__recvfrom(int, void *, size_t, int, struct sockaddr *,
			  socklen_t *);
ssize_t lothars__recvmsg(int, struct msghdr *, int);
int lothars__select(int, fd_set *, fd_set *, fd_set *, struct timeval *);
void lothars__send(int, const void *, size_t, int);
void lothars__sendto(int, const void *, size_t, int, const struct sockaddr *,
		     socklen_t);
void lothars__sendmsg(int, const struct msghdr *, int);
void lothars__setsockopt(int, int, int, const void *, socklen_t);
void lothars__shutdown(int, int);
int lothars__sockatmark(int);
int lothars__socket(int, int, int);
void lothars__socketpair(int, int, int, int *);
struct addrinfo *lothars__host_serv(const char *, const char *, int, int);

#endif /* DEVKIT_LIB_SOCKET */
