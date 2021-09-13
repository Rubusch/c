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

static pid_t pid_listener;

void canif__listener()
{
	struct can_frame frame;
	int ret = -1;

	do {
		ret = read(sockfd, &frame, sizeof(frame));
		if (-1 == ret) {
			perror("Error in read()");
			return;
		}

		ret = canif__recv_cb(frame.can_id, frame.can_dlc, frame.data);
		if (0 > ret) {
			perror("Error in canif__recv_cb()");
			return;
		}
	} while (1);
}


/*
  "public" interface
*/

int canif__startup(const char *ifname, size_t ifname_size)
{
	if (-1 == (sockfd = socket(PF_CAN, SOCK_RAW, CAN_RAW))) {
		perror("Error while opening socket");
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, ifname_size);
	ifr.ifr_name[ifname_size-1] = '\0';
	if (0 > ioctl(sockfd, SIOCGIFINDEX, &ifr)) {
		perror("Error in ioctl");
		return -1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.can_family  = AF_CAN;
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

//int canif__shutdown(const char *ifname, size_t ifname_size)
int canif__shutdown()
{
	return close(sockfd);
}

// TODO simplify     
int canif__send(const uint32_t *can_id, const uint8_t *can_dlc, uint8_t data[])
{
	int nbytes = -1;
	struct can_frame frame;
/*
  // TODO test, rm
	frame.can_id  = 0x123;
	frame.can_dlc = 2;
	frame.data[0] = 0x11;
	frame.data[1] = 0x22;
*/
	frame.can_id = *can_id;
	frame.can_dlc = *can_dlc;
	memcpy(frame.data, data, *can_dlc);

	nbytes = write(sockfd, &frame, sizeof(struct can_frame));
	// TODO check write error conditions

	return nbytes;
}

void canif__register_recv(int (*recv_cb)(uint32_t can_id, uint8_t can_dlc, uint8_t data[]))
{
	canif__recv_cb = recv_cb;
}

