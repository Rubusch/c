/*
  unix socket - client


  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 1173
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdbool.h>
#include <unistd.h>
#include <errno.h>

#include "unixsockets.h"


int main(int argc, char **argv)
{
	struct sockaddr_un svaddr, claddr;
	int sockfd, idx;
	size_t msglen;
	ssize_t nbytes;
	char resp[BUF_SIZE];

	if (2 > argc) {
		fprintf(stderr, "usage:\n"
			"$ %s <msg...>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/*
	  create client socket; bind to unique pathname (based on PID)
	*/

	sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (-1 == sockfd) {
		perror("socket()");
		exit(EXIT_FAILURE);
	}

	memset(&claddr, 0, sizeof(struct sockaddr_un));
	claddr.sun_family = AF_UNIX;
	snprintf(claddr.sun_path, sizeof(claddr.sun_path),
		 "/tmp/unixsocket.%ld", (long) getpid());

	if (-1 == bind(sockfd, (struct sockaddr*)&claddr, sizeof(struct sockaddr_un))) {
		perror("bind()");
		exit(EXIT_FAILURE);
	}

	/*
	  construct address of server
	*/

	memset(&svaddr, 0, sizeof(struct sockaddr_un));
	svaddr.sun_family = AF_UNIX;
	strncpy(svaddr.sun_path, SV_SOCK_PATH, sizeof(svaddr.sun_path) - 1);

	/*
	  send messages to server; echo responses on stdout
	*/

	for (idx = 1; idx < argc; idx++) {
		msglen = strlen(argv[idx]);  // may be longer than BUF_SIZE
		if (msglen != sendto(sockfd, argv[idx], msglen, 0, (struct sockaddr*) &svaddr, sizeof(struct sockaddr_un))) {
			fprintf(stderr, "sendto()\n");
		}

		nbytes = recvfrom(sockfd, resp, BUF_SIZE, 0, NULL, NULL);
		if (-1 == nbytes) {
			perror("recvfrom()");
			exit(EXIT_FAILURE);
		}

		fprintf(stderr, "response %d: %.*s\n", idx, (int) nbytes, resp);
	}

	remove(claddr.sun_path);  // remove client socket pathname

	fprintf(stderr, "client: done!\n");  // closes our socket - server sees EOF
	exit(EXIT_SUCCESS);
}
