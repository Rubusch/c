/*
  portscanner

  A portscanner is basically a client which tries to connect servers.

  The actual port scanner w/o convenience would be less than 100
  lines, perhaps just 50 lines.
*/

#define _GNU_SOURCE /* sync() */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/wait.h> /* wait() */
#include <unistd.h> /* read(), write(), fork(), close() */
#include <netdb.h> /* socket(), SOCK_STREAM, AF_UNSPEC, getaddrinfo() */
#include <stdarg.h> /* va_start(), va_end(),... */
#include <errno.h>

/*
  constants
*/

#define MAXLINE 4096 /* max text line length */
#define MIN_PORT 0 // e.g. 0
#define MAX_PORT 100 // e.g. 1024

/*
  forwards
*/

// error
void err_sys(const char *, ...);
void err_quit(const char *, ...);

// socket
int lothars__socket(int, int, int);

// inet
void lothars__inet_pton(int, const char *, void *);

// unix
void lothars__close(int *);

/*
  helpers
*/

/*
   Print message and return to caller. Caller specifies "errnoflag".

   #include <stdarg.h>

   @errnoflag: The flag for the errno number.
   @fmt: The format.
   @ap: The argument pointer for further arguments in va_list.
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

/* error */

/*
   Fatal error related to system call. Print message and terminate.
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
   Fatal error unrelated to system call. Print message and terminate.
*/
void err_quit(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	err_doit(0, fmt, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}

/* socket */

/*
  The socket() function shall create an unbound socket in a
  communications domain, and return a file descriptor that can be used
  in later function calls that operate on sockets.

  #include <sys/socket.h>

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
		err_sys("%s() error", __func__);
	}
	return res;
}

/* inet */

/*
  inet_pton - convert IPv4 and IPv6 addresses from text to binary form

  This function converts the character string src into a network
  address structure in the af address family, then copies the network
  address structure to dst. The af argument must be either AF_INET or
  AF_INET6.

  #include <arpa/inet.h>

  @family: The address family AF_INET or AF_INET6 (af).
  @strptr: Points to a source string containing an IP address (src).
  @addrptr: Points to a destination string, in case of a struct
  in_addr, defer to its s_addr member to be initialized.

  Returns 1 on success, 0 if strptr did not contain a valid internet
  address and -1 if family was not a valid address family.
*/
void lothars__inet_pton(int family, const char *strptr, void *addrptr)
{
	int res;
	if (0 > (res = inet_pton(family, strptr, addrptr))) {
		err_sys("%s() error for %s (check the passed address family?)",
			__func__, strptr); // errno set
	} else if (0 == res) {
		err_quit("%s() error for %s (check the passed strptr)",
			 __func__, strptr); // errno not set
	}
}

/* unix */

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
  main
*/
int main(int argc, char **argv)
{
	struct sockaddr_in addr_target;
	struct servent *servServer = NULL;
	int sd = 0;
	unsigned long min_port = 0, max_port = 0, port = 0;
	char targetip[18];

	memset(targetip, '\0', sizeof(targetip));

	if (argc != 4) {
		fprintf(stderr, "usage: %s <ip> <min port> <max port>\n",
			argv[0]);
		exit(EXIT_FAILURE);
	}

	strncpy(targetip, argv[1], sizeof(targetip));
	fprintf(stdout, "targetip '%s'\n", targetip);

	min_port = atol(argv[2]);
	fprintf(stdout, "min_port %lu\n", min_port);

	max_port = atol(argv[3]);
	fprintf(stdout, "max_port %lu\n", max_port);

	if (min_port > max_port) {
		fprintf(stdout, "!!! swapping start and end port !!!\n");
		port = max_port;
		max_port = min_port;
		min_port = port;
	}

	fprintf(stdout, "\nscanning target %s:\n", targetip);
	for (port = min_port; port <= max_port; ++port) {
		lothars__inet_pton(AF_INET, targetip,
				   &addr_target.sin_addr.s_addr);
		sd = lothars__socket(AF_INET, SOCK_STREAM, 0);

		// must be set after sin_addr was init
		addr_target.sin_port = htons(port);
		addr_target.sin_family = AF_INET;

		if (0 == connect(sd, (struct sockaddr *)&addr_target,
				 sizeof(addr_target))) {
			fprintf(stdout, "\t%lu ", port);
			if (NULL != (servServer = getservbyport( // FIXME: replace by getnameinfo()
					     addr_target.sin_port, "tcp"))) {
				fprintf(stdout, "(%s)", servServer->s_name);
			} else {
				fprintf(stdout, "(unknown service)");
			}
			fprintf(stdout, " is open\n");
			//		} else {
			//			fprintf(stdout, "\t%lu is closed\n", port);
		}

		lothars__close(&sd);
	}
	fprintf(stdout, "READY.\n");
	exit(EXIT_SUCCESS);
}
