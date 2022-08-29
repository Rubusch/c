/*
  unix socket - server


  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 1208
*/

#define _DEFAULT_SOURCE


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdbool.h>
#include <unistd.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "commons.h"

#include "tlpi/get_num.h"


/*
  read_line()
 */
ssize_t
read_line(int fd, void* buffer, size_t n)
{
	ssize_t nread;  /* # of bytes fetched by last read() */
	size_t total_read;  /* total bytes read so far */
	char *buf;
	char ch;

	if (n <= 0 || NULL == buffer) {
		errno = EINVAL;
		return -1;
	}

	buf = buffer;  /* no pointer arithmetic on 'void*' */

	total_read = 0;
	while (true) {
		nread = read(fd, &ch, 1);
		if (-1 == nread) {
			if (EINTR == errno) {  /* interrupted -> restart read() */
				continue;
			} else {
				return -1;  /* some other error */
			}
		} else if (0 == nread) {  /* EOF */
			if (0 == total_read) {  /* no bytes read; return 0 */
				return 0;
			} else {  /* some bytes read; add '\0' */
				break;
			}

		} else {  /* 'nread' must be 1 if we get here */
			if (total_read < n - 1) {  /* discard > (n - 1) bytes */
				total_read++;
				*buf++ = ch;
			}

			if (ch == '\n') {
				break;
			}
		}
	}

	*buf = '\0';
	return total_read;
}
/* --- */

int main(int argc, char *argv[])
{
	struct sockaddr_storage claddr;
	int sockfd, clientfd, optval, reqlen;
	socklen_t addrlen;
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	uint32_t seqnum;
	char reqlenstr[INT_LEN];  /* length of requested sequence */
	char seqnumstr[INT_LEN];  /* start of granted sequence */

#define ADDRSTRLEN (NI_MAXHOST + NI_MAXSERV + 10)
	char addrstr[ADDRSTRLEN];
	char host[NI_MAXHOST];
	char service[NI_MAXSERV];

	if (1 < argc) {
		fprintf(stderr, "usage:\n"
			"$ %s [init-seq-num]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	seqnum = (argc > 1) ? get_int(argv[1], 0, "init-seq-num") : 0;

	if (SIG_ERR == signal(SIGPIPE, SIG_IGN)) {
		perror("signal()");
		exit(EXIT_FAILURE);
	}

	/*
	  call getaddrinfo() to obtain a list of addresses that we can
	  try for binding to
	*/

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_family = AF_UNSPEC;  /* allows IPv4 or IPv6 */
	hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;  /* wildcard IP address; serice name is numeric */

	if (0 != getaddrinfo(NULL, SERVER_PORT, &hints, &result)) {
		perror("getaddrinfo()");
		exit(EXIT_FAILURE);
	}

	/*
	  walk through returned list until we find an address
	  structure that can be used to successfully create and bind a
	  socket
	*/

	optval = 1;
	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (-1 == sockfd) {
			continue;  /* on error, try next address */
		}

		if (-1 == setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval))) {
			perror("setsockopt()");
			exit(EXIT_FAILURE);
		}

		if (0 == bind(sockfd, rp->ai_addr, rp->ai_addrlen)) {
			break;  /* success */
		}

		/*
		  bind failed: close this socket and try next address
		 */

		fprintf(stderr, "bind() failed\n");
		close(sockfd);
	}

	if (NULL == rp) {
		fprintf(stderr, "fatal: could not bind socket to any address\n");
	}

	if (-1 == listen(sockfd, 0)) {  /* backlog == 0 */
		perror("listen()");
		exit(EXIT_FAILURE);
	}

	freeaddrinfo(result);

	while (true) {  /* handle clients iteratively */

		/*
		  accept a client connection, obtaining client's address
		*/

		addrlen = sizeof(struct sockaddr_storage);
		clientfd = accept(sockfd, (struct sockaddr*) &claddr, &addrlen);
		if (-1 == clientfd) {
			perror("accept()");
			continue;
		}

		if (0 == getnameinfo((struct sockaddr*) &claddr, addrlen, host, NI_MAXHOST, service, NI_MAXSERV, 0)) {
			snprintf(addrstr, ADDRSTRLEN, "(%s, %s)", host, service);
		} else {
			snprintf(addrstr, ADDRSTRLEN, "(?UNKNOWN?)");
		}
		fprintf(stderr, "connection from %s\n", addrstr);

		/*
		  read client request, send sequence number back
		*/

		if (0 >= read_line(clientfd, reqlenstr, INT_LEN)) {
			close(clientfd);
			continue;   /* failed read, skip request */
		}

		reqlen = atoi(reqlenstr);
		if (0 >= reqlen) {  /* watch for misbehaving clients */
			close(clientfd);
			continue;   /* bad request; skip it */
		}

		snprintf(seqnumstr, INT_LEN, "%d\n", seqnum);
		if (strlen(seqnumstr) != write(clientfd, seqnumstr, strlen(seqnumstr))) {
			fprintf(stderr, "error on write\n");
		}

		seqnum += reqlen;  /* update sequence number */

		if (-1 == close(clientfd)) {
			perror("close()");
			exit(EXIT_FAILURE);
		}

	}

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
