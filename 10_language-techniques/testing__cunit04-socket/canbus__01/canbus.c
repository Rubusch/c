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

#ifdef TESTING
#include "../test__canbus.h"
#endif

/*
int main(void)
/*/
int main(int argc, char* argv[])
// */
{
#ifdef TESTING
	TESTING_SETUP /* enable testing: pass -DTESTING in Makefile */
#endif
	int sockfd = -1;
	int nbytes = -1;
	struct sockaddr_can addr;
	struct can_frame frame;
	struct ifreq ifr;
	char ifname[IFNAMSIZ];
	int ret = -1;

	memset(ifname, '\0', IFNAMSIZ);
	strcpy(ifname, "can0");
	if (1 < argc) {
		if (strlen(argv[1]) < IFNAMSIZ) {
			memset(ifname, '\0', IFNAMSIZ);
			strncpy(ifname, argv[1], strlen(argv[1]));
		}
	}

#ifdef TESTING
	TESTING_PROBE__IFNAME
#endif

	do {
		sockfd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
		if (0 > sockfd) {
			perror("Error while opening socket");
			ret = sockfd;
			break;
		}

		strcpy(ifr.ifr_name, ifname);
		if (0 > (ret = ioctl(sockfd, SIOCGIFINDEX, &ifr))) {
			perror("Error in ioctl");
			break;
		}

		addr.can_family = AF_CAN;
		addr.can_ifindex = ifr.ifr_ifindex;
		printf("%s at index %d\n", ifr.ifr_name, ifr.ifr_ifindex);

		if (0 > (ret = bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)))) {
			perror("Error in binding socket to address");
			break;
		}

		frame.can_id = 0x123;
		frame.can_dlc = 2;
		frame.data[0] = 0x11;
		frame.data[1] = 0x22;

		nbytes = write(sockfd, &frame, sizeof(struct can_frame));
		if (0 > nbytes) {
			ret = nbytes;
			break;
		}
		printf("wrote %d bytes\n", nbytes);
	} while (0);

	close(sockfd);

	if (0 > ret) {
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}
