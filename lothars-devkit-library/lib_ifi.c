// ifi.c
/*
  interface info (ifi)

  based on a setup published by Stevens (1996)
*/



// constants

/*
// NB: ifi needs a declaration of _XOPEN_SOURCE or similar, e.g.

#define _XOPEN_SOURCE
#include <linux/if.h>
#include <sys/socket.h>
#include <sys/un.h>
*/

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



// forwards

struct ifi_info* get_ifi_info(int, int);
struct ifi_info* lothars__get_ifi_info(int, int);
void free_ifi_info(struct ifi_info *);



// implementation

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
