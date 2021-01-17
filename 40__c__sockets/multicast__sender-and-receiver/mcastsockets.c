/*
  multicast socket - start sender and receiver

  IPv4

  There are a few special IPv4 multicast addresses:

  - 224.0.0.1 is the all-hosts group. All multicast-capable nodes
    (hosts, routers, printers, etc.) on a subnet must join this group
    on all multicast-capable interfaces.

  - 224.0.0.2 is the all-routers group. All multicast-capable routers
    on a subnet must join this group on all multicast-capable
    interfaces.

  IPv4 does not have a separate scope field for multicast
  packets. Historically, the IPv4 TTL field in the IP header has
  doubled as a multicast scope field: A TTL of 0 means
  interface-local, 1 means link-local; up through 32 means site-local;
  up through 64 means region-local; up through 128 means
  continent-local, and up through 255 are unrestricted in scope
  (global).


  ---

  IPv6

  There are a few special IPv6 multicast addresses:

  - ff01::1 and ff02::1 are the all-nodes group at interface-local and
    link-local scope. All nodes (hosts, routers, printers, etc.) on a
    subnet must join these groups on all multicast-capable
    interfaces. This is similar to the IPv4 224.0.0.1 multicast
    address.

  - ff01::2, ff02::2 and ff05::2 are the all-routers group at
    interface-local, link-local, and site-local scopes. All routers on
    a subnet must join these groups on all multicast-capable
    interfaces. This is similar to the IPv4 224.0.0.2 multicast
    address.

  IPv6 multicast addresses have an explicit 4-bit scope field that
  specifies how 'far' the multicast packet will travel. IPv6 packets
  also have a hop limit field that limits the number of times the
  packet is forwarded by a router.

   1: interface-local
   2: link-local
   4: admin-local
   5: site-local
   8: organization-local
  14: global


  ---

  Especially in the case of streaming multimedia, the combination of
  an IP multicast address (either IPv4 or IPv6) and a transport-layer
  port (typically UDP) is referred to as a session. These sessions -
  e.g. an audio/video teleconference may comprise two sessions -
  almost always use different ports and sometimes also use different
  groups for flexibility in choice when receiving.


  Usage - connect to an existing MCAST group

  $> ./mcastsockets.exe 239.255.1.2 12345
      from 172.24.37.78:51297: macosx, 21891
      (...)

  ---

  References: Unix Network Programming, Stevens (1996) and developer
  manpages, e.g. on die.net
*/


#define _GNU_SOURCE /* struct ip_mreq */

#include <stdio.h> /* readline() */
#include <stdlib.h>
#include <string.h>

#include <netinet/in.h> // !!! place this header always before <linux/.. headers, or struct sockaddr might be unknown !!!
#include <sys/utsname.h> /* uname() */
#include <unistd.h> /* getpid(), fork(), sleep() */
#include <netdb.h> /* freeadddrinfo(), getaddrinfo(), gai_strerror() */
#include <stdarg.h> /* va_start(), va_end(),... */
#include <arpa/inet.h> /* inet_ntop() */
#include <sys/un.h>  /* struct sockaddr_un, close() */
#include <net/if.h> /* if_nametoindex() */
#include <errno.h>


/*
  constants
*/

// send one dataram every five seconds
#define SENDRATE 5
#define MAXLINE  4096 /* max text line length */


/*
  forwards
*/

// error
void err_sys(const char *, ...);
void err_quit(const char *, ...);

// sock
void lothars__bind(int, const struct sockaddr *, socklen_t);
ssize_t lothars__recvfrom(int, void *, size_t, int, struct sockaddr *, socklen_t *);
void lothars__sendto(int, const void *, size_t, int, const struct sockaddr *, socklen_t);
void lothars__setsockopt(int, int, int, const void *, socklen_t);
int lothars__socket(int, int, int);

// demo / socket
int lothars__family_to_level(int);
int lothars__socket_to_family(int);
int lothars__udp_client(const char*, const char*, struct sockaddr**, socklen_t*);
char* lothars__sock_ntop(const struct sockaddr*, socklen_t);

