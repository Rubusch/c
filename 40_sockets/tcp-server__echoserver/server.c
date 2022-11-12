/*
  tcp echo server

  implements a simple tcp echo server as forked concurent server (one
  child per client)

  REFERENCES: Socket API, R. Stevens
*/

#define _GNU_SOURCE /* sync() */

#include <stdio.h>
#include <stdlib.h>
#include <netinet/ip.h> /* PF_INET, IPPROTO_TCP,... */
#include <string.h> /* strerror(), memset(),... */
#include <stdarg.h> /* va_start(), va_end(),... */
#include <unistd.h> /* fork(), close(),... */
#include <errno.h>

/*
  constants
*/

#define MAXSIZE 4096
#define MAXLINE 4096 /* max text line length */

/*
  forwards
*/

// error handling
void err_sys(const char *, ...);

// sock
int lothars__accept(int, struct sockaddr *, socklen_t *);
void lothars__bind(int, const struct sockaddr *, socklen_t);
void lothars__listen(int, int);
ssize_t lothars__recv(int, void *, size_t, int);
void lothars__send(int, const void *, size_t, int);
int lothars__socket(int, int, int);

// unix
void lothars__close(int *);
pid_t lothars__fork();

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
	char buf[MAXLINE + 1];
	memset(buf, '\0', sizeof(buf));

	errno_save = errno; // value caller might want printed
	vsnprintf(buf, MAXLINE, fmt, ap); // safe
	n_len = strlen(buf);
	if (errnoflag) {
		snprintf(buf + n_len, MAXLINE - n_len, ": %s",
			 strerror(errno_save));
	}

	strcat(buf, "\n");

	fflush(stdout); // in case stdout and stderr are the same
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
	va_list ap;
	va_start(ap, fmt);
	err_doit(1, fmt, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}

/*
  The accept() function shall extract the first connection on the
  queue of pending connections, create a new socket with the same
  socket type protocol and address family as the specified socket, and
  allocate a new file descriptor for that socket.

  @param: fd - Specifies a socket that was created with socket(), has
      been bound to an address with bind(), and has issued a
      successful call to listen().

  @param: sa - Either a null pointer, or a pointer to a sockaddr
      structure where the address of the connecting socket shall be
      returned.

  @param: salenptr - Points to a socklen_t structure which on input
      specifies the length of the supplied sockaddr structure, and on
      output specifies the length of the stored address.

*/
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

/*
  The bind() function shall assign a local socket address address to a
  socket identified by descriptor socket that has no local socket
  address assigned. Sockets created with the socket() function are
  initially unnamed; they are identified only by their address family.

  @fd: Specifies the file descriptor of the socket to be bound.

  @sa: Points to a sockaddr structure containing the address to be
      bound to the socket. The length and format of the address depend
      on the address family of the socket.

  @salen: Specifies the length of the sockaddr structure pointed to by
      the address argument.
*/
void lothars__bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
	if (0 > bind(fd, sa, salen)) {
		err_sys("bind error");
	}
}

/*
  The listen() function shall mark a connection-mode socket, specified
  by the socket argument, as accepting connections.

  @fd: Specifies the file descriptor associated with the socket.

  @backlog: The backlog argument provides a hint to the implementation
      which the implementation shall use to limit the number of
      outstanding connections in the socket's listen queue.
*/
void lothars__listen(int fd, int backlog)
{
	char *ptr = NULL;
	// can override 2nd argument with environment variable
	if (NULL != (ptr = getenv("LISTENQ"))) {
		backlog = atoi(ptr);
	}

	if (0 > listen(fd, backlog)) {
		err_sys("listen error");
	}
}

/*
  The recv() function shall receive data from a connection-mode or
  connectionless-mode socket. It is normally used with connected
  sockets because it does not permit the application to retrieve the
  source address of received data.

  @fd: Specifies the socket file descriptor.

  @ptr: Points to a buffer where the message should be stored.

  @nbytes: Specifies the length in bytes of the buffer pointed to by
      the buffer argument.

  @flags: Specifies the type of message reception. Values of this
      argument are formed by logically OR'ing zero or more of the
      following values:

      MSG_PEEK
        Peeks at an incoming message. The data is treated as unread
        and the next recv() or similar function shall still return
        this data.

      MSG_OOB
        Requests out-of-band data. The significance and semantics of
        out-of-band data are protocol-specific.

      MSG_WAITALL
        On SOCK_STREAM sockets this requests that the function block
        until the full amount of data can be returned. The function
        may return the smaller amount of data if the socket is a
        message-based socket, if a signal is caught, if the connection
        is terminated, if MSG_PEEK was specified, or if an error is
        pending for the socket.
*/
ssize_t lothars__recv(int fd, void *ptr, size_t nbytes, int flags)
{
	ssize_t res;
	if (0 > (res = recv(fd, ptr, nbytes, flags))) {
		err_sys("recv error");
	}
	return res;
}

