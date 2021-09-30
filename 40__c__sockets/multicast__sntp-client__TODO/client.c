/*
  SNTP - Simple Network Time Protocol

  NTP client that listens for NTP broadcasts or multicasts on all
  attached networks and then prints the time difference between the
  NTP packet and the host's current time-of-day.  We do not try to
  adjust the time-of-day, as that takes superuser privileges.

  TODO           
*/

//#define _POSIX_C_SOURCE 1
#define _XOPEN_SOURCE 600 /* struct iconf, freeaddrinfo(), getaddrinfo(), gai_strerror() */
//#define _POSIX_SOURCE 600




#include <stdio.h> /* readline() */
#include <stdlib.h>
#include <string.h>
#include <strings.h> /* bzero() */

/* // FIXME struct group_req unknown storage size                            
#define __UAPI_DEF_IP_MREQ 1
#define __UAPI_DEF_SOCKADDR_IN 0
#define __UAPI_DEF_IN_ADDR 0
#define __UAPI_DEF_IPPROTO 0

//#include <linux/libc-compat.h>
// */

// TODO try to fix the include order         
#include <sys/utsname.h>
#include <unistd.h> /* read(), write(), close() */
#include <stdarg.h> /* va_start(), va_end(),... */
#include <sys/wait.h> /* waitpid(), SIGINT,... */
#include <sys/resource.h> /* getrusage(), struct rusage,... */
#include <arpa/inet.h> /* inet_ntop(),... */       
#include <sys/un.h>  /* unix sockets, close() */
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h> /* freeadddrinfo(), getaddrinfo() */
#if HAVE_STROPTS_H
#include <stropts.h> /* ioctl() */
#else
#include <sys/ioctl.h> /* ioctl() */
#endif
#include <linux/sockios.h> /* struct ifreq, SIOCGIFFLAGS, SIOCGIFCONF,... together with _XOPPEN_SOURCE delcaration */
#include <linux/in.h> /* struct ip_mreqn */   
#include <time.h> /* time(), ctime() */
#include <sys/time.h> /* gettimeofday() */
//* // linux and posix
#include <linux/if.h> // struct ifreq, struct ifconf, getnameinfo(), NI_NUMERICHOST,... NB: turn off <net/if.h> when using <linux/if.h>
// */
#include <net/if.h>
// */

#include <errno.h>

#include "sntp.h"


/*
  constants
*/

#define MAXLINE  4096 /* max text line length */


// interface information (ifi)

#define IFI_NAME 16 /* same as IFNAMSIZ in <net/if.h> */
#define IFI_HADDR  8 /* allow for 64-bit EUI-64 in future */

/* ifi_info list */
struct ifi_info {
	char ifi_name[IFI_NAME]; /* interface name, null-terminated */
	short ifi_index; /* interface index */
	short ifi_mtu; /* interface MTU */
	uint8_t ifi_haddr[IFI_HADDR]; /* hardware address */
	uint16_t ifi_hlen; /* # bytes in hardware address: 0, 6, 8 */
	short ifi_flags; /* IFF_xxx constants from <net/if.h> */
	short ifi_myflags; /* our own IFI_xxx flags */
	struct sockaddr *ifi_addr; /* primary address */
	struct sockaddr *ifi_brdaddr; /* broadcast address */
	struct sockaddr *ifi_dstaddr; /* destination address */
	struct ifi_info *ifi_next; /* next of these structures */
};

#define IFI_ALIAS 1   /* ifi_addr is an alias */


/*
  forwards
*/

// error
void err_sys(const char *, ...);
void err_quit(const char *, ...);

// sock
void lothars__bind(int, const struct sockaddr *, socklen_t);
ssize_t lothars__recvfrom(int, void *, size_t, int, struct sockaddr *, socklen_t *);
int lothars__socket(int, int, int);

// unix
void lothars__gettimeofday(struct timeval *);
int lothars__ioctl(int, int, void *);
void* lothars__malloc(size_t);

// socket demo
int lothars__udp_client(const char*, const char*, struct sockaddr**, socklen_t*);
int lothars__family_to_level(int);
void sock_set_addr(struct sockaddr*, socklen_t, const void*);
void sock_set_wild(struct sockaddr*, socklen_t);
char* lothars__sock_ntop(const struct sockaddr*, socklen_t);