// unix
pid_t lothars__fork();
void* lothars__malloc(size_t);

// mcast
void lothars__mcast_join(int, const struct sockaddr *, socklen_t, const char *, uint32_t);
void lothars__mcast_set_loop(int, int);


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


/*
  The bind() function shall assign a local socket address address to a
  socket identified by descriptor socket that has no local socket
  address assigned. Sockets created with the socket() function are
  initially unnamed; they are identified only by their address family.

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
		err_sys("bind error");
	}
}


/*
  The recvfrom() function shall receive a message from a
  connection-mode or connectionless-mode socket. It is normally used
  with connectionless-mode sockets because it permits the application
  to retrieve the source address of received data.
*/
ssize_t lothars__recvfrom(int fd
		  , void *ptr
		  , size_t nbytes
		  , int flags
		  , struct sockaddr *sa
		  , socklen_t *salenptr)
{
	ssize_t  res;
	if (0 > (res = recvfrom(fd, ptr, nbytes, flags, sa, salenptr))) {
		err_sys("recvfrom error");
	}
	return res;
}


/*
  The sendto() function shall send a message through a connection-mode
  or connectionless-mode socket. If the socket is connectionless-mode,
  the message shall be sent to the address specified by dest_addr. If
  the socket is connection-mode, dest_addr shall be ignored.
*/
void lothars__sendto( int fd
	      , const void *ptr
	      , size_t nbytes
	      , int flags
	      , const struct sockaddr *sa
	      , socklen_t salen)
{
	if ((ssize_t) nbytes != sendto(fd, ptr, nbytes, flags, sa, salen)) {
		err_sys("sendto error");
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
  demo snippet - family to level
*/
int lothars__family_to_level(int family)
{
	switch (family) {
	case AF_INET:
		return IPPROTO_IP;

#ifdef IPV6
	case AF_INET6:
		return IPPROTO_IPV6;
#endif

	default:
		err_sys("%s() error", __func__);
		return -1;
	}
}


/*
  demo snippets - lothars__socket_to_family
*/
int lothars__socket_to_family(int fd_sock)
{
	struct sockaddr_storage ss;
	socklen_t len;

	len = sizeof(ss);
	if (0 > getsockname(fd_sock, (struct sockaddr *) &ss, &len)) {
		err_sys("%s() error", __func__);
		return -1;
	}
	return ss.ss_family;
}


/*
*/
int lothars__udp_client(const char *host, const char *serv, struct sockaddr **saptr, socklen_t *lenp)
{
	int fd_sock, eai;
	struct addrinfo hints, *res=NULL, *ressave=NULL;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if (0 != (eai = getaddrinfo(host, serv, &hints, &res))) {
		err_quit("udp_client error for %s, %s: %s", host, serv, gai_strerror(eai));
	}
	ressave = res;

	do {
		fd_sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (fd_sock >= 0)
			break;  // success
	} while (NULL != (res = res->ai_next));

	if (res == NULL) { // errno set from final socket()
		err_sys("%s() error for %s, %s", __func__, host, serv);
	}

	*saptr = lothars__malloc(res->ai_addrlen);
	memcpy(*saptr, res->ai_addr, res->ai_addrlen);
	*lenp = res->ai_addrlen;

	freeaddrinfo(ressave);

	return fd_sock;
}


/*
  sock_ntop
*/
char* sock_ntop(const struct sockaddr *sa, socklen_t salen)
{
	char portstr[8];
	static char str[128];  // Unix domain is largest

	switch (sa->sa_family) {
	case AF_INET:
	{
		struct sockaddr_in *sin = (struct sockaddr_in *) sa;
		if (NULL == inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str))) {
			return NULL;
		}
		if (0 != ntohs(sin->sin_port)) {
			snprintf(portstr, sizeof(portstr), ":%d", ntohs(sin->sin_port));
			strcat(str, portstr);
		}
		return str;
	}

#ifdef IPV6
	case AF_INET6:
	{
		struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *) sa;
		str[0] = '[';
		if (NULL == inet_ntop(AF_INET6, &sin6->sin6_addr, str + 1, sizeof(str) - 1)) {
			return NULL;
		}
		if (0 != ntohs(sin6->sin6_port)) {
			snprintf(portstr, sizeof(portstr), "]:%d", ntohs(sin6->sin6_port));
			strcat(str, portstr);
			return str;
		}
		return (str + 1);
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
		snprintf(str, sizeof(str), "sock_ntop: unknown AF_xxx: %d, len %d", sa->sa_family, salen);
		return(str);
	}
	return NULL;
}