/*
  The send() function shall initiate transmission of data from the
  specified socket to its peer. The send() function shall send a
  message only when the socket is connected (including when the peer
  of a connectionless socket has been set via connect()).

  @fd: Specifies the socket file descriptor.

  @ptr: Points to the buffer containing the message to send.

  @nbytes: Specifies the length of the message in bytes.

  @flags: Specifies the type of message transmission. Values of this
      argument are formed by logically OR'ing zero or more of the
      following flags:

        MSG_EOR
          Terminates a record (if supported by the protocol).

	MSG_OOB
          Sends out-of-band data on sockets that support out-of-band
          communications. The significance and semantics of
          out-of-band data are protocol-specific.
*/
void lothars__send(int fd, const void *ptr, size_t nbytes, int flags)
{
	if (send(fd, ptr, nbytes, flags) != (ssize_t)nbytes) {
		err_sys("send error");
	}
}

/*
  The socket() function shall create an unbound socket in a
  communications domain, and return a file descriptor that can be used
  in later function calls that operate on sockets.

  @family: Specifies the communications domain in which a socket is to
      be created.

  @type: Specifies the type of socket to be created.

  @protocol: Specifies a particular protocol to be used with the
      socket. Specifying a protocol of 0 causes socket() to use an
      unspecified default protocol appropriate for the requested
      socket type.
*/
int lothars__socket(int family, int type, int protocol)
{
	int res;
	if (0 > (res = socket(family, type, protocol))) {
		err_sys("socket error");
	}
	return res;
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
	sync();
}

/*
  The fork() function shall create a new process.
*/
pid_t lothars__fork(void)
{
	pid_t pid;
	if (-1 == (pid = fork())) {
		err_sys("fork error");
	}
	return pid;
}

/******************************************************************************/
// worker implementation

void worker(int fd_sock)
{
	ssize_t nbytes;
	char buf[MAXSIZE];
	memset(buf, '\0', MAXSIZE);
	char *ptr = buf;

	fprintf(stdout, "connected\n");
	do {
		// receive
		nbytes = lothars__recv(fd_sock, ptr, MAXSIZE - 1, 0);

		if (0 < (nbytes = strlen(buf))) {
			fprintf(stdout, "%s", buf);

			// echo back
			lothars__send(fd_sock, buf, nbytes, 0);
		}
		memset(buf, '\0', MAXSIZE);
		ptr = buf;

	} while (1);
	fprintf(stdout, "READY.\n");
}

/******************************************************************************/

/*
  main
*/
int main(int argc, char *argv[])
{
	int fd_listen, fd_conn;
	pid_t childpid;
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;
	char port[16];
	memset(port, '\0', sizeof(port));

	if (2 != argc) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	strncpy(port, argv[1], sizeof(port));
	fprintf(stdout, "port: '%s'\n", port);

	// socket
	fd_listen = lothars__socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	// bind socket to servaddr
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(port));
	lothars__bind(fd_listen, (struct sockaddr *)&servaddr,
		      sizeof(servaddr));

	// listen
	lothars__listen(fd_listen, 7);

	do {
		// accept
		clilen = sizeof(cliaddr);
		fd_conn = lothars__accept(fd_listen,
					  (struct sockaddr *)&cliaddr, &clilen);

		if (0 == (childpid = lothars__fork())) {
			// child
			lothars__close(&fd_listen);
			worker(fd_conn);
			fprintf(stdout, "READY.\n");
			exit(EXIT_SUCCESS);
		}

		lothars__close(&fd_conn);
	} while (1);

	lothars__close(&fd_conn);
	lothars__close(&fd_listen);

	fprintf(stdout, "READY.\n");
	exit(EXIT_SUCCESS);
}
