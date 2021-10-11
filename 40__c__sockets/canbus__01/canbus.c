/*
  socketCAN

  basic socketCAN example from wikipedia

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

  REFERENCE:
  https://en.wikipedia.org/wiki/SocketCAN

 */

#define _GNU_SOURCE /* struct ifreq */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <linux/sockios.h>

#include <linux/can.h>

#include <mcheck.h> // debugging

int main(void)
{
	int sockfd;
	int nbytes;
	struct sockaddr_can addr;
	struct can_frame frame;
	struct ifreq ifr;
	const char *ifname = "can0";

	mtrace();

	do {
		sockfd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
		if (-1 == sockfd) {
			perror("Error while opening socket");
			break;
		}

		strcpy(ifr.ifr_name, ifname);
		if (0 > ioctl(sockfd, SIOCGIFINDEX, &ifr)) {
			perror("Error in ioctl");
			break;
		}

		addr.can_family = AF_CAN;
		addr.can_ifindex = ifr.ifr_ifindex;
		printf("%s at index %d\n", ifname, ifr.ifr_ifindex);

		if (-1 ==
		    bind(sockfd, (struct sockaddr *)&addr, sizeof(addr))) {
			perror("Error in binding socket to address");
			break;
		}

		frame.can_id = 0x123;
		frame.can_dlc = 2;
		frame.data[0] = 0x11;
		frame.data[1] = 0x22;

		nbytes = write(sockfd, &frame, sizeof(struct can_frame));
		printf("wrote %d bytes\n", nbytes);
	} while (0);

	close(sockfd);

	muntrace();
	exit(EXIT_SUCCESS);
}
