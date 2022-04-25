/*
  usage:
  $ ./timer.elf 3
  <waits for 3 seconds, then times out by ALARM signal>
    caught signal
    read timed out
    READY.


  demonstrates a timed read


  1. call sigaction() to establish a handler for SIGALRM, omitting the
  SA_RESTART flag, so that system calls are not restarted

  2. call alarm() or setitimer() to establish a timer specifying the
  upper limit of time for which we wish the system call to block

  3. make the blocking system call

  4. after the system call returns, call alarm() or setitimer() once
  more to disable the timer (in case the system call completed before
  the timer expired)

  5. check to see whether the blocking system call failed with errno
  set to EINTR (interrupted system call)

  based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 486
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <signal.h>
#include <unistd.h>
#include <errno.h>

#include "tlpi/get_num.h"

#define BUF_SIZE 200

static void
signal_handler(int sig)
{
	fprintf(stderr, "caught signal\n"); // unsafe: stdio in signal handler
}

int
main(int argc, char *argv[])
{
	struct sigaction sa;
	char buf[BUF_SIZE];
	ssize_t nread;
	int saved_errno;

	if (2 > argc) {
		fprintf(stderr, "usage:\n$ %s [num-secs [restart-flag]]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/*
	  set up handler for SIGALRM, allow system calls to be
	  interrupted, unless the second command-line argument was
	  supplied
	 */
	sa.sa_flags = (argc > 2) ? SA_RESTART : 0;
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = signal_handler;
	if (-1 == sigaction(SIGALRM, &sa, NULL)) {
		perror("sigaction() failed");
		exit(EXIT_FAILURE);
	}

	alarm((argc > 1) ? get_int(argv[1], GN_NONNEG, "num-secs") : 10);

	nread = read(STDIN_FILENO, buf, BUF_SIZE);
	saved_errno = errno;    // in case alarm() changes it
	alarm(0);               // ensure timer is turned off
	errno = saved_errno;

	// determine result of read()
	if (nread == -1) {
		if (EINTR == errno) {
			fprintf(stderr, "read timed out\n");
		} else {
			fprintf(stderr, "read");
		}
	} else {
		fprintf(stderr, "successful read (%ld bytes): %.*s",
			(long) nread, (int) nread, buf);
	}

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
