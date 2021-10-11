/*
  can_interface: implementation

  @author: Lothar Rubusch <L.Rubusch@gmail.com>
  @license: GPLv3
 */

#include "can_interface.h"

#define CANIF__DEBUG 1

#if defined CANIF__DEBUG

static void canif__print(const char *func, const struct can_frame *frame)
{
	fprintf(stdout, "CAN_FRAME DEBUG %s() - id: %x, dlc: %d, data: ", func,
		frame->can_id, frame->can_dlc);
	for (int idx = 0; idx < frame->can_dlc; idx++) {
		fprintf(stdout, "%x ", frame->data[idx]);
	}
	fprintf(stdout, "\n");
}

#endif

/*
  "private"
*/
static int sockfd;
static pid_t pid_listener;
static struct sockaddr_can addr;
static struct ifreq ifr;

void canif__listener()
{
	struct can_frame frame;
	content_p content = NULL;
	int ret = -1;

#if defined CANIF__DEBUG
	fprintf(stdout, "%s() started\n", __func__);
#endif

	do {
		content = NULL;
		ret = read(sockfd, &frame, sizeof(frame));
		if (-1 == ret) {
			perror("Error in read()");
			break;
		}

#if defined CANIF__DEBUG
		canif__print(__func__, &frame);
#endif

		if ((CANIF_ID | CANIF_MGMT) == frame.can_id) {
#if defined CANIF__DEBUG
			fprintf(stdout, "%s() management channel\n", __func__);
			fprintf(stdout,
				"DEBUG: triggering socket signalling - %x\n",
				frame.can_id);
#endif
			if (CANIF_MGMT_TERM == frame.data[0]) {
				break;
			}

		} else if ((CANIF_ID | CANIF_COM) == frame.can_id) {
#if defined CANIF__DEBUG
			fprintf(stdout, "%s() communication channel\n",
				__func__);
#endif

			// check size of fifo, if limit reached, drop package (free frame!)
			if (RCV_QUEUE_MAX >= fifo__size())
				continue;

			// alloc content element
			content = malloc(sizeof(*content));
			if (NULL == content) {
				perror("Error in allocation content_p");
				break;
			}
			memset(content->msg, '\0', MESSAGE_LENGTH);

			// copy data to content->msg
			content->len = frame.can_dlc;
			memcpy(content->msg, frame.data, frame.can_dlc);

			// push content to fifo
			fifo__push(content);
			// (improvement: use locks to guarantee access to the fifo)

		} else {
#if defined CANIF__DEBUG
			fprintf(stdout, "%s() dropped\n", __func__);
#endif

			// dropped
			continue;
		}

	} while (1);

#if defined CANIF__DEBUG
	fprintf(stdout, "%s() ended\n", __func__);
#endif
}

/*
  "public" interface
*/

int canif__startup(const char *ifname, size_t ifname_size)
{
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
	fprintf(stdout, "%s at index %d\n", ifname, ifr.ifr_ifindex);
	if (-1 == bind(sockfd, (struct sockaddr *)&addr, sizeof(addr))) {
		perror("Error at binding socket to address");
		close(sockfd);
		return -1;
	}

	pid_listener = fork();
	if (0 > pid_listener) {
		perror("Error at fork()");
	} else if (0 == pid_listener) {
		canif__listener();
		canif__shutdown();
		return -1;
	}

	return 0;
}

int canif__shutdown()
{
#if defined CANIF__DEBUG
	sleep(10);
#endif

	content_p content = NULL;

	while (0 < fifo__size()) {
		content = fifo__pop();
		free(content);
		content = NULL;
	}

	return close(sockfd);
}

int canif__send(const uint32_t *can_id, const uint8_t *can_dlc, uint8_t data[])
{
	int nbytes = -1;
	struct can_frame frame;

	frame.can_id = *can_id;
	frame.can_dlc = *can_dlc;
	memcpy(frame.data, data, *can_dlc);
	nbytes = write(sockfd, &frame, sizeof(struct can_frame));

	return nbytes;
}

int canif__recv(uint8_t *can_dlc, uint8_t data[])
{
	content_p content = fifo__pop();

	if (NULL == content)
		return -1;

	*can_dlc = content->len;
	memcpy(data, content->msg, content->len);
	free(content);

	return 0;
}
