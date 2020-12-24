// client.c
/*
  tcp server types - TCP testing client for all server types
*/

/* struct addressinfo (ai) and getaddressinfo (gai) will need _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h> /* wait() */
#include <unistd.h> /* read(), write(), fork(), close() */
#include <netdb.h> /* socket(), SOCK_STREAM, AF_UNSPEC, getaddrinfo() */
#include <stdarg.h> /* va_start(), va_end(),... */
#include <errno.h>


/*
  constants
*/

#define MAXLINE  4096 /* max text line length */
#define BUFFSIZE 8192 /* buffer size for reads and writes */
#define MAXN 1234 /* max number of bytes to request from server */
#define NCHILDREN 7 /* number of children */
#define NLOOPS 20 /* number of sockets to open, write, read and close again */
#define NBYTES 24 /* number of max bytes to read,
		     NB: if this is greater than 24
		     readn will always fail consequence is a
		     "Connection reset by peer" error message */


/*
  forwards
*/

pid_t lothars__fork();
int lothars__tcp_connect(const char*, const char*);
ssize_t lothars__readn(int, void *, size_t);
void lothars__write(int, void *, size_t);
void lothars__close(int);


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
	vsnprintf(buf, MAXLINE, fmt, ap);
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
  read "num" bytes from a descriptor
*/
ssize_t readn(int fd, void *vptr, size_t num)
{
	size_t nleft;
	ssize_t nread;
	char *ptr=NULL;

	ptr = vptr;
	nleft = num;
	while (nleft > 0) {
		if (0 > (nread = read(fd, ptr, nleft))) {
			if (errno == EINTR) {
				nread = 0;  // and call read() again
			} else {
				return -1;
			}
		}else if (nread == 0) {
			break; // EOF
		}
		nleft -= nread;
		ptr   += nread;
	}
	return (num - nleft);  // return >= 0
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

/*
  fatal error unrelated to system call Print message and terminate
*/
void err_quit(const char *fmt, ...)
{
	va_list  ap;
	va_start(ap, fmt);
	err_doit(0, fmt, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}


pid_t lothars__fork(void)
{
	pid_t pid;
	if (-1 == (pid = fork())) {
		err_sys("fork error");
	}
	return pid;
}


int lothars__tcp_connect(const char *host, const char *serv)
{
	int sockfd, eai;
	struct addrinfo hints, *res, *ressave;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if (0 != (eai = getaddrinfo(host, serv, &hints, &res))) {
		err_quit("tcp_connect error for %s, %s: %s", host, serv, gai_strerror(eai));
	}

	ressave = res;

	do {
		if (0 > (sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol))) {
			continue; // ignore this one
		}

		if (0 == connect(sockfd, res->ai_addr, res->ai_addrlen)) {
			break;  // success
		}

		lothars__close(sockfd); // ignore this one
	} while (NULL != (res = res->ai_next));

	if (NULL == res) { // errno set from final connect()
		err_sys("tcp_connect error for %s, %s", host, serv);
	}

	freeaddrinfo(ressave);

	return sockfd;
}


ssize_t lothars__readn(int fd, void *ptr, size_t nbytes)
{
	ssize_t  res;
	if (0 > (res = readn(fd, ptr, nbytes))) {
		err_sys("readn error");
	}
	return res;
}


void lothars__write(int fd, void *ptr, size_t nbytes)
{
	if (nbytes != write(fd, ptr, nbytes)) {
		err_sys("write error");
	}
}


void lothars__close(int fd)
{
	if (-1 == close(fd)) {
		err_sys("close error");
	}
}


/*
  main()

  starts a bunch of connections to a server (provided ip and port),
  sends some bytes and waits on answer from the server

  a tcp server is expected, serverip and port needs to be provided
*/
int main(int argc, char** argv)
{
	int idx
		, jdx
		, fd_connect
		, n_children
		, n_loops
		, n_bytes;

	pid_t pid;
	ssize_t bytes;
	char request[MAXLINE], reply[MAXN];
	char serverip[16]; memset(serverip, '\0', sizeof(serverip));
	char port[16]; memset(port, '\0', sizeof(port));

	if (3 != argc) {
		fprintf(stderr, "usage: %s <serverip> <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	strncpy(serverip, argv[1], sizeof(serverip));
	fprintf(stdout, "serverip: '%s'\n", serverip);

	strncpy(port, argv[2], sizeof(port));
	fprintf(stdout, "port: '%s'\n", port);

	// number of children
	n_children = NCHILDREN;

	// number of sockets to open, write, read and close again
	n_loops = NLOOPS;

	// number of max bytes to read
	n_bytes = NBYTES;

	// init request to send to server - ends with \n
	snprintf(request, sizeof(request), "%d\n", n_bytes);

	// per child...
	for (idx=0; idx<n_children; ++idx) {

		// if is not parent!
		if (0 == (pid = lothars__fork())) {

			// per loop...
			for (jdx=0; jdx<n_loops; ++jdx) {

				// connect to serverip and port
				fd_connect = lothars__tcp_connect(serverip, port);

				// write to socket
				lothars__write(fd_connect, request, strlen(request));

				// read response
				if (n_bytes != (bytes = lothars__readn(fd_connect, reply, n_bytes))) {
					err_quit("server returned %d bytes", bytes);
				}

				// close socket
				lothars__close(fd_connect);
			}
			fprintf(stdout, "child %d: received '%s'\n", idx, reply);
			fprintf(stdout, "child %d: READY.\n", idx);
			exit(EXIT_FAILURE);
		}

	} // parent loops around to fork() again

	while (0 < wait(NULL))
		;

	if (errno != ECHILD) {
		err_sys("wait error");
	}

	exit(EXIT_SUCCESS);
}
