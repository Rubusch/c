/*
  tcp server, poll() implementation

  The tcp poll server accepts several clients. Instead of forking out
  (concurrent server) or serving each client in a different thread or
  lightweight process, respectively, this server uses I/O multiplexing
  through poll().

  poll() is active I/O multiplexing, and thus probably less suited
  than a comparable approach by select().


  ---
  compiler/library info

  definition of POLLRDNORM:

  either generally
  #define _GNU_SOURCE 1

  or more specific
  #define _XOPEN_SOURCE 500

  alternatively use
  POLLIN


  ---
  References:
  UNIX Network Programming, Stevens (1996), see p.187
*/

#define _XOPEN_SOURCE 600 /* sync() */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h> /* va_list, va_start,... */

#include <sys/types.h> /* bind() */
#include <sys/socket.h> /* socket(), bind(), listen() */
#include <arpa/inet.h> /* sockaddr_in */
#include <unistd.h> /* close(), sync() with _XOPEN_SOURCE */
#include <errno.h>
#include <sys/poll.h> /* struct pollfd, poll() */

/*
  constants
*/

#define OPEN_MAX 256
#define MAXLINE  4096 /* max text line length */
#define LISTENQ 1024 /* the listen queue - serving as backlog for listen, can also be provided as env var */
#define POLLTIMEOUT -1 /* poll() timeout, or -1 infinite */


/*
  forwards
*/

// error handling
void err_sys(const char *, ...);
void err_quit(const char *, ...);

// socket
int lothars__socket(int, int, int);
void lothars__bind(int, const struct sockaddr *, socklen_t);
void lothars__listen(int, int);
int lothars__accept(int, struct sockaddr *, socklen_t *);
int lothars__poll(struct pollfd *, unsigned long, int);

// read / write
ssize_t lothars__read(int, void *, size_t);
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
   Fatal error related to system call. Print message and terminate.
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
   Fatal error unrelated to system call. Print message and terminate.
*/
void err_quit(const char *fmt, ...)
{
	va_list  ap;
	va_start(ap, fmt);
	err_doit(0, fmt, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}


/*
  socket
*/

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


/*
  The bind() function shall assign a local socket address address to a
  socket identified by descriptor socket that has no local socket
  address assigned. Sockets created with the socket() function are
  initially unnamed; they are identified only by their address family.

  #include <sys/types.h>
  #include <sys/socket.h>

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
		err_sys("%s() error", __func__);
	}
}


/*
  The listen() function shall mark a connection-mode socket, specified
  by the socket argument, as accepting connections.

  #include <sys/socket.h>

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
		err_sys("%s() error", __func__);
	}
}


/*
  The accept() function shall extract the first connection on the
  queue of pending connections, create a new socket with the same
  socket type protocol and address family as the specified socket, and
  allocate a new file descriptor for that socket.

  #include <sys/socket.h>

  @fd: Specifies a socket that was created with socket(), has been
      bound to an address with bind(), and has issued a successful
      call to listen().
  @sa: Either a null pointer, or a pointer to a sockaddr structure
      where the address of the connecting socket shall be returned.
  @salenptr: Points to a socklen_t structure which on input specifies
      the length of the supplied sockaddr structure, and on output
      specifies the length of the stored address.
*/
int lothars__accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
	int res;
again:
	if (0 > (res = accept(fd, sa, salenptr))) {
		if ((errno == EPROTO) || (errno == ECONNABORTED)) {
			goto again;
		} else {
			err_sys("%s() error", __func__);
		}
	}
	return res;
}


/*
  The poll() function provides applications with a mechanism for
  multiplexing input/output over a set of file descriptors. For each
  member of the array pointed to by fds, poll() shall examine the
  given file descriptor for the event(s) specified in events. The
  number of pollfd structures in the fds array is specified by
  nfds. The poll() function shall identify those file descriptors on
  which an application can read or write data, or on which certain
  events have occurred.

  #include <poll.h>

  @fdarray: The fds argument specifies the file descriptors to be
      examined and the events of interest for each file descriptor.
  @nfds: The numer of fdarray [fds].
  @timeout: Alternatively, provide a timeout.

  Returns a positive value indicates the total number of file
  descriptors that have been selected, a value of 0 indicates that the
  call timed out.
*/
int lothars__poll(struct pollfd *fdarray, unsigned long nfds, int timeout)
{
	int res;
	if (0 > (res = poll(fdarray, nfds, timeout))) {
		err_sys("%s() error", __func__);
	} else if (0 == res) {
		err_sys("%s() timed out error", __func__);
	}
	return res;
}


/*
  read/write
*/


/*
  The read() function shall attempt to read nbyte bytes from the file
  associated with the open file descriptor, fildes, into the buffer
  pointed to by buf. The behavior of multiple concurrent reads on the
  same pipe, FIFO, or terminal device is unspecified.

  #include <unistd.h>

  @fd: The file descriptor to read from (fildes).
  @ptr: A pointer to the buffer to read into (buf).
  @nbytes: The number of bytes to read.

  Return is phony for compatibility at failure, else returns the
  number of bytes actually read and may be smaller or equal to nbytes.
*/
ssize_t lothars__read(int fd, void *ptr, size_t nbytes)
{
	ssize_t  bytes;
	if (-1 == (bytes = read(fd, ptr, nbytes))) {
		err_sys("read error");
	}
	return bytes;
}


