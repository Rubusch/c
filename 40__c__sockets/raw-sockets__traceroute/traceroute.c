/*
  traceroute

  A traceroute implementation via raw sockets (receive) and udp
  sockets (send). The packets are send via udp, implementing
  timestamp, sequence numbers and ttl, the ICMP answer is caught via
  raw socket, timestamped and registered. Typical traceroute output.

  NB: Needs ROOT permissions for execution.

  ---
  References:
  Unix Network Programming, Stevens (1996)

  FIXME: no diference in options -m and -v -> check
*/

#include "traceroute.h"


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
struct addrinfo* lothars__host_serv(const char *host, const char *serv, int family, int socktype)
{
	struct addrinfo hints, *res = NULL;
	int eai = -1;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_flags = AI_CANONNAME; // always return canonical name
	hints.ai_family = family;  // 0, AF_INET, AF_INET6, etc.
	hints.ai_socktype = socktype; // 0, SOCK_STREAM, SOCK_DGRAM, etc
	if (0 != (eai = getaddrinfo(host, serv, &hints, &res))) {
		err_quit("%s() error for %s, %s: %s", __func__
			 , (host == NULL) ? "(no hostname)" : host
			 , (serv == NULL) ? "(no service name)" : serv
			 , gai_strerror(eai));
	}

	return res;
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
Sigfunc* signal_intr(int signo, Sigfunc *func)
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
Sigfunc* lothars__signal_intr(int signo, Sigfunc *func)
{
	Sigfunc *sigfunc;
	if (SIG_ERR == (sigfunc = signal_intr(signo, func))) {
		err_sys("%s() error", __func__);
	}
	return sigfunc;
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
	if (-1 == gettimeofday(tv, NULL)) { // if 'tzp' is not a NULL pointer, the behavior is unspecified
		err_sys("%s() error", __func__);
	}
}


/*
  The malloc() function allocates size bytes and returns a pointer to
  the allocated memory. The memory is not initialized. If size is 0,
  then malloc() returns either NULL, or a unique pointer value that
  can later be successfully passed to free().

  #include <stdlib.h>

  @size: Size of the memory in byte to allocate.
*/
void* lothars__malloc(size_t size)
{
	void *ptr;
	if (NULL == (ptr = malloc(size))) {
		err_sys("%s() error", __func__);
	}
	return ptr;
}


/* inet */

/*
  sock_ntop_host() - wrapper of inet_ntop() to return hostname.

  inet_ntop() extends the inet_ntoa(3) function to support multiple
  address families, inet_ntoa(3) is now considered to be deprecated in
  favor of inet_ntop().

  @sa: The server address object.
  @salen: The length of the server address object.

  Returns the host/ip.
*/
char* sock_ntop_host(const struct sockaddr *sa, socklen_t salen)
{
	static char str[128];  // Unix domain is largest

	switch (sa->sa_family){
	case AF_INET:
	{
		struct sockaddr_in *sin = (struct sockaddr_in *) sa;
		if (NULL == inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str))) {
			return NULL;
		}
		return str;
	}

#ifdef IPV6
	case AF_INET6:
	{
		struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *) sa;
		if (NULL == inet_ntop(AF_INET6, &sin6->sin6_addr, str, sizeof(str))) {
			return NULL;
		}
		return str;
	}
#endif

#ifdef AF_UNIX
	case AF_UNIX:
	{
		struct sockaddr_un *unp = (struct sockaddr_un *) sa;
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
		snprintf(str, sizeof(str), "sock_ntop_host: unknown AF_xxx: %d, len %d", sa->sa_family, salen);
		return str;
	}
	return NULL;
}
char* lothars__sock_ntop_host(const struct sockaddr *sa, socklen_t salen)
{
	char *ptr = NULL;
	if (NULL == (ptr = sock_ntop_host(sa, salen))) {
		err_sys("%s() error", __func__); // inet_ntop() sets errno
	}
	return ptr;
}


/* snippets socket */

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


/*
  sock_set_port()

  Obtain the specific struct sockaddr_?? instance and initialize the
  specific member variable with the port number. The function
  represents a generic interface for initializing AF_INET,
  AF_INET6,... and others.

  @sa: The generic struct sockaddr instance.
  @salen: The size of the sockaddr instance.
  @port: The port number to set.
*/
void sock_set_port(struct sockaddr *sa, socklen_t salen, int port)
{
	switch (sa->sa_family) {
	case AF_INET:
	{
		struct sockaddr_in *sin = (struct sockaddr_in *) sa;
		sin->sin_port = port;
		return;
	}

#ifdef IPV6
	case AF_INET6:
	{
		struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *) sa;
		sin6->sin6_port = port;
		return;
	}
#endif
	}
}


