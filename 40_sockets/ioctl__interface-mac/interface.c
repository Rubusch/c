// Interface.c
/*
  getting interface information
*/

/* struct addressinfo (ai) and getaddressinfo (gai) will need _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE */

#define _XOPEN_SOURCE 600 /* SIOCGIFFLAGS, SIOCGIFCONF,... */
#define _GNU_SOURCE /* getnameinfo(), NI_NUMERICHOST,... */

#include <stdio.h> /* readline() */
#include <stdlib.h>
#include <string.h> /* memcpy() */

#include <netinet/in.h> // !!! place this header before <linux/.. headers or struct sockaddr might be unknown !!!
#include <sys/un.h> /* unix sockets, close() */
#include <unistd.h> /* close() */
#include <netdb.h> /* NI_NUMERICHOST,... */
#if HAVE_STROPTS_H
#include <stropts.h> /* ioctl() */
#else
#include <sys/ioctl.h> /* ioctl() */
#endif
#include <stdarg.h> /* va_start(), va_end(),... */

// here both header families (linux and posix) seem to be ok
//*
//#include <linux/if.h> // struct ifreq, struct ifconf, getnameinfo(), NI_NUMERICHOST,... NB: turn off <net/if.h> when using <linux/if.h>
//#include <linux/if_arp.h> // struct arpreq
// */
#include <net/if_arp.h>
#include <net/if.h>
// */
#include <linux/sockios.h> /* struct ifreq, SIOCGIFFLAGS, SIOCGIFCONF,... together with _XOPPEN_SOURCE delcaration */
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
void lothars__close(int *);
int lothars__ioctl(int, int, void *);
void *lothars__malloc(size_t);

// sock
int lothars__socket(int, int, int);

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

/*
  helpers / wrappers

  mainly to cover error handling and display error message
*/

/*
  fatal error related to system call Print message and terminate
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
   fatal error unrelated to system call Print message and terminate
*/
void err_quit(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	err_doit(0, fmt, ap);
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

int lothars__ioctl(int fd, int request, void *arg)
{
	int res;
	if (-1 == (res = ioctl(fd, request, arg))) {
		err_sys("ioctl error");
	}
	return res; // streamio of I_LIST returns value
}

void *lothars__malloc(size_t size)
{
	void *ptr;
	if (NULL == (ptr = malloc(size))) {
		err_sys("malloc error");
	}
	return ptr;
}

int lothars__socket(int family, int type, int protocol)
{
	int res;
	if (0 > (res = socket(family, type, protocol))) {
		err_sys("socket error");
	}
	return res;
}

/********************************************************************************************/
// worker

void get_interfaces(int family)
{
	int fd_sock;
	struct ifreq *ifr;
	struct ifconf ifc;
	int idx;
	size_t len;

	fd_sock = lothars__socket(family, SOCK_DGRAM, 0);

	/* show interfaces */

	// get ifc.ifc_buf allocated by ifc.ifc_len amount of memory
	ifc.ifc_len = 110 * sizeof(*ifr);
	ifc.ifc_buf = lothars__malloc(
		ifc.ifc_len); // HACK: a mere guess, following R. Stevens

	lothars__ioctl(fd_sock, SIOCGIFCONF, &ifc);
	ifr = ifc.ifc_req;

	for (idx = 0, len = 0; idx < ifc.ifc_len;) {
		// on Linux
		len = sizeof(*ifr);
		// on some other unices there might be something possible like
		//len = IFNAMSIZ + ifr->ifr_addr.sa_len;

		/* name of interface */
		fprintf(stdout, "%-16s", ifr->ifr_name);
		{
			/* host ip */
			struct ifreq ifr_ifaddr;
			char host[128];
			memset(host, '\0', sizeof(host));
			memset(&ifr_ifaddr, 0, sizeof(ifr_ifaddr));

			// details to interface
			strncpy(ifr_ifaddr.ifr_name, ifr->ifr_name, IFNAMSIZ);

			lothars__ioctl(fd_sock, SIOCGIFADDR, &ifr_ifaddr);

			switch (ifr_ifaddr.ifr_addr.sa_family) {
			case AF_INET:
			case AF_INET6:
				getnameinfo(&ifr_ifaddr.ifr_addr,
					    sizeof(ifr_ifaddr.ifr_addr), host,
					    sizeof(host), 0, 0, NI_NUMERICHOST);
				break;
			default:
				strcpy(host, "unknown");
				break;
			}
			fprintf(stdout, "%s", host);
		}
		fprintf(stdout, "\t");

		{
			/* obtain mac address */
			/*
// Unix: note - R. Stevens describes the following approach for unix (SUN, HP-UX,...)
			struct sockaddr_in *sin;
			struct arpreq arpreq;
			uint8_t *parp = NULL;

			sin = (struct sockaddr_in*) &arpreq.arp_pa;
			memcpy(sin, (char*) &ifr->ifr_addr, sizeof(*sin));
			if (0 <= ioctl(fd_sock, SIOCGARP, (caddr_t) &arpreq)) {
				parp = (uint8_t*) &arpreq.arp_ha.sa_data[0];
				fprintf(stdout, " %x:%x:%x:%x:%x:%x", *parp, *(parp + 1), *(parp + 2), *(parp + 3), *(parp + 4), *(parp + 5));
/*/
			// Linux: usage of SIOCGARP seems to be rather a Unix approach, where Linux uses SIOCGIFHWADDR
			//        (not all interfaces have a mac!)
			struct ifreq ifr_hwifaddr;
			uint8_t *mac = NULL;

			strncpy(ifr_hwifaddr.ifr_name, ifr->ifr_name, IFNAMSIZ);

			lothars__ioctl(fd_sock, SIOCGIFHWADDR, &ifr_hwifaddr);

			if (ARPHRD_ETHER == ifr_hwifaddr.ifr_hwaddr.sa_family) {
				mac = (uint8_t *)&ifr_hwifaddr.ifr_hwaddr
					      .sa_data;
				fprintf(stdout, "%x:%x:%x:%x:%x:%x", mac[0],
					mac[1], mac[2], mac[3], mac[4], mac[5]);
				// */
			} else {
				fprintf(stdout, "<unknown>");
			}
		}
		fprintf(stdout, "\n");

		// HACK: move the char-ized pointer to ifr by len bytes (i.e. obtain next element)
		ifr = (struct ifreq *)(len + (char *)ifr);

		idx += len;
	}

	// cleanup
	if (NULL != ifc.ifc_buf)
		free(ifc.ifc_buf);
	ifc.ifc_buf = NULL;

	lothars__close(&fd_sock);
}

/********************************************************************************************/

/*
  main function to the UDP server (forked)
*/
int main(int argc, char **argv)
{
	int family;

	if (2 != argc) {
		fprintf(stderr, "usage: %s <inet4|inet6>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (0 == strncmp(argv[1], "inet4", sizeof("inet4"))) {
		family = PF_INET;
	} else if (0 == strncmp(argv[1], "inet6", sizeof("inet6"))) {
		family = PF_INET6;
	} else {
		err_quit("invalid address family, '%s'", argv[1]);
	}

	fprintf(stdout, "interfaces with an assigned IP:\n");
	get_interfaces(family);

	fprintf(stdout, "\n");
	fprintf(stdout, "READY.\n");
	exit(EXIT_SUCCESS);
}
