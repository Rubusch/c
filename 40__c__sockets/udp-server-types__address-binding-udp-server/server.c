// server.c
/*
  binding interface addresses with udp server
*/

// TODO check the HAVE_ macros (haddr unitialized w/o any HAVE_ enabled... )           





/* struct addressinfo (ai) and getaddressinfo (gai) will need _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE */

#define _XOPEN_SOURCE /* SIOCGIFFLAGS, SIOCGIFCONF,... */

#include <stdio.h> /* readline() */
#include <stdlib.h>
#include <string.h> /* memset() */
#include <strings.h> /* bzero() */
#include <stropts.h> /* ioctl() */
#include <linux/sockios.h> /* SIOCGIFFLAGS, SIOCGIFCONF,... together with _XOPPEN_SOURCE delcaration */
#include <arpa/inet.h> /* inet_ntop(), inet_pton() */
#include <unistd.h> /* read(), write(), close() */
#include <netdb.h> /* freeadddrinfo(), getaddrinfo() */
#include <stdarg.h> /* va_start(), va_end(),... */
#include <sys/wait.h> /* waitpid(), SIGINT,... */
#include <sys/resource.h> /* getrusage(), struct rusage,... */
#include <sys/socket.h> /* sockets,... */
#include <sys/un.h>  /* unix sockets */
#include <time.h> /* time(), ctime() */
#include <errno.h>


/*
  constants
*/

#define MAXLINE 4096 /* max text line length */


/*
  forwards
*/

// error
void err_sys(const char *, ...);
void err_quit(const char *, ...);

// unix
void lothars__close(int);
int lothars__ioctl(int, int, void *);
pid_t lothars__fork();
void* lothars__malloc(size_t);

// sock
void lothars__bind(int, const struct sockaddr *, socklen_t);
ssize_t lothars__recvfrom(int, void *, size_t, int, struct sockaddr *, socklen_t *);
void lothars__sendto(int, const void *, size_t, int, const struct sockaddr *, socklen_t);
void lothars__setsockopt(int, int, int, const void *, socklen_t);
int lothars__socket(int, int, int);

char* lothars__sock_ntop(const struct sockaddr *, socklen_t);

// ifi headers
//#include <net/if.h>
#include <linux/if.h>

#define IFI_NAME 16 /* same as IFNAMSIZ in <linux/if.h> */
#define IFI_HADDR  8 /* allow for 64-bit EUI-64 in future */

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

struct ifi_info* get_ifi_info(int, int);
struct ifi_info* lothars__get_ifi_info(int, int);
void free_ifi_info(struct ifi_info *);


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


void lothars__close(int fd)
{
	if (-1 == close(fd)) {
		err_sys("close error");
	}
}


int lothars__ioctl(int fd, int request, void *arg)
{
	int  res;
	if (-1 == (res = ioctl(fd, request, arg))) {
		err_sys("ioctl error");
	}
	return res; // streamio of I_LIST returns value
}


pid_t lothars__fork(void)
{
	pid_t pid;
	if (-1 == (pid = fork())) {
		err_sys("fork error");
	}
	return pid;
}


void lothars__bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
	if (0 > bind(fd, sa, salen)) {
		err_sys("bind error");
	}
}


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


int lothars__socket(int family, int type, int protocol)
{
	int res;
	if (0 > (res = socket(family, type, protocol))) {
		err_sys("socket error");
	}
	return res;
}


