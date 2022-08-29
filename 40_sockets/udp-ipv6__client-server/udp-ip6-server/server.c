/*
  unix socket - server


  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 1208
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>

#include "udp-ip6.h"


int main(int argc, char *argv[])
{
	struct sockaddr_in6 svaddr, claddr;
	int sockfd, idx;
	ssize_t nbytes;
	socklen_t len;
	char buf[BUF_SIZE];
	char client_addrstr[INET6_ADDRSTRLEN];

	sockfd = socket(AF_INET6, SOCK_DGRAM, 0);   // create server socket
	if (-1 == sockfd) {
		perror("socket()");
		exit(EXIT_FAILURE);
	}

	/*
	  construct address and bind server socket to it
	*/

	memset(&svaddr, 0, sizeof(struct sockaddr_in6));
	svaddr.sin6_family = AF_INET6;
	svaddr.sin6_addr = in6addr_any;
	svaddr.sin6_port = htons(PORT_SERVER);

	if (-1 == bind(sockfd, (struct sockaddr*) &svaddr, sizeof(struct sockaddr_in6))) {
		perror("bind()");
		exit(EXIT_FAILURE);
	}

	/*
	  receive messages, convert to uppercase and return to client
	  (similar to UNIX domain socket demo)
	*/

	while (true) {
		len = sizeof(struct sockaddr_in6);
		nbytes = recvfrom(sockfd, buf, BUF_SIZE, 0, (struct sockaddr*) &claddr, &len);
		if (-1 == nbytes) {
			perror("recvfrom()");
			exit(EXIT_FAILURE);
		}

		if (NULL == inet_ntop(AF_INET6, &claddr.sin6_addr, client_addrstr, INET6_ADDRSTRLEN)) {
			fprintf(stderr, "warning: couldn't convert client address to string\n");
		} else {
			fprintf(stderr, "server received %ld bytes from (%s, %u)\n",
				(long) nbytes, client_addrstr, ntohs(claddr.sin6_port));
		}

		for (idx = 0; idx < nbytes; idx++) {
			buf[idx] = toupper((unsigned char) buf[idx]);
		}

		if (nbytes != sendto(sockfd, buf, nbytes, 0, (struct sockaddr*)&claddr, len)) {
			fprintf(stderr, "fatal: sendto()\n");
		}
	}

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
