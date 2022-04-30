/*
  usage:
    $ ./fork.elf
      [19:04:56 2565] parent aboud to wait for signal
      [19:04:56 2566] child started - doing some work
      [19:04:58 2566] child about to signal parent
      [19:04:58 2565] parent got signal
      READY.


  avoiding race conditions by synchronizing with signals

  after a fork(), if either process needs to wait for the other to
  complete an action, then the active process can send a signal after
  completing the action; the other process waits for the signal.

  in this program, we assume that it is the parent that must wait on
  the child to carry out some action. the signal-related calls in the
  parent and child can be swapped if the child must wait on the
  parent. it is even possible for both parent and child to signal each
  other multiple times in order to coordinate their actions, although
  in practice, such coordination is more likely to be done using
  semaphores, file locks, or message passing.

  NB: we block the synchronization signal (SIGUSR1) before the fork()
  call. If the parent tried blocking the signal after the fork(), it
  would remain vulnerable to the very race condition we are trying to
  avoid.


  based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 528
 */
#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

#define SYNC_SIG SIGUSR1

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

static void
signal_handler(int sig)
{
	// empty
}

int
main(int argc, char *argv[])
{
	pid_t child_pid;
	sigset_t block_mask, orig_mask, empty_mask;
	struct sigaction sa;

	setbuf(stdout, NULL); // disable buffering of stdout

	sigemptyset(&block_mask);
	sigaddset(&block_mask, SYNC_SIG);
	if (-1 == sigprocmask(SIG_BLOCK, &block_mask, &orig_mask)) {
		/*
		  SIG_BLOCK

		  the signals specified in the signal set pointed to
		  by set are added to the signal mask. in other words,
		  the signal mask is set to the union of its current
		  value and set.
		*/
		perror("sigprocmask() failed");
		exit(EXIT_FAILURE);
	}

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = signal_handler;
	if (-1 == sigaction(SYNC_SIG, &sa, NULL)) {
		perror("sigaction() failed");
		exit(EXIT_FAILURE);
	}

	switch (child_pid = fork()) {
	case -1:
		fprintf(stderr, "fork() failed\n");
		exit(EXIT_FAILURE);

	case 0: // child: change file offset and status flags
		printf("[%s %ld] child started - doing some work\n",
		       curr_time("%T"), (long) getpid());

		sleep(2);

		// and then signals the parent that it's done
		printf("[%s %ld] child about to signal parent\n",
		       curr_time("%T"), (long) getpid());

		if (-1 == kill(getppid(), SYNC_SIG)) {
			perror("kill() failed");
			_exit(EXIT_FAILURE);
		}

		// now child can do other things...
		_exit(EXIT_SUCCESS);

	default: // parent

		// parent may do some work here, and then waits for
		// child to complete the required action
		printf("[%s %ld] parent aboud to wait for signal\n",
		       curr_time("%T"), (long) getpid());

		sigemptyset(&empty_mask);
		if (-1 == sigsuspend(&empty_mask) && errno != EINTR) {
			perror("sigsuspend() failed");
			exit(EXIT_FAILURE);
		}

		printf("[%s %ld] parent got signal\n",
		       curr_time("%T"), (long) getpid());

		// if required, return signal mask to its original state

		if (-1 == sigprocmask(SIG_SETMASK, &orig_mask, NULL)) {
			/*
			  SIG_SETMASK

			  the signal set pointed to by set is assigned
			  to the signal mask
			 */
			perror("sigprocmask() failed");
			exit(EXIT_FAILURE);
		}

		// parent carries on to do other things...
		fprintf(stderr, "READY.\n");
		exit(EXIT_SUCCESS);
	}

	// never reaches to here...
	exit(EXIT_SUCCESS);
}
