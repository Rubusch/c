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

api_frame_p api__frame(uint32_t can_id, uint8_t can_dlc, uint8_t data[])
{
	api_frame_p pf;
	pf = malloc(sizeof(*pf));
	if (NULL == pf) {
		perror("malloc() failed");
		exit(EXIT_FAILURE);
	}
	pf->frame_id = can_id;
	pf->frame_dlc = can_dlc;
	memset(pf->data, '\0', FRAME_MAX_DLEN);
	memcpy(pf->data, data, pf->frame_dlc);

	return pf;
}

void api__display_msg(api_frame_p pf)
{
	if (!pf) {
		fprintf(stderr, "DEBUG: pf: NULL\n");
		return;
	}
	fprintf(stdout, "DEBUG: frame_id: '%d', frame_dlc: '%d', data: '%s'\n", pf->frame_id, pf->frame_dlc, pf->data);
}

void api__prepare_msg(api_frame_p pf)
{
	pf->frame_id = 0;
	pf->frame_dlc = 0;
	memset(pf->data, 0, FRAME_MAX_DLEN);

	pf->frame_id = 0x123;

	pf->frame_dlc = 3;

	pf->data[0] = 0x11;
	pf->data[1] = 0x22;
	pf->data[2] = 0x33;
}

int on_receive(uint32_t can_id, uint8_t can_dlc, uint8_t data[])
{
	api_frame_p pf;
	pf = api__frame(can_id, can_dlc, data);
	if (!pf) {
		perror("Internal error in receiver");
		return -1;
	}
	api__display_msg(pf);
	return 0;
}

int
main(void)
{
	char if_name[] = "can0";

	canif__register_recv(&on_receive);

	do {
		// startup
		if (0 > canif__startup(if_name, sizeof(if_name))) {
			break;
		}


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

		// print message
		api__display_msg(&api_frame);


	} while (0);

	// cleanup
	canif__shutdown();

	exit(EXIT_SUCCESS); /* cleans up remaining acquired memory */
}
