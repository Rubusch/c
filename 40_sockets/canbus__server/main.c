/*
  socketCAN - beacon sender

  @author: Lothar Rubusch <L.Rubusch@gmail.com>
  @license: GPLv3

  REFERENCE:
  https://en.wikipedia.org/wiki/SocketCAN
  https://www.kernel.org/doc/Documentation/networking/can.txt
 */

#include <stdint.h> /* uint8_t,... */
#include <mcheck.h> /* DEBUGGING */
#include <linux/can.h> /* CAN_MAX_DLEN */
#include <signal.h>

#include "can_interface.h"

static struct sigaction sa;

/*
pdu_p prepare_pdu(uint8_t destination, uint8_t dlc)
{
	pf->frame_id = 0;
	pf->frame_dlc = 0;
	memset(pf->data, 0, FRAME_MAX_DLEN);

	pf->frame_id = 0x123;

	pf->frame_dlc = 2;

	pf->data[0] = 0x11;
	pf->data[1] = 0x22;

	return NULL;
}
// */

/*
int message_handler()
{
	int ret = 0;

	// TODO fetch from rx_queue via canif__recv()
	// TODO concat the incoming pdu stream pieces, until _EMPTY_ frame received on ID
	return ret;
}
// */

void signalhandler(int sig, siginfo_t *siginfo, void *context)
{
	fprintf(stderr, "%s(): signal received\n", __func__);
	canif__shutdown();
	exit(EXIT_SUCCESS);
}


int main(int argc, char *argv[])
{
// TODO args: canif, canid(listen), canid(send)?
	int ret = -1;
	char if_name[CANIF_NAMESIZE];
	memset(if_name, '\0', CANIF_NAMESIZE);

	if (1 == argc) {
		strcpy(if_name, "can0");
		//  TODO 0x000 - 0x7ff, can identifier to set in beacon messages
	} else {
		int len = strlen(argv[1]);
		if (CANIF_NAMESIZE <= len - 1) {
			fprintf(stderr, "usage:\n\t%s\nor\n\t%s <IF Name> [<CAN ID>]\n",
				argv[0], argv[0]);
			exit(EXIT_FAILURE);
		}
		strncpy(if_name, argv[1], len);
	}
	fprintf(stderr, "if_name = '%s'\n", if_name);

#ifdef DEBUG
	mtrace();
#endif /* DEBUG */

	sa.sa_sigaction = &signalhandler;
	sa.sa_flags = SA_SIGINFO;
	sigaction(SIGTERM, &sa, NULL);

	ret = canif__startup(if_name, sizeof(if_name));
	if (ret < 0)
		goto err;

	
	// TODO fork: sender, who fetches from tx_queue (FIFO)
	// TODO fork: receiver, who puts on rx_queue (FIFO)
	// TODO fork/remain: call control() to fetch from rx_queue and deal (states) with messages, responding on tx_queue

	// TODO control (message_handler) fetches via canif__recv() from canif__fifo

/*
		// prepare dummy
		prepare_msg(&api_frame);

		// send (similar "client")
		ret = canif__send(&api_frame.frame_id, &api_frame.frame_dlc, api_frame.data);
		if (0 > ret) {
			break;
		}

		// send (similar "client")
		ret = canif__send(&api_frame.frame_id, &api_frame.frame_dlc, api_frame.data);
		if (0 > ret) {
			break;
		}

		// send (similar "client")
		ret = canif__send(&api_frame.frame_id, &api_frame.frame_dlc, api_frame.data);
		if (0 > ret) {
			break;
		}

		// send (similar "client")
		ret = canif__send(&api_frame.frame_id, &api_frame.frame_dlc, api_frame.data);
		if (0 > ret) {
			break;
		}
// */
//	} while (0);


err:
#ifdef DEBUG
	muntrace();
#endif /* DEBUG */
	canif__shutdown();
	if (ret >= 0)
		goto out;
	exit(EXIT_FAILURE);
out:
	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS); /* cleans up remaining acquired memory */
}
