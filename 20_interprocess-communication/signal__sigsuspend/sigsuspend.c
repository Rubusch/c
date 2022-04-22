/*
  usage:
  $ ./sigsuspend.elf
      initial signal mask is:
                      <empty signal set>
      === LOOP 1
      starting critical section, signal mask is:
                      2 (Interrupt)
                      3 (Quit)


--> CTRL-c (SIGINT) - blocked, caught later

      ^Cbefore sigsuspend() - pending signals:
                      2 (Interrupt)
      caught signal 2 (Interrupt)   <--------------- now caught
      === LOOP 2
      starting critical section, signal mask is:
                      2 (Interrupt)
                      3 (Quit)


--> CTRL-c (SIGINT) - blocked

      ^Cbefore sigsuspend() - pending signals:
                      2 (Interrupt)
      caught signal 2 (Interrupt)  <---------------- now caught
      === LOOP 3
      starting critical section, signal mask is:
                      2 (Interrupt)
                      3 (Quit)


--> time is up, SIGINT is not blocked anymore...

      before sigsuspend() - pending signals:
                      <empty signal set>


--> CTRL-c (SIGINT)

      ^Ccaught signal 2 (Interrupt)
      === LOOP 4
      starting critical section, signal mask is:
                      2 (Interrupt)
                      3 (Quit)
      before sigsuspend() - pending signals:
                      <empty signal set>

--> send SIGTERM via htop

      Terminated


  demonstrates sigsuspend()

  - display the initial value of the process signal mask using
    print_sig_mask()

  - block SIGINT and SIGQUIT, and save the original process
    signal_mask

  - establish the same handler for both SIGINT and SIGQUIT; this
    handler displays a message, and, if it was invoked via delivery of
    SIGQUIT, sets the global variable got_sigquit


  references:
  based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 466
  https://man7.org/tlpi/index.html

  tpli codes, distribution version e.g. here:
  https://github.com/bradfa/tlpi-dist
 */

#define _GNU_SOURCE /* strsinal() dclaration from <string.h> */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

//#include "tlpi/get_num.h"
static volatile sig_atomic_t got_sigquit = 0;

void
print_sigset(FILE *of, const char *prefix, const sigset_t *sigset)
{
	int sig, cnt;

	cnt = 0;
	for (sig = 1; sig < NSIG; sig++) {
		if (sigismember(sigset, sig)) {
			cnt++;
			fprintf(of, "%s%d (%s)\n",
				prefix, sig, strsignal(sig));
		}
	}

	if (0 == cnt) {
		fprintf(of, "%s<empty signal set>\n", prefix);
	}
}

int
print_sig_mask(FILE* of, const char *msg)
{
	sigset_t curr_mask;

	if (NULL != msg) {
		fprintf(of, "%s", msg);
	}

	if (-1 == sigprocmask(SIG_BLOCK, NULL, &curr_mask)) {
		return -1;
	}

	print_sigset(of, "\t\t", &curr_mask);
	return 0;
}

int
print_pending_sigs(FILE *of, const char *msg)
{
	sigset_t pending_sigs;

	if (NULL != msg) {
		fprintf(of, "%s", msg);
	}

	if (-1 == sigpending(&pending_sigs)) {
		return -1;
	}

	print_sigset(of, "\t\t", &pending_sigs);
	return 0;
}

/* --- */

static void
signal_handler(int sig)
{
	// UNSAFE: never use stdio in signal handler
	fprintf(stderr, "caught signal %d (%s)\n",
		sig, strsignal(sig));

	if (SIGQUIT == sig) {
		got_sigquit = 1;
	}
}

int
main(int argc, char* argv[])
{
	int nloops;
	time_t start_time;
	sigset_t orig_mask, block_mask;
	struct sigaction sa;

	print_sig_mask(stderr, "initial signal mask is:\n");

	// prepare sigset masks
	sigemptyset(&block_mask);
	sigaddset(&block_mask, SIGINT);
	sigaddset(&block_mask, SIGQUIT);
	if (-1 == sigprocmask(SIG_BLOCK, &block_mask, &orig_mask)) {
		perror("sigprocmask() failed");
		exit(EXIT_FAILURE);
	}

	// register handler
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = signal_handler;

	// init sigaction with signals
	if (-1 == sigaction(SIGINT, &sa, NULL)) {
		perror("sigaction() failed with SIGINT");
		exit(EXIT_FAILURE);
	}

	if (-1 == sigaction(SIGQUIT, &sa, NULL)) {
		perror("sigaction() failed with SIGQUIT");
		exit(EXIT_FAILURE);
	}

	// simulation of critical seciton - "game loop"
	for (nloops = 1; !got_sigquit; nloops++) {
		fprintf(stderr, "=== LOOP %d\n", nloops);

		// simulate a critical section by delaying a few secs

		print_sig_mask(stderr, "starting critical section, signal mask is:\n");
		for (start_time = time(NULL); time(NULL) < start_time + 4; ) {
			continue; // run for a few seconds elapsed time
		}

		print_pending_sigs(stderr, "before sigsuspend() - pending signals:\n");
		if (-1 == sigsuspend(&orig_mask) && errno != EINTR) {
			perror("sigsuspend() failed");
			exit(EXIT_FAILURE);
		}
	}

	if (-1 == sigprocmask(SIG_SETMASK, &orig_mask, NULL)) {
		fprintf(stderr, "sigprocmask() - SIG_SETMASK\n");
		exit(EXIT_FAILURE);
	}

	print_sig_mask(stderr, "=== exited loop\nrestored signal mask to:\n");

	// do other processing

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
