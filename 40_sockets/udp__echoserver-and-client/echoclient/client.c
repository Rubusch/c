/*
  unix socket - client


  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 1242
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdbool.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>

#include "commons.h"


/*
  returns a file descriptor on success, or -1 on error
*/
int inet_connect(const char *host, const char* service, int type)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int sockfd, res;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;
	hints.ai_family = AF_UNSPEC;  // allows IPv4 or IPv6
	hints.ai_socktype = type;
	hints.ai_flags = AI_PASSIVE;

	res = getaddrinfo(host, service, &hints, &result);
	if (0 != res) {
		errno = ENOSYS;
		return -1;
	}

	/*
	  walk through returned list until we find an address
	  structure that can be used to successfully connect a socket
	*/

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		// socket()
		sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (-1 == sockfd)
			continue;  // on error, try next address

		// connect()
		if (-1 != connect(sockfd, rp->ai_addr, rp->ai_addrlen))
			break;  // success

		// close()
		//
		// failed: close this socket and try next address
		close(sockfd);
	}

	freeaddrinfo(result);

	return (NULL == rp) ? -1 : sockfd;
}
/* --- */

int
main(int argc, char *argv[])
{
	int sfd;
	ssize_t nread;
	char buf[BUF_SIZE];

	if (2 != argc) {
		fprintf(stderr, "usage:\n$ %s host\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	sfd = inet_connect(argv[1], SERVICE, SOCK_STREAM);
	if (-1 == sfd) {
		perror("inet_connect()");
		exit(EXIT_FAILURE);
	}

	switch (fork()) {
	case -1:
		perror("fork()");
		exit(EXIT_FAILURE);
	case 0: /* child - read server's response, echo on stdout */
		while (true) {
			nread = read(sfd, buf, BUF_SIZE);
			if (0 >= nread) {
				fprintf(stderr, "exit on EOF or error\n");
				break;
			}
			fprintf(stderr "%.*s", (int) nread, buf);
		}
		_exit(EXIT_SUCCESS);

	default: /* parent - write contents of stdin to socket */
		while (true) {
			nread = read(STDIN_FILENO, buf, BUF_SIZE);
			if (0 >= nread) {
				break;
			}
			if (nread != write(sfd, buf, nread)) {
				fprintf(stderr, "write() failed");
				exit(EXIT_FAILURE);
			}

			/*
			  close writing channel, so server sees EOF
			*/

			if (-1 == shutdown(sfd, SHUT_WR)) {
				perror("shutdown()");
				exit(EXIT_FAILURE);
			}
			exit(EXIT_SUCCESS);
		}
	}
}
