/*
  unix socket - client


  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 1169
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
	struct sockaddr_un addr;
	int sockfd;
	ssize_t nread;
	char buf[BUF_SIZE];

	sockfd = socket(AF_UNIX, SOCK_STREAM, 0); // create client socket
	if (-1 == sockfd) {
		perror("socket()");
		exit(EXIT_FAILURE);
	}

	/*
	  construct server address, and make the connection
	*/

	memset(&addr, 0, sizeof(struct sockaddr_un));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, SV_SOCK_PATH, sizeof(addr.sun_path) - 1);

	if (-1 == connect(sockfd, (struct sockaddr*) &addr, sizeof(struct sockaddr_un))) {
		perror("connect()");
		exit(EXIT_FAILURE);
	}

	/*
	  copy stdin to socket

	  NB: unixsocket also allows for passing descriptors!!!
	*/

	while (0 < (nread = read(STDIN_FILENO, buf, BUF_SIZE))) {
		if (nread != write(sockfd, buf, nread)) {
			fprintf(stderr, "fatal: partial/failed write\n");
		}
	}

	if (-1 == nread) {
		perror("read()");
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "client: done!\n");  // closes our socket - server sees EOF
	exit(EXIT_SUCCESS);
}
