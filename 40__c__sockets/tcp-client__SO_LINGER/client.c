// tcp_client_SO_LINGER
/*
  ipv4 tcp client

  socket options: SO_LINGER, shutdown tcp socket
  don't wait on timeout

  (based on Stevens / UNIX Network Programming)
 */

#include <stdlib.h> // exit()
#include <stdio.h> // perror()
#include <string.h> // memset()
#include <stdarg.h> /* va_start(), va_end(),... */
#include <sys/socket.h> // socket(), inet_pton()
#include <sys/types.h> // inet_pton()
#include <arpa/inet.h> // sockaddr_in, inet_pton()
#include <errno.h> // errno
#include <unistd.h> // read(), write()
#include <limits.h> // SSIZE_MAX


/*
  constants
*/

#define MAXLINE  4096 /* max text line length */
#define BUFSIZE 1024


/*
  forwards
*/

void err_sys(const char *, ...);
void err_quit(const char *fmt, ...);

// sock
void lothars__connect(int, const struct sockaddr *, socklen_t);
void lothars__setsockopt(int, int, int, const void *, socklen_t);
int lothars__socket(int, int, int);

// inet
void lothars__inet_pton(int, const char *, void *);

// read / write
ssize_t lothars__readn(int, void *, size_t);
void lothars__writen(int, void *, size_t);

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


void lothars__connect(int fd, const struct sockaddr *sa, socklen_t salen)
{
	if (0 > connect(fd, sa, salen)) {
		err_sys("connect error");
	}
}


void lothars__setsockopt(int fd
			 , int level
			 , int optname
			 , const void *optval
			 , socklen_t optlen)
{
	if (0 > setsockopt(fd, level, optname, optval, optlen)) {
		err_sys("setsockopt error");
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


void lothars__inet_pton(int family, const char *strptr, void *addrptr)
{
	int res;
	if (0 > (res = inet_pton(family, strptr, addrptr))) {
		err_sys("inet_pton error for %s", strptr); // errno set
	} else if (0 == res) {
		err_quit("inet_pton error for %s", strptr); // errno not set
	}
}


/*
  readn

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
		if ( (nread = read(fd, ptr, nleft)) < 0) {
			if (errno == EINTR) {
				nread = 0;  // and call read() again
			} else {
				return -1;
			}
		} else if (nread == 0) {
			break; // EOF
		}
		nleft -= nread;
		ptr   += nread;
	}
	return (num - nleft);  // return >= 0
}


ssize_t lothars__readn(int fd, void *ptr, size_t nbytes)
{
	ssize_t  res;
	if (0 > (res = readn(fd, ptr, nbytes))) {
		err_sys("readn error");
	}
	return res;
}


/*
  writen

  write "num" bytes to a descriptor
*/
ssize_t writen(int fd, const void *vptr, size_t num)
{
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;

	ptr = vptr;
	nleft = num;
	while (0 < nleft) {
		if (0 >= (nwritten = write(fd, ptr, nleft))) {
			if ((nwritten < 0) && (errno == EINTR)) {
				// and call write() again
				nwritten = 0;
			} else {
				// error
				return -1;
			}
		}

		nleft -= nwritten;
		ptr += nwritten;
	}
	return num;
}


void lothars__writen(int fd, void *ptr, size_t nbytes)
{
	if (writen(fd, ptr, nbytes) != nbytes) {
		err_sys("writen error");
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
	sync();
}


/********************************************************************************************/
// worker implementation


void work_client(int fd_sock)
{
        ssize_t num;
	const int buflen = strlen("ping") + 1;
        char buf[buflen];

	// send "ping"
	memset(buf, '\0', buflen);
        strcpy( buf, "ping" );
        num = strlen(buf) + 1; // will wait untill buffer fills up to buflen
        lothars__writen(fd_sock, buf, num);
        fprintf(stdout, "client sent \'%s' with size \'%ld\'\n", buf, num);

        // receive "ping"
	memset(buf, '\0', buflen);
        lothars__readn(fd_sock, buf, sizeof("ping"));
        fprintf(stderr, "server said '%s'\n", buf);
}


/********************************************************************************************/


/*
  main()
*/
int main(int argc, char* argv[])
{
	int fd_sock;
	struct linger ling;
	struct sockaddr_in servaddr; // addr obj for server
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

	// socket
	fd_sock = lothars__socket(AF_INET, SOCK_STREAM, 0);

	// connect
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(port));
	lothars__inet_pton(AF_INET, serverip, &servaddr.sin_addr);
	lothars__connect(fd_sock, (struct sockaddr*) &servaddr, sizeof(servaddr));

	// control
	work_client(fd_sock);

	// set SO_LINGER, don't keep socket alife untill TCP times out
	ling.l_onoff = 1;
	ling.l_linger = 0;
	lothars__setsockopt(fd_sock, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling));

	// close
	lothars__close(&fd_sock);

	fprintf(stdout, "READY.\n");
	exit(EXIT_SUCCESS);
}
