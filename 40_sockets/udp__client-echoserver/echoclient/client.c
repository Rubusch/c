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

int main(int argc, char **argv)
{
	int sockfd, idx;
	size_t len;
	ssize_t nread;
	char buf[BUF_SIZE];

	if (argc < 2) {
		fprintf(stderr, "CLIENT - usage:\n$ %s host msg...\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/*
	  construct server address from first command-line argument

	  NB: "SERVICE" here means port number!
	*/

	sockfd = inet_connect(argv[1], SERVICE, SOCK_DGRAM);
	if (-1 == sockfd) {
		fprintf(stderr, "CLIENT - could not connect to server socket\n");
		exit(EXIT_FAILURE);
	}

	/*
	  send remaining command-line arguments to server as separate datagrams
	*/

	for (idx = 2; idx < argc; idx++) {
		len = strlen(argv[idx]);
		if (len != write(sockfd, argv[idx], len)) {
			fprintf(stderr, "CLIENT - partial/failed write\n");
			exit(EXIT_FAILURE);
		}
		nread = read(sockfd, buf, BUF_SIZE);
		if (-1 == nread) {
			perror("CLIENT - read() failed, did you use 'localhost'? Try '127.0.0.1' instead.");
			exit(EXIT_FAILURE);
		}

		fprintf(stderr, "CLIENT - [%ld bytes] '%.*s'\n",
			(long) nread, (int) nread, buf);
	}

	fprintf(stderr, "CLIENT - done!\n");  /* closes clientfd */
	exit(EXIT_SUCCESS);
}
