/*
  can_interface: implementation

  @author: Lothar Rubusch <L.Rubusch@gmail.com>
  @license: GPLv3
 */

#include "can_interface.h"

/*
  "private"
*/
static int sockfd;
static struct sockaddr_can addr;
static struct ifreq ifr;

/*
   private interface
*/

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
	fprintf(stdout, "found %s interface initialized at index %d\n", ifname,
		ifr.ifr_ifindex);
	if (-1 == bind(sockfd, (struct sockaddr *)&addr, sizeof(addr))) {
		perror("Error at binding socket to address");
		close(sockfd);
		return -1;
	}

	return 0;
}

int canif__shutdown()
{
	return close(sockfd);
}

int canif__send(const uint32_t *can_id, const uint8_t *can_dlc, uint8_t data[])
{
	int nbytes = -1;
	struct can_frame frame;

	frame.can_id = *can_id;
	frame.can_dlc = *can_dlc;
	memcpy(frame.data, data, *can_dlc);

#if defined CANIF__DEBUG
	canif__print(__func__, &frame);
#endif

	nbytes = write(sockfd, &frame, sizeof(frame));
	if (0 > nbytes) {
		perror("Error in write");
	}
	return nbytes;
}

int canif__recv(uint32_t *can_id, uint8_t *can_dlc, uint8_t data[],
		long timeout)
{
	struct timeval tv;

	fd_set rset; // select - rset, actually checked
	struct can_frame frame;
	int ret = -1;

	tv.tv_sec = timeout / 1000;
	tv.tv_usec = (timeout % 1000) * 1000;

	FD_ZERO(&rset);
	FD_SET(sockfd, &rset);

	// check against 1, to avoid overrun frame data size, reading just bogus
	if (1 > select(sockfd + 1, &rset, NULL, NULL, &tv)) {
		fprintf(stderr, "select() timed out\n");
		return -1;
	}
	if (FD_ISSET(sockfd, &rset)) {
		fprintf(stdout, "%s(): read\n", __func__);
		ret = read(sockfd, &frame, sizeof(frame));
		if (0 > ret) {
			perror("Error in read()");
			return ret;
		}
	}

#if defined CANIF__DEBUG
	canif__print(__func__, &frame);
#endif

	*can_id = frame.can_id;
	*can_dlc = frame.can_dlc;
	memcpy(data, frame.data, frame.can_dlc);

	return ret;
}
