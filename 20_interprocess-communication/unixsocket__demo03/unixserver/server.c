/*
  unix socket - server


  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 1168
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdbool.h>
#include <unistd.h>
#include <errno.h>

#include "unixsockets.h"


int main(int argc, char *argv[])
{
	struct sockaddr_un addr;
	int sockfd, clientfd;
	ssize_t nread;
	char buf[BUF_SIZE];

	sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (-1 == sockfd) {
		perror("socket()");
		exit(EXIT_FAILURE);
	}

	/*
	  construct server socket address, bind socket to it, and make
	  this a listening socket
	*/

	if (strlen(SV_SOCK_PATH) > sizeof(addr.sun_path) - 1) {
		fprintf(stderr, "fatal: server socket path too long: %s\n",
			SV_SOCK_PATH);
	}

	if (-1 == remove(SV_SOCK_PATH) && errno != ENOENT) {
		fprintf(stderr, "remove-%s\n", SV_SOCK_PATH);
		exit(EXIT_FAILURE);
	}

	memset(&addr, 0, sizeof(struct sockaddr_un));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, SV_SOCK_PATH, sizeof(addr.sun_path) - 1);

	if (-1 == bind(sockfd, (struct sockaddr*) &addr, sizeof(struct sockaddr_un))) {
		perror("bind()");
		exit(EXIT_FAILURE);
	}

	/*
	  listen(sockfd, backlog);

	  backlog - The backlog argument defines the maximum length to
          which the queue of pending connections for sockfd may grow.
          If a connection request arrives when the queue is full, the
          client may receive an error with an indication of
          ECONNREFUSED or, if the underlying protocol supports
          retransmission, the request may be ignored so that a later
          reattempt at connection succeeds.
	*/
	if (-1 == listen(sockfd, 3)) {
		perror("listen()");
		exit(EXIT_FAILURE);
	}

	while (true) {
		/*
		  accept a connecction. The connection is returned on
		  a new socket, 'clientfd'; the listening socket
		  ('sockfd') remains open and can be used to accept
		  further connections
		*/

		clientfd = accept(sockfd, NULL, NULL);
		if (-1 == clientfd) {
			perror("accept()");
			exit(EXIT_FAILURE);
		}

		/*
		  transfer data from connected socket to stdout until EOF
		*/

		while (0 < (nread = read(clientfd, buf, BUF_SIZE))) {
			if (nread != write(STDOUT_FILENO, buf, nread)) {
				fprintf(stderr, "fatal: partial/failed write\n");
			}

			if (-1 == nread) {
				perror("read()");
				exit(EXIT_FAILURE);
			}

			if (-1 == close(clientfd)) {
				perror("close()");
				exit(EXIT_FAILURE);
			}
		}
	}

	fprintf(stderr, "server: READY.\n");
	exit(EXIT_SUCCESS);
}
