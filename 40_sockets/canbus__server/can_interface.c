/*
  can_interface: implementation

  @author: Lothar Rubusch <L.Rubusch@gmail.com>
  @license: GPLv3
 */

#include "can_interface.h"
#include "macros.h"

#include <stdbool.h>
#include <pthread.h>

#ifndef _GNU_SOURCE
# define _GNU_SOURCE 1
#endif
#include <string.h> /* strerror_r() - threadsave */


/* private */

static int sockfd;

/* pthread: no static INITIALIZERs, we want control startup and
 * teardown of ptread stuff */
static pthread_t tx_listener_tid;
static pthread_mutex_t tx_mutex;
static pthread_cond_t tx_cv;
static pthread_attr_t tx_listener_attr;

static pthread_t rx_listener_tid;
static pthread_mutex_t rx_mutex;
//static pthread_cond_t rx_cv = PTHREAD_COND_INITIALIZER;
static pthread_attr_t rx_listener_attr;

#ifndef TESTING
static struct sockaddr_can addr;
static struct ifreq ifr;
#endif /* not TESTING */

static void canif__failure(char* msg, const char* func)
{
	char message[BUFSIZ];
	sprintf(message, "%s in %s", msg, func);
	perror(message);
	canif__shutdown();
	exit(EXIT_FAILURE);
}

__attribute__((__unused__))    
static void *canif__rx_listener()
{
	dbg(__func__);

	struct can_frame *frame = NULL;
	int ret = -1;

	do {
		if (!frame) {
// FIXME: not leak safe    
//			frame = malloc(sizeof(*frame));
//			if (!frame) {
//				canif__failure("allocation failed", __func__);
//			}
		}
#ifdef TESTING
		ret = test__rx_ret; /* defaults to 0 */
#else
		ret = read(sockfd, frame, sizeof(frame));
#endif /* TESTING */
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

//		pthread_mutex_lock(&rx_mutex);
		rx__enqueue(frame);
//		pthread_mutex_unlock(&rx_mutex);

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
		pthread_cond_wait(&tx_cv, &tx_mutex);
		pthread_mutex_unlock(&tx_mutex);

		tx__dequeue(&frame);

		if (!frame) {
			fprintf(stderr, "%s(): frame was empty\n", __func__);
			continue;
		}
#ifdef TESTING
		test__frame = frame;
		test__tx_nbytes = test__frame->can_dlc;
		nbytes = test__tx_nbytes;
#else
		nbytes = write(sockfd, &frame, sizeof(struct can_frame));
#endif /* TESTING */

		if (0 > nbytes) {
			fprintf(stderr, "write() failed\n");
		}

#ifdef TESTING
		; // NOTE: free can_frame in test environment!
#else
		free(frame); frame = NULL;
#endif /* TESTING */
	} while (true);

	dbg_end(__func__);
	pthread_exit(NULL);
}


/* public */

int canif__startup(const char *ifname, size_t ifname_size)
{
	dbg(__func__);

	pthread_mutex_init(&tx_mutex, NULL);
	pthread_mutex_init(&rx_mutex, NULL);

// when using attributes, e.g. shared...
//	pthread_condattr_t tx_cv_attr;
//	pthread_condattr_init(&tx_cv_attr);
//	pthread_condattr_setpshared(&tx_cv_attr, PTHREAD_PROCESS_SHARED);
//	pthread_cond_init(&tx_cv, &tx_cv_attr);

	pthread_cond_init(&tx_cv, NULL);
//	rx_cv = PTHREAD_COND_INITIALIZER;

	int ret;
	sockfd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	if (-1 == sockfd) {
		canif__failure("socket() failed", __func__);
	}

#ifdef TESTING
	;
#else

	strncpy(ifr.ifr_name, ifname, ifname_size);
	ifr.ifr_name[ifname_size - 1] = '\0';
	if (0 > ioctl(sockfd, SIOCGIFINDEX, &ifr)) {
		canif__failure("Error in ioctl", __func__);
	}

	memset(&addr, 0, sizeof(addr));
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;
	fprintf(stderr, "%s at index %d\n", ifname, ifr.ifr_ifindex);
	if (-1 == bind(sockfd, (struct sockaddr *)&addr, sizeof(addr))) {
		close(sockfd);
		canif__failure("Error at binding socket to address", __func__);
	}
#endif /* TESTING */

/*
	pthread_attr_init(&rx_listener_attr);
	pthread_attr_setdetachstate(&rx_listener_attr, PTHREAD_CREATE_DETACHED);
	ret = pthread_create(&rx_listener_tid, &rx_listener_attr, canif__rx_listener, NULL);
	if (0 != ret) {
		canif__failure("pthread_create()", __func__);
	}
// */

	pthread_attr_init(&tx_listener_attr);
	pthread_attr_setdetachstate(&tx_listener_attr, PTHREAD_CREATE_DETACHED);
	ret = pthread_create(&tx_listener_tid, &tx_listener_attr, canif__tx_listener, NULL);
	if (0 != ret) {
		canif__failure("pthread_create()", __func__);
	}

// TODO join needed?
//	void *status;
//	pthread_join(rx_listener_tid, &status);
//	pthread_join(tx_listener_tid, &status);
//	canif__shutdown();

	return 0;
}

int canif__shutdown()
{
	dbg(__func__);

	pthread_cancel(tx_listener_tid);
	pthread_cancel(rx_listener_tid);

// TODO rm, static initializer
	pthread_mutex_destroy(&tx_mutex);
	pthread_mutex_destroy(&rx_mutex);

// TODO rm, static initializer
	pthread_cond_destroy(&tx_cv);
//	pthread_cond_destroy(&rx_cv);

	pthread_attr_destroy(&tx_listener_attr);
	pthread_attr_destroy(&rx_listener_attr);

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

	sleep(1); /* for safety */
	dbg_end(__func__);
#ifdef TESTING
	return 0;
#else
	return close(sockfd);
#endif /* TESTING */
}

int canif__send(const pdu_t *pdu)
{
	dbg(__func__);

	int ret = -1;
	struct can_frame *frame;
	frame = malloc(sizeof(*frame));
	if (!frame) { canif__failure("allocation failed", __func__); }

	init_can_id(pdu, (uint16_t*) &frame->can_id);
	init_can_dlc(pdu, &frame->can_dlc);
	init_can_data(pdu, frame->data);

	dbg_frame(frame);

	ret = tx__enqueue(frame);
	pthread_cond_signal(&tx_cv); // signal: offer a ticket for a single read on the queue to the listener

	dbg_end(__func__);
	return ret;
}

int canif__recv(pdu_id_t *pdu_id, uint8_t *pdu_dlc, pdu_payload_t *data)
{
	dbg(__func__);

	struct can_frame* frame;

	pthread_mutex_lock(&rx_mutex);
// FIXME: message arrived and on queue, rather test if queue empty
//	pthread_cond_wait(&rx_cv, &rx_mutex);
	rx__dequeue(&frame); // TODO check memory leakage
	pthread_mutex_unlock(&rx_mutex);

	pdu_id->id = (uint16_t) frame->can_id;
	*pdu_dlc = frame->can_dlc;
	memcpy(data, frame->data, frame->can_dlc);

	free(frame);

	dbg_end(__func__);
	return 0;
}
