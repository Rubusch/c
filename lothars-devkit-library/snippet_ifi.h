#ifndef DEVKIT_SNIPPET_IFI
#define DEVKIT_SNIPPET_IFI

/*

  ---
  References:
  Unix Network Programming, Stevens (1996)
*/

/* includes */

/*
// NB: ifi needs a declaration of _XOPEN_SOURCE or similar, e.g.
*/
#define _XOPEN_SOURCE 600

#include <unistd.h>
#include <fcntl.h>

/* // linux headers
// NB: this may not be sufficient..
#include <linux/in.h> // struct sockaddr_in
#include <linux/in6.h> // struct sockaddr_in6
#include <linux/un.h> // struct sockaddr_un
/*/ // alternative unix implementation
#include <netinet/in.h> // !!! place this header before <linux/.. headers or struct sockaddr might be unknown !!!
#include <sys/un.h> // struct sockaddr_un (official)
// */
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/sockios.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <ifaddrs.h>

#include "lib_socket.h"
#include "lib_unix.h"
#include "lib_error.h"

/* constants */

#define IFI_NAME 16 /* same as IFNAMSIZ in <net/if.h> */
#define IFI_HADDR 8 /* allow for 64-bit EUI-64 in future */

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

#define IFI_ALIAS 1 /* ifi_addr is an alias */

/* forwards */

struct ifi_info *get_ifi_info(int, int);
struct ifi_info *lothars__get_ifi_info(int, int);
void free_ifi_info(struct ifi_info *);

#endif /* DEVKIT_SNIPPET_IFI */