/*
  The write() function shall attempt to write nbyte bytes from the
  buffer pointed to by buf to the file associated with the open file
  descriptor, fildes.

  This wrapper writes exactly number of bytes to the descriptor.

  #include <unistd.h>

  @fd: The associated open file descriptor.
  @ptr: Points to the buffer to write to.
  @nbytes: The number of bytes to write.
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
#if _XOPEN_SOURCE >= 500
	sync();
#endif /* _XOPEN_SOURCE */
}


/********************************************************************************************/
// worker implementation

/********************************************************************************************/


/*
  main
*/
int main(int argc, char* argv[])
{
	int idx=-1, maxidx=-1;
	int nready; // multiplex: number of sd ready to read / process
	int sd_listen=-1; // sd for waiting on connections
	int sd_conn=-1; // accepted connection sd
	int sd=-1; // sd for looping through sd list
	char buf[MAXLINE]; memset(buf, '\0', sizeof(buf));
	socklen_t clilen;
	struct sockaddr_in cliaddr;     // address object for accepting socket connections
	struct sockaddr_in servaddr;    // address object for binding to this server
	struct pollfd client[OPEN_MAX]; // array of client sd's
	char port[16]; memset(port, '\0', sizeof(port));

	if (2 != argc) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	strncpy(port, argv[1], sizeof(port));
	fprintf(stdout, "port: '%s'\n", port);

	// socket
	sd_listen = lothars__socket(AF_INET, SOCK_STREAM, 0);

	// bind
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // in case, bind to all interfaces
	servaddr.sin_port = htons(atoi(port));
	lothars__bind(sd_listen, (struct sockaddr*) &servaddr, sizeof(servaddr));

	// listen
	lothars__listen(sd_listen, LISTENQ);

	/*
	  poll - init struct pollfd array: client[]
	*/
	client[0].fd = sd_listen;

	fprintf(stderr, "_XOPEN_SOURCE was");
#if _XOPEN_SOURCE >= 500
	client[0].events = POLLRDNORM;
#else
	fprintf(stderr, " NOT");
	client[0].events = POLLIN;
#endif
	fprintf(stderr, " defined\n");


	for (idx = 1; idx < OPEN_MAX; ++idx) {
		// -1 indicates available entry
		client[idx].fd = -1;
	}
	maxidx = 0; // max index into client[] array


	while (1) {
		/*
		  poll - waits for one of a set of file descriptors
		  (client[]) to become ready to perform I/O.

		  On success, a positive number is returned; this is
		  the number of structures which have nonzero revents
		  fields (in other words, those descriptors with
		  events or errors reported). A value of 0 indicates
		  that the call timed out and no file descriptors were
		  ready. On error, -1 is returned, and errno is set
		  appropriately.

		  Returns the number of sockets ready to read: nready.
		*/
		nready = lothars__poll(client, maxidx+1, POLLTIMEOUT);

#if _XOPEN_SOURCE >= 500
		if (client[0].revents & POLLRDNORM) { // new client connection, _XOPEN_SOURCE
#else
		if (client[0].revents & POLLIN) { // new client connection
#endif
			// accept to cliaddr
			clilen = sizeof( cliaddr );
			sd_conn = lothars__accept(sd_listen, (struct sockaddr*) &cliaddr, &clilen);

			/*
			  Get latest free place in fd list, to store
			  new fd.
			*/
			for (idx=1; idx<OPEN_MAX; ++idx) {
				if (0 > client[idx].fd) {
					client[idx].fd = sd_conn; // save descriptor
					break;
				}
			}
			// client register - check if full
			if (idx == OPEN_MAX) {
				perror("too many clients");
				exit(EXIT_FAILURE);
			}

			// there is data to read
#if _XOPEN_SOURCE >= 500
			client[idx].events = POLLRDNORM;
#else
			client[idx].events = POLLIN;
#endif
			// max index in client[] array
			if (maxidx < idx) maxidx = idx;

			// no more readable descriptors
			if (0 >= --nready) continue;
		} /* for */


		/*
		  check all accepted clients for data
		*/
		for (idx = 1; idx <= maxidx; ++idx) {

			// get client (current loop pointer: sd), continue in case of -1
			if (0 > (sd = client[idx].fd) ) {
				continue;
			}
#if _XOPEN_SOURCE >= 500
			if (client[idx].revents & (POLLRDNORM | POLLERR) ) {
#else
			if (client[idx].revents & (POLLIN | POLLERR)) {
#endif
				ssize_t num;

				// read from socket...
				memset(buf, '\0', sizeof(buf));
				if (0 == (num =	lothars__read(sd, buf, MAXLINE))) {
					// ...nothing - connection closed by client
					fprintf(stderr, "client[%d] closed connection\n", idx);
					lothars__close(&sd);
					client[idx].fd = -1;

				} else {
					// ...something
					fprintf( stderr, "client[%d] says: '%s'\n", idx, buf );
					// echo server
					lothars__writen(sd, buf, num);
				}

				// check if no more readable descriptors
				if (0 >= --nready) {
					break;
				}
			}
		} /* for idx in maxindex */
	}

	fprintf(stdout, "READY.\n");
	exit(EXIT_SUCCESS);
}