/*
  sock_cmp_addr()

  Comparator of the addresses of two sockaddr instances. They must
  have the same address family. The function implements a generic
  frontend to compare address instances for various families, AF_INET,
  AF_INET6, AF_UNIX...

  @sa1: The first struct sockaddr instance.
  @sa2: The second struct sockaddr instance.
  @salen: The size of the struct sockaddr instance(s).

  Returns a memcmp() comparison: The memcmp() function returns an
  integer less than, equal to, or greater than zero if the first n
  bytes of s1 is found, respectively, to be less than, to match, or be
  greater than the first n bytes of s2. I.e. 0 means both are equal.
*/
int sock_cmp_addr(const struct sockaddr *sa1, const struct sockaddr *sa2, socklen_t salen)
{
	if (sa1->sa_family != sa2->sa_family) {
		return -1;
	}

	switch (sa1->sa_family) {
	case AF_INET:
	{
		return memcmp(&((struct sockaddr_in *) sa1)->sin_addr,
			      &((struct sockaddr_in *) sa2)->sin_addr,
			      sizeof(struct in_addr));
	}

#ifdef IPV6
	case AF_INET6:
	{
		return memcmp(&((struct sockaddr_in6 *) sa1)->sin6_addr,
			      &((struct sockaddr_in6 *) sa2)->sin6_addr,
			      sizeof(struct in6_addr));
	}
#endif

#ifdef AF_UNIX
	case AF_UNIX:
	{
		return strcmp(((struct sockaddr_un *) sa1)->sun_path,
			      ((struct sockaddr_un *) sa2)->sun_path);
	}
#endif
	}
	return -1;
}


/********************************************************************************************/
// worker implementation

/* init strategy object */
struct protocol proto_v4 = { icmpcode_v4, recv_v4, NULL, NULL, NULL, NULL, 0, IPPROTO_ICMP, IPPROTO_IP, IP_TTL };
#ifdef IPV6
struct protocol proto_v6 = { icmpcode_v6, recv_v6, NULL, NULL, NULL, NULL, 0, IPPROTO_ICMPV6, IPPROTO_IPV6, IPV6_UNICAST_HOPS };
#endif


/* file wide globals in .c file to hide them from outside */

int datalen = sizeof(struct rec);

int ttl; // actual time to live (ttl)
int max_ttl = 30;
int verbose;

int probe; // actual probe counter
int n_probes = 3; // number of probes to take, typically e.g. 3

// source and destination ports
uint16_t d_port = 32768 + 666;
uint16_t s_port = 0;

// own pid
pid_t pid;

char recvbuf[BUFSIZE];
char sendbuf[BUFSIZE];

// number of data bytes following the ICMP header
char *host=NULL;

// increment for each sendto()
int gotalarm;
int n_sent;

// send on UDP sock, read on raw ICMP sock
int fd_send; // socket file descriptor: udp socket
int fd_recv; // socket file descriptor: raw socket



/*
  set flag to note that alarm occured and interrupt the recvfrom
*/
void sig_alrm(int signo)
{
	gotalarm = 1;
}


