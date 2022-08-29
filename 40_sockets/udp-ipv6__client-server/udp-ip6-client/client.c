/*
  unix socket - client


  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 1209
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdbool.h>
#include <unistd.h>
#include <errno.h>

#include "udp-ip6.h"


int main(int argc, char **argv)
{
	struct sockaddr_in6 svaddr;
	int sockfd, idx;
	size_t msglen;
	ssize_t nbytes;
	char resp[BUF_SIZE];

	if (3 > argc) {
		fprintf(stderr, "usage:\n"
			"$ %s <host address> <msg...>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/*
	  create client socket
	*/

	sockfd = socket(AF_INET6, SOCK_DGRAM, 0);
	if (-1 == sockfd) {
		perror("socket()");
		exit(EXIT_FAILURE);
	}

	memset(&svaddr, 0, sizeof(struct sockaddr_in6));
	svaddr.sin6_family = AF_INET6;
	svaddr.sin6_port = htons(PORT_SERVER);
	if (0 >= inet_pton(AF_INET6, argv[1], &svaddr.sin6_addr)) {
		fprintf(stderr, "inet_pton failed for address '%s'\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	/*
	  send messages to server; echo responses on stdout
	*/

	for (idx = 2; idx < argc; idx++) {
		msglen = strlen(argv[idx]);  // may be longer than BUF_SIZE
		if (msglen != sendto(sockfd, argv[idx], msglen, 0,
				     (struct sockaddr*) &svaddr,
				     sizeof(struct sockaddr_in6))) {
			fprintf(stderr, "fatal: sendto()\n");
		}

		nbytes = recvfrom(sockfd, resp, BUF_SIZE, 0, NULL, NULL);
		if (-1 == nbytes) {
			perror("recvfrom()");
			exit(EXIT_FAILURE);
		}

		fprintf(stderr, "response %d: %.*s\n",
			idx - 1, (int) nbytes, resp);
	}

	fprintf(stderr, "done!\n");  // closes our socket - server sees EOF
	exit(EXIT_SUCCESS);
}
