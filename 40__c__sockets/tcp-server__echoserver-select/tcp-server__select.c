/*
  tcp echo server, select() implementation


  The tcp select server accepts several clients. Instead of forking
  out (concurrent server) or serving each client in a different thread
  or lightweight process, respectively, this server uses passive I/O
  multiplexing through select().

  poll() is active I/O multiplexing, and thus probably less suited
  than a comparable approach by select(). This approach here depicts
  the select implementation.


  NB: The maximum socket queue to listen can be configured / printed
  with:

  $ sudo sysctl net.ipv4.tcp_max_syn_backlog
      net.ipv4.tcp_max_syn_backlog = 512

  i.e. should not be more than 1024

  ---
  References:
  UNIX Network Programming, Stevens (1996), p. 179
*/

#define _XOPEN_SOURCE 600 /* sync() */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h> /* va_list, va_start,... */

#include <sys/types.h> /* bind() */
#include <sys/socket.h> /* socket(), bind(), listen() */
#include <sys/select.h> /* select(), FD_... */
#include <arpa/inet.h> /* sockaddr_in */
#include <unistd.h> /* close(), sync() with _XOPEN_SOURCE */
#include <errno.h>

/*
  constants
*/

#define OPEN_MAX 256
#define MAXLINE  4096 /* max text line length */
#define LISTENQ 1024 /* the listen queue - serving as backlog for listen, can also be provided as env var */


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
int lothars__select(int, fd_set *, fd_set *, fd_set *, struct timeval *);

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
  The pselect() function shall examine the file descriptor sets whose
  addresses are passed in the readfds, writefds, and errorfds
  parameters to see whether some of their descriptors are ready for
  reading, are ready for writing, or have an exceptional condition
  pending, respectively.

  The select() function shall be equivalent to the pselect() function,
  except as follows:

  - For the select() function, the timeout period is given in seconds
    and microseconds in an argument of type struct timeval, whereas
    for the pselect() function the timeout period is given in seconds
    and nanoseconds in an argument of type struct timespec.

  - The select() function has no sigmask argument; it shall behave as
    pselect() does when sigmask is a null pointer.

  - Upon successful completion, the select() function may modify the
    object pointed to by the timeout argument.

  #include <sys/select.h>

  @nfds: The nfds argument specifies the range of descriptors to be
      tested. The first nfds descriptors shall be checked in each set;
      that is, the descriptors from zero through nfds-1 in the
      descriptor sets shall be examined.
  @readfs: If the readfds argument is not a null pointer, it points to
      an object of type fd_set that on input specifies the file
      descriptors to be checked for being ready to read, and on output
      indicates which file descriptors are ready to read.
  @writefds: If the writefds argument is not a null pointer, it points
      to an object of type fd_set that on input specifies the file
      descriptors to be checked for being ready to write, and on
      output indicates which file descriptors are ready to write.
  @exceptfds: If the errorfds argument is not a null pointer, it
      points to an object of type fd_set that on input specifies the
      file descriptors to be checked for error conditions pending, and
      on output indicates which file descriptors have error conditions
      pending.
  @timeout: The timeout parameter controls how long the pselect() or
      select() function shall take before timing out. If the timeout
      parameter is not a null pointer, it specifies a maximum interval
      to wait for the selection to complete.
