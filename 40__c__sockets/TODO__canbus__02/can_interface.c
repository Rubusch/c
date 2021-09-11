/*
  can_interface: implementation

  @author: Lothar Rubusch <L.Rubusch@gmail.com>
  @license: GPLv3
 */

#include "can_interface.h"


// TODO static if instance
static int sockfd;
static struct sockaddr_can addr;
static struct ifreq ifr;

// TODO received message stack


int canif__startup(const char *ifname, size_t ifname_size)
{
	if (-1 == (sockfd = socket(PF_CAN, SOCK_RAW, CAN_RAW))) {
		perror("Error while opening socket");
		exit(EXIT_FAILURE);
	}

	strcpy(ifr.ifr_name, ifname);
	if (0 > ioctl(sockfd, SIOCGIFINDEX, &ifr)) {
		perror("Error in ioctl");
		exit(EXIT_FAILURE);
	}

	addr.can_family  = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;
	printf("%s at index %d\n", ifname, ifr.ifr_ifindex);

	if (-1 == bind(sockfd, (struct sockaddr *)&addr, sizeof(addr))) {
		perror("Error in binding socket to address");
		return -2;
	}

	// TODO
	return -1;
}

int canif__shutdown()
{
	// TODO
	return -1;
}

int canif__send()
{
	int nbytes = -1;
	struct can_frame frame;

	frame.can_id  = 0x123;
	frame.can_dlc = 2;
	frame.data[0] = 0x11;
	frame.data[1] = 0x22;

	nbytes = write(sockfd, &frame, sizeof(struct can_frame));

	// TODO
	return nbytes;
}

int canif__recv()
{
	// TODO
	return -1;
}




// TODO   

