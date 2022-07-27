/*
  usage:
    $ pidof exim4
      917

    $ ./priority.elf 917
      917: RR    10
      READY.

  pid = 917, has scheduling policy 'RR', and priority '10'


  The program uses sched_getscheduler() and sched_getparam() to
  retrieve the policy and priority of all of the processes whose
  process IDs are given as command-line arguments.


  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 745
 */
#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sched.h>

#include "tlpi/get_num.h"



int
main(int argc, char *argv[])
{
	int idx, pol;
	struct sched_param sp;

	for (idx = 1; idx < argc; idx++) {
		pol = sched_getscheduler(get_long(argv[idx], 0, "pid"));
		if (-1 == pol) {
			perror("sched_getscheduler");
			exit(EXIT_FAILURE);
		}

		if (-1 == sched_getparam(get_long(argv[idx], 0, "pid"), &sp)) {
			perror("sched_getparam");
			exit(EXIT_FAILURE);
		}

		fprintf(stderr, "%s: %-5s %2d\n", argv[idx],
			(pol == SCHED_OTHER) ? "OTHER" :
			(pol == SCHED_RR) ? "RR" :
			(pol == SCHED_FIFO) ? "FIFO" :
#ifdef SCHED_BATCH
			(pol == SCHED_BATCH) ? "BATCH" :
#endif
#ifdef SCHED_IDLE
			(pol == SCHED_IDLE) ? "IDLE" :
#endif
			"???", sp.sched_priority);
	}

	fprintf(stderr, "READY.\n"); // never reaches here
	exit(EXIT_SUCCESS);
}
