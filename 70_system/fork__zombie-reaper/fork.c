/*
  usage:
  $ ./fork.elf 1 2 4
    21:19:33 main(): child 1 (PID=26351) exiting
    child: READY.
    21:19:33 sigchld_handler(): caught SIGCHLD
    21:19:33 sigchld_handler(): reaped child 26351 - child exited, status=0
    21:19:34 main(): child 2 (PID=26352) exiting
    child: READY.
    21:19:36 main(): child 3 (PID=26353) exiting
    child: READY.
    21:19:38 sigchld_handler(): returning
    21:19:38 sigchld_handler(): caught SIGCHLD
    21:19:38 sigchld_handler(): reaped child 26352 - child exited, status=0
    21:19:38 sigchld_handler(): reaped child 26353 - child exited, status=0
    21:19:43 sigchld_handler(): returning
    21:19:43 main(): all 3 children have terminated; SIGCHLD was caught 2 times
    parent: READY.


  zombie - if a parent creates a child, but fails to perform a
  'wait()', then an entry for the zombie child will be maintained
  indefinitely in the kernel's process table. if a large number of
  such zombie children are created, they will eventually fill the
  kernel process table, preventing the creation of new
  processes. since the zombies can't be killed by a signal, the only
  way to remove them from the system is to kill their parent (or wait
  for it to exit), at which time the zombies are adopted and waited on
  by 'init', and consequently removed from the system.

  the SIGCHLD signal is sent to a parent process whenever one of its
  children terminates. by default, this signal is ignored, but we can
  catch it by installing a signal handler.


  based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 557
 */
#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

#include "tlpi/get_num.h"

static volatile int num_live_children = 0; /* number of children
					    * started but not yet
					    * waited on */

/*
  current_time()

  Return a string containing the current time formatted according to
  the specification in 'format' (see strftime(3) for specifiers). If
  'format' is NULL, we use "%c" as a specifier (which gives the date
  and time as for ctime(3), but without the trailing newline). Returns
  NULL on error.

  based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 194
*/
#ifndef BUFSIZ
# define BUFSIZ 1024
#endif

char*
curr_time(const char *format)
{
	static char buf[BUFSIZ]; // non-reentrant!!!
	time_t t;
	size_t ret;
	struct tm *tm;

	t = time(NULL);
	tm = localtime(&t);
	if (NULL == tm) {
		return NULL;
	}

	ret = strftime(buf, BUFSIZ, (format != NULL) ? format : "%c", tm);

	return (0 == ret) ? NULL : buf;
}
/* --- */

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
/* --- */

static void
sigchld_handler(int sig)
{
	int status, saved_errno;
	pid_t child_pid;

	// UNSAFE! don't use stdio inside the signal handler, since
	// not asynch-signal-safe

	saved_errno = errno; // in case errno, gets modified
	fprintf(stderr, "%s %s(): caught SIGCHLD\n", curr_time("%T"), __func__);

	/*
	  1.) this handler displays the process ID and wati status of
          each reaped child.
	*/
	while (0 < (child_pid = waitpid(-1, &status, WNOHANG))) {
		fprintf(stderr, "%s %s(): reaped child %ld - ",
			curr_time("%T"), __func__, (long) child_pid);
		print_wait_status(NULL, status);
		num_live_children--;
	}

	if (-1 == child_pid && errno != ECHILD) {
		perror("waitpid() failed");
		_exit(EXIT_FAILURE);
	}

	/*
	  2.) this handler displays the process ID and wait status of
	  each reaped child. in order to see that multiple SIGCHLD
	  signals are not queued while the handler is already invoked,
	  execution of the handler is artificially lengthened by a
	  call to sleep().
	 */
	sleep(5);
	fprintf(stderr, "%s %s(): returning\n", curr_time("%T"), __func__);

	errno = saved_errno;
}

int
main(int argc, char *argv[])
{
	int idx, cnt;
	sigset_t block_mask, empty_mask;
	struct sigaction sa;

	if (2 > argc) {
		fprintf(stderr, "usage:\n$ %s child-sleep-time...\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	setbuf(stdout, NULL);

	cnt = 0;
	num_live_children = argc -1;

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = sigchld_handler;
	if (-1 == sigaction(SIGCHLD, &sa, NULL)) {
		perror("sigaction() failed");
		exit(EXIT_FAILURE);
	}

	// block SIGCHLD to prevent its delivery if a child terminates
	// before the parent commences the sigsuspend() loop below


	/*
	  3.) note the use of sigprocmask() to block the SIGCHLD
	  signal before any children are created
	 */
	sigemptyset(&block_mask);
	sigaddset(&block_mask, SIGCHLD);
	if (-1 == sigprocmask(SIG_SETMASK, &block_mask, NULL)) {
		perror("sigprocmask() failed");
		exit(EXIT_FAILURE);
	}

	/*
	  4.) the main program creates one child process for each
	  (integer) command-line argument.
	 */
	for (idx = 1; idx < argc; idx++) {
		switch (fork()) {
		case -1:
			perror("fork() failed");
			_exit(EXIT_FAILURE);

		case 0:  // child: sleeps and then exits
			/*
			  5.) each of these children sleeps for the
			  number of seconds specified in the
			  corresponding command-line argument and then
			  exits.
			 */
			sleep(get_int(argv[idx], GN_NONNEG, "child-sleep-time"));
			fprintf(stderr, "%s %s(): child %d (PID=%ld) exiting\n",
				curr_time("%T"), __func__, idx, (long) getpid());

			fprintf(stderr, "child: READY.\n");
			_exit(EXIT_SUCCESS);

		default:  // parent: loops to create next child
			break;
		}
	}

	// parent: wait for SIGCHLD until all children are dead
	sigemptyset(&empty_mask);
	while (0 < num_live_children) {
		if (-1 == sigsuspend(&empty_mask) && errno != EINTR) {
			perror("sigsuspend() failed");
			exit(EXIT_FAILURE);
		}
		cnt++;
	}

	fprintf(stderr, "%s %s(): all %d children have terminated; SIGCHLD was caught %d times\n",
		curr_time("%T"), __func__, argc - 1, cnt);

	// never reached
	fprintf(stderr, "parent: READY.\n");
	exit(EXIT_SUCCESS);
}
