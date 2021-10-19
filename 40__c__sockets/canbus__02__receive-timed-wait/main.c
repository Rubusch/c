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

  blocking read with timeout (select + tv)


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
typedef struct api_frame_s *api_frame_p;
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

int main(void)
{
	int ret = -1;
	char if_name[] = "can0";

	ret = canif__startup(if_name, sizeof(if_name));
	if (0 > ret) {
		goto err;
	}

	api__prepare_msg(&api_frame);

	// listen
	do {
		ret = canif__recv(&api_frame.frame_id, &api_frame.frame_dlc,
				  api_frame.data, 10000);
		if (0 > ret) {
			fprintf(stderr, "canif__recv() failed\n");
			goto err;
		}

		sleep(1);
	} while (0 == ret);

	// send returning the frame
	ret = canif__send(&api_frame.frame_id, &api_frame.frame_dlc,
			    api_frame.data);
	if (0 > ret) {
		goto err;
	}


err:
	// cleanup
	canif__shutdown();
	if (0 == ret)
		goto done;
	exit(EXIT_FAILURE);
done:
	fprintf(stdout, "READY.\n");
	exit(EXIT_SUCCESS); /* cleans up remaining acquired memory */
}
