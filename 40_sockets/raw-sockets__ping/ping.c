/*
  raw socket demo: ping

  This is an elaborated ping example. The achitecture is based on Unix
  Network Programming's ping demo.

  NB: ping needs to be executed with ROOT permissions.
*/

#include "ping.h"

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

/*
  host server - by getaddrinfo()

  Given node and service, which identify an Internet host and a
  service, getaddrinfo() returns one or more addrinfo structures, each
  of which contains an Internet address that can be specified in a
  call to bind(2) or connect(2). The getaddrinfo() function combines
  the functionality provided by the gethostbyname(3) and
  getservbyname(3) functions into a single interface, but unlike the
  latter functions, getaddrinfo() is reentrant and allows programs to
  eliminate IPv4-versus-IPv6 dependencies.

  @host: The hostname or IP.
  @serv: The servername or IP.
  @family: Can be 0, AF_INET, AF_INET6, etc.
  @socktype: Can be 0, SOCK_STREAM, SOCK_DGRAM, etc.

  Return pointer to first on linked list.
*/
struct addrinfo *lothars__host_serv(const char *host, const char *serv,
				    int family, int socktype)
{
	struct addrinfo hints, *res = NULL;
	int eai = -1;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_flags = AI_CANONNAME; // always return canonical name
	hints.ai_family = family; // 0, AF_INET, AF_INET6, etc.
	hints.ai_socktype = socktype; // 0, SOCK_STREAM, SOCK_DGRAM, etc
	if (0 != (eai = getaddrinfo(host, serv, &hints, &res))) {
		err_quit("%s() error for %s, %s: %s", __func__,
			 (host == NULL) ? "(no hostname)" : host,
			 (serv == NULL) ? "(no service name)" : serv,
			 gai_strerror(eai));
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
void lothars__setsockopt(int fd, int level, int optname, const void *optval,
			 socklen_t optlen)
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
void lothars__sendto(int fd, const void *ptr, size_t nbytes, int flags,
		     const struct sockaddr *sa, socklen_t salen)
{
	if ((ssize_t)nbytes != sendto(fd, ptr, nbytes, flags, sa, salen)) {
		err_sys("%s() error", __func__);
	}
}

/* unix */

/*
  signal_intr / sigaction - examine and change a signal action

  The sigaction() function allows the calling process to examine
  and/or specify the action to be associated with a specific
  signal. The argument sig specifies the signal; acceptable values are
  defined in <signal.h>.

  #include <signal.h>

  @signo: The signal number.
  @func: The signal handler function.
*/
Sigfunc *signal_intr(int signo, Sigfunc *func)
{
	struct sigaction act, oact;

	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0; // in case adjust

	if (0 > sigaction(signo, &act, &oact)) {
		return SIG_ERR;
	}
	return oact.sa_handler;
}
Sigfunc *lothars__signal_intr(int signo, Sigfunc *func)
{
	Sigfunc *sigfunc;
	if (SIG_ERR == (sigfunc = signal_intr(signo, func))) {
		err_sys("%s() error", __func__);
	}
	return sigfunc;
}

/*
  The malloc() function allocates size bytes and returns a pointer to
  the allocated memory. The memory is not initialized. If size is 0,
  then malloc() returns either NULL, or a unique pointer value that
  can later be successfully passed to free().

  #include <stdlib.h>

  @size: Size of the memory in byte to allocate.
*/
void *lothars__malloc(size_t size)
{
	void *ptr;
	if (NULL == (ptr = malloc(size))) {
		err_sys("%s() error", __func__);
	}
	return ptr;
}

/*
  The gettimeofday() function shall obtain the current time, expressed
  as seconds and microseconds since the Epoch, and store it in the
  timeval structure pointed to by tv. The resolution of the system
  clock is unspecified.

  #include <sys/time.h>

  @tv: Current time is stored in *tv.
*/
void lothars__gettimeofday(struct timeval *tv)
{
	if (-1 ==
	    gettimeofday(
		    tv,
		    NULL)) { // if 'tzp' is not a NULL pointer, the behavior is unspecified
		err_sys("%s() error", __func__);
	}
}

/* inet */

/*
  sock_ntop_host() - wrapper of inet_ntop() to return hostname.

  @sa: The server address object.
  @salen: The length of the server address object.

  Returns the host/ip.
*/
char *sock_ntop_host(const struct sockaddr *sa, socklen_t salen)
{
	static char str[128]; // Unix domain is largest

	switch (sa->sa_family) {
	case AF_INET: {
		struct sockaddr_in *sin = (struct sockaddr_in *)sa;
		if (NULL ==
		    inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str))) {
			return NULL;
		}
		return str;
	}

#ifdef IPV6
	case AF_INET6: {
		struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)sa;
		if (NULL ==
		    inet_ntop(AF_INET6, &sin6->sin6_addr, str, sizeof(str))) {
			return NULL;
		}
		return str;
	}
#endif

#ifdef AF_UNIX
	case AF_UNIX: {
		struct sockaddr_un *unp = (struct sockaddr_un *)sa;
		/*
		   OK to have no pathname bound to the socket: happens on
		   every connect() unless client calls bind() first.
		*/
		if (0 == unp->sun_path[0]) {
			strcpy(str, "(no pathname bound)");
		} else {
			snprintf(str, sizeof(str), "%s", unp->sun_path);
		}
		return str;
	}
#endif
	default:
		snprintf(str, sizeof(str),
			 "sock_ntop_host: unknown AF_xxx: %d, len %d",
			 sa->sa_family, salen);
		return str;
	}
	return NULL;
}
char *lothars__sock_ntop_host(const struct sockaddr *sa, socklen_t salen)
{
	char *ptr = NULL;
	if (NULL == (ptr = sock_ntop_host(sa, salen))) {
		err_sys("%s() error", __func__); // inet_ntop() sets errno
	}
	return ptr;
}

