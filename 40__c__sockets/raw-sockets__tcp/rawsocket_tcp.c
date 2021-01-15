/*
  Demo: TCP on raw socket

  The demo shows sending a reimplemented TCP header packet through a
  raw socket. Just datagram, no payload/data.

  The snippet shows how to configure source IP, source port, target
  IP, target port from the command line arguments.


  NB: Execution needs ROOT permissions!

  ---
  References:
  Unix Network Programming, Stevens (1996)
  https://beej.us/guide/bgnet/html/
  idea from https://www.tenouk.com/Module43a.html
*/
//---cat rawtcp.c---

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h> /* va_start, va_end,... */
#include <string.h>
#include <errno.h>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <sys/socket.h>


// TODO rm
//// Run as root or SUID 0, just datagram no data/payload
//#include <unistd.h>
//#include <stdio.h>
//#include <sys/socket.h>
//#include <netinet/ip.h>
//#include <netinet/tcp.h>

/*
  constants
*/

#define PACKET_BUFFER_SIZE 8192 /* size of the "poor man's socket buffer" */
#define MAXLINE  4096 /* max text line length */


/*
  forwards
*/

// error
void err_sys(const char *, ...);
void err_quit(const char *, ...);

// socket
void lothars__sendto(int, const void *, size_t, int, const struct sockaddr *, socklen_t);
void lothars__setsockopt(int, int, int, const void *, socklen_t);
int lothars__socket(int, int, int);

// inet
void lothars__inet_pton(int, const char*, void*);

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


/* error */

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


/* socket */

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
		close(fd); /* strict */
		sync();
		err_sys("%s() error", __func__);
	}
}


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
  @strptr: A pointer to a source string to be converted (src).
  @addrptr: The pointer to a destination

  Returns 1 on success, 0 if strptr did not contain a valid internet
  address and -1 if family was not a valid address family.
