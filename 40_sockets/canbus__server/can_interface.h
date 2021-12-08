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
#include <pthread.h>
#include <linux/can.h>

#include "queue.h"
#include "can_frame.h"
#include "macros.h"


// TODO verify, or use "void*"
//create_queue(tx, void*, 1000)
create_queue(tx, struct can_frame*, 1000)
//create_queue(rx, void*, 1000)
create_queue(rx, struct can_frame*, 1000)

// TODO rework          
#define CANIF_NAMESIZE 16

/*
  e.g. shutdown $> cansend can0 410#15
 */


/* ID */// TODO rm
//#define CANIF_ID 0x010

/* channel on ID, to be ORed to the ID, e.g. 0x410 or 0x210 */
//#define CANIF_MGMT 0x400
//#define CANIF_COM 0x200
// further facilities can be here..

/* data, if MGMT the data[0] byte */
//#define CANIF_MGMT_TERM 0x15

/* drop limit */
//#define RCV_QUEUE_MAX 100


//#define dbg(content) fprintf(stderr, "%s(): %s\n", __func__, content);



/**
   start the interface socket, bind it to the name and unique
   identifier; in case fork off the server thread (listener thread)
 */
int canif__startup(const char *ifname, size_t ifname_size);

/**
   teardown of the socket
 */
int canif__shutdown();

/**
   send a can frame (put on tx_queue)
 */
int canif__send(const pdu_t *pdu);

/**
   fetches asynchronously from the rx_queue, return nbytes fetched or 0
 */
//int canif__recv(pdu_id_t *pdu_id, uint8_t *pdu_dlc, pdu_payload_t *data);
int canif__recv(pdu_p *pdu);

#ifdef TESTING

// TODO make fifo for test_frames (series)
struct can_frame *test__frame;
int test__tx_nbytes;
int test__rx_ret;

#endif /* TESTING */

#endif /* CAN_INTERFACE_H_ */
