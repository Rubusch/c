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

__attribute__((__unused__))   
static int sockfd;

/* pthread: no static INITIALIZERs, we want control startup and
 * teardown of ptread stuff */
//*
static pthread_t tx_listener_tid;
static pthread_mutex_t tx_mutex;
static pthread_cond_t tx_cv;
static pthread_attr_t tx_listener_attr;
/*/
pthread_t tx_listener_tid;
pthread_mutex_t tx_mutex;
pthread_cond_t tx_cv;
pthread_attr_t tx_listener_attr;
// */

//*
static pthread_t rx_listener_tid;
static pthread_mutex_t rx_mutex;
//static pthread_cond_t rx_cv = PTHREAD_COND_INITIALIZER;
static pthread_attr_t rx_listener_attr;
/*/
pthread_t rx_listener_tid;
pthread_mutex_t rx_mutex;
//static pthread_cond_t rx_cv = PTHREAD_COND_INITIALIZER;
pthread_attr_t rx_listener_attr;
// */

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

static void *canif__rx_listener()
{
	dbg(__func__);

	struct can_frame *frame = NULL; // TODO fix supposed leak by making this pointer global, then if not NULL delete in shutdown
	int ret = -1;

	do {

#ifdef TESTING
		while(NULL == (frame = test__frame)) {
			sleep(1);
		}
		ret = test__rx_ret; /* defaults to 0 */


#else
		if (!frame) {
// FIXME: leaks allocated memory when shutting down, improve    
			frame = malloc(sizeof(*frame));
			if (!frame) {
				canif__failure("allocation failed", __func__);
			}
		}
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

		pthread_mutex_lock(&rx_mutex);
		rx__enqueue(frame);
 		pthread_mutex_unlock(&rx_mutex);

		frame = NULL;

#ifdef TESTING
		test__frame = NULL;
#endif /* TESTING */

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
/*
	pthread_condattr_t tx_cv_attr;
	pthread_condattr_init(&tx_cv_attr);
	pthread_condattr_setpshared(&tx_cv_attr, PTHREAD_PROCESS_SHARED);
	pthread_cond_init(&tx_cv, &tx_cv_attr);
/*/
	pthread_cond_init(&tx_cv, NULL);
// */

	int ret;
#ifdef TESTING
	;
#else
	sockfd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	if (-1 == sockfd) {
		canif__failure("socket() failed", __func__);
	}

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

	pthread_attr_init(&rx_listener_attr);
	pthread_attr_setdetachstate(&rx_listener_attr, PTHREAD_CREATE_DETACHED);
	ret = pthread_create(&rx_listener_tid,
			     &rx_listener_attr,
			     canif__rx_listener,
			     NULL);
	if (0 != ret) {
		canif__failure("pthread_create()", __func__);
	}

	pthread_attr_init(&tx_listener_attr);
	pthread_attr_setdetachstate(&tx_listener_attr, PTHREAD_CREATE_DETACHED);
	ret = pthread_create(&tx_listener_tid,
			     &tx_listener_attr,
			     canif__tx_listener,
			     NULL);
	if (0 != ret) {
		canif__failure("pthread_create()", __func__);
	}

	return 0;
}

int canif__shutdown()
{
	dbg(__func__);

	struct can_frame *content = NULL;
	pthread_cancel(tx_listener_tid);
	pthread_cancel(rx_listener_tid);


	while (0 < rx__size()) {
		rx__dequeue(&content);
		if (content != NULL) {
			free(content);
		}
		content = NULL;
	}

	while (0 < tx__size()) {
		tx__dequeue(&content);
		if (content != NULL) {
			free(content);
		}
		content = NULL;
	}

	pthread_mutex_destroy(&tx_mutex);
	pthread_mutex_destroy(&rx_mutex);

	pthread_cond_destroy(&tx_cv);

	pthread_attr_destroy(&tx_listener_attr);
	pthread_attr_destroy(&rx_listener_attr);

	sleep(1); /* safety! - pthread teardown followed by a startup
		   * right away (e.g. unittests) may cause
		   * segmentation violation faults, due to destruction
		   * followed by immediate recreation and/or still
		   * usage of to be freed element e.g. cond var,
		   * i.e. of the same synch mechanisms itself */

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

	if (!pdu) {
		return -1;
	}

	int ret = -1;
	struct can_frame *frame;
	frame = malloc(sizeof(*frame));
	if (!frame) { canif__failure("allocation failed", __func__); }
	memset(frame, 0, sizeof(*frame));

	init_can_id(pdu, (uint16_t*) &frame->can_id);
	init_can_dlc(pdu, &frame->can_dlc);
	init_can_data(pdu, frame->data);
	dbg_frame(__func__, frame);
	ret = tx__enqueue(frame);

	pthread_cond_signal(&tx_cv); // signal: offer a ticket for a
				     // single read on the queue to
				     // the listener
	dbg_end(__func__);
	return ret;
}

int canif__recv(pdu_p *pdu)
{
	dbg(__func__);

	int ret = -1;
	struct can_frame* frame = NULL;

	pthread_mutex_lock(&rx_mutex);
	if (rx__empty()) {
		pthread_mutex_unlock(&rx_mutex);
		return 0;
	}

	rx__dequeue(&frame);
	pthread_mutex_unlock(&rx_mutex);

	ret = frame->can_dlc;
	*pdu = create_pdu();
	init_pdu_from_can(*pdu, frame);
	free(frame); frame = NULL;

	dbg_end(__func__);
	return ret;
}