// ifi
struct ifi_info* get_ifi_info(int, int);
struct ifi_info* lothars__get_ifi_info(int, int);
void free_ifi_info(struct ifi_info *);

// mcast
void lothars__mcast_join(int, const struct sockaddr *, socklen_t, const char *, uint32_t);

            
//lothars__family_to_level(



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
		err_sys("gettimeofday error");
	}
}


/*
  The ioctl() function shall perform a variety of control functions on
  STREAMS devices.

  #include <sys/ioctl.h>

  @fd: The file descriptor on the stream.
  @request: The ioctl() request (see manpages).
  @arg: An optional argument.
*/
int lothars__ioctl(int fd, int request, void *arg)
{
	int  res;
	if (-1 == (res = ioctl(fd, request, arg))) {
                err_sys("%s() error", __func__);
	}
	return res; // streamio of I_LIST returns value
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
		err_sys("malloc error");
	}
	return ptr;
}


/*
  udp_client.c
*/
int lothars__udp_client(const char *host, const char *serv, struct sockaddr **saptr, socklen_t *lenp)
{
	int fd_sock, eai;
	struct addrinfo hints, *res=NULL, *ressave=NULL;

//	bzero(&hints, sizeof(struct addrinfo)); // TODO rm
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
  sock_set_addr.c
*/
void sock_set_addr(struct sockaddr *sa, socklen_t salen, const void *addr)
{
	switch (sa->sa_family) {
	case AF_INET:
	{
		struct sockaddr_in *sin = (struct sockaddr_in *) sa;
		memcpy(&sin->sin_addr, addr, sizeof(struct in_addr));
		return;
	}

#ifdef IPV6
	case AF_INET6:
	{
		struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *) sa;
		memcpy(&sin6->sin6_addr, addr, sizeof(struct in6_addr));
		return;
	}
#endif
	}
}


