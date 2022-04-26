/*
  usage:
  $ ./time.elf 3 10
      slept for:  3.000148 secs
      READY.

  $ ./time.elf 3 0
      slept for:  3.000137 secs
      READY.

  use SIGINT to interrupt, nanosleep will continue showing the still remaining time

  $ ./nanosleep.elf 2 500000
  --> CTRL+c
      ^Cslept for:  0.770465 secs
      remaining:  1.230136657
  --> CTRL+c
      ^Cslept for:  1.340465 secs
      remaining:  0.660850211
  --> CTRL+c
      ^Cslept for:  1.882180 secs
      remaining:  0.119346649
      slept for:  2.001704 secs
      READY.

  NB: a resolution in nano secconds is not realistic, nsec values >
  1000 will start influencing the result, e.g.

  $ ./nanosleep.elf 2 500000
      slept for:  2.000600 secs
      READY.

  $ ./nanosleep.elf 2 500000
      slept for:  2.000629 secs
      READY.


  demonstrates usage of nanosleep, together with a signal handler for
  SIGINT, the function will sleep for n secs and m nanosecs

  NB: realistic resolution won't be in nano seconds

  NBB: on linux programs using this API might be compiled with the
  "-lrt" option, in order to link against the librt (realtime)
  library.


  based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 490
*/

#define _POSIX_C_SOURCE 199309

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <stdbool.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include <errno.h>

#include "tlpi/get_num.h"

static void
sigint_handler(int sig)
{
	return; // just interrupt the nanosleep
}

int
main(int argc, char *argv[])
{
	struct timeval start, finish;
	// NB: struct timeval has "tv_usec"
	struct timespec request, remain; // needed for nanosleep()
	// NB: struct timespec has "tv_nsec"
	struct sigaction sa;
	int ret;

	if (3 != argc) {
		fprintf(stderr, "usage:\n$ %s <secs> <nanosecs>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	request.tv_sec = get_long(argv[1], 0, "secs");
	request.tv_nsec = get_long(argv[2], 0, "nanosecs");

	// allow SIGINT handler to interrupt nanosleep()

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = sigint_handler;
	if (-1 == sigaction(SIGINT, &sa, NULL)) {
		perror("sigaction() failed");
		exit(EXIT_FAILURE);
	}

	// init start
	if (-1 == gettimeofday(&start, NULL)) {
		perror("gettimeofday() failed");
		exit(EXIT_FAILURE);
	}

	while (true) {
		ret = nanosleep(&request, &remain); // nanosleep syntax
		/*
		  suspends the execution of the calling thread until
		  either at least the time specified in *req has
		  elapsed, or the delivery of a signal that triggers
		  the invocation of a handler in the calling thread or
		  that terminates the process
		 */

		if (-1 == ret && errno != EINTR) {
			perror("nanosleep() failed");
			exit(EXIT_FAILURE);
		}

		// init finish
		if (-1 == gettimeofday(&finish, NULL)) {
			perror("gettimeofday() failed");
			exit(EXIT_FAILURE);
		}

		fprintf(stderr, "slept for: %9.6f secs\n",
			finish.tv_sec - start.tv_sec + (finish.tv_usec - start.tv_usec) / 1000000.0);

		if (0 == ret) {
			break; // nanosleep() completed
		}

		fprintf(stderr, "remaining: %2ld.%09ld\n",
			(long) remain.tv_sec, (long) remain.tv_nsec);

		request = remain;
	}

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}

