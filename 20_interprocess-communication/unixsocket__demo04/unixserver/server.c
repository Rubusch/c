/*
  unix socket - server


  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 1172
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>

#include "unixsockets.h"


int main(int argc, char *argv[])
{
	struct sockaddr_un svaddr, claddr;
	int sockfd, idx;
	ssize_t nbytes;
	socklen_t len;
	char buf[BUF_SIZE];

	sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);   // create server socket
	if (-1 == sockfd) {
		perror("socket()");
		exit(EXIT_FAILURE);
	}

	/*
	  construct well-known address and bind server socket to it
	*/

	if (strlen(SV_SOCK_PATH) > sizeof(svaddr.sun_path) -1) {
		fprintf(stderr, "fatal: server socket path too long: %s\n", SV_SOCK_PATH);
	}

	if (-1 == remove(SV_SOCK_PATH) && ENOENT != errno) {
		fprintf(stderr, "fatal: remove-%s\n", SV_SOCK_PATH);
		exit(EXIT_FAILURE);
	}

	memset(&svaddr, 0, sizeof(struct sockaddr_un));
	svaddr.sun_family = AF_UNIX;
	strncpy(svaddr.sun_path, SV_SOCK_PATH, sizeof(svaddr.sun_path) - 1);

	if (-1 == bind(sockfd, (struct sockaddr*) &svaddr, sizeof(struct sockaddr_un))) {
		perror("bind()");
		exit(EXIT_FAILURE);
	}

	/*
	  receive messages, convert to uppercase, and return to client
	*/

	while (true) {
		len = sizeof(struct sockaddr_un);
		nbytes = recvfrom(sockfd, buf, BUF_SIZE, 0, (struct sockaddr*) &claddr, &len);
		if (-1 == nbytes) {
			perror("recvfrom()");
			exit(EXIT_FAILURE);
		}

		fprintf(stderr, "server received %ld bytes from %s\n", (long) nbytes, claddr.sun_path);
		for (idx = 0; idx < nbytes; idx++) {
			buf[idx] = toupper((unsigned char) buf[idx]);
		}

		if (nbytes != sendto(sockfd, buf, nbytes, 0, (struct sockaddr*)&claddr, len)) {
			fprintf(stderr, "fatal: sendto()\n");
		}
	}

	fprintf(stderr, "server: READY.\n");
	exit(EXIT_SUCCESS);
}
