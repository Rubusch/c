/*
 * tcp echo client
 *
 * sends a prepare message as string to a tcp echo server, the
 * received string will have maintained escape sequences
 *
 * @author: Lothar Rubusch
 * @email: L.Rubusch@gmx.ch
 * @license: GPLv3
 * @date: 2013-april-28
 */

#define _GNU_SOURCE /* sync() */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdarg.h> /* va_start(), va_end(),... */
#include <unistd.h>
#include <errno.h>

/*
  constants
*/

#define MAXLINE 4096 /* max text line length */

/*
  forwards
*/

// errors
void err_sys(const char *, ...);

// sock
void lothars__connect(int, const struct sockaddr *, socklen_t);
ssize_t lothars__recv(int, void *, size_t, int);
void lothars__send(int, const void *, size_t, int);
int lothars__socket(int, int, int);

// unix
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
	char buf[MAXLINE + 1];
	memset(buf, '\0', sizeof(buf));
	errno_save = errno; // value caller might want printed
	vsnprintf(buf, MAXLINE, fmt, ap);
	n_len = strlen(buf);
	if (errnoflag) {
		snprintf(buf + n_len, MAXLINE - n_len, ": %s",
			 strerror(errno_save));
	}

	strcat(buf, "\n");

	fflush(stdout); // in case stdout and stderr are the same
	fputs(buf, stderr);
	fflush(stderr);
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
  The connect() function shall attempt to make a connection on a socket.

  @fd: Specifies the file descriptor associated with the socket.

  @sa: Points to a sockaddr structure containing the peer address. The
      length and format of the address depend on the address family of
      the socket.

  @salen: Specifies the length of the sockaddr structure pointed to by
      the address argument.
*/
void lothars__connect(int fd, const struct sockaddr *sa, socklen_t salen)
{
	if (0 > connect(fd, sa, salen)) {
		err_sys("connect error");
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

/********************************************************************************************/
// worker implementation

/********************************************************************************************/

/*
  main()
*/
int main(int argc, char *argv[])
{
	int fd_sock;
	struct sockaddr_in serveraddr;
	char buf[MAXLINE];
	memset(buf, '\0', MAXLINE);
	unsigned int echolen;
	int nbytes = 0;
	int read = 0;
	char message[4096];
	memset(message, '\0', 4096);
	char serverip[16];
	memset(serverip, '\0', sizeof(serverip));
	char port[16];
	memset(port, '\0', sizeof(port));

	if (3 != argc) {
		fprintf(stderr, "usage: %s <serverip> <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	strncpy(serverip, argv[1], sizeof(serverip));
	fprintf(stdout, "serverip: '%s'\n", serverip);

	strncpy(port, argv[2], sizeof(port));
	fprintf(stdout, "port: '%s'\n", port);

	/*
	  message

	  a lot of escapes, will be sent and received as string, thus
	  escape sequences will persist in the received message
	 */
	strcpy(message,
	       "<?xml version=\"1.0\"?> \n<TASK task_name=\"guiagent\">\n  <DATA_OUT "
	       "name=\"xxx\" dim=\"0\" type=\"cmd\" value=\"agtstart\">\n <DST "
	       "name=\"agentname\" />\n </DATA_OUT>\n </TASK>");

	fprintf(stdout, "socket()\n");
	fd_sock = lothars__socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	fprintf(stdout, "connect()");
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET; // AF_INET for addresses
	serveraddr.sin_addr.s_addr = inet_addr(serverip);
	serveraddr.sin_port = htons(atoi(port));
	lothars__connect(fd_sock, (struct sockaddr *)&serveraddr,
			 sizeof(serveraddr));
	fprintf(stdout, " - ok\n");

	fprintf(stdout, "send()");
	echolen = strlen(message);
	lothars__send(fd_sock, message, echolen, 0);
	fprintf(stdout, " - ok\n");

	while (nbytes < echolen) {
		read = 0;
		fprintf(stdout, "recv()");
		read = lothars__recv(fd_sock, buf, MAXLINE - 1, 0);
		fprintf(stdout, " - ok\n");
		nbytes += read;
		buf[read] = '\0';
		fprintf(stdout, "received:\n%s\n\n", buf);
	}

	lothars__close(&fd_sock);

	fprintf(stdout, "READY.\n");
	exit(EXIT_SUCCESS);
}
