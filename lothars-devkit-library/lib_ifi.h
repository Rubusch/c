#ifndef DEVKIT_LIB_IFI
#define DEVKIT_LIB_IFI

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


#endif /* DEVKIT_LIB_IFI */
