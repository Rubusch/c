/*
  can_interface: implementation

  @author: Lothar Rubusch <L.Rubusch@gmail.com>
  @license: GPLv3
 */

#include "can_interface.h"
#include "macros.h"

#include <stdbool.h>
#include <pthread.h>



/*
  "private"
*/
static int sockfd;
static pthread_t tx_listener_tid;
static pthread_mutex_t tx_mutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_t rx_listener_tid;
static pthread_mutex_t rx_mutex = PTHREAD_MUTEX_INITIALIZER;

//static pid_t pid_listener;
static struct sockaddr_can addr;
static struct ifreq ifr;

static void canif__failure(char* msg, const char* func)
{
	char message[BUFSIZ];
	sprintf(message, "%s in %s", msg, func);
	perror(message);
	canif__shutdown();
	exit(EXIT_FAILURE);
}


static void *canif__rx_listener()
{
	dbg(__func__);

	struct can_frame *frame = NULL;
	int ret = -1;

	do {
		if (!frame) {
			frame = malloc(sizeof(*frame));
			if (!frame) {
				canif__failure("allocation failed", __func__);
			}
		}

		ret = read(sockfd, frame, sizeof(frame));
		if (0 > ret) {
			canif__failure("read() failed", __func__);
			break;
		} else if (ret == 0) {
			continue;
		}

// TODO check if package can be dropped, when off - promiscuous
//		if (! (frame->can_id & CANIF_MY_ID)) {
//			fprintf(stderr, "%s() dropped\n", __func__);
//			continue;
//		}

		pthread_mutex_lock(&rx_mutex);
		rx__enqueue(frame);
		pthread_mutex_unlock(&rx_mutex);

		frame = NULL;
	} while (true);

	dbg_end(__func__);
	pthread_exit(NULL);
}


static void *canif__tx_listener()
{
	dbg(__func__);
	struct can_frame *frame = NULL;
	int nbytes = -1;

	do {
		pthread_mutex_lock(&tx_mutex);
		tx__dequeue(&frame);
		pthread_mutex_unlock(&tx_mutex);

		if (!frame) {
			continue;
		}

#ifdef TESTING
		test_frame = frame;
		nbytes = test__tx_nbytes;
#else
		nbytes = write(sockfd, &frame, sizeof(struct can_frame));
#endif /* TESTING */
		if (0 > nbytes) {
			fprintf(stderr, "write() failed\n");
		}

		free(frame); frame = NULL;
	} while (true);

	dbg_end(__func__);
	pthread_exit(NULL);
}


/*
  "public" interface
*/

int canif__startup(const char *ifname, size_t ifname_size)
{
	int ret;
#ifdef TESTING

	// TODO testing preparations   

#else
	sockfd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	if (-1 == sockfd) {
		perror("Error while opening socket");
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, ifname_size);
	ifr.ifr_name[ifname_size - 1] = '\0';
	if (0 > ioctl(sockfd, SIOCGIFINDEX, &ifr)) {
		perror("Error in ioctl");
		return -1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;
	fprintf(stderr, "%s at index %d\n", ifname, ifr.ifr_ifindex);
	if (-1 == bind(sockfd, (struct sockaddr *)&addr, sizeof(addr))) {
		perror("Error at binding socket to address");
		close(sockfd);
		return -1;
	}
#endif /* TESTING */

	ret = pthread_create(&rx_listener_tid, NULL, canif__rx_listener, NULL);
	if (0 != ret) {
		canif__failure("pthread_create()", __func__);
	}

	ret = pthread_create(&tx_listener_tid, NULL, canif__tx_listener, NULL);
	if (0 != ret) {
		canif__failure("pthread_create()", __func__);
	}

	void *status;
	pthread_join(rx_listener_tid, &status);
	pthread_join(tx_listener_tid, &status);
	canif__shutdown();

	return 0;
}


int canif__shutdown()
{
	dbg(__func__);
	struct can_frame *content = NULL;

	while (0 < rx__size()) {
		rx__dequeue(&content);
		free(content);
		content = NULL;
	}

	while (0 < tx__size()) {
		tx__dequeue(&content);
		free(content);
		content = NULL;
	}

	dbg_end(__func__);
	return close(sockfd);
}


//int canif__send(const pdu_id_t *pdu_id, uint8_t pdu_dlc, const pdu_payload_t *data)
int canif__send(const pdu_t *pdu)
{
	dbg(__func__);

	int ret = -1;
	struct can_frame *frame;

	frame = malloc(sizeof(*frame));
	if (!frame) { canif__failure("allocation failed", __func__); }

	frame->can_id = (uint16_t) pdu->id.id;
	frame->can_dlc = pdu->dlc;
//	memcpy(frame->data, pdu->data->payload, frame->can_dlc); // TODO 

	dbg_frame(frame);

	pthread_mutex_lock(&tx_mutex);
	ret = tx__enqueue(frame);
	pthread_mutex_unlock(&tx_mutex);

	dbg_end(__func__);
	return ret;
}


int canif__recv(pdu_id_t *pdu_id, uint8_t *pdu_dlc, pdu_payload_t *data)
{
	dbg(__func__);
	struct can_frame* frame;

	pthread_mutex_lock(&rx_mutex);
	rx__dequeue(&frame); // TODO check memory leakage
	pthread_mutex_unlock(&rx_mutex);

	pdu_id->id = (uint16_t) frame->can_id;
	*pdu_dlc = frame->can_dlc;
	memcpy(data, frame->data, frame->can_dlc);

	free(frame);

	dbg_end(__func__);
	return 0;
}
