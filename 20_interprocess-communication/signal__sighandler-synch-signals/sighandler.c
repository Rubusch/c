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



  demostrates a signal handler dealing with asynchronous signals
  i.e. "realtime" signals, this demonstrates a regular signalhandler

  use SIGINT or SIGTERM to terminate program

  NB: make sure "sighandler" zombies are removed properly!


  references:
  based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 470
  https://man7.org/tlpi/index.html

  tpli codes, distribution version e.g. here:
  https://github.com/bradfa/tlpi-dist
 */

#define _GNU_SOURCE /* strsinal() dclaration from <string.h> */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <signal.h>
#include <time.h>
#include <unistd.h>

#include "tlpi/get_num.h"

int
main(int argc, char* argv[])
{
	int sig;
	siginfo_t si;
	sigset_t all_sigs;

	if (2 > argc) {
		fprintf(stderr, "usage:\n$ %s <delay secs>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "%s: PID is %ld\n", argv[0], (long) getpid());

	// block all signals (except SIGKILL and SIGSTOP)
	sigfillset(&all_sigs);
	if (-1 == sigprocmask(SIG_SETMASK, &all_sigs, NULL)) {
		perror("sigprocmask() failed");
		exit(EXIT_FAILURE);
	}
	fprintf(stderr, "%s: signals blocked\n", argv[0]);

	// delay so that signals can be sent to us
	fprintf(stderr, "%s: about to delay %s seconds\n", argv[0], argv[1]);
	sleep(get_int(argv[1], GN_GT_0, "delay-secs"));
	fprintf(stderr, "%s: finished delay\n", argv[0]);

	for (;;) { // fetch signals until SIGINT (^C) or SIGTERM
		sig = sigwaitinfo(&all_sigs, &si);
		if (-1 == sig) {
			perror("sigwaitinfo() failed");
			exit(EXIT_FAILURE);
		}

		if (SIGINT == sig || SIGTERM == sig) {
			fprintf(stderr, "EXIT: sig was SIGINT or SIGTERM\n");
			exit(EXIT_SUCCESS);
		}

		fprintf(stderr, "got signal: %d (%s)\n",
			sig, strsignal(sig));
		fprintf(stderr, "\tsi_signo=%d, si_code=%d (%s), si_value=%d\n",
			si.si_signo, si.si_code, (si.si_code == SI_USER) ? "SI_USER" :
			(si.si_code == SI_QUEUE) ? "SI_QUEUE" : "other",
			si.si_value.sival_int);
		fprintf(stderr, "\tsi_pid=%ld, si_uid=%ld\n",
			(long) si.si_pid, (long) si.si_uid);
	}

	fprintf(stderr, "%s() - READY.\n", __func__);
	exit(EXIT_SUCCESS);
}
