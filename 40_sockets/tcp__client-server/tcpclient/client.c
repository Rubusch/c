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
#include <netdb.h>
#include <errno.h>

#include "commons.h"

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

int main(int argc, char **argv)
{
	char *reqlenstr;  /* request length of sequence */
	char seqnumstr[INT_LEN];  /* start of granted sequence */
	int clientfd;
	ssize_t nread;
	struct addrinfo hints;
	struct addrinfo *result, *rp;

	if (2 > argc) {
		fprintf(stderr, "usage:\n"
			"$ %s [<server address>] <sequence len>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/*
	  call getaddrinfo() to obtain a list of addresses that we can
	  try connecting to
	*/

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;
	hints.ai_family = AF_UNSPEC;  /* allows IPv4 or IPv6 */
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_NUMERICSERV;

	if (0 != getaddrinfo(argv[1], SERVER_PORT, &hints, &result)) {
		perror("getaddrinfo()");
		exit(EXIT_FAILURE);
	}

	/*
	  walk through returned list until we find an address
	  structure that can be used to successfully connect a socket
	*/

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		clientfd = socket(rp->ai_family, rp->ai_socktype,
				  rp->ai_protocol);

		if (-1 == clientfd) {  /* on error, try next address */
			continue;
		}

		if (-1 != connect(clientfd, rp->ai_addr, rp->ai_addrlen)) {
			break;  /* success */
		}

		/*
		  connect failed; lcose this socket and try next address
		*/
		close(clientfd);
	}

	if (NULL == rp) {
		fprintf(stderr, "fatal: could not connect socket to any address\n");
	}

	freeaddrinfo(result);

	/*
	  send requested sequence length, with terminating newline
	*/

	reqlenstr = (argc > 2) ? argv[2] : "1";
	if (strlen(reqlenstr) != write(clientfd, reqlenstr, strlen(reqlenstr))) {
		fprintf(stderr, "fatal: partial/failed write (reqlenstr)\n");
	}
	if (1 != write(clientfd, "\n", 1)) {
		fprintf(stderr, "fatal: partial/failed write (newline)\n");
	}

	/*
	  read and display sequence number returned by server
	*/

	nread = read_line(clientfd, seqnumstr, INT_LEN);
	if (-1 == nread) {
		perror("read_line()");
		exit(EXIT_FAILURE);
	}
	if (0 == nread) {
		fprintf(stderr, "fatal: unexpeccted EOF from server\n");
	}

	fprintf(stderr, "sequence number: %s\n", seqnumstr);  /* includes '\n' */

	fprintf(stderr, "done!\n");  /* closes clientfd */
	exit(EXIT_SUCCESS);
}