*/
void lothars__inet_pton(int family, const char *strptr, void *addrptr)
{
	int res;
	if (0 > (res = inet_pton(family, strptr, addrptr))) {
		err_sys("%s() error for %s (check the passed address family?)", __func__, strptr); // errno set
	} else if (0 == res) {
		err_quit("%s() error for %s (check the passed strptr)", __func__, strptr); // errno not set
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


// fabricated IP header
struct ipheader {
	uint8_t  iph_ihl:5, iph_ver:4; // little endian
	uint8_t  iph_tos;
	uint16_t iph_len;
	uint16_t iph_ident;
	uint8_t  iph_flags;
	uint16_t iph_offset;
	uint8_t  iph_ttl;
	uint8_t  iph_protocol;
	uint16_t iph_chksum;
	uint32_t iph_sourceip;
	uint32_t iph_destip;
};


/* fabricated TCP header */
struct tcpheader {
	uint16_t tcph_srcport;
	uint16_t tcph_destport;
	uint32_t tcph_seqnum;
	uint32_t tcph_acknum;
	uint8_t  tcph_reserved:4, tcph_offset:4;
	// unsigned char tcph_flags;
	uint32_t tcp_res1:4,    // little-endian
		tcph_hlen:4,    // length of tcp header in 32-bit words
		tcph_fin:1,     // finish flag "fin"
		tcph_syn:1,     // synchronize sequence numbers to start a connection
		tcph_rst:1,     // reset flag
		tcph_psh:1,     // push, sends data to the application
		tcph_ack:1,     // acknowledge
		tcph_urg:1,     // urgent pointer
		tcph_res2:2;
	uint16_t tcph_win;
	uint16_t tcph_chksum;
	uint16_t tcph_urgptr;
};


/*
  Computes a checksum over a buffer of nbytes length.

  - The total udp header length: 8 bytes (=64 bits)
  - Algorithm by RFC:
      "The checksum field is the 16 bit one's complement of the one's
      complement sum of all 16 bit words in the header.  For purposes
      of computing the checksum, the value of the checksum field is
      zero."

  @buf: The buffer to compute the checksum.
  @nbytes: The size of the buffer.

  Returns the checksum over the buffer.
*/
unsigned short checksum(unsigned short *buf, int nbytes)

{
	unsigned long sum;
	for (sum=0; nbytes>0; nbytes--) {
		sum += *buf++;
	}
	sum = (sum >> 16) + (sum &0xffff);
	sum += (sum >> 16);
	return (unsigned short)(~sum);
}


/********************************************************************************************/


/*
  main
*/
int main(int argc, char *argv[])
{
	int fd_sock;
	char packet_buffer[PACKET_BUFFER_SIZE]; // no payload for this demo
	struct ipheader *ip = NULL;
	struct tcpheader *tcp = NULL;
	struct sockaddr_in din;
	int cnt = 0;
	int on = 1;
	const int *val = &on;
	char source_ip[16]; memset(source_ip, '\0', sizeof(source_ip));
	char source_port[16]; memset(source_port, '\0', sizeof(source_port));
	char destination_ip[16]; memset(destination_ip, '\0', sizeof(destination_ip));
	char destination_port[16]; memset(destination_port, '\0', sizeof(destination_port));

	// init
	memset(packet_buffer, '\0', sizeof(packet_buffer));
	ip = (struct ipheader *) packet_buffer;
	tcp = (struct tcpheader *) (packet_buffer + sizeof(struct ipheader));

	// input processing
	if (argc != 5) {
		fprintf(stderr, "Usage:\n");
		fprintf(stderr, "$ %s <source ip> <source port> <destination ip> <destination port>\n", argv[0]);
		fprintf(stderr, "e.g.\n$ sudo %s 127.0.0.1 21 192.168.123.123 8080 \n", argv[0]);
		exit(EXIT_FAILURE);
	}

	strncpy(source_ip, argv[1], sizeof(source_ip)-1);
	fprintf(stdout, "source_ip: %s\n", source_ip);

	strncpy(source_port, argv[2], sizeof(source_port)-1);
	fprintf(stdout, "source_port: %s\n", source_port);

	strncpy(destination_ip, argv[3], sizeof(destination_ip)-1);
	fprintf(stdout, "destination_ip: %s\n", destination_ip);

	strncpy(destination_port, argv[4], sizeof(destination_port)-1);
	fprintf(stdout, "destination_port: %s\n", destination_port);


	/* raw socket setup */
	fd_sock = lothars__socket(PF_INET, SOCK_RAW, IPPROTO_TCP);
	fprintf(stdout, "socket() - using SOCK_RAW and tcp protocol.\n");


	/* packet headers */
	ip->iph_ihl = 5;
	ip->iph_ver = 4;
	ip->iph_tos = 16;
	ip->iph_len = sizeof(struct ipheader) + sizeof(struct tcpheader);
	ip->iph_ident = htons(54321);
	ip->iph_offset = 0;
	ip->iph_ttl = 64;
	ip->iph_protocol = 6; // TCP
	ip->iph_chksum = 0; // done by kernel
	ip->iph_sourceip = inet_addr(source_ip); // source ip can be set and also spoofed here!!!
	ip->iph_destip = inet_addr(destination_ip); // destination ip can be set and also spoofed here!!!

	tcp->tcph_srcport = htons(atoi(source_port)); // change souerce port as needed
	tcp->tcph_destport = htons(atoi(destination_port));
	tcp->tcph_seqnum = htonl(1);
	tcp->tcph_acknum = 0;
	tcp->tcph_offset = 5;
	tcp->tcph_syn = 1;
	tcp->tcph_ack = 0;
	tcp->tcph_win = htons(32767);
	tcp->tcph_chksum = 0; // done by kernel
	tcp->tcph_urgptr = 0;

	ip->iph_chksum = checksum((unsigned short *) packet_buffer, sizeof(*ip) + sizeof(*tcp)); // integrity check


	/* kernel */

	// let the kernel know not to fill up the packet structure -
	// we build our own
	lothars__setsockopt(fd_sock, IPPROTO_IP, IP_HDRINCL, val, sizeof(on));
	fprintf(stdout, "setsockopt()...OK\n");


	/* send loop */
	fprintf(stdout, "Using:::::Source IP: %s port: %u, Target IP: %s port: %u.\n"
		, source_ip, atoi(source_port), destination_ip, atoi(destination_port));

	din.sin_family = AF_INET; // prepare sendto() destination
	din.sin_port = htons(atoi(destination_port));
	lothars__inet_pton(AF_INET, destination_ip, &din.sin_addr.s_addr);
	for (cnt = 0; cnt < 5; ++cnt) {
		lothars__sendto(fd_sock, packet_buffer, ip->iph_len, 0, (struct sockaddr *)&din, sizeof(din));
		fprintf(stdout, "Count #%u - sendto() is OK\n", cnt);
		sleep(1);
	}


	lothars__close(&fd_sock);
	fprintf(stdout, "READY.\n");
	exit(EXIT_SUCCESS);
}
