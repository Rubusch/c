/*
  fifo client (fifo pipe)


  The client performs the following steps

  - Create a FIFO to be used for receiving a response from the
    server. This is done before sending the request, in order to
    ensure that the FIFO exists by the time the server attempts to
    open it and send a response message.

  - Construct a message for the server containing the client's process
    ID and a number (taken from an optional command-line argument)
    specifying the lenght of the sequence that the client wishes the
    server to assign to it. (If no command-line arguemnt is supplied,
    the default sequence length is 1).

  - Open the server FIFO and send the message to the server.

  - Open the client FIFO and read and print the server's response.


  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 914
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <errno.h>

#include "tlpi/get_num.h"
#include "fifo_seqnum.h"


static char fifo_client[CLIENT_FIFO_NAME_LEN];


static void
remove_fifo(void)
{
	unlink(fifo_client);
}

int
main(int argc, char *argv[])
{
	int fd_server, fd_client;
	struct request req;
	struct response resp;

	fprintf(stderr, "usage:\n"
		"$ %s [seq-len]\n", argv[0]);

	/*
	  create our FIFO (before sending request, to avoid a race)
	*/

	umask(0);  // so we get the permissions we want

	snprintf(fifo_client, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE,
		 (long) getpid());

	if (-1 == mkfifo(fifo_client, S_IRUSR | S_IWUSR | S_IWGRP)
	    && errno != EEXIST) {
		fprintf(stderr, "mkfifo() %s\n", fifo_client);
		exit(EXIT_FAILURE);
	}

	if (0 != atexit(remove_fifo)) {  // register exit handler
		perror("atexit()");
		exit(EXIT_FAILURE);
	}

	/*
	  construct request message, open server FIFO, and send request
	*/

	req.pid = getpid();
	req.seqlen = (argc > 1) ? get_int(argv[1], GN_GT_0, "seq-len") : 1;

	fd_server = open(SERVER_FIFO, O_WRONLY);
	if (-1 == fd_server) {
		fprintf(stderr, "open() %s\n", SERVER_FIFO);
		exit(EXIT_FAILURE);
	}

	if (write(fd_server, &req, sizeof(req)) != sizeof(req)) {
		fprintf(stderr, "can't write to server\n");
		exit(EXIT_FAILURE);
	}

	/*
	  open our FIFO, read and display response
	*/

	fd_client = open(fifo_client, O_RDONLY);
	if (-1 == fd_client) {
		fprintf(stderr, "open() %s\n", fifo_client);
		exit(EXIT_FAILURE);
	}

	if (read(fd_client, &resp, sizeof(resp)) != sizeof(resp)) {
		fprintf(stderr, "can't read from server\n");
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