char* lothars__sock_ntop(const struct sockaddr *sa, socklen_t salen)
{
	char *ptr = NULL;
	if (NULL == (ptr = sock_ntop(sa, salen))) {
		err_sys("sock_ntop error"); /* inet_ntop() sets errno */
	}
	return ptr;
}


/*
  The fork() function shall create a new process.
*/
pid_t lothars__fork(void)
{
	pid_t pid;
	if (-1 == (pid = fork())) {
		err_sys("fork error");
	}
	return pid;
}


void* lothars__malloc(size_t size)
{
	void *ptr;
	if (NULL == (ptr = malloc(size))) {
		err_sys("malloc error");
	}
	return ptr;
}


/*
  mcast
*/

int mcast_join( int fd_sock
                , const struct sockaddr *grp
                , socklen_t grplen
                , const char *ifname
                , uint32_t ifindex)
{
#ifdef MCAST_JOIN_GROUP
	struct group_req req;
	if (0 < ifindex) {
		req.gr_interface = ifindex;
	} else if (NULL != ifname) {
		if (0 == (req.gr_interface = if_nametoindex(ifname))) {
			errno = ENXIO; // interface name not found
			return -1;
		}
	} else {
		req.gr_interface = 0;
	}

	if (grplen > sizeof(req.gr_group)) {
		errno = EINVAL;
		return -1;
	}

	memcpy(&req.gr_group, grp, grplen);
	return setsockopt(fd_sock, lothars__family_to_level(grp->sa_family), MCAST_JOIN_GROUP, &req, sizeof(req));
#else
	switch(grp->sa_family){
	case AF_INET:
	{
		struct ip_mreq  mreq;
		struct ifreq  ifreq;

		memcpy(&mreq.imr_multiaddr, &((const struct sockaddr_in *) grp)->sin_addr, sizeof(struct in_addr));

		if (0 < ifindex) {
			if (NULL == if_indextoname(ifindex, ifreq.ifr_name)) {
				errno = ENXIO; // interface index not found
				return -1;
			}
			goto doioctl;

		} else if (NULL != ifname) {
			strncpy(ifreq.ifr_name, ifname, IFNAMSIZ);
		doioctl:
			if (0 > ioctl(fd_sock, SIOCGIFADDR, &ifreq)) {
				return -1;
			}

			memcpy(&mreq.imr_interface, &((struct sockaddr_in *) &ifreq.ifr_addr)->sin_addr, sizeof(struct in_addr));
		} else {
			mreq.imr_interface.s_addr = htonl(INADDR_ANY);
		}

		return setsockopt(fd_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
	}
#ifdef IPV6
#ifndef IPV6_JOIN_GROUP  // APIv0 compatibility
#define IPV6_JOIN_GROUP  IPV6_ADD_MEMBERSHIP
#endif
	case AF_INET6:
	{
		struct ipv6_mreq mreq6;

		memcpy(&mreq6.ipv6mr_multiaddr, &((const struct sockaddr_in6 *) grp)->sin6_addr, sizeof(struct in6_addr));

		if (0 < ifindex) {
			mreq6.ipv6mr_interface = ifindex;
		} else if (NULL != ifname) {
			if (0 == (mreq6.ipv6mr_interface = if_nametoindex(ifname))) {
				errno = ENXIO; // i/f name not found
				return -1;
			}
		} else {
			mreq6.ipv6mr_interface = 0;
		}

		return setsockopt(fd_sock, IPPROTO_IPV6, IPV6_JOIN_GROUP, &mreq6, sizeof(mreq6));
	}
#endif

	default:
		errno = EAFNOSUPPORT;
		return -1;
	}
#endif
}


void lothars__mcast_join( int fd_sock
                  , const struct sockaddr *grp
                  , socklen_t grplen
                  , const char *ifname
                  , uint32_t ifindex)
{
	if (0 > mcast_join(fd_sock, grp, grplen, ifname, ifindex)) {
		err_sys("mcast_join error");
	}
}


/*
*/
int mcast_set_loop(int fd_sock, int onoff)
{
	switch (lothars__socket_to_family(fd_sock)) {
	case AF_INET:
	{
		uint8_t flag;

		flag = onoff;
		// specify multicast loopback
		return setsockopt(fd_sock, IPPROTO_IP, IP_MULTICAST_LOOP, &flag, sizeof(flag));
	}

#ifdef IPV6
	case AF_INET6:
	{
		uint32_t flag;

		flag = onoff;
		return setsockopt(fd_sock, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, &flag, sizeof(flag));
	}
#endif

	default:
		errno = EAFNOSUPPORT;
		return -1;
	}
}


void lothars__mcast_set_loop( int fd_sock
                      , int onoff)
{
	if (0 > mcast_set_loop(fd_sock, onoff)) {
		err_sys("mcast_set_loop error");
	}
}


/********************************************************************************************/
// worker implementation

/*
  send
*/
void send_all(int fd_send, struct sockaddr* sadest, socklen_t salen)
{
	char line[MAXLINE];
	struct utsname name;

	if (0 > uname(&name)) {
		err_sys("uname error");
	}

	snprintf(line, sizeof(line), "%s, %d\n", name.nodename, getpid());

	while (1) {
		lothars__sendto(fd_send, line, strlen(line), 0, sadest, salen);
		sleep(SENDRATE); // wait sendrate to resend
	}
}


/*
  receive
*/
void recv_all(int fd_recv, socklen_t salen)
{
	int idx;
	char line[MAXLINE + 1];
	socklen_t len;
	struct sockaddr *safrom;

	safrom = lothars__malloc(salen);

	while (1) {
		len = salen;
		idx = lothars__recvfrom(fd_recv, line, MAXLINE, 0, safrom, &len);

		// '\0' termination
		line[idx] = '\0';
		fprintf(stdout, "from %s: %s", lothars__sock_ntop(safrom, len), line);
	}
}


/********************************************************************************************/


/*
  main
*/
int main(int argc, char *argv[])
{
	int fd_send, fd_recv;
	const int on = 1;
	socklen_t salen;
	struct sockaddr *sa_send, *sa_recv;
	char hostip[16]; memset(hostip, '\0', sizeof(hostip));
	char port[16]; memset(port, '\0', sizeof(port));

	if (3 != argc) {
		fprintf(stderr, "usage: %s <multicastip> <port>\ne.g.\n$> %s 239.255.1.2 8080\n", argv[0], argv[0]);
		exit(EXIT_FAILURE);
	}
	strncpy(hostip, argv[1], sizeof(hostip));
	fprintf(stdout, "hostip: '%s'\n", hostip);

	strncpy(port, argv[2], sizeof(port));
	fprintf(stdout, "port: '%s'\n", port);

	fd_send = lothars__udp_client(hostip, port, (void*) &sa_send, &salen);

	fd_recv = lothars__socket(sa_send->sa_family, SOCK_DGRAM, 0);

	lothars__setsockopt(fd_recv, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	sa_recv = lothars__malloc(salen);
	memcpy(sa_recv, sa_send, salen);

	lothars__bind(fd_recv, sa_recv, salen);

	// multicast: join group
	lothars__mcast_join(fd_recv, sa_send, salen, NULL, 0);

	// multicast: set loop
	lothars__mcast_set_loop(fd_send, 0);


	// split into sender and receiver
	if (0 == lothars__fork()) {
		// child - receives
		recv_all(fd_recv, salen);
		fprintf(stdout, "child: READY.\n");
	}

	// parent - sends
	send_all(fd_send, sa_send, salen);

	fprintf(stdout, "parent: READY.\n");
	exit(EXIT_SUCCESS);
}
