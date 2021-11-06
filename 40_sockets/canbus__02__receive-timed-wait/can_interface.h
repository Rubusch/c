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
#include <stdint.h> /* uint8_t */

#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <linux/sockios.h>

#include <linux/can.h>

#define CANIF__DEBUG 1

/**
   start the interface socket, bind it to the name and unique
   identifier; in case fork off the server thread (listener thread)
 */
int canif__startup(const char *ifname, size_t ifname_size);

/**
 */
int canif__shutdown();

/**
 */
int canif__send(const uint32_t *can_id, const uint8_t *can_dlc, uint8_t data[]);

/**
   fetches asynchronously from the queue, returns nbytes or 0 in case the queue was empty
 */
int canif__recv(uint32_t *can_id, uint8_t *can_dlc, uint8_t data[],
		long timeout);

#endif /* CAN_INTERFACE_H_ */
