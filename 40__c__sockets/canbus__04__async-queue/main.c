/*
  socketCAN demo

  based on socketCAN, wikipedia and kernel.org

  +------------------------+ ============
  | Application            |     |
  +------------------------+     |
               A                 |  communication
               |                 }- stack,
               V                 |  e.g. CANopen
  +------------------------+     |
  | Socket Layer           |     |
  +------------------------+ ============
  | PF_CAN | PF_INET | ... |  |
  +--------+---------+-----+  }- SocketCAN
  | Network Device Drivers |  |
  +------------------------+ ===
  | CAN Controller         |
  +------------------------+

  asynchronous implementation using a listener and fifo

  CAN_A $ ip l set can0 up type can bitrate 125000 sample-point 0.875 restart-ms 100

                                     CAN_B $ $ ip l set can0 up type can bitrate 125000 sample-point 0.875 restart-ms 100

  CAN_A $ ./can_demo.exe
      if_name = 'can0'
      can0 at index 4
      canif__listener() started

                                     // send "COMMUNICATION" message 11.22.33
                                     CAN_B $ cansend can0 210#11.22.33

      CAN_FRAME DEBUG canif__listener() - id: 210, dlc: 3, data: 11 22 33
      canif__listener() communication channel

                                     // send message for someone else (shall be dropped)
                                     CAN_B $ cansend can0 000#11.22.33

      CAN_FRAME DEBUG canif__listener() - id: 0, dlc: 3, data: 11 22 33
      canif__listener() dropped

                                     // send TERM message on ID "MANAGEMENT" channel
                                     CAN_B $ cansend can0 410#15

      CAN_FRAME DEBUG canif__listener() - id: 410, dlc: 1, data: 15
      canif__listener() management channel
      DEBUG: triggering socket signalling - 410
      canif__listener() ended
      READY.


  checks:
  CAN_A $ mtrace ./can_demo.exe ./out.log
      No memory leaks.

  $ ip -statistics -details link show can0
    4: can0: <NOARP,UP,LOWER_UP,ECHO> mtu 16 qdisc pfifo_fast state UP mode DEFAULT group default qlen 10
      link/can  promiscuity 0 minmtu 0 maxmtu 0
      can state ERROR-ACTIVE restart-ms 100
            bitrate 125000 sample-point 0.875
            tq 500 prop-seg 6 phase-seg1 7 phase-seg2 2 sjw 1
            mcp251x: tseg1 3..16 tseg2 2..8 sjw 1..4 brp 1..64 brp-inc 1
            clock 6000000
            re-started bus-errors arbit-lost error-warn error-pass bus-off
            0          0          0          1          1          0         numtxqueues 1 numrxqueues 1 gso_max_size 65536 gso_max_segs 65535
      RX: bytes  packets  errors  dropped overrun mcast
      90         40       0       0       0       0
      TX: bytes  packets  errors  dropped carrier collsns
      160        78       0       0       0       0


  @author: Lothar Rubusch <L.Rubusch@gmail.com>
  @license: GPLv3

  REFERENCE:
  https://en.wikipedia.org/wiki/SocketCAN
  https://www.kernel.org/doc/Documentation/networking/can.txt
 */

#include <stdint.h> /* uint8_t,... */
#include <mcheck.h> /* DEBUGGING */

#include <linux/can.h> /* CAN_MAX_DLEN */
#define FRAME_MAX_DLEN CAN_MAX_DLEN

#include "can_interface.h"


typedef struct api_frame_s {
	uint32_t frame_id;
	uint8_t frame_dlc;
	uint8_t data[FRAME_MAX_DLEN]; /* can be a union, proprietary
				       * frame format - dlc will be
				       * size as number of used
				       * bytes! */
} api_frame_t;
typedef struct api_frame_s* api_frame_p;
static api_frame_t api_frame;

void api__prepare_msg(api_frame_p pf)
{
	pf->frame_id = 0;
	pf->frame_dlc = 0;
	memset(pf->data, 0, FRAME_MAX_DLEN);

	pf->frame_id = 0x123;

	pf->frame_dlc = 2;

	pf->data[0] = 0x11;
	pf->data[1] = 0x22;
}

int main(int argc, char* argv[])
{
	int ret = -1;
	char if_name[CANIF_NAMESIZE];
	memset(if_name, '\0', CANIF_NAMESIZE);
	if (1 == argc) {
		strcpy(if_name, "can0");
	} else {
		int len = strlen(argv[1]);
		if (CANIF_NAMESIZE <= len-1) {
			fprintf(stderr, "usage:\n\t%s\nor\n\t%s <IF Name>\n", argv[0], argv[0]);
			exit(EXIT_FAILURE);
		}
		strncpy(if_name, argv[1], len);
	}
	fprintf(stdout, "if_name = '%s'\n", if_name);

	mtrace();

	do {
		// startup
		canif__startup(if_name, sizeof(if_name));

		// prepare dummy
		api__prepare_msg(&api_frame);

		// send (similar "client")
		if (0 > canif__send(&api_frame.frame_id, &api_frame.frame_dlc, api_frame.data)) {
			break;
		}

		// send (similar "client")
		if (0 > canif__send(&api_frame.frame_id, &api_frame.frame_dlc, api_frame.data)) {
			break;
		}

		// send (similar "client")
		if (0 > canif__send(&api_frame.frame_id, &api_frame.frame_dlc, api_frame.data)) {
			break;
		}

		// send (similar "client")
		if (0 > canif__send(&api_frame.frame_id, &api_frame.frame_dlc, api_frame.data)) {
			break;
		}

	} while (0);

	// finally fetch all - separate thread
	do {
		ret = canif__recv(&api_frame.frame_dlc, api_frame.data);
		if (0 > ret)
			break;

		fprintf(stdout, "RECV: ");
		for (int idx=0; idx<api_frame.frame_dlc; idx++) {
			fprintf(stdout, "%x", api_frame.data[idx]);
		}
		fprintf(stdout, ", (%d)\n", api_frame.frame_dlc);

	} while (ret == 0);
	canif__shutdown();

	muntrace();

	fprintf(stdout, "READY.\n");
	exit(EXIT_SUCCESS); /* cleans up remaining acquired memory */
}
