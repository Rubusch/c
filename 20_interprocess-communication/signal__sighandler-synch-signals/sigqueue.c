/*
  usage of ./sighandler.elf and sigqueue.elf

--> start signal handler
  $ ./sighandler.elf 60 &
    [1] 24089
    ./sighandler.elf: PID is 24089
    ./sighandler.elf: signals blocked
    ./sighandler.elf: about to delay 60 seconds
    ./sigqueue.elf 24089 43 100
    ./sigqueue.elf: PID is 24094, UID is 1000
    main() - READY.

--> start sigqueue.elf
  $ ./sigqueue.elf 24089 42 200
    ./sigqueue.elf: PID is 24099, UID is 1000
    main() - READY.

  $ ./sighandler.elf: finished delay
    got signal: 42 (Real-time signal 8)
            si_signo=42, si_code=-1 (SI_QUEUE), si_value=200
            si_pid=24099, si_uid=1000
    got signal: 43 (Real-time signal 9)
            si_signo=43, si_code=-1 (SI_QUEUE), si_value=100
            si_pid=24094, si_uid=1000
    ./

  $ echo $$
    1357

  $ kill -USR1 24089

  $ got signal: 10 (User defined signal 1)
          si_signo=10, si_code=0 (SI_USER), si_value=0
          si_pid=1357, si_uid=1000

--> ENTER
  $ kill %1
    EXIT: sig was SIGINT or SIGTERM
    [1]+  Done                    ./sighandler.elf 60

--> ENTER
  $


  demostrates a signal handler dealing with synchronous signals

  sighandler.elf: start detached and use the PID obtained
  sigqueue.elf: sends signals to the particular PID


  references:
  based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 459
  https://man7.org/tlpi/index.html

  tpli codes, distribution version e.g. here:
  https://github.com/bradfa/tlpi-dist
 */

#define _GNU_SOURCE /* strsinal() dclaration from <string.h> */
#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <signal.h>
#include <unistd.h>

#include "tlpi/get_num.h"

int
main(int argc, char* argv[])
{
	int sig, nsigs, idx, sig_data;
	union sigval sv;

	if (argc < 4) {
		fprintf(stderr, "usage:\n$ %s <pid> <signum> <data> [num-sigs]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/*
	  display our PID and UID, so that they can be compared with
	  the corresponding fields of the siginfo_t argument supplied
	  to the handler in the receiving process
	*/

	fprintf(stderr, "%s: PID is %ld, UID is %ld\n",
		argv[0], (long) getpid(), (long) getuid());

	sig = get_int(argv[2], 0, "sig_num");
	sig_data = get_int(argv[3], GN_ANY_BASE, "data");
	nsigs = (argc > 4) ? get_int(argv[4], GN_GT_0, "num_sigs") : 1;

	for (idx = 0; idx < nsigs; idx++) {
		sv.sival_int = sig_data + idx;
		if (-1 == sigqueue(get_long(argv[1], 0, "pid"), sig, sv)) {
			perror("sigqueue() failed");
			exit(EXIT_FAILURE);
		}
	}

	fprintf(stderr, "%s() - READY.\n", __func__);
	exit(EXIT_SUCCESS);
}