/*
  tv_sub

  Subract a timevalue (in) from a passed timevalue (out).

  @out: The timevalue to subtract from and holding the result.
  @in: The timevalue to subtract.
*/
void tv_sub(struct timeval *out, struct timeval *in)
{
	if (0 > (out->tv_usec -= in->tv_usec)) { // out -= in
		--out->tv_sec;
		out->tv_usec += 1000000;
	}
	out->tv_sec -= in->tv_sec;
}

/********************************************************************************************/
// worker implementation

/*
  IPv4 handler

  ICMP protocol:

  |                             len                             |
  +------------------------------+------------------------------+
  |             hlen1            |           icmplen            |
  +---------------+--------------+--------------+---------------+
  | IPv4 header   | IPv4 options | ICMPv4       | ICMP          |
  +---------------+--------------+--------------+---------------+
  | 20 bytes      | 0 - 40 bytes | 8 bytes                      |
  +---------------+--------------+------------------------------+

*/
void proc_v4(char *ptr, ssize_t len, struct msghdr *msg,
	     struct timeval *tv_recv)
{
	int32_t hlen1, icmplen;
	double rtt; // round trip time
	struct ip *ip = NULL;
	struct icmp *icmp = NULL;
	struct timeval *tv_send = NULL;

	setbuf(stdout, NULL); // make stdout unbuffered

	// init with start of IP header
	ip = (struct ip *)ptr;

	// length of IP header
	hlen1 = ip->ip_hl << 2;
	if (ip->ip_p != IPPROTO_ICMP) {
		// no ICMP protocol
		return;
	}

	// init with start of ICMP header
	icmp = (struct icmp *)(ptr + hlen1);
	if (8 > (icmplen = len - hlen1)) {
		// malformed packet
		return;
	}

	if (ICMP_ECHOREPLY == icmp->icmp_type) {
		if (pid != icmp->icmp_id) {
			// not a response to our ECHO_REQUEST
			return;
		}

		if (16 > icmplen) {
			// not enough data to use
			return;
		}

		tv_send = (struct timeval *)icmp->icmp_data;
		tv_sub(tv_recv, tv_send);

		// calculate round trip time
		rtt = tv_recv->tv_sec * 1000.0 + tv_recv->tv_usec / 1000.0;

		fprintf(stdout,
			"%d bytes from %s: seq=%u, ttl=%d, rtt=%.3f ms",
			icmplen,
			lothars__sock_ntop_host(proto->sa_recv, proto->salen),
			icmp->icmp_seq, ip->ip_ttl, rtt);

	}
	if (verbose) {
		fprintf(stdout, ", type=%d, code=%d\n",
			icmp->icmp_type, icmp->icmp_code);
	} else {
		fprintf(stderr, "\n");
	}
}

/*
  init IPv6 handler
*/
void init_v6()
{
#ifdef IPV6
	const int8_t on = 1;

	if (0 == verbose) {
		// install a filter that only passes ICMP6_ECHO_REPLY unless verbose
		struct icmp6_filter v6_filter;
		ICMP6_FILTER_SETBLOCKALL(&v6_filter);
		ICMP6_FILTER_SETPASS(ICMP6_ECHO_REPLY, &v6_filter);
		setsockopt(fd_sock, IPPROTO_IPV6, ICMP6_FILTER, &v6_filter,
			   sizeof(v6_filter));
		// ignore error return, the filter is an optimization
	}

	// ignore error returned below, we just won't receive the hop limit
#ifdef IPV6_RECVHOPLIMIT
	// RFC 3542
	lothars__setsockopt(fd_sock, IPPROTO_IPV6, IPV6_RECVHOPLIMIT, &on,
			    sizeof(on));
#else
	// RFC 2292
	lothars__setsockopt(fd_sock, IPPROTO_IPV6, IPV6_HOPLIMIT, &on,
			    sizeof(on));
#endif
#endif
}