/*
  The game loop performing the traceroute stored in the strategy
  object.
*/
void worker_routine(void)
{
	int seq=-1, code=-1, done=-1;
	double rtt=0.0;
	struct rec *rec=NULL;
	struct timeval tv_recv;

	// generate raw socket for receiving
	fd_recv = lothars__socket(proto->sa_send->sa_family, SOCK_RAW, proto->icmp_proto);

	// don't need special permissions anymore
	setuid(getuid());

#ifdef IPV6
	if ((proto->sa_send->sa_family == AF_INET6) && (verbose == 0)) {
		struct icmp6_filter v6_filter;
		ICMP6_FILTER_SETBLOCKALL(&v6_filter);
		ICMP6_FILTER_SETPASS(ICMP6_TIME_EXCEEDED, &v6_filter);
		ICMP6_FILTER_SETPASS(ICMP6_DST_UNREACH, &v6_filter);
		setsockopt(fd_recv, IPPROTO_IPV6, ICMP6_FILTER, &v6_filter, sizeof(v6_filter));  
	}
#endif

	// create UDP socket for sending
	fd_send = lothars__socket(proto->sa_send->sa_family, SOCK_DGRAM, 0);

	// make sure the same family for bind and send
	proto->sa_bind->sa_family = proto->sa_send->sa_family;

	// our source UDP port: the udp socket is bound to an
	// interface and port for sending
	s_port = (getpid() & 0xffff) | 0x8000;
	sock_set_port(proto->sa_bind, proto->sa_len, htons(s_port));
	lothars__bind(fd_send, proto->sa_bind, proto->sa_len);

	sig_alrm(SIGALRM);

	seq = 0;
	done = 0;
	for (ttl=1; (ttl<=max_ttl) && (done == 0); ++ttl) {
		lothars__setsockopt(fd_send, proto->ttl_level, proto->ttl_optname, &ttl, sizeof(ttl));
		memset(proto->sa_last, 0, proto->sa_len);
		fprintf(stdout, "%2d ", ttl);
		fflush(stdout);

		for (probe=0; probe<n_probes; ++probe) {
			rec = (struct rec*) sendbuf;
			rec->rec_seq = ++seq; // use sequence numbers
			rec->rec_ttl = ttl;
			lothars__gettimeofday(&rec->rec_tv); // use timestamp

			sock_set_port(proto->sa_send, proto->sa_len, htons(d_port + seq));
			lothars__sendto(fd_send, sendbuf, datalen, 0, proto->sa_send, proto->sa_len);

			// now, call receiver function (depends on IPv4, IPv6,...)
			if (-3 == (code = (*proto->recv) (seq, &tv_recv))) {
				// timeout, no reply
				fprintf(stdout, " *");

			} else {
				char str[NI_MAXHOST];
				if (0 != sock_cmp_addr(proto->sa_recv, proto->sa_last, proto->sa_len)) {
					if (0 == getnameinfo(proto->sa_recv, proto->sa_len, str, sizeof(str), NULL, 0, 0)) {
						fprintf(stdout, " %s (%s)", str, lothars__sock_ntop_host(proto->sa_recv, proto->sa_len));
					} else {
						fprintf(stdout, " %s", lothars__sock_ntop_host(proto->sa_recv, proto->sa_len));
					}
					memcpy(proto->sa_last, proto->sa_recv, proto->sa_len);
				}
				tv_sub(&tv_recv, &rec->rec_tv);
				rtt = tv_recv.tv_sec * 1000.0 + tv_recv.tv_usec / 1000.0;
				fprintf(stdout, " %.3f ms", rtt);

				// prot unreachable, at destination
				if (code == -1) {
					++done;
				} else if (0 <= code) {
					fprintf(stdout, " (ICMP %s)", (*proto->icmpcode) (code)); // print icmp code
				}
			}
			fflush(stdout); // "real time" output
		}
		fprintf(stdout, "\n");
	}
}


/*
  IPv4 receive function, inits the tv instance with the timestamp

  Returns
  -3 on timeout
  -2 on ICMP time exceeded in transit (caller keeps going)
  -1 on ICMP prot unreachable (caller is done)
  >= 0 return value is some other ICMP unreachable code
*/
int recv_v4(int seq, struct timeval* tv)
{
	int hlen1, hlen2, icmplen, ret;
	socklen_t len;
	ssize_t cnt;
	struct ip *ip=NULL, *ip_host=NULL;
	struct icmp *icmp=NULL;
	struct udphdr *udp=NULL;

	gotalarm = 0; // global
	alarm(3);
	while (1) {
		// alarm expired
		if (gotalarm) {
			return -3;
		}

		len = proto->sa_len;
		cnt = recvfrom(fd_recv, recvbuf, sizeof(recvbuf), 0, proto->sa_recv, &len);
		if (cnt < 0) {
			if (errno == EINTR) { // deal with interrupt (ignore)
				continue;
			} else {
				err_sys("recvfrom error");
			}
		}

		// start reading IP header
		ip = (struct ip*) recvbuf;
		hlen1 = ip->ip_hl << 2; // length of IP header

		// start of ICMP header
		icmp = (struct icmp*) (recvbuf + hlen1);
		if (8 > (icmplen = cnt - hlen1)) {
			// not enough to look at ICMP header
			continue;
		}

		// evaluation
		if ((icmp->icmp_type == ICMP_TIMXCEED) && (icmp->icmp_code == ICMP_TIMXCEED_INTRANS)) {
			if (icmplen < 8 + sizeof(struct ip)) {
				// not enough data to look at inner IP
				continue;
			}

			ip_host = (struct ip*) (recvbuf + hlen1 + 8);
			hlen2 = ip_host->ip_hl << 2;
			if (icmplen < 8 + hlen2 + 4) {
				// not enough data to look at UDP ports
				continue;
			}

			udp = (struct udphdr*) (recvbuf + hlen1 + 8 + hlen2);
			if ((ip_host->ip_p == IPPROTO_UDP)
			   && (udp->source == htons(s_port))
			   && (udp->dest == htons(d_port)) ) {
				// we hit an intermediate router
				ret = -2;
				break;
			}

		} else if (icmp->icmp_type == ICMP_UNREACH) {
			if (icmplen < 8 + sizeof(struct ip)) {
				// not enough data to look at inner IP
				continue;
			}

			ip_host = (struct ip*) (recvbuf + hlen1 + 8);
			hlen2 = ip_host->ip_hl << 2;
			if (icmplen < 8 + hlen2 + 4) {
				// not enough data to look at UDP ports
				continue;
			}

			udp = (struct udphdr*) (recvbuf + hlen1 + 8 + hlen2);
			if ((ip_host->ip_p == IPPROTO_UDP)
			   && (udp->source == htons(s_port))
			   && (udp->dest = htons(d_port + seq))) {

				if (icmp->icmp_code == ICMP_UNREACH_PORT) {
					// have reached destination
					ret = -1;
				} else {
					// 0, 1, 2, ...
					ret = icmp->icmp_code;
				}
				break;
			}
		}
		if (verbose) {
			fprintf(stdout, " (from %s: type=%d, code %d)\n"
			       , lothars__sock_ntop_host(proto->sa_recv, proto->sa_len)
			       , icmp->icmp_type
			       , icmp->icmp_code);
		}
		// some other ICMP error, recvfrom() again
	}

	// don't leave alarm running
	alarm(0);

	// get time of packet arrival
	lothars__gettimeofday(tv);

	return ret;
}


