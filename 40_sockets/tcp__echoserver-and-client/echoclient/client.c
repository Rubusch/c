/*
  unix socket - client


  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 1258
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdbool.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>

#include "commons.h"

#define BUF_SIZE 1024


/*
  returns a file descriptor on success, or -1 on error
*/
int inet_connect(const char *host, const char* service, int type)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int sockfd, res;

	fprintf(stderr, "CLIENT: %s() - start\n", __func__);
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_canonname = NULL;
	hints.ai_next = NULL;
	hints.ai_family = AF_UNSPEC;  // allows IPv4 or IPv6
	hints.ai_socktype = type;
	hints.ai_addr = NULL;
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
		fprintf(stderr, "CLIENT: %s() - socket()\n", __func__);
		sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (-1 == sockfd)
			continue;  // on error, try next address

		// connect()
		fprintf(stderr, "CLIENT: %s() - connect( %d, %ld, %d )\n", __func__, sockfd, (long) &rp->ai_addr, rp->ai_addrlen);
		if (-1 != connect(sockfd, rp->ai_addr, rp->ai_addrlen))
			break;  // success

		// failed: close this socket and try next address
		fprintf(stderr, "CLIENT: %s() - close(), FAILED\n", __func__);
		close(sockfd);
	}
	freeaddrinfo(result);

	return (NULL == rp) ? -1 : sockfd;
}
/* --- */

int main(int argc, char **argv)
{
	int sockfd;

	if (argc < 2) {
		fprintf(stderr, "CLIENT: usage\n$ %s host msg...\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/*
	  construct server address from first command-line argument

	  NB: "SERVICE" here means port number!
	*/
	fprintf(stderr, "CLIENT: host = '%s' port = '%s'\n", argv[1], SERVICE);
	sockfd = inet_connect(argv[1], SERVICE, SOCK_STREAM);
	if (-1 == sockfd) {
		fprintf(stderr, "CLIENT - could not connect to server socket\n");
		exit(EXIT_FAILURE);
	}

	ssize_t nread;
	char buf[BUF_SIZE];

	// send remaining command-line arguments to server as separate datagrams
	fprintf(stderr, "CLIENT: fork()\n");
	switch (fork()) {
	case -1: return -1;
	case 0: // child: read server's response, echo on stdout
		while (true) {
			fprintf(stderr, "child: read()\n");
			nread = read(sockfd, buf, BUF_SIZE);
			fprintf(stderr, "child: read something: buf = '%s', nread = %d\n", buf, nread);

			if (0 >= nread) {
				break; // exit on EOF or error
			}
			fprintf(stderr, "child: %.*s", (int) nread, buf);
		}
		fprintf(stderr, "child: exiting, nread = '%d'\n", nread);
		exit(EXIT_SUCCESS);
	default: // parent
		while (true) {

			fprintf(stderr, "parent: read() from STDIN\n");
			nread = read(STDIN_FILENO, buf, BUF_SIZE);
			fprintf(stderr, "parent: STDIN read something: buf = '%s', nread = %d\n", buf, nread);
			if (0 >= nread) {
				break;
			}

			fprintf(stderr, "parent: write('%s')\n", buf);
			if (nread != write(sockfd, buf, nread)) {
				perror("parent: write() failed");
				exit(EXIT_FAILURE);
			}

			// close writing channel, so server sees EOF
			sleep(5);
			fprintf(stderr, "parent: exiting\n");
			if (-1 == shutdown(sockfd, SHUT_WR)) {
				perror("parent: shutdown()");
				exit(EXIT_FAILURE);
			}
			exit(EXIT_SUCCESS);
		}
	}

	fprintf(stderr, "CLIENT: done!\n");  // closes clientfd
	exit(EXIT_SUCCESS);
}
