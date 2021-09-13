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

  asynchronous implementation

  startup initializes and binds a socketcan socket (CAN_RAW), and
  forks off a listener process; the process listenes on the socket, in
  case it receives it puts the frames onto a QUEUE; a canif__recv()
  pops what is already in the queue.
  sending goes directly

  @author: Lothar Rubusch <L.Rubusch@gmail.com>
  @license: GPLv3

  REFERENCE:
  https://en.wikipedia.org/wiki/SocketCAN
  https://www.kernel.org/doc/Documentation/networking/can.txt
 */
// TODO verification via rapsi CAN HAT RS485

#include <stdint.h> /* uint8_t,... */

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

int
main(void)
{
	int ret = -1;
	char if_name[] = "can0";

	do {
		// startup
		canif__startup(if_name, sizeof(if_name));

		// prepare dummy
		api__prepare_msg(&api_frame);

		// send (similar "client")
		canif__send(&api_frame.frame_id, &api_frame.frame_dlc, api_frame.data);

		// listen (similar "server")
		// loop on a receive stack, continue if something was received
		while (0 == (ret = canif__recv(&api_frame.frame_id, &api_frame.frame_dlc, api_frame.data))) {
			sleep(1);
		}
		if (0 > ret) {
			perror("canif__recv() failed");
			break;
		}

		// print message
		//TODO api__print();   

	} while (0);

	// cleanup
	canif__shutdown();
	// TODO       

	exit(EXIT_SUCCESS); /* cleans up remaining acquired memory */
}
