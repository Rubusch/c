/*
  interface header
  - maintain the interface
  - provide:
  -- startup interface
  -- shutdown interface
  -- send through interface
  -- receive from interface

  @author: Lothar Rubusch <L.Rubusch@gmail.com>
  @license: GPLv3
 */

#ifndef CAN_INTERFACE_H_
#define CAN_INTERFACE_H_


#define _GNU_SOURCE /* struct ifreq */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <linux/sockios.h>

#include <linux/can.h>




int canif__startup(const char *ifname, size_t ifname_size);
int canif__shutdown();
int canif__send();
int canif__recv();


#endif /* CAN_INTERFACE_H_ */
