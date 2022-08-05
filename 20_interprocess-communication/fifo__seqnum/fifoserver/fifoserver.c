/*
  fifo server (fifo pipe)


  The server performs the following steps:

  - Create the server's well-known FIFO and open the FIFO for
    reading. The server must be run before any clients, so that the
    server FIFO exists by the time a client attempts to open it. The
    server's open() blocks until the first client opens the other end
    of the server FIFO for writing.

  - Open the server's FIFO once more, this time for writing. This will
    never block, since the FIFO has already been opened for
    reading. This second open is a convenience to ensure that the
    server doesn't see end-of-life if all clients close th ewrite end
    of the FIFO.

  - Ignore the SIGPIPE signal, so that if the server attempts to write
    to a client FIFO that doesn't have a reader, then, rather than
    being sent a SIGPIPE signal (which kills a process by default), it
    receives an EPIPE error from the write() system call.

  - Enter a loop that reads and responds to each incoming client
    request. To send the response, the server constructs the name of
    the client FIFP and then opens that FIFO.

  - If th eserver encounters an error in opening the client FIFO, it
    abandons that client's request

  This example is an iterative server in which the server reads and
  handles each client request before going on to handle the next
  client. An iterative server design is suitable when each client
  request can be quickly processed and responded to, so that other
  client requests are not delayed. An alternative design is a
  concurrent server, in which the main server process employs a
  separate child process (or thread) to handle each client request.
  [see socket server examples]

  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 912
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#include "fifo_seqnum.h"


int
main(int argc, char *argv[])
{
	int fd_server, fd_dummy, fd_client;
	char fifo_client[CLIENT_FIFO_NAME_LEN];
	struct request req;
	struct response resp;
	int seqnum = 0;   // this is our "service"

	/*
	  create well-known FIFO, and open it for reading
	*/

	umask(0);    // so we get the permissions we want

	if (-1 == mkfifo(SERVER_FIFO, S_IRUSR | S_IWUSR | S_IWGRP)
	    && errno != EEXIST) {
		fprintf(stderr, "mkfifo() %s\n", SERVER_FIFO);
		exit(EXIT_FAILURE);
	}

	fd_server = open(SERVER_FIFO, O_RDONLY);
	if (-1 == fd_server) {
		fprintf(stderr, "open() %s\n", SERVER_FIFO);
		exit(EXIT_FAILURE);
	}

	/*
	  open an extra write descriptor, so that we never see EOF
	*/

	fd_dummy = open(SERVER_FIFO, O_WRONLY);
	if (-1 == fd_dummy) {
		fprintf(stderr, "open() %s\n", SERVER_FIFO);
		exit(EXIT_FAILURE);
	}

	if (SIG_ERR == signal(SIGPIPE, SIG_IGN)) {
		perror("signal()");
		exit(EXIT_FAILURE);
	}

	while (true) {   // read request and send responses
		if (read(fd_server, &req, sizeof(req)) != sizeof(req)) {
			fprintf(stderr, "fatal: error reading request\n");
			continue;  // either partial read or error
		}

		/*
		  open client FIFO (previously created by client
		*/

		snprintf(fifo_client, CLIENT_FIFO_NAME_LEN,
			 CLIENT_FIFO_TEMPLATE, (long) req.pid);

		fd_client = open(fifo_client, O_WRONLY);
		if (-1 == fd_client) {
			fprintf(stderr, "open() %s\n", fifo_client);
			continue;
		}

		/*
		  send response and close FIFO
		*/

		resp.seqnum = seqnum;
		if (write(fd_client, &resp, sizeof(resp)) != sizeof(resp)) {
			fprintf(stderr, "fatal: error writing to FIFO %s\n",
				fifo_client);
		}
		if (-1 == close(fd_client)) {
			perror("close()");
			exit(EXIT_FAILURE);
		}

		seqnum += req.seqlen;   // update our sequence number
		fprintf(stderr, "server: seqnum %d\n", seqnum);
	}

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
