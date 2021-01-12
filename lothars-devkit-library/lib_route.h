#ifndef DEVKIT_LIB_ROUTE
#define DEVKIT_LIB_ROUTE


/*
  routing

  routing socket support (still some if_nametoindex / if_indextoname
  missing, supported by lib_socket)

  ---
  References:
  Unix Network Programming, Stevens (1996)
*/
// TODO rework                                             


/* includes */

//#define _XOPEN_SOURCE 600

#include <stdlib.h>
#include <stdio.h>

#include <fcntl.h> /* fcntl(), F_GETFL */
#include <unistd.h> /* socklen_t */
#include <netinet/in.h> /* struct sockaddr */
#include <linux/rtnetlink.h> /* RTAX_MAX */
#include <sys/sysctl.h> /* CTL_NET, sysctl() */

#include "lib_error.h"


/* constants / macros */

/*
  Round up 'a' to next multiple of 'size', which must be a power of 2
*/
#define ROUNDUP(a, size) (((a) & ((size)-1)) ? (1 + ((a) | ((size)-1))) : (a))

/*
  Step to next socket address structure. If sa_len is 0, assume it is
  sizeof(u_long).
*/

/*#define NEXT_SA(ap)					\
	ap = (struct sockaddr *)					\
		((caddr_t) ap + (ap->sa_len ? ROUNDUP(ap->sa_len, sizeof (u_long)) : \
				 sizeof(u_long)))
// */


/* forwards */

//void  lothars__get_rtaddrs(int, struct sockaddr *, struct sockaddr **);
//char* lothars__net_rt_iflist(int, int, size_t *);
//char* lothars__net_rt_dump(int, int, size_t *);
//const char* lothars__sock_masktop(struct sockaddr *, socklen_t);


#endif /* DEVKIT_LIB_ROUTE */