/*
  IPv6 handler
*/
void proc_v6(char *ptr, ssize_t len, struct msghdr *msg,
	     struct timeval *tv_recv)
{
#ifdef IPV6
	double rtt; // round trip time
	struct icmp6_hdr *icmp6 = NULL;
	struct timeval *tv_send = NULL;
	struct cmsghdr *cmsg = NULL;
	int32_t hlim;

	icmp6 = (struct icmp6_hdr *)ptr;

	if (8 > len) {
		// malformed packet
		return;
	}

	if (ICMP6_ECHO_REPLY == icmp6->icmp6_type) {
		if (pid != icmp6->icmp6_id) {
			// not a response to our ECHO_REQUEST
			return;
		}

		if (16 > len) {
			// not enough data to use
			return;
		}

		tv_send = (struct timeval *)(icmp6 + 1);
		tv_sub(tv_recv, tv_send);
		rtt = tv_recv->tv_sec * 1000.0 + tv_recv->tv_usec / 1000.0;

		hlim = -1;
		for (cmsg = CMSG_FIRSTHDR(msg); cmsg != NULL;
		     cmsg = CMSG_NXTHDR(msg, cmsg)) {
			if ((cmsg->cmsg_level == IPPROTO_IPV6) &&
			    (cmsg->cmsg_type == IPV6_HOPLIMIT)) {
				hlim = *(u_int32_t *)CMSG_DATA(cmsg);
				break;
			}
		}

		fprintf(stdout, "%d bytes from %s: seq=%u, hlim=", len,
			lothars__sock_ntop_host(proto->sa_recv, proto->salen),
			icmp6->icmp6_seq);

		if (-1 == hlim) {
			// ancilliary data missing
			fprintf(stdout, "???");
		} else {
			fprintf(stdout, "%d", hlim);
		}

		fprintf(stdout, ", rtt=%.3f ms\n", rtt);

	} else if (verbose) {
		fprintf(stdout, " %d bytes from %s: type=%d, code=%d\n", len,
			lothars__sock_ntop_host(proto->sa_recv, proto->salen),
			icmp6->icmp6_type, icmp6->icmp6_code);
	}
#endif
}

/*
  send - IPv4
*/
void send_v4()
{
	int32_t len;
	struct icmp *icmp = NULL;

	icmp = (struct icmp *)sendbuf;
	icmp->icmp_type = ICMP_ECHO;
	icmp->icmp_code = 0;
	icmp->icmp_id = pid;
	icmp->icmp_seq = ++n_sent; // sequence numbers..

	// fill with pattern
	memset(icmp->icmp_data, 0xa5, datalen);

	lothars__gettimeofday((struct timeval *)icmp->icmp_data);

	// checksum ICMP header and data
	len = 8 + datalen;
	icmp->icmp_cksum = 0;
	icmp->icmp_cksum = checksum((u_int16_t *)icmp, len);

	lothars__sendto(fd_sock, sendbuf, len, 0, proto->sa_send, proto->salen);
}

/*
  calculate checksum
*/
uint16_t checksum(uint16_t *addr, int32_t len)
{
	register int32_t n_left = len;
	register uint32_t sum = 0;
	register uint16_t *word = addr;
	uint16_t answer = 0;

	// our algorithm is simple, using a 32 bit accumulator (sum),
	// we add sequential 16 bit words to it, and at the end, fold
	// back all the carry bits from the top 16 bits into the lower
	// 16 bits
	while (1 < n_left) {
		sum += *word++;
		/*
		  C's fetch-and-advance

		  The expression "*word++" means:
		  1. step
		  sum += *word;

		  2. step
		  ++word;
		*/
		n_left -= 2;
	}

	// move up an odd byte, if necessary
	if (1 == n_left) {
		*(unsigned char *)(&answer) = *(unsigned char *)word;
		sum += answer;
	}

	// add back carry outs from top 16 bits to low 16 bits

	// add hi 16 to low 16
	sum = (sum >> 16) + (sum & 0xffff);

	// add carry
	sum += (sum >> 16);

	// truncate to 16 bits
	answer = ~sum;

	return answer;
}

/*
  send - IPv6
*/
void send_v6()
{
#ifdef IPV6
	int32_t len;
	struct icmp6_hdr *icmp6 = NULL;

	icmp6 = (struct icmp6_hdr *)sendbuf;
	icmp6->icmp6_type = ICMP6_ECHO_REQUEST;
	icmp6->icmp6_code = 0;
	icmp6->icmp6_id = pid;
	icmp6->icmp6_seq = ++n_sent;

	// fill with pattern
	memset((icmp6 + 1), 0xa5, datalen);
	lothars__gettimeofday((struct timeval *)(icmp6 + 1));

	// 8-byte ICMPv6 header
	len = 8 + datalen;

	lothars__sendto(fd_sock, sendbuf, len, 0, proto->sa_send, proto->salen);

	// kernel claculates and stores checksum for us
#endif
}

