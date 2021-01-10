/*
  Send Multicast Datagram code example.

  Sending and receiving a multicast datagram

  IP multicasting provides the capability for an application to send a
  single IP datagram that a group of hosts in a network can receive.
  The hosts that are in the group may reside on a single subnet or may
  be on different subnets that have been connected by multicast
  capable routers.

  Hosts may join and leave groups at any time.  There are no
  restrictions on the location or number of members in a host group.
  A class D Internet address in the range 224.0.0.1 to 239.255.255.255
  identifies a host group.

  An application program can send or receive multicast datagrams by
  using the socket() API and connectionless SOCK_DGRAM type sockets.
  Each multicast transmission is sent from a single network interface,
  even if the host has more than one multicasting-capable interface.

  It is a one-to-many transmission method.  You cannot use
  connection-oriented sockets of type SOCK_STREAM for multicasting.

  When a socket of type SOCK_DGRAM is created, an application can use
  the setsockopt() function to control the multicast characteristics
  associated with that socket.  The setsockopt() function accepts the
  following IPPROTO_IP level flags:

       - IP_ADD_MEMBERSHIP: Joins the multicast group specified.

       - IP_DROP_MEMBERSHIP: Leaves the multicast group specified.

       - IP_MULTICAST_IF: Sets the interface over which outgoing
         multicast datagrams are sent.

       - IP_MULTICAST_TTL: Sets the Time To Live (TTL) in the IP
         header for outgoing multicast datagrams.  By default it is
         set to 1.  TTL of 0 are not transmitted on any sub-network.
         Multicast datagrams with a TTL of greater than 1 may be
         delivered to more than one sub-network, if there are one or
         more multicast routers attached to the first sub-network.

       - IP_MULTICAST_LOOP: Specifies whether or not a copy of an
         outgoing multicast datagram is delivered to the sending host
         as long as it is a member of the multicast group.

  The following examples enable a socket to send and receive multicast
  datagrams.  The steps needed to send a multicast datagram differ
  from the steps needed to receive a multicast datagram.


  ---
  References:
  Unix Network Programming, Stevens

  Some of the explanations are taken from tenouk's tutorial, to
  be found here: https://www.tenouk.com/Module41c.html
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> /* close() */
#include <stdarg.h> /* va_start(), va_end(),... */
#include <errno.h>


/*
  constants
*/

#define MAXLINE  4096 /* max text line length */
struct in_addr addr_local;
struct sockaddr_in saddr_group;
int fd_sock;
char databuf[MAXLINE] = "Multicast test message lol!";

/*
  forwards
*/

// error
void err_sys(const char *, ...);

// unix
void lothars__close(int);

// sock
int lothars__socket(int, int, int);
void lothars__setsockopt(int, int, int, const void *, socklen_t);
void lothars__sendto(int, const void *, size_t, int, const struct sockaddr *, socklen_t);

// inet
int lothars__inet_aton(const char *, struct in_addr *);

// read/write
ssize_t lothars__read(int, void *, size_t);


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


// error

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

  #include <unistd.h>

  @fd: The file descriptor to the specific connection.
*/
void lothars__close(int fd)
{
	if (-1 == close(fd)) {
                err_sys("%s() error", __func__);
	}
}


// sock

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
  The setsockopt() function shall set the option specified by the
  option_name argument, at the protocol level specified by the level
  argument, to the value pointed to by the option_value argument for
  the socket associated with the file descriptor specified by the
  socket argument.

  #include <sys/socket.h>

  @fd: The socket file descriptor to provide the peer address.
  @level: The level argument specifies the protocol level at which the
      option resides.
  @optname: The option_name argument specifies a single option to be
      set.
  @optval: The value stored in the object pointed to.
  @optlenptr: The option_len argument shall be modified to indicate
      the actual length of the value.
*/
void lothars__setsockopt(int fd
			 , int level
			 , int optname
			 , const void *optval
			 , socklen_t optlen)
{
	if (0 > setsockopt(fd, level, optname, optval, optlen)) {
		lothars__close(fd_sock);
		err_sys("%s() error", __func__);
	}
}


/*
  The sendto() function shall send a message through a connection-mode
  or connectionless-mode socket. If the socket is connectionless-mode,
  the message shall be sent to the address specified by dest_addr. If
  the socket is connection-mode, dest_addr shall be ignored.

  #include <sys/socket.h>

  @fd: Specifies the socket file descriptor.
  @ptr: Points to a buffer containing the message to be sent.
  @nbytes: Specifies the size of the message in bytes.
  @flags: Specifies the type of message transmission.
  @sa: Points to a sockaddr structure containing the destination
      address.
  @salen: Specifies the length of the sockaddr structure pointed to by
      the dest_addr argument.
*/
void lothars__sendto( int fd
		, const void *ptr
		, size_t nbytes
		, int flags
		, const struct sockaddr *sa
		, socklen_t salen)
{
	if ((ssize_t) nbytes != sendto(fd, ptr, nbytes, flags, sa, salen)) {
		err_sys("%s() error", __func__);
	}
}


