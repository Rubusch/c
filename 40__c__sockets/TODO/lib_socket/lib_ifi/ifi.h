// ifi.h
/*
  interface info

  header for the programs that need interface configuration info.
  e.g. UDP binding support

  based on Stevens unix network programming (1996)
*/

#ifndef __ifi_h
#define __ifi_h


#include "../lib_socket.h"

#include <net/if.h>


#define IFI_NAME 16 // same as IFNAMSIZ in <net/if.h>
#define IFI_HADDR  8 // allow for 64-bit EUI-64 in future

struct ifi_info{
  char ifi_name[IFI_NAME]; // interface name, null-terminated 
  short ifi_index; // interface index 
  short ifi_mtu; // interface MTU
  u_char ifi_haddr[IFI_HADDR]; // hardware address
  u_short ifi_hlen; // # bytes in hardware address: 0, 6, 8 
  short ifi_flags; // IFF_xxx constants from <net/if.h> 
  short ifi_myflags; // our own IFI_xxx flags
  struct sockaddr *ifi_addr; // primary address
  struct sockaddr *ifi_brdaddr; // broadcast address
  struct sockaddr *ifi_dstaddr; // destination address
  struct ifi_info *ifi_next; // next of these structures 
};

#define IFI_ALIAS 1   // ifi_addr is an alias


struct ifi_info* get_ifi_info(int, int);
struct ifi_info* _get_ifi_info(int, int);
void free_ifi_info(struct ifi_info *);

#endif