/*
  read loop
*/
void readloop()
{
	int32_t size;
	char recvbuf[BUFSIZE]; // the receiving buffer
	char controlbuf[BUFSIZE];
	struct msghdr msg;
	struct iovec iov;
	ssize_t cnt;
	struct timeval tval;

	// create raw socket, using the specified protocol family
	fd_sock = lothars__socket(proto->sa_send->sa_family, SOCK_RAW,
				  proto->icmp_proto);

	// don't need special permissions any more
	setuid(getuid());

	if (proto->finit) { // opt: call init()
		(*proto->finit)();
	}

	// OK if setsockopt fails..
	size = 60 * 1024;
	lothars__setsockopt(fd_sock, SOL_SOCKET, SO_RCVBUF, &size,
			    sizeof(size));

	// send first packet
	sig_alrm(SIGALRM);

	// reading 1: set the iov pointer on the address of the "receiving buffer",
	// in order to read the stream into the "receiving buffer"
	iov.iov_base = recvbuf;
	iov.iov_len = sizeof(recvbuf);

	msg.msg_name = proto->sa_recv;

	// reading 2: setting the io vector's address to the message header structure
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = controlbuf;

	while (1) {
		msg.msg_namelen = proto->salen;
		msg.msg_controllen = sizeof(controlbuf);

		// reading 3: actual reading in-loop, writing into the "msg" structure's io vector
		if (0 > (cnt = recvmsg(fd_sock, &msg, 0))) {
			if (errno == EINTR)
				continue;
			// only an interrupt was caught..
			//
			// it cannot be the common recvmsg()
			// wrapper, when EINTR shall be
			// ignored
			else
				err_sys("recvmsg error");
		}

		lothars__gettimeofday(&tval);

		// reading 4: parsing the information in "fproc" function...
		// call "procedure" function pointer with arguments (within while loop!)
		(*proto->fproc)(recvbuf, cnt, &msg, &tval); // call process()
	}
}

/*
  mini signal handler

  sends packets actually timed in 1 sec intervals
*/
void sig_alrm(int32_t signo)
{
	(*proto->fsend)();
	alarm(1);
}

/*
  init globally the used protocol structures
*/
struct protocol proto_v4 = {
	proc_v4, send_v4, NULL, NULL, NULL, 0, IPPROTO_ICMP
};
#ifdef IPV6
struct protocol proto_v6 = { proc_v6, send_v6, init_v6,       NULL,
			     NULL,    0,       IPPROTO_ICMPV6 };
#endif

// data that goes with ICMP echo request
int32_t datalen = 56;

/********************************************************************************************/

/*
  main
*/
int main(int argc, char *argv[])
{
	int32_t cnt;
	struct addrinfo *ai = NULL;
	char *pchr = NULL;

	// don't want getopt() writing to stderr
	opterr = 0;
	while (-1 != (cnt = getopt(argc, argv, "v"))) {
		switch (cnt) {
		case 'v':
			++verbose;
			break;

		case '?':
			err_quit("unrecognized option: %c", cnt);
		}
	}

	if (optind != argc - 1) {
		err_quit("usage:\n$ sudo %s [ -v ] <hostname>", argv[0]);
	}
	host = argv[optind];

	// ICMP ID field is 16 bits
	pid = getpid() & 0xffff;

	// set signal handler - to actually SEND the packets
	lothars__signal_intr(SIGALRM, sig_alrm);

	// set address information (==ai)
	ai = lothars__host_serv(host, NULL, 0, 0);

	pchr = lothars__sock_ntop_host(ai->ai_addr, ai->ai_addrlen);
	fprintf(stdout, "PING %s (%s): %d data bytes\n",
		(ai->ai_canonname ? ai->ai_canonname : pchr), pchr, datalen);

	// initialize sending pointer "proto" according to the used protocol
	if (ai->ai_family == AF_INET) {
		proto = &proto_v4;
#ifdef IPV6
	} else if (ai->ai_family == AF_INET6) {
		proto = &proto_v6;
		if (IN6_IS_ADDR_V4MAPPED(&(
			    ((struct sockaddr_in6 *)ai->ai_addr)->sin6_addr))) {
			err_quit("cannot ping IPv4-mapped IPv6 address");
		}
#endif
	} else {
		err_quit("unknown address family %d", ai->ai_family);
	}

	// start prg
	proto->sa_send = ai->ai_addr;
	proto->sa_recv = lothars__malloc(ai->ai_addrlen);
	proto->salen = ai->ai_addrlen;

	readloop();

	exit(EXIT_SUCCESS);
}
