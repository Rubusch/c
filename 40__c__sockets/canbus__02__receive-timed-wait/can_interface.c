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

	return 0;
}

int canif__shutdown()
{
	// free queue
	uint32_t dummy_can_id;
	uint8_t dummy_can_dlc;
	uint8_t dummy_data[CAN_MAX_DLEN];

	while (0 == canif__recv(&dummy_can_id, &dummy_can_dlc, dummy_data))
	       ;

	return close(sockfd);
}

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

int canif__recv(uint32_t *can_id, uint8_t* can_dlc, uint8_t data[])
{
	long timeout = 10000;
	struct timeval tv;

	fd_set rset; // select - rset, actually checked
	struct can_frame frame;
	int ret = -1;

	tv.tv_sec = timeout / 1000;
	tv.tv_usec = (timeout % 1000) * 1000;

	FD_ZERO(&rset);
	FD_SET(sockfd, &rset);

	if (0 > select(sockfd+1, &rset, NULL, NULL, &tv)) { // TODO set timeout? (probably only reason for select approach)
		// TODO in case '1 > select('  
		perror("Error in select");
		return -1;
	}
	if (FD_ISSET(sockfd, &rset)) {
		ret = read(sockfd, &frame, sizeof(frame));
		if (0 > ret) {
			perror("Error in read()");
			return ret;
		}
	}
	*can_id = frame.can_id;
	*can_dlc = frame.can_dlc;
	memcpy(data, frame.data, frame.can_dlc);
	return 0;
}