*/
int lothars__select(int nfds
	    , fd_set *readfds
	    , fd_set *writefds
	    , fd_set *exceptfds
	    , struct timeval *timeout)
{
	int  res;
	if (0 > (res = select(nfds, readfds, writefds, exceptfds, timeout))) {
		err_sys("%s() error", __func__);
	}
	return res;  /* can return 0 on timeout */
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
	int sd_listen=-1; // sd, for waiting on connections
	int sd_conn=-1; // sd, accepted connection
	int sd=-1; // sd, for looping through sd list
	char buf[MAXLINE]; memset(buf, '\0', sizeof(buf));
	socklen_t clilen;
	struct sockaddr_in cliaddr;     // address object for accepting socket connections
	struct sockaddr_in servaddr;    // address object for binding to this server
	int sd_max; // sd, to be increased at new connections
	int client[FD_SETSIZE];
	fd_set rset, allset; // select - rset, actually checked; allset, for registering fd's
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

	// client register - init values and list entries to -1
	sd_max = sd_listen; // initialize
	maxidx = -1; // index into client[] array
	for (idx=0; idx < FD_SETSIZE; ++idx) {
		client[idx] = -1; // -1 indicates available entry
	}

	// select - zero allset
	fprintf(stderr, "FD_ZERO( &allset )\n");
	FD_ZERO(&allset);

	// select - register sd_listen in allset
	fprintf(stderr, "FD_SET( sd_listen, &allset )\n");
	FD_SET(sd_listen, &allset);


	while (1) {
		// select - structure assignment allset to rset!!!
		rset = allset;

		// select - call
		fprintf(stderr, "SELECT on &rset\n");
		/*
		  select return value

		  On success, select() and pselect() return the number
		  of file descriptors contained in the three returned
		  descriptor sets (that is, the total number of bits
		  that are set in readfds, writefds, exceptfds) which
		  may be zero if the timeout expires before anything
		  interesting happens. On error, -1 is returned, and
		  errno is set appropriately; the sets and timeout
		  become undefined, so do not rely on their contents
		  after an error.
		*/
		nready = lothars__select(sd_max+1, &rset, NULL, NULL, NULL );


		// select - sd_listen available for read and write
		if (FD_ISSET(sd_listen, &rset)) { // new client connection
			fprintf( stderr, "FD_ISSET(sd_listen, &rset)\n");

			// accept
			clilen = sizeof( cliaddr );
			sd_conn = lothars__accept(sd_listen, (struct sockaddr*) &cliaddr, &clilen);

			/*
			  Get latest free place in fd list, to store
			  new fd.
			*/
			for (idx=0; idx<FD_SETSIZE; ++idx) {
				if (0 > client[idx] ) {
					fprintf( stderr, "\tregister new client\n" );
					client[idx] = sd_conn; // save descriptor
					break;
				}
			}
			// client register - check if full
			if (idx == FD_SETSIZE){
				perror("too many clients");
				exit(EXIT_FAILURE);
			}

			// select - register sd_conn in allset
			FD_SET(sd_conn, &allset);
			if (sd_conn > sd_max) {
				fprintf( stderr, "\tupdate sd_max for select\n");
				sd_max = sd_conn;
			}

			// max index in client[] array
			if (maxidx < idx) maxidx = idx;

			// no more readable descriptors
			if (0 >= --nready) continue;
		} /* for */


		fprintf( stderr, "loop over all clients to check for data\n");
		for (idx=0; idx <= maxidx; ++idx) { // loop over all clients to check for data
			if (0 > (sd = client[idx])) {
				continue; // -1, no client fd set
			}


			// select - sd available for read and write
			if (FD_ISSET(sd, &rset)) {
				fprintf( stderr, "FD_ISSET( sd, &rset )\n" );

				ssize_t num;
				memset(buf, '\0', sizeof(buf));
				if (0 == (num = lothars__read(sd, buf, MAXLINE))) {
					fprintf( stderr, "\tconnection closed by client\n");

					if ( 0 > close( sd ) ){
						perror( "close failed" );
						exit( EXIT_FAILURE );
					}

					// select - clear sd from allset
					fprintf( stderr, "FD_CLR(sd, &allset)\n" );
					FD_CLR( sd, &allset );
					client[idx] = -1;
				} else {
					// work
					fprintf(stderr, "\tclient says '%s', answering...\n", buf);

					// send echo
					//writen(sd, buf, num);
					lothars__writen(sd, buf, num);
				}

				if (--nready <= 0) {
					fprintf(stderr, "\tselect return value '%d' - nothing left to process\n", nready);
					break; // no more readable descriptors
				}
			}


		}
	}

	fprintf(stdout, "READY.\n");
	exit(EXIT_SUCCESS);
}
