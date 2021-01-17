// getip.c
/*
  $ getip.exe <if>

  shows the ip of a connected interface
*/

#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/ioctl.h>
#include <linux/if.h> /* struct ifreq */
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h> /* inet_ntoa() -> TODO replace by inet_ntop() */

int main(int argc, char **argv) {

	struct ifreq ifa;
	struct sockaddr_in *i;
	int fd;

	if(argc != 2) {
		fprintf(stderr, "Usage: %s <iface>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (strlen (argv[1]) >= sizeof ifa.ifr_name) {
		fprintf (stderr, "%s is too long\n", argv[1]);
		exit (EXIT_FAILURE);
	}

	strcpy (ifa.ifr_name, argv[1]);

	if (0 > (fd = socket(AF_INET, SOCK_DGRAM, 0))) {
		perror ("socket");
		exit (EXIT_FAILURE);
	}

	if (0 > ioctl(fd, SIOCGIFADDR, &ifa)) {
		perror ("ioctl");
		exit (EXIT_FAILURE);
	}

	i = (struct sockaddr_in*)&ifa.ifr_addr;
	puts (inet_ntoa(i->sin_addr));

	exit(EXIT_SUCCESS);
}
