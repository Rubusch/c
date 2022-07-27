/*
  usage:
    $ pidof exim4
      917

    $ ./priority.elf r 10 917
      sched_setscheduler: Operation not permitted

    $ sudo ./priority.elf r 10 917
      READY.

    $ top
      ...
      917 Debian-ex -11   0 17204  2320  1628 S  0.0  0.1  0:00.00 ├─ exim4 -bd -q30m
      ...

  The program uses sched_setscheduler() to set the policy and priority
  of the processes specified by its command-line arguments. The first
  argument is a letter specifying a scheduling policy, the second is
  an integer priority, and the remaining arguments are the process IDs
  of the processes whose scheduling attributes are to be changed.


  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 743
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

	if (3 > argc || NULL == strchr("rfobi", argv[1][0])) {
		fprintf(stderr, "$ %s policiy priority [pid...]\n"
			"\tpolicy is 'r' (RR), 'f' (FIFO), "
#ifdef SCHED_BATCH
			"'b' (BATCH), "
#endif
#ifdef SCHED_IDLE
			"'i' (IDLE), "
#endif
			"or 'o' (OTHER)\n",
			argv[0]);
		exit(EXIT_FAILURE);
	}

	pol = (argv[1][0] == 'r') ? SCHED_RR :
		(argv[1][0] == 'f') ? SCHED_FIFO :
#ifdef SCHED_BATCH
		(argv[1][0] == 'b') ? SCHED_BATCH :
#endif
#ifdef SCHED_IDLE
		(argv[1][0] == 'i') ? SCHED_IDLE :
#endif
		SCHED_OTHER;

	sp.sched_priority = get_int(argv[2], 0, "priority");
	for (idx = 3; idx < argc; idx++) {
		if (sched_setscheduler(get_long(argv[idx], 0, "pid"), pol, &sp) == -1) {
			perror("sched_setscheduler");
			exit(EXIT_FAILURE);
		}
	}

	fprintf(stderr, "READY.\n"); // never reaches here
	exit(EXIT_SUCCESS);
}
