// server.c
/*
  TCP iteractive server (no process control, faster is impossible!)

  An iterative TCP server proceses each client's request completely
  before moving on to the next client. Iterative TCP serers are rare!


  the listen queue

  NB: stevens also provides the alternative to initialize LISTENQ by env
  variable inside listen() wrapper

  NB: LISTENQ could be derived from SOMAXCONN in <sys/socket.h>, but
  many kernels still #define it as 5, while actually supporting many
  more (stevens)
 */

/* struct addressinfo (ai) and getaddressinfo (gai) will need _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> /* read(), write(), close() */
#include <arpa/inet.h> /* htons(), htonl(), accept(), socket(),... */
#include <stdarg.h> /* va_start(), va_end(),... */
#include <time.h> /* time(), ctime() */
#include <errno.h>


/*
  constants
*/

#define MAXLINE  4096 /* max text line length */
#define BUFFSIZE 8192 /* buffer size for reads and writes */
#define LISTENQ 1024 /* the listen queue - serving as backlog for listen */


/*
  forwards
*/

int lothars__socket(int, int, int);
void lothars__bind(int, const struct sockaddr *, socklen_t);
void lothars__listen(int, int);
int lothars__accept(int, struct sockaddr *, socklen_t *);
void lothars__write(int, void *, size_t);
void lothars__close(int *);

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
	if(errnoflag){
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


void lothars__listen(int fd, int backlog)
{
	if (0 > listen(fd, backlog)) {
		err_sys("listen error");
	}
}


int lothars__accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
	int res;
again:
	if (0 > (res = accept(fd, sa, salenptr))) {
		if ((errno == EPROTO) || (errno == ECONNABORTED)) {
			goto again;
		} else {
			err_sys("accept error");
		}
	}
	return res;
}


int lothars__socket(int family, int type, int protocol)
{
	int res;
	if (0 > (res = socket(family, type, protocol))) {
		err_sys("socket error");
	}
	return res;
}


void lothars__write(int fd, void *ptr, size_t nbytes)
{
	if (nbytes != write(fd, ptr, nbytes)) {
		err_sys("write error");
	}
}


/*
  The close() function shall deallocate the file descriptor indicated
  by fd. To deallocate means to make the file descriptor available for
  return by subsequent calls to open() or other functions that
  allocate file descriptors. All outstanding record locks owned by the
  process on the file associated with the file descriptor shall be
  removed (that is, unlocked).

  If close() is interrupted by a signal that is to be caught, it shall
  return -1 with errno set to [EINTR] and the state of fildes is
  unspecified. If an I/O error occurred while reading from or writing
  to the file system during close(), it may return -1 with errno set
  to [EIO]; if this error is returned, the state of fildes is
  unspecified.

  This wrapper sets the fp to NULL;

  #include <unistd.h>

  @fd: Points to the file descriptor to the specific connection.
*/
void lothars__close(int *fd)
{
	if (NULL == fd) {
		fprintf(stderr, "%s() fd was NULL\n", __func__);
		return;
	}
	if (-1 == close(*fd)) {
		err_sys("%s() error", __func__);
	}
	*fd = 0;
#if _XOPEN_SOURCE >= 500
	sync();
#endif /* _XOPEN_SOURCE */
}


/*
  main

  the server answers an incoming connection with a timestring,
  listening on any address

  the iterative tcp server implementation is a server w/o fork(),
  it has the fastest response times, but is very limited
*/
int main(int argc, char** argv)
{
	int fd_listen, fd_conn;
	struct sockaddr_in servaddr;
	char buf[MAXLINE]; memset(buf, '\0', sizeof(buf));
	int port;
	time_t ticks;

	if (2 != argc) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	port = atoi(argv[1]);

	// set up listen socket
	fd_listen = lothars__socket(AF_INET, SOCK_STREAM, 0);

	// set up server address
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port); // connect to provided port

	// bind "listen socket" to the "server address"
	lothars__bind(fd_listen, (struct sockaddr*) &servaddr, sizeof(servaddr));

	// listen on socket (queue length == LISTENQ == 1024)
	lothars__listen(fd_listen, LISTENQ);

	// server loop
	while (1) {
		// accept (blocking!) - ...listen socket to connection socket
		fd_conn = lothars__accept(fd_listen, (struct sockaddr*) NULL, NULL);

		// example action: send a string containing the system time
		ticks = time(NULL);
		snprintf(buf, sizeof(buf), "%.24s", ctime(&ticks));
		fprintf(stdout, "sending \"%s\"\n", buf);
		lothars__write(fd_conn, buf, strlen(buf));

		// close connection after sending
		lothars__close(&fd_conn);
		fprintf(stdout, "READY.\n");
	}

	// never reaches here
	exit(EXIT_SUCCESS);
}