/*
  sock_set_wild.c
*/
void sock_set_wild(struct sockaddr *sa, socklen_t salen)
{
	const void *wildptr;

	switch (sa->sa_family) {
	case AF_INET:
	{
		static struct in_addr in4addr_any;
		in4addr_any.s_addr = htonl(INADDR_ANY);
		wildptr = &in4addr_any;
		break;
	}

#ifdef IPV6
	case AF_INET6:
	{
		wildptr = &in6addr_any; // in6addr_any.c
		break;
	}
#endif

	default:
		return;
	}
	sock_set_addr(sa, salen, wildptr);
	return;
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


/* ifi */
struct ifi_info* get_ifi_info(int family, int doaliases)
{
	struct ifi_info *ifi=NULL;
	struct ifi_info *ifihead=NULL;
	struct ifi_info **ifipnext=NULL;

	int fd_sock
		, len
		, lastlen
		, flags
		, myflags
		, idx = 0
		, hlen = 0;

	char *ptr=NULL
		, *buf=NULL
		, lastname[IFNAMSIZ]
		, *cptr=NULL
		, *haddr=NULL
		, *sdlname=NULL;

	struct ifconf ifc;
	struct ifreq *ifr=NULL, ifrcopy;
	struct sockaddr_in *sinptr=NULL;
	struct sockaddr_in6 *sin6ptr=NULL;

	/* create an UDP socket for using with ioctl() */
	fd_sock = lothars__socket(PF_INET, SOCK_DGRAM, 0);


	/* figure out the size of ifc.ifc_buf i.e. ifc.ifc_len, and
	 * allocate corresponding amount of memory via ioctl() call
	 */
	lastlen = 0;
	len = 100 * sizeof(struct ifreq); // initial buffer size guess
	while (1) {
		buf = lothars__malloc(len);
		ifc.ifc_len = len;
		ifc.ifc_buf = buf;
		/* issue SIOCGIFCONF request in a loop

		   One fundamental problem with the SCIOCGIFCONF
		   request is that some implementations did not return
		   an error if the buffer was not large enough to hold
		   the result.

		   Instead the result was truncated and success is
		   returned (a return value of 0 from ioctl()),
		   i.e. the only way to know if the buffer is large
		   enough is the following:

		   1.) issue the request,
		   2.) save the return length,
		   3.) issue the request again with a larger buffer and
		   4.) compare the length with the saved value

		   The buffer is large enough only if the two lengths
		   are the same.
		 */
		//fprintf(stdout, "\t1.) and 3.) Issue SIOCGIFCONF request to ioctl\n");
		if (0 > ioctl(fd_sock, SIOCGIFCONF, &ifc)) { // NB: direct call to ioctl()
			if ((errno != EINVAL) || (lastlen != 0)) {
				err_sys("ioctl error");
			}
		} else {
			if (ifc.ifc_len == lastlen) {
				//fprintf(stdout, "\t4.) Comparison of lengthes were equal - ok! ifc.ifc_len is now set, ifc.ifc_buf has the correct allocated size\n");
				break;  // success, len has not changed
			}
			//fprintf(stdout, "\t2.) Save the ioctl return value\n");
			lastlen = ifc.ifc_len;
		}
		// each time around the loop, increase the buffer size to hold 10 more 'struct ifreq's
		len += 10 * sizeof(struct ifreq);
		free(buf);
	}

	// initialize linked list pointers
	ifihead = NULL;
	ifipnext = &ifihead;
	memset(lastname, '\0', sizeof(lastname));
	sdlname = NULL;

	/* main loop */

	for (ptr = buf; ptr < buf + ifc.ifc_len; ) {
		ifr = (struct ifreq *) ptr; // update to the (next) ifreq instance

		switch (ifr->ifr_addr.sa_family){
#ifdef IPV6
                /*
		  guess we have ipv6, then we need the sockaddr_sa_len
		  field, since the other addressing would be 16-byte
		  for IPv4 addresses, but too small for 28-byte IPv6
		  addresses
		*/
		case PF_INET6:
			//len = sizeof(struct sockaddr_in6); // NB: this is probably not working, better use 'sizeof(*ifr)'
			break;
#endif
		case PF_INET:
		default:
			len = sizeof(*ifr); // NB: on some UNIX and Darwin better use 'sockaddr.sa_len + IFNAMSIZ'
			break;
		}

		ptr += len;
		if (ifr->ifr_addr.sa_family != family) {
			continue; // ignore if not desired address family
		}

		myflags = 0;
		if (NULL != (cptr = strchr(ifr->ifr_name, ':'))) {
			*cptr = 0;  // replace colon with null
		}

		if (0 == strncmp(lastname, ifr->ifr_name, IFNAMSIZ)) {
			if (0 == doaliases) {
				continue; // already processed this interface
			}
			myflags = IFI_ALIAS;
		}
		memcpy(lastname, ifr->ifr_name, IFNAMSIZ);

		/*
		  issue an SIOCGIFFLAGS to ioctl() to fetch the
		  interface flags

		  make a copy of the 'struct ifreq' instance before
		  issuing the ioctl(), becaues if not, this request
		  would overwrite the IP address of the interface
		  since both are members of the same union

		  NB: ifrcopy is a instance on the stack
		*/

		ifrcopy = *ifr;
		lothars__ioctl(fd_sock, SIOCGIFFLAGS, &ifrcopy);
		flags = ifrcopy.ifr_flags;
		if ((flags & IFF_UP) == 0) {
			continue; // ignore if interface not up
		}

		/* allocate and initialize ifi_info structure */
		ifi = lothars__malloc(sizeof(*ifi));
		*ifipnext = ifi; // prev points to this new one
		ifipnext = &ifi->ifi_next; // pointer of pointer to next one goes here

		// pre-init in order to make free() working consistently
		ifi->ifi_addr = NULL;
		ifi->ifi_brdaddr = NULL;
		ifi->ifi_dstaddr = NULL;

		ifi->ifi_flags = flags;  // IFF_xxx values
		ifi->ifi_myflags = myflags; // IFI_xxx values

#if defined(SIOCGIFMTU)
		lothars__ioctl(fd_sock, SIOCGIFMTU, &ifrcopy);
		ifi->ifi_mtu = ifrcopy.ifr_mtu;
#else
		ifi->ifi_mtu = 0;
#endif

		memcpy(ifi->ifi_name, ifr->ifr_name, IFI_NAME);
		ifi->ifi_name[IFI_NAME-1] = '\0';

		// If the sockaddr_dl is from a different interface, ignore it
		if (sdlname == NULL || strcmp(sdlname, ifr->ifr_name) != 0) {
			idx = hlen = 0;
		}
		ifi->ifi_index = idx;
		ifi->ifi_hlen = hlen;
		if (ifi->ifi_hlen > IFI_HADDR) {
			ifi->ifi_hlen = IFI_HADDR;
		}

		if (hlen) {
			memcpy(ifi->ifi_haddr, haddr, ifi->ifi_hlen);
		}

		switch (ifr->ifr_addr.sa_family) {
		case PF_INET:
			sinptr = (struct sockaddr_in *) &ifr->ifr_addr;
			/*
			  copy the IP address that was returned from
			  the original SIOCGIFCONF request
			 */
			ifi->ifi_addr = lothars__malloc(sizeof(struct sockaddr_in));
			memcpy(ifi->ifi_addr, sinptr, sizeof(struct sockaddr_in));

#ifdef SIOCGIFBRDADDR
			/*
			  if the interface supports broadcasting,
			  fetch the broadcast address with an ioctl()
			  of SIOCGIFBRDADDR
			*/
			if (flags & IFF_BROADCAST) {
				lothars__ioctl(fd_sock, SIOCGIFBRDADDR, &ifrcopy);
				sinptr = (struct sockaddr_in *) &ifrcopy.ifr_broadaddr;
				ifi->ifi_brdaddr = lothars__malloc(sizeof(struct sockaddr_in));
				memcpy(ifi->ifi_brdaddr, sinptr, sizeof(struct sockaddr_in));
			}
#endif

#ifdef SIOCGIFDSTADDR
			/*
			  if the interface is a point-to-point
			  interface, the SIOCGIFDSTADDR returns the IP
			  adddress of the other end of the link
			 */
			if (flags & IFF_POINTOPOINT) {
				lothars__ioctl(fd_sock, SIOCGIFDSTADDR, &ifrcopy);
				sinptr = (struct sockaddr_in *) &ifrcopy.ifr_dstaddr;
				ifi->ifi_dstaddr = lothars__malloc(sizeof(struct sockaddr_in));
				memcpy(ifi->ifi_dstaddr, sinptr, sizeof(struct sockaddr_in));
			}
#endif
			break;

			/*
			  similar to IPv4, but IPv6 does not support broadcasting, thus no SIOCGIFBRDADDR
			*/
		case PF_INET6:
			sin6ptr = (struct sockaddr_in6 *) &ifr->ifr_addr;
			ifi->ifi_addr = lothars__malloc(sizeof(struct sockaddr_in6));
			memcpy(ifi->ifi_addr, sin6ptr, sizeof(struct sockaddr_in6));

#ifdef SIOCGIFDSTADDR
			if (flags & IFF_POINTOPOINT) {
				lothars__ioctl(fd_sock, SIOCGIFDSTADDR, &ifrcopy);
				sin6ptr = (struct sockaddr_in6 *) &ifrcopy.ifr_dstaddr;
				ifi->ifi_dstaddr = lothars__malloc(sizeof(struct sockaddr_in6));
				memcpy(ifi->ifi_dstaddr, sin6ptr, sizeof(struct sockaddr_in6));
			}
#endif
			break;

		default:
			break;
		}
	}

	free(buf); buf = NULL;
	return ifihead; // pointer to first structure in linked list
}


void free_ifi_info(struct ifi_info *ifihead)
{
	struct ifi_info *ifi=NULL, *ifinext=NULL;

	for (ifi = ifihead; ifi != NULL; ifi = ifinext) {
		if (NULL != ifi->ifi_addr) {
			free(ifi->ifi_addr); ifi->ifi_addr = NULL;
		}

		if (NULL != ifi->ifi_brdaddr) {
			free(ifi->ifi_brdaddr); ifi->ifi_brdaddr = NULL;
		}

		if (NULL != ifi->ifi_dstaddr) {
			free(ifi->ifi_dstaddr); ifi->ifi_dstaddr = NULL;
		}

		ifinext = ifi->ifi_next; // can't fetch ifi_next after free()
		free(ifi); // the ifi_info{} itself
	}
}


struct ifi_info* lothars__get_ifi_info(int family, int doaliases)
{
	struct ifi_info *ifi=NULL;

	if (NULL == (ifi = get_ifi_info(family, doaliases))) {
		err_quit("get_ifi_info error");
	}
	return ifi;
}


/* mcast */

/*
*/
int mcast_join( int fd_sock
                , const struct sockaddr *grp
                , socklen_t grplen
                , const char *ifname
                , uint32_t ifindex)
{
#ifdef MCAST_JOIN_GROUP

//	struct group_req req; FIXME             
	struct group_req req; // FIXME
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
		struct ip_mreqn mreq; // TODO update lib          
		struct ifreq ifreq;

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
	return 0; // TODO update     
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





/********************************************************************************************/
// worker implementation



/*
  SNTP procedure
*/
void sntp_proc(char *buf, ssize_t num, struct timeval *nowptr)
{
	int version, mode;
	uint32_t nsec, useci;
	double usecf;
	struct timeval diff;
	struct ntpdata *ntp=NULL;

	if (num < (ssize_t) sizeof(struct ntpdata)) {
		fprintf(stdout, "\npacket too small: %ld bytes\n", num);
		return;
	}

	ntp = (struct ntpdata*) buf;
	version = (ntp->status & VERSION_MASK) >> 3;

	mode = ntp->status & MODE_MASK;
	fprintf(stdout, "\nv%d, mode %d, strat %d, ", version, mode, ntp->stratum);
	if (mode == MODE_CLIENT) {
		fprintf(stdout, "client\n");
		return;
	}

	// date conversion
	nsec = ntohl(ntp->xmt.int_part) - JAN_1970;

	// 32-bit integer fraction
	useci = ntohl(ntp->xmt.fraction);

	// integer fraction -> double
	usecf = useci;

	// divide by 2^32 -> [0.0; 1.0]
	usecf /= 4294967296.0;

	// fraction -> parts per million
	useci = usecf * 1000000.0;

	if (0 > (diff.tv_sec = nowptr->tv_sec - nsec)) {
		diff.tv_usec += 1000000;
		--diff.tv_sec;
	}

	// diff in microsec (network delay not taken into account!)
	useci = (diff.tv_sec * 1000000) + diff.tv_usec;
	fprintf(stdout, "clock difference = %d usec\n", useci);
}


/********************************************************************************************/


/*
  main..
*/
int main(int argc, char** argv)
{
	int fd_sock;
	char buf[MAXLINE];
	ssize_t num;
	socklen_t salen, len;
	struct sockaddr *mcastsa=NULL, *wild=NULL, *from=NULL;
	struct timeval now;

	fd_sock = lothars__udp_client("10.0.2.2", "ntp", (void*) &mcastsa, &salen);

	wild = lothars__malloc(salen);

	// copy family and port
	memcpy(wild, mcastsa, salen);

	sock_set_wild(wild, salen);
	lothars__bind(fd_sock, wild, salen);

#ifdef MCAST
	struct ifi_info *ifi=NULL;

	// obtain interface list and process each one
	for (ifi =_get_ifi_info(mcastsa->sa_family, 1)
		    ; ifi != NULL
		    ; ifi = ifi->ifi_next) {
		if (ifi->ifi_flags & IFF_MULTICAST) {
			lothars__mcast_join(fd_sock, mcastsa, salen, ifi->ifi_name, 0);
			fprintf(stdout, "joined %s on %s\n", lothars__sock_ntop(mcastsa, salen), ifi->ifi_name);
		}
	}
#endif

	from = lothars__malloc(salen);
	while (1) {
		len = salen;
		num = lothars__recvfrom(fd_sock, buf, sizeof(buf), 0, from, &len);
		lothars__gettimeofday(&now);
		sntp_proc(buf, num, &now);
	}

	fprintf(stdout, "READY.\n");
	exit(EXIT_SUCCESS);
}

