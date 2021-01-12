#ifndef DEVKIT_LIB_MCAST
#define DEVKIT_LIB_MCAST


/*
  multi cast (mcast) snippets

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


  Especially in the case of streaming multimedia, the combination of
  an IP multicast address (either IPv4 or IPv6) and a transport-layer
  port (typically UDP) is referred to as a session. These sessions -
  e.g. an audio/video teleconference may comprise two sessions -
  almost always use different ports and sometimes also use different
  groups for flexibility in choice when receiving.

  ---

  References:
  Unix Network Programming, Stevens (1996) book and sources
*/


/* includes */

//#define _XOPEN_SOURCE 600

#include <stdlib.h>
#include <stdio.h>

#include <fcntl.h> /* fcntl(), F_GETFL */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h> /* IFNAMSIZ */
#include <linux/sockios.h> /* SIOCGIFADDR */

#include "lib_socket_demo.h"
#include "lib_error.h"


/* forwards */

void lothars__mcast_leave(int, const struct sockaddr *, socklen_t);
void lothars__mcast_join(int, const struct sockaddr *, socklen_t, const char *, uint32_t);
void lothars__mcast_leave_source_group(int, const struct sockaddr *, socklen_t, const struct sockaddr *, socklen_t);
void lothars__mcast_join_source_group(int, const struct sockaddr *, socklen_t, const struct sockaddr *, socklen_t, const char *, uint32_t);
void lothars__mcast_block_source(int, const struct sockaddr *, socklen_t, const struct sockaddr *, socklen_t);
void lothars__mcast_unblock_source(int, const struct sockaddr *, socklen_t, const struct sockaddr *, socklen_t);
int lothars__mcast_get_if(int);
int lothars__mcast_get_loop(int);
int lothars__mcast_get_ttl(int);
void lothars__mcast_set_if(int, const char *, uint32_t);
void lothars__mcast_set_loop(int, int);
void lothars__mcast_set_ttl(int, int);


#endif /* DEVKIT_LIB_MCAST */
