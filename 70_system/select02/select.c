/*
  select() - I/O multiplexing

  usage:
  $ ./select.elf 0 0r
    ready = FD 0
    0:
    timeout after select(): 0.000
    READY.


  now, some interaction

  $ ./select.elf 20 0r
  <ENTER>
    ready = FD 1
    0: r
    timeout after select(): 9.297
    READY.


  monitor two FDs: FD 0, to see if input is available, and FD 1, to
  see if output is possible. In this case we specify the timeout as
  NULL ('-'), i.e. infinity

  $ ./select.elf - 0r 1w
    ready = FD 1
    0:
    1: w
    READY.

  the select() call returned immediately, informing us that output was
  possible on FD1


  IO Models

  - IO multiplexing: select()
    select() is portable among unices
    select() is old and classic
    select() is slow and limited

  - IO multiplexing: poll()
    a bit modernized select(), basically similar to select() a bit
    less limited API

  - signal-driven IO
    better performance
    better for large number of FDs

  - epoll()
    best performance
    not portable among other UNIXes

  reference: The Linux Programming Interface, Michael Kerrisk, 2010, p. 1334
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/time.h>
#include <sys/select.h>
#include <unistd.h>

#include "tlpi/get_num.h"

static void
usage(const char *name)
{
	fprintf(stderr, "usage: %s {timeout|-} fd-num[rw]...\n", name);
	fprintf(stderr, "\t-\tinfinite timeout\n");
	fprintf(stderr, "\tr\tmonitor for read\n");
	fprintf(stderr, "\tw\tmonitor for write\n");
	fprintf(stderr, "\te.g.: %s - 0rw 1w\n", name);
	exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
	fd_set readfds, writefds;
	int ready, nfds, fd, nread, idx;
	struct timeval timeout;
	struct timeval *pto;
	char buf[10]; // large enough to hold "rw\0"

	if (argc < 2) {
		usage(argv[0]);
	}

	// timeout for select() is specified in argv[1]

	if (0 == strcmp(argv[1], "-")) {
		pto = NULL;
	} else {
		pto = &timeout;
		timeout.tv_sec = get_long(argv[1], 0, "timeout");
		timeout.tv_usec = 0;   // no microseconds!
	}

	// process remaining arguments to build file descriptor sets

	nfds = 0;
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);

	for (idx = 2; idx < argc; idx++) {
		nread = sscanf(argv[idx], "%d%2[rw]", &fd, buf);
		if (nread != 2) {
			usage(argv[0]);
		}

		if (fd >= FD_SETSIZE) {
			fprintf(stderr,
				"ERR: file descriptor exceeds limit (%d)\n",
				FD_SETSIZE);
		}

		if (fd >= nfds) {
			nfds = fd + 1;
		}

		// strchr() - locate character in string
		if (NULL != strchr(buf, 'r')) {
			FD_SET(fd, &readfds);
		}

		if (NULL != strchr(buf, 'w')) {
			FD_SET(fd, &writefds);
		}
	}

	// we've built all of the arguments - now call select()

	ready = select(nfds, &readfds, &writefds, NULL, pto); // ignore excpetions
	if (-1 == ready) {
		perror("select()");
		exit(EXIT_FAILURE);
	}

	// display results of select()

	fprintf(stderr, "ready = FD %d\n", ready);
	for (fd = 0; fd < nfds; fd++) {
		fprintf(stderr, "%d: %s%s\n", fd,
		       FD_ISSET(fd, &readfds) ? "r" : "",
		       FD_ISSET(fd, &writefds) ? "w" : "");
	}

	if (NULL != pto) {
		fprintf(stderr, "timeout after select(): %ld.%03ld\n",
			(long) timeout.tv_sec, (long) timeout.tv_usec / 1000);
	}

	fprintf(stdout, "READY.\n");
	exit(EXIT_SUCCESS);
}