/*
  receive function, returns
  -3 on timeout
  -2 on ICMP time exceeded in transit (caller keeps going)
  -1 on ICMP prot unreachable (caller is done)
  >= 0 return value is some other ICMP unreachable code
*/
int recv_v6(int seq, struct timeval *tv)
{
	int ret=-1;
#ifdef IPV6
	int hlen2, icmp6len;
	ssize_t cnt;
	socklen_t len;
	struct ip6_hdr *ip_host6=NULL;
	struct icmp6_hdr *icmp6=NULL;
	struct udphdr *udp=NULL;

	gotalarm = 0;
	alarm(3);
	while (1) {
		if (gotalarm) {
			// alarm expired
			return -3;
		}

		len = proto->sa_len;
		if (0 > (cnt = recvfrom(fd_recv, recvbuf, sizeof(recvbuf), 0, proto->sa_recv, &len))) {
			if (errno == EINTR) {
				continue;
			} else {
				err_sys("recvfrom error");
			}
		}

		// ICMP header
		icmp6 = (struct icmp6_hdr*) recvbuf;
		if (8 > (icmp6len = cnt)) {
			// not enough to look at ICMP header
			continue;
		}

		if ( (icmp6->icmp6_type == ICMP6_TIME_EXCEEDED)
		    && (icmp6->icmp6_code == ICMP6_TIME_EXCEED_TRANSIT)) {

			if (icmp6len < (8 + sizeof(struct ip6_hdr) + 4)) {
				// not enough data to look at inner header
				continue;
			}

			ip_host6 = (struct ip6_hdr*) (recvbuf + 8);
			hlen2 = sizeof(struct ip6_hdr);
			udp = (struct udphdr*) (recvbuf + 8);
			if ((ip_host6->ip6_nxt == IPPROTO_UDP)
			   && (udp->source == htons(s_port))
			   && (udp->dest == htons(d_port + seq)) ) {

				// we hit an intermediate router
				ret = -2;
			}
			break;

		} else if (icmp6->icmp6_type == ICMP6_DST_UNREACH) {
			if (icmp6len < 8 + sizeof(struct ip6_hdr) + 4) {
				// not enough data to look at inner header
				continue;
			}

			ip_host6 = (struct ip6_hdr*) (recvbuf + 8);
			hlen2 = sizeof(struct ip6_hdr);
			udp = (struct udphdr*) (recvbuf + 8 + hlen2);
			if ((ip_host6->ip6_nxt == IPPROTO_UDP)
			   && (udp->source == htons(s_port))
			   && (udp->dest == htons(d_port + seq))) {

				if (icmp6->icmp6_code == ICMP6_DST_UNREACH_NOPORT) {
					ret = -1;
				} else {
					ret = icmp6->icmp6_code; // 0, 1, 2, ...
				}
				break;
			}
		} else if (verbose) {
			fprintf(stdout, " (from %s: type=%d, code=%d)\n"
			       , lothars__sock_ntop_host(proto->sa_recv, proto->sa_len)
			       , icmp6->icmp6_type
			       , icmp6->icmp6_code);
		}
		// some other ICMP error, recvfrom() again
	}

	alarm(0);
	lothars__gettimeofday(tv);
#endif
	return ret;
}