/*
  sock_ntop.c
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
//		if (0 == unp->sun_path[0]) { // FIXME
		if (0 == unp->sun_path[0]) {
			strcpy(str, "(no pathname bound)");
		} else {
			snprintf(str, sizeof(str), "%s", unp->sun_path);
		}
		return str;
	}
#endif

#ifdef HAVE_SOCKADDR_DL_STRUCT  
	case AF_LINK:
	{
		struct sockaddr_dl *sdl = (struct sockaddr_dl *) sa;
		if (sdl->sdl_nlen > 0) {
			snprintf(str
				 , sizeof(str)
				 , "%*s (index %d)"
				 , sdl->sdl_nlen
				 , &sdl->sdl_data[0]
				 , sdl->sdl_index);
		} else {
			snprintf(str, sizeof(str), "AF_LINK, index=%d", sdl->sdl_index);
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
	struct ifi_info *ifi;
	struct ifi_info *ifihead;
	struct ifi_info **ifipnext;

	int fd_sock
		, len
		, lastlen
		, flags
		, myflags
		, idx = 0
		, hlen = 0;

	char *ptr
		, *buf
		, lastname[IFNAMSIZ]
		, *cptr
		, *haddr
		, *sdlname;

	struct ifconf ifc;
	struct ifreq *ifr, ifrcopy; /* NB: ifrcopy is NOT a pointer! */
	struct sockaddr_in *sinptr;
	struct sockaddr_in6 *sin6ptr;

	/* create an UDP socket for using with ioctl() */
	fd_sock = lothars__socket(AF_INET, SOCK_DGRAM, 0);

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
		fprintf(stdout, "1.) and 3.) Issue SIOCGIFCONF request to ioctl\n");
		if (0 > ioctl(fd_sock, SIOCGIFCONF, &ifc)) {
			if ((errno != EINVAL) || (lastlen != 0)) {
				err_sys("ioctl error");
			}
		} else {
			if (ifc.ifc_len == lastlen) {
				fprintf(stdout, "4.) Comparison of lengthes were equal - ok\n");
				break;  // success, len has not changed
			}
			fprintf(stdout, "2.) Save the ioctl return value\n");
			lastlen = ifc.ifc_len;
		}
		// each time around the loop, increase the buffer size to hold 10 more 'struct ifreq's
		len += 10 * sizeof(struct ifreq);
		free(buf);
	}

	// initialize linked list pointers
	ifihead = NULL;
	ifipnext = &ifihead;
	lastname[0] = 0;
	sdlname = NULL;

	for (ptr = buf; ptr < buf + ifc.ifc_len; ) {
		ifr = (struct ifreq *) ptr;

//#ifdef HAVE_SOCKADDR_SA_LEN /* if there is sockaddr sa_len available.. */
// TODO test out         
//		len = max(sizeof(struct sockaddr), ifr->ifr_addr.sa_len);
//#else
		switch (ifr->ifr_addr.sa_family){
//#ifdef IPV6 /* guess we have ipv6, then we need the sockaddr_sa_len
//		field, since the other addressing would be 16-byte for
//		IPv4 addresses, but too small for 28-byte IPv6
//		addresses */ case AF_INET6: len = sizeof(struct
//		sockaddr_in6); break; #endif

		case AF_INET:
		default:
			len = sizeof(struct sockaddr);
			break;
		}
//#endif // HAVE_SOCKADDR_SA_LEN

		ptr += sizeof(ifr->ifr_name) + len; // for next one in buffer



// no AF_LINK available                          
//#ifdef HAVE_SOCKADDR_DL_STRUCT
//		// assumes that AF_LINK precedes AF_INET or AF_INET6
//		if (ifr->ifr_addr.sa_family == AF_LINK) {
//			struct sockaddr_dl *sdl = (struct sockaddr_dl *)&ifr->ifr_addr;
//			sdlname = ifr->ifr_name;
//			idx = sdl->sdl_index;
//			haddr = sdl->sdl_data + sdl->sdl_nlen;
//			hlen = sdl->sdl_alen;
//		}
//#endif


// TODO else, haddr uninitialized?!!!            

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
		ifi = lothars__malloc(sizeof(struct ifi_info));
		*ifipnext = ifi;   // prev points to this new one
		ifipnext = &ifi->ifi_next; // pointer to next one goes here

		ifi->ifi_flags = flags;  // IFF_xxx values
		ifi->ifi_myflags = myflags; // IFI_xxx values

#if defined(SIOCGIFMTU) // && defined(HAVE_STRUCT_IFREQ_IFR_MTU) TODO rm, =1
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

		if(hlen){
			memcpy(ifi->ifi_haddr, haddr, ifi->ifi_hlen);
		}

		switch (ifr->ifr_addr.sa_family) {
		case AF_INET:
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
//				ifi->ifi_brdaddr = lothars__calloc(1, sizeof(struct sockaddr_in)); // TODO rm
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
		case AF_INET6:
			sin6ptr = (struct sockaddr_in6 *) &ifr->ifr_addr;
//			ifi->ifi_addr = lothars__calloc(1, sizeof(struct sockaddr_in6)); // TODO rm
			ifi->ifi_addr = lothars__malloc(sizeof(struct sockaddr_in6));
			memcpy(ifi->ifi_addr, sin6ptr, sizeof(struct sockaddr_in6));

#ifdef SIOCGIFDSTADDR
			if (flags & IFF_POINTOPOINT) {
				lothars__ioctl(fd_sock, SIOCGIFDSTADDR, &ifrcopy);
				sin6ptr = (struct sockaddr_in6 *) &ifrcopy.ifr_dstaddr;
//				ifi->ifi_dstaddr = lothars__calloc(1, sizeof(struct sockaddr_in6)); // TODO rm
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
	struct ifi_info *ifi, *ifinext;
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
	struct ifi_info *ifi;
	if (NULL == (ifi = get_ifi_info(family, doaliases))) {
		err_quit("get_ifi_info error");
	}
	return ifi;
}



/********************************************************************************************/
// worker implementation

/*
  worker
*/
void server_dg_echo(int fd_sock, struct sockaddr* pcliaddr, socklen_t clilen, struct sockaddr* serveraddr)
{
	int cnt;
	char mesg[MAXLINE];
	socklen_t len;

	while (1) {
		len = clilen;
		cnt = lothars__recvfrom(fd_sock, mesg, MAXLINE, 0, pcliaddr, &len);
		fprintf(stdout, "child %d, datagram from %s", getpid(), lothars__sock_ntop(pcliaddr, len));
		fprintf(stdout, ", to %s\n", lothars__sock_ntop(serveraddr, clilen));

		lothars__sendto(fd_sock, mesg, cnt, 0, pcliaddr, len);
	}
}


/********************************************************************************************/


/*
  main function to the UDP server (forked)
*/
int main(int argc, char** argv)
{
	int fd_sock;
	const int on = 1;
	pid_t pid;
	struct ifi_info *ifi; //, *ifi_head;
	struct sockaddr_in *sa, cliaddr, wildaddr;
	char port[16]; memset(port, '\0', sizeof(port));

	if (2 != argc) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	strncpy(port, argv[1], sizeof(port));
	fprintf(stdout, "port: '%s'\n", port);

	for (/*ifi_head =*/ ifi = lothars__get_ifi_info(AF_INET, 1)
		    ; ifi != NULL
		    ; ifi = ifi->ifi_next) {
		// bind unicast address
		fd_sock = lothars__socket(AF_INET, SOCK_DGRAM, 0);
		lothars__setsockopt(fd_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

		sa = (struct sockaddr_in*) ifi->ifi_addr;
		sa->sin_family = AF_INET;
		sa->sin_port = htons(atoi(port));
		lothars__bind(fd_sock, (struct sockaddr*) sa, sizeof(*sa));
		fprintf(stdout, "bound %s\n", lothars__sock_ntop((struct sockaddr*) sa, sizeof(*sa)));

		if (0 == (pid = lothars__fork())) {
			// child
			server_dg_echo(fd_sock, (struct sockaddr*) &cliaddr, sizeof(cliaddr), (struct sockaddr*) sa);
			exit(EXIT_SUCCESS); // never executed
		}

		/*
		  If the interface supports broadcasting, a UDP socket is created
		  and the broadcast address is bound to it. We allow the bind to fail
		  with EADDRINUSE because if an interface has multiple addresses
		  (aliases) on the same broadcast address.
		*/

		if (ifi->ifi_flags & IFF_BROADCAST) {
			// try to bind broadcast address
			fd_sock = lothars__socket(AF_INET, SOCK_DGRAM, 0);
			lothars__setsockopt(fd_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

			sa = (struct sockaddr_in*) ifi->ifi_brdaddr;
			sa->sin_family = AF_INET;
			sa->sin_port = htons(atoi(port));
			if (0 > bind(fd_sock, (struct sockaddr*) sa, sizeof(*sa))) {
				if (errno == EADDRINUSE) {
					fprintf(stdout, "EADDRINUSE: %s\n", lothars__sock_ntop((struct sockaddr*) sa, sizeof(*sa)));
					lothars__close(fd_sock);
					continue;
				} else {
					err_sys("bind error for %s", lothars__sock_ntop((struct sockaddr*) sa, sizeof(*sa)));
				}
			}

			fprintf(stdout, "bound %s\n", lothars__sock_ntop((struct sockaddr*) sa, sizeof(*sa)));

			if (0 == (pid = lothars__fork())) {
				// child
				server_dg_echo(fd_sock, (struct sockaddr*) &cliaddr, sizeof(cliaddr), (struct sockaddr*) sa);
				exit(EXIT_SUCCESS); // never executed
			}
		}
	}

	// bind wildcard address

	fd_sock = lothars__socket(AF_INET, SOCK_DGRAM, 0);
	lothars__setsockopt(fd_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	bzero(&wildaddr, sizeof(wildaddr));
	wildaddr.sin_family = AF_INET;
	wildaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	wildaddr.sin_port = htons(atoi(port));
	lothars__bind(fd_sock, (struct sockaddr*) &wildaddr, sizeof(wildaddr));

	fprintf(stdout, "bound %s\n", lothars__sock_ntop((struct sockaddr*) &wildaddr, sizeof(wildaddr)));
	if (0 == (pid = lothars__fork())) {
		// worker
		server_dg_echo(fd_sock, (struct sockaddr*) &cliaddr, sizeof(cliaddr), (struct sockaddr*) sa);
		exit(EXIT_SUCCESS); // never executed
	}

	fprintf(stdout, "READY.\n");
	exit(EXIT_SUCCESS);
}

