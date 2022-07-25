/*
  usage:
    $ ./priority.elf
        usage:
        $ ./priority.elf {p|g|u} who priority
        set priority of: p = process; g = process group; u = processes for user

    $ pidof /lib/systemd/systemd-udevd
        161

    $ sudo ./priority.elf p 161 10
        nice value = 10
        READY.


  In Linux kernels before 2.6.12 an unprivileged process may use
  setpriority() only to (irreversibly) lower its own or another
  process's priority. A privileged (CAP_SYS_NICE) process can use
  setpriority() to rais priorities.

  As an example, if a process's RLIMIT_NICE soft limit is 25, then its
  nice value can be set to -5. From this formula, and the knowledge
  that the nice value has the range +19 (low) to -20 (high), we can
  deduce that the effectively useful range of the RLIMIT_NICE limit is
  1 (low) to 40 (high).

  Demonstrates the usage of setpriority() to change the nice value of
  another (target) process, if the effective user ID of the process
  calling setpriority() matches the real or effective user ID of the
  target process, and the change to the nice value is consistent with
  the target process's RLIMIT_NICE limit.


  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 736
 */
#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/time.h>
#include <sys/resource.h>
#include <errno.h>
#include <unistd.h>

#include "tlpi/get_num.h"



int
main(int argc, char *argv[])
{
	int which, prio;
	id_t who;

	if (argc != 4 || NULL == strchr("pgu", argv[1][0])) {
		fprintf(stderr, "usage:\n$ %s {p|g|u} who priority\n"
			"set priority of: p = process; g = process group; u = processes for user\n",
			argv[0]);
		exit(EXIT_FAILURE);
	}

	/* set nice value according to command-line arguments */
	which = ('p' == argv[1][0]) ? PRIO_PROCESS :
		('g' == argv[1][0]) ? PRIO_PGRP : PRIO_USER;
	who = get_long(argv[2], 0, "who");
	prio = get_int(argv[3], 0, "prio");

	if (-1 == setpriority(which, who, prio)) {
		perror("setpriority()");
		exit(EXIT_FAILURE);
	}

	/* retrieve nice value to check the change */
	errno = 0; // successful call may return -1
	prio = getpriority(which, who);
	if (-1 == prio && errno != 0) {
		perror("getpriority()");
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "nice value = %d\n", prio);

	fprintf(stderr, "READY.\n"); // never reaches here
	exit(EXIT_SUCCESS);
}
