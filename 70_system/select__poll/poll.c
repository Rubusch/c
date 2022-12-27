/*
  poll() - I/O multiplexing alternative to select (busy wait)

  usage:
    $ ./poll.elf 10 5
      writing to fd:   4 (read fd:   3)
      writing to fd:   4 (read fd:   3)
      writing to fd:   6 (read fd:   5)
      writing to fd:  20 (read fd:  19)
      writing to fd:  12 (read fd:  11)
      poll() returned: 4
      readable:   3
      readable:   5
      readable:  11
      readable:  19
      READY.


  When is a filedescriptor ready?

  SUSv3 says that a file descriptor (with O_NONBLOCK clear) is
  considered to be ready if a call to an I/O function would not block,
  regardless of whether the function would actually transfer data!!

  select() and poll() tell us whether an I/O operation would not
  block, rather than whether it would successfully transfer data. In
  this light, let us consider how these system calls operate for
  different types of file descriptors.

  The select() column indicates whether a file descriptor is marked as
  readable (r), writeable (w), or having an esceptional condition (x).

  The poll() column indicates the bit(s) returned in the 'revents'
  field. In these tables, we omit mention of POLLRDNORM, POLLWRNORM,
  POLRDBAND, and POLLWRBAND. Although some of these flags may be
  returned in revents in various circumstances (if they are specified
  in events), they convey no useful information beyond that provided
  by POLLIN, POLLOUT, POLLHUP, and POLLERR.
  [tlpi, Kerrisk, p. 1341]


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

  references:
  The Linux Programming Interface, Michael Kerrisk, 2010, p. 1340
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <time.h>
#include <sys/poll.h>
#include <unistd.h>

#include "tlpi/get_num.h"


int
main(int argc, char *argv[])
{
	int npipes, idx, ready, rnd_pipe, nwrites;
	int (*pfds)[2];     // file descriptors for all pipes
	struct pollfd *fdpoll;

	if (2 > argc) {
		fprintf(stderr, "usage:\n$ %s num-pipes [num-writes]\n",
			argv[0]);
	}

	/*
	  allocate the arrays that we use; the arrays are sized
	  according to the number of pipes specified on command line
	*/

	npipes = get_int(argv[1], GN_GT_0, "npipes");    
//	pfds = calloc(npipes, sizeof(int[2])); // TODO     
	pfds = calloc(npipes, sizeof(*pfds));
	if (NULL == pfds) {
		perror("calloc(pfds)");
		exit(EXIT_FAILURE);
	}

	fdpoll = calloc(npipes, sizeof(*fdpoll));
	if (NULL == fdpoll) {
		perror("calloc(fdpoll)");
		exit(EXIT_FAILURE);
	}

	/*
	  create the number of pipes specified on command line
	*/

	for (idx = 0; idx < npipes; idx++) {
		if (-1 == pipe(pfds[idx])) {
			fprintf(stderr, "pipe %d\n", idx);
			exit(EXIT_FAILURE);
		}
	}

	/*
	  perform specified number of writes to random pipes
	*/

	nwrites = (2 < argc) ? get_int(argv[2], GN_GT_0, "nwrites") : 1;
	srandom((int) time(NULL));
	for (idx = 0; idx < nwrites; idx++) {
		rnd_pipe = random() % npipes;
		fprintf(stderr, "writing to fd: %3d (read fd: %3d)\n",
			pfds[rnd_pipe][1], pfds[rnd_pipe][0]);
		if (-1 == write(pfds[rnd_pipe][1], "a", 1)) {
			fprintf(stderr, "write %d\n", pfds[rnd_pipe][1]);
		}
	}

	/*
	  Build the file descriptor list to be supplied to
	  poll(). This list is set to contain the file descriptors for
	  the read ends of all of the pipes.
	*/

	for (idx = 0; idx < npipes; idx++) {
		fdpoll[idx].fd = pfds[idx][0];
		fdpoll[idx].events = POLLIN;
	}

	ready = poll(fdpoll, npipes, 0);
	if (-1 == ready) {
		perror("poll()");
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "poll() returned: %d\n", ready);

	/*
	  check which pipes have data available for reading
	*/

	for (idx = 0; idx < npipes; idx++) {
		if (fdpoll[idx].revents & POLLIN) {
			fprintf(stderr, "readable: %3d\n", fdpoll[idx].fd);
		}
	}

	fprintf(stdout, "READY.\n");
	exit(EXIT_SUCCESS);
}
