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

  asynchronous implementation via callback and async listener


  device #1:
  $ ip l set can0 up type can bitrate 125000 sample-point 0.875 restart-ms 100
  $ ./can_demo.exe can0
    if_name = 'can0'
    can0 at index 4
    CAN_FRAME DEBUG canif__send() - id: 123, dlc: 3, data: 11 22 33
    CAN_FRAME DEBUG canif__send() - id: 123, dlc: 3, data: 11 22 33
    CAN_FRAME DEBUG canif__send() - id: 123, dlc: 3, data: 11 22 33
    CAN_FRAME DEBUG canif__send() - id: 123, dlc: 3, data: 11 22 33
    DEBUG: frame_id: '123', frame_dlc: '3', 112233
  $
  <main() came back, the listener continues to run>
  <receiving "terminate">
  $ CAN_FRAME DEBUG canif__listener() - id: 410, dlc: 1, data: 15
    DEBUG: triggering socket signalling - 410
    CANIF - shutting down.


  device #2:
  $ ip l set can0 up type can bitrate 125000 sample-point 0.875 restart-ms 100
  <send terminate command>
  $ cansend can0 410#15


  device #3 (or second shell on #2)
  $ candump -L can0
    (1632840876.558852) can0 123#112233
    (1632840876.560014) can0 123#112233
    (1632840876.561136) can0 123#112233
    (1632840876.562281) can0 123#112233
  <now, send terminate command>
    (1632841020.926788) can0 410#15


  @author: Lothar Rubusch <L.Rubusch@gmail.com>
  @license: GPLv3

  REFERENCE:
  https://en.wikipedia.org/wiki/SocketCAN
  https://www.kernel.org/doc/Documentation/networking/can.txt
 */

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
	fprintf(stdout, "DEBUG: frame_id: '%X', frame_dlc: '%d', ", pf->frame_id, pf->frame_dlc);
	for (int idx=0; idx<pf->frame_dlc ; idx++)
		fprintf(stdout, "%X", pf->data[idx]);
	fprintf(stdout, "\n");
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
main(int argc, char* argv[])
{
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

	// register callback
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

	exit(EXIT_SUCCESS); /* cleans up remaining acquired memory */
}