/*
  inet_aton() converts the Internet host address cp from the IPv4
  numbers-and-dots notation into binary form (in network byte order)
  and stores it in the structure that inp points to. inet_aton()
  returns nonzero if the address is valid, zero if not. The address
  supplied in cp can have one of the following forms:

  a.b.c.d, a.b.c, a.b., a

  An example of the use of inet_aton() and inet_ntoa() is shown
  below. Here are some example runs:

    $ ./a.out 226.000.000.037      # Last byte is in octal
    226.0.0.31
    $ ./a.out 0x7f.1               # First byte is in hex
    127.0.0.1

  #ifdef _BSD_SOURCE || _SVID_SOURCE
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>

  @cp: The Internet host address in numbers-and-dots notation.
  @inp: A pointer to a structure which contains the binary Internet
  address.

  Return is phony for compatibility.
*/
int lothars__inet_aton(const char *cp, struct in_addr *inp)
{
	int res;
	if (0 == (res = inet_aton(cp, inp))) {
		err_sys("%s() invalid address", __func__);
	}
	/*
	  NB: in case of failure there's no close(fd_sock)
	  when the program exits, resources are going to be freed by OS, anyway

	  take care if this might be not the (regular) case e.g. due
	  to some socket options
	*/
	return res;
}


/*
  The read() function shall attempt to read nbyte bytes from the file
  associated with the open file descriptor, fildes, into the buffer
  pointed to by buf. The behavior of multiple concurrent reads on the
  same pipe, FIFO, or terminal device is unspecified.

  #include <unistd.h>

  @fd: The filedescriptor to read from (fildes).
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



/********************************************************************************************/
// worker implementation


/********************************************************************************************/


/*
  main
*/
int main(int argc, char *argv[])
{
	char senderip[16]; memset(senderip, '\0', sizeof(senderip));
	char groupip[16]; memset(groupip, '\0', sizeof(groupip));
	char port[16]; memset(port, '\0', sizeof(port));

	if (4 != argc) {
		fprintf(stderr, "usage: %s <senderip> <groupip> <port>\ne.g.\n$> %s 203.106.93.94 225.1.1.1 5555\n", argv[0], argv[0]);
//		fprintf(stderr, "usage: %s <senderip> <groupip> <port>\ne.g.\n$> %s 203.106.93.94 226.1.1.1 4321\n", argv[0], argv[0]);
		exit(EXIT_FAILURE);
	}
	strncpy(senderip, argv[1], sizeof(senderip));
	fprintf(stdout, "senderip: '%s'\n", senderip);

	strncpy(groupip, argv[2], sizeof(groupip));
	fprintf(stdout, "groupip: '%s'\n", groupip);

	strncpy(port, argv[3], sizeof(port));
	fprintf(stdout, "port: '%s'\n", port);


	// create a datagram socket on which to send
	fd_sock = lothars__socket(AF_INET, SOCK_DGRAM, 0);
	fprintf(stdout, "Opening the datagram socket...OK.\n");


	// initialize the group sockaddr structure with a group
	// address and port
	memset((char *) &saddr_group, 0, sizeof(saddr_group));
	saddr_group.sin_family = AF_INET;
	saddr_group.sin_addr.s_addr = inet_addr(groupip); // e.g. "226.1.1.1"
	saddr_group.sin_port = htons(atoi(port)); // e.g. 4321


/* disable loopback so you do not receive your own datagrams
	{
		char loopch = 0;
		lothars__setsockopt(fd_sock, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&loopch, sizeof(loopch));
		fprintf(stdout, "Disabling the loopback...OK.\n");
	}
// */

	// set local interface for outbound multicast datagrams; the
	// IP address specified must be associated with a local,
	// multicast capable interface
	lothars__inet_aton(senderip, &addr_local);
	lothars__setsockopt(fd_sock, IPPROTO_IP, IP_MULTICAST_IF, (char *)&addr_local, sizeof(addr_local));
	fprintf(stdout, "Setting the local interface...OK\n");


	// send a message to the multicast group specified by the saddr_group sockaddr structure.
	lothars__sendto(fd_sock, databuf, MAXLINE, 0, (struct sockaddr*)&saddr_group, sizeof(saddr_group));
	fprintf(stdout, "Sending datagram message...OK\n");


//* try the re-read from the socket if the loopback is not disable
	lothars__read(fd_sock, databuf, MAXLINE);
	fprintf(stdout, "Reading datagram message from client...OK\n");
	fprintf(stdout, "The message is: %s\n", databuf);
// */

	fprintf(stdout, "READY.\n");
	exit(EXIT_SUCCESS);
}
