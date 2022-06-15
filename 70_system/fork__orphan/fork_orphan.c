/*
  usage:
  $ ./fork_orphan.elf s p
    parent: PID=2110, PPID=1229, PGID=2110, SID=1229
    child: PID=2111, PPID=2110, PGID=2110, SID=1229
    PID=2111 stopping
    parent exiting
    PID=2111: caught signal 18 (Continued)
    PID=2111: caught signal 1 (Hangup)
    raspberrypi<23:06:42>::pi("/usr/src/github__c/70_system/fork__orphan/");

  $ ./fork_orphan.elf s p
    parent: PID=2221, PPID=1229, PGID=2221, SID=1229
    child: PID=2222, PPID=2221, PGID=2221, SID=1229
    PID=2222 stopping
    child: PID=2223, PPID=2221, PGID=2221, SID=1229
    PID=2223 pausing
    parent exiting
    PID=2223: caught signal 18 (Continued)
    PID=2222: caught signal 18 (Continued)
    PID=2223: caught signal 1 (Hangup)
    PID=2222: caught signal 1 (Hangup)



  orphan - consider the following scenario:

  1. Before the parent process exits, the child was stopped (perhaps
     because the parent sent it a stop signal).

  2. When the parent process exits, the shell removes the parent's
     process group from its list of jobs. The child is adopted by
     "init" and becomes a background process for the terminal. The
     process group containing the child is orphaned.

  3. At this point, there is no process that monitors the state of the
     stopped child via wait()

  After establishing handlers for SIGHUP and SIGCONT this program
  creates one child process for each command-line argument. Each child
  then stops itself (by raising SIGSTOP), or waits for signals (using
  pause()). The choice of action by the child is determined by whether
  or not the corresponding command-line argument starts with the
  letter 's' for stop. We use a command-line argument starting with
  the letter 'p' to stepcify the converse action of calling pause()
  [any other character will do as well].

  After creating all of th echildren the parent sleeps for a few
  seconds to allow the children time to get set up. The parent then
  exits, at which point the process group containing the children
  becomes orphaned. If any of the children receives a signal as a
  consequence of the process group becoming orphaned, the signal
  handler is invoked, and it displays the child's process ID and the
  signal number.

  based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 728
 */
#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <signal.h>
#include <libgen.h>
#include <unistd.h>

static void
handler(int sig)
{
	// UNSAFE: don't use stdio functions here - not asynch signal safe
	fprintf(stderr, "PID=%ld: caught signal %d (%s)\n",
		(long) getpid(), sig, strsignal(sig));
}

int
main(int argc, char *argv[])
{
	int idx;
	struct sigaction sa;

	if (2 > argc) {
		fprintf(stderr, "usage:\n$ %s {s} ...\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	setbuf(stdout, NULL);

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = handler;
	if (-1 == sigaction(SIGHUP, &sa, NULL)) {
		fprintf(stderr, "sigaction() failed setting SIGHUP handler\n");
		exit(EXIT_FAILURE);
	}

	if (-1 == sigaction(SIGCONT, &sa, NULL)) {
		fprintf(stderr, "sigaction() failed setting SIGCONT handler\n");
		exit(EXIT_FAILURE);
	}

	printf("parent: PID=%ld, PPID=%ld, PGID=%ld, SID=%ld\n",
	       (long) getpid(), (long) getppid(), (long) getpgrp(), (long) getsid(0));

	/*
	  create one child for each command-line argument
	*/

	for (idx = 1; idx < argc; idx++) {
		switch (fork()) {
		case -1:
			fprintf(stderr, "fork() failed\n");
			exit(EXIT_FAILURE);

		case 0: // child
			printf("child: PID=%ld, PPID=%ld, PGID=%ld, SID=%ld\n",
			       (long) getpid(), (long) getppid(), (long) getpgrp(), (long) getsid(0));

			if ('s' == argv[idx][0]) {  // 's' - stop via signal
				printf("PID=%ld stopping\n", (long) getpid());
				raise(SIGSTOP);
			} else { // wait for signal
				alarm(60); // ...die if not SIGHUPed...
				printf("PID=%ld pausing\n", (long) getpid());
				pause();
			}
			_exit(EXIT_SUCCESS);

		default: // parent carries on round loop...
			break;
		}
	}

	/*
	  parent falls through to here after creating all children
	*/

	sleep(3); //  give children a chance to start...
	printf("parent exiting\n");

	//  ..and orphan them and their group
	exit(EXIT_FAILURE);
}
