// server.c
/*
  simple udp echo server example


  socket functions
  for UDP client/server
                                UDP server

                               +----------+
                               | socket() |
                               +----------+
                                    |
                                    V
                                +--------+
                                | bind() |
                                +--------+
                                    |
                                    V
                              +------------+
       UDP client             | recvfrom() |<-------+
                              +------------+        |
      +----------+                  |               |
      | socket() |                  V               |
      +----------+          blocks until datagram   |
           |                received from client    |
           V                        |               |
      +----------+                  |               |
  +-->| sendto() |-----______       |               |
  |   +----------+           ------>|               |
  |        |                        |               |
  |        |                        V               |
  |        |                  process request       |
  |        |                        |               |
  |        |                        V               |
  |        |                   +----------+         |
  | +------------+     ____----| sendto() |---------+
  +-| recvfrom() |<----        +----------+
    +------------+
           |
           V
      +---------+
      | close() |
      +---------+

*/

/* struct addressinfo (ai) and getaddressinfo (gai) will need _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE */

#include <stdio.h> /* readline() */
#include <stdlib.h>
#include <string.h>
#include <unistd.h> /* read(), write(), close() */
#include <netdb.h> /* freeadddrinfo(), getaddrinfo() */
#include <stdarg.h> /* va_start(), va_end(),... */
#include <sys/wait.h> /* waitpid(), SIGINT,... */
#include <sys/resource.h> /* getrusage(), struct rusage,... */
#include <time.h> /* time(), ctime() */
#include <errno.h>


/*
  constants
*/

#define MAXLINE 4096 /* max text line length */


/*
  forwards
*/

// error handling
void err_sys(const char *, ...);

// socket
void lothars__bind(int, const struct sockaddr *, socklen_t);
ssize_t lothars__recvfrom(int, void *, size_t, int, struct sockaddr *, socklen_t *);
void lothars__sendto(int, const void *, size_t, int, const struct sockaddr *, socklen_t);
int lothars__socket(int, int, int);


/*
  internal helpers
*/

/*
  print message and return to caller Caller specifies "errnoflag"

  error handling taken from "Unix Sockets" (Stevens)
*/
static void err_doit(int errnoflag, const char *fmt, va_list ap)
{
	int errno_save, n_len;
	char buf[MAXLINE + 1]; memset(buf, '\0', sizeof(buf));

	errno_save = errno; // value caller might want printed
	vsnprintf(buf, MAXLINE, fmt, ap); // safe
	n_len = strlen(buf);
	if (errnoflag) {
		snprintf(buf + n_len, MAXLINE - n_len, ": %s", strerror(errno_save));
	}

	strcat(buf, "\n");

	fflush(stdout);  // in case stdout and stderr are the same
	fputs(buf, stderr);
	fflush(stderr);

	return;
}


/*
  helpers / wrappers

  mainly to cover error handling and display error message
*/

/*
  fatal error related to system call Print message and terminate
*/
void err_sys(const char *fmt, ...)
{
	va_list  ap;
	va_start(ap, fmt);
	err_doit(1, fmt, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}


void lothars__bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
	if (0 > bind(fd, sa, salen)) {
		err_sys("bind error");
	}
}


ssize_t lothars__recvfrom(int fd
		  , void *ptr
		  , size_t nbytes
		  , int flags
		  , struct sockaddr *sa
		  , socklen_t *salenptr)
{
	ssize_t  res;
	if (0 > (res = recvfrom(fd, ptr, nbytes, flags, sa, salenptr))) {
		err_sys("recvfrom error");
	}
	return res;
}


void lothars__sendto( int fd
	      , const void *ptr
	      , size_t nbytes
	      , int flags
	      , const struct sockaddr *sa
	      , socklen_t salen)
{
	if (sendto(fd, ptr, nbytes, flags, sa, salen) != (ssize_t) nbytes) {
		err_sys("sendto error");
	}
}


int lothars__socket(int family, int type, int protocol)
{
	int res;
	if (0 > (res = socket(family, type, protocol))) {
		err_sys("socket error");
	}
	return res;
}


/********************************************************************************************/
// worker implementation



/********************************************************************************************/


/*
  main - simple udp server
*/
int main(int argc, char** argv)
{
	int fd_sock;
	struct sockaddr_in servaddr, cliaddr;
	char port[16]; memset(port, '\0', sizeof(port));

	if (2 != argc) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	strncpy(port, argv[1], sizeof(port));
	fprintf(stdout, "port: '%s'\n", port);

	// socket
	fd_sock = lothars__socket(AF_INET, SOCK_DGRAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(port));

	// bind
	lothars__bind(fd_sock, (struct sockaddr*) &servaddr, sizeof(servaddr));

	// worker
	{
		struct sockaddr *pcliaddr = (struct sockaddr*) &cliaddr;

		int n_bytes;
		socklen_t len;
		char msg[MAXLINE];

		len = sizeof(cliaddr);
		while (1) {
			// reset message
			memset(msg, '\0', sizeof(msg));

			// receive
			n_bytes = lothars__recvfrom(fd_sock, msg, MAXLINE, 0, pcliaddr, &len);
			fprintf(stdout, "%s", msg);

			// sende
			lothars__sendto(fd_sock, msg, n_bytes, 0, pcliaddr, len);
		}
	}

	fprintf(stdout, "READY.\n");
	exit(EXIT_SUCCESS);
}