/*
  Returns the description to a provided ICMP code to display.
*/
const char* icmpcode_v4(int code)
{
	static char errbuf[100];
	memset(errbuf, '\0', sizeof(errbuf));

	switch (code) {
	case  0:
		return("network unreachable");
	case  1:
		return("host unreachable");
	case  2:
		return("protocol unreachable");
	case  3:
		return("port unreachable");
	case  4:
		return("fragmentation required but DF bit set");
	case  5:
		return("source route failed");
	case  6:
		return("destination network unknown");
	case  7:
		return("destination host unknown");
	case  8:
		return("source host isolated (obsolete)");
	case  9:
		return("destination network administratively prohibited");
	case 10:
		return("destination host administratively prohibited");
	case 11:
		return("network unreachable for TOS");
	case 12:
		return("host unreachable for TOS");
	case 13:
		return("communication administratively prohibited by filtering");
	case 14:
		return("host recedence violation");
	case 15:
		return("precedence cutoff in effect");

	default:
		sprintf(errbuf, "[unknown code %d]", code);
		return errbuf;
	}
}


/*
  Returns the description to a provided ICMP code to display.
*/
const char* icmpcode_v6(int code)
{
	static char errbuf[100];
	memset(errbuf, '\0', sizeof(errbuf));
#ifdef IPV6
	switch (code) {
	case ICMP6_DST_UNREACH_NOROUTE:
		return "no route to host";
	case ICMP6_DST_UNREACH_ADMIN:
		return "administratively prohibited";
	case ICMP6_DST_UNREACH_BEYONDSCOPE:
		return "beyond scope of source address";
	case ICMP6_DST_UNREACH_ADDR:
		return "address unreachable";
	case ICMP6_DST_UNREACH_NOPORT:
		return "port unreachable";
	default:
		sprintf(errbuf, "[unknown code %d]", code);
	}
#endif
	return errbuf;
}

/********************************************************************************************/


/*
  main
*/
int main(int argc, char** argv)
{
	int cnt;
	struct addrinfo *ai=NULL;
	char *pchr=NULL;

	/* argument parsing */
	opterr = 0;
	while (-1 != (cnt = getopt(argc, argv, "m:v"))) {
		switch (cnt) {
		case 'm':
			if(1 >= (max_ttl = atoi(optarg))){
				err_quit("invalid -m value");
			}
			break;

		case 'v':
			++verbose;
			break;

		case '?':
			err_quit("unrecognized option: %c", cnt);
			break;
		}
	}
	if (optind != argc - 1) {
		err_quit("usage: traceroute [ -m <maxttl> -v ] <hostname>");
	}
	host = argv[optind];
	pid = getpid();
	lothars__signal_intr(SIGALRM, sig_alrm);

	ai = lothars__host_serv(host, NULL, 0, 0);
	pchr = lothars__sock_ntop_host(ai->ai_addr, ai->ai_addrlen);
	fprintf(stdout, "traceroute to %s (%s): %d hops max, %d data bytes\n"
	       , (ai->ai_canonname ? ai->ai_canonname : pchr)
	       , pchr
	       , max_ttl
	       , datalen);

	// initialize according to protocol
	if (ai->ai_family == AF_INET) {
		proto = &proto_v4;

#ifdef IPV6
	} else if (ai->ai_family == AF_INET6) {
		proto = &proto_v6;
		if (IN6_IS_ADDR_V4MAPPED( &(((struct sockaddr_in6*) ai->ai_addr)->sin6_addr))) {
			err_quit("cannot traceroute IPv4-mapped IPv6 address");
		}
#endif
	} else {
		err_quit("unknown address family %d", ai->ai_family);
	}

	// destination address
	proto->sa_send = ai->ai_addr;

	proto->sa_recv = lothars__malloc(ai->ai_addrlen);
	proto->sa_last = lothars__malloc(ai->ai_addrlen);
	proto->sa_bind = lothars__malloc(ai->ai_addrlen);
	proto->sa_len = ai->ai_addrlen;

	worker_routine();

	fprintf(stdout, "READY.\n");

	exit(EXIT_SUCCESS);
}
