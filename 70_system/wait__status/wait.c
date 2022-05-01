/*
  usage:
      $ ./wait.elf 23
      child started with PID = 6393
      waitpid() returned: PID=6393; status=0x1700 (23, 0)
      child exited, status=23

    $ ./wait.elf &
      [1] 6397

  <ENTER>
      child started with PID = 6398

    $ kill -STOP 6398

    $ waitpid() returned: PID=6398; status=0x137f (19, 127)
      child stopped by signal 19 (Stopped (signal))

    $ kill -CONT 6398
      waitpid() returned: PID=6398; status=0xffff (255, 255)
      child continued

    $ kill -ABRT 6398
      waitpid() returned: PID=6398; status=0x0006 (0, 6)
      child killed by signal 6 (Aborted)

    $ ls -l core
      ls: cannot access 'core': No such file or directory
      [1]+  Done                    ./wait.elf

  -> display RLIMIT_CORE limit
    $ ulimit -c
      0

  -> now allow core dumps
    $ ulimit -c unlimited

    $ ./wait.elf &
      [1] 6892
      child started with PID = 6893

    $ kill -ABRT 6893

    $ waitpid() returned: PID=6893; status=0x0086 (0, 134)
      child killed by signal 6 (Aborted)
      (core dumped)

      [1]+  Done                    ./wait.elf

    $ ls -l core
      -rw------- 1 pi pi 225280 May  1 21:43 core


  the print_wait_status() function uses all of the macros. this
  program creates a child process that either loops continuously
  called pause() (during which time signals can be sent to the child)
  or, if an integer command-line argument was supplied, exits
  immediately using this integer as the exit status. in the meantime,
  the parent monitors the child via waitpid(), printing the returned
  status value and passing this value to print_wiat_status(). the
  parent exits when it detects that the child has either exited
  normally or been terminated by a signal.

  demonstrates the use of signals as the notification mechanism for a
  POSIX timer


  based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 546
 */
#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/wait.h>
#include <stdbool.h>
#include <unistd.h>

#include "tlpi/get_num.h"


/*
  NOTE: the following function employs printf(), which is not
  async-signal-safe. as such this functio is also not
  async-signal-safe

  examine a wait() status using the W* macros.
 */
void
print_wait_status(const char* msg, int status)
{
	if (NULL != msg) {
		fprintf(stderr, "%s", msg);
	}

	if (WIFEXITED(status)) {
/*
  WIFEXITED()    evaluates to a non-zero value if "status" was returned for a
                 child process that terminated normally

		 this macro returns true if the child process exited normally
*/
		fprintf(stderr, "child exited, status=%d\n", WEXITSTATUS(status));
/*
  WEXITSTATUS()  if the value is non-zero, this macro evaluates to the low-order
                 8 bits of "status" that the child process passed to _exit() or
                 exit(), or the value the child pid returned from main()
*/
	} else if (WIFSIGNALED(status)) {
/*
  WIFSIGNALED()  this macro returns true if the child process was
                 killed by a signal.
*/
		fprintf(stderr, "child killed by signal %d (%s)\n",
			WTERMSIG(status), strsignal(WTERMSIG(status)));
/*
  WTERMSIG()     if WIFSIGNALED is non-zero, this macro evaluates to the number
                 of the signal that caused the termination of the child pid
*/

#ifdef WCOREDUMP
		if (WCOREDUMP(status)) {
/*

*/
			fprintf(stderr, " (core dumped)");
		}
#endif
		fprintf(stderr, "\n");
	} else if (WIFSTOPPED(status)) {
		fprintf(stderr, "child stopped by signal %d (%s)\n",
			WSTOPSIG(status), strsignal(WSTOPSIG(status)));

#ifdef WIFCONTINUED
	} else if (WIFCONTINUED(status)) {
/*
      WIFCONTINUED this macro returns true of the child was resumed by
		 delivery of SIGCONT
*/
		fprintf(stderr, "child continued\n");
#endif
	} else { // should never happen
		fprintf(stderr, "what happened to this child? (status=%x)\n",
			(unsigned int) status);
	}
}

int
main(int argc, char *argv[])
{
	int status;
	pid_t child_pid;

	fprintf(stderr, "usage:\n%s [exit-status]\n\n", argv[0]);

	switch (fork()) {
	case -1 :
		fprintf(stderr, "fork() failed\n");
		_exit(EXIT_FAILURE);
	case 0:  // child: either exits immediately with given status
		 // or loops waiting for signals
		fprintf(stderr, "child started with PID = %ld\n",
			(long) getpid());
		if (argc > 1) {  // status supplied on command line?
			_exit(get_int(argv[1], 0, "exit-status"));
		} else {  // otherwiese, wait for signals
			while (true) {
				pause();
			}
		}
		_exit(EXIT_FAILURE);  // never reaches here, but good practice

	default:  // parent: repeatedly wait on child until it either
		  // exits or is terminated by a singal
		while (true) {
			child_pid = waitpid(-1, &status, WUNTRACED
#ifdef WCONTINUED  /* not present on older versions of Linux */
					    | WCONTINUED
/*
      WCONTINUED func shall report the status of any continued child process
                 specified by pid whose status has not been reported since it
                 continued from a job control stop
*/
#endif
				);

			if (-1 == child_pid) {
				fprintf(stderr, "waitpid() failed\n");
				exit(EXIT_FAILURE);
			}

			// print status in hex, and as separate decimal bytes
			fprintf(stderr, "waitpid() returned: PID=%ld; status=0x%04x (%d, %d)\n",
				(long) child_pid, (unsigned int) status, status >>8, status & 0xff);

			print_wait_status(NULL, status);

			if (WIFEXITED(status) || WIFSIGNALED(status)) {
				exit(EXIT_SUCCESS);
			}
		}
	}

	fprintf(stderr, "READY.\n"); // never reaches here
	exit(EXIT_SUCCESS);
}
