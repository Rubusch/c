/*
  usage: see in sigqueue.c


  demostrates a signal handler dealing with asynchronous signals
  i.e. "realtime" signals, this demonstrates a regular signalhandler

  use SIGINT or SIGTERM to terminate program


  references:
  based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 462
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

static volatile int handler_sleep_time;
static volatile int sig_cnt = 0;
static volatile sig_atomic_t done = 0;

static void
siginfo_handler(int sig, siginfo_t *si, void *ucontext)
{
	// UNSAFE: don't use stdio inside signalhandlers
	if (sig == SIGINT || sig == SIGTERM) {
		done = 1;
		return;
	}

	sig_cnt++;
	fprintf(stderr, "caught signal %d\n", sig);
	fprintf(stderr, "\tsi_signo = %d, si_code = %d (%s), ",
		si->si_signo, si->si_code,
		(si->si_code == SI_USER) ? "SI_USER" :
		(si->si_code == SI_QUEUE) ? "SI_QUEUE" : "other");
	fprintf(stderr, "si_value = %d\n", si->si_value.sival_int);
	fprintf(stderr, "\tsi_pid = %ld, si_uid = %ld\n",
		(long) si->si_pid, (long) si->si_uid);

	sleep(handler_sleep_time);
}

int
main(int argc, char* argv[])
{
	struct sigaction sa;
	int sig;
	sigset_t prev_mask, block_mask;

	if (argc < 2) {
		fprintf(stderr, "usage:\n$ %s [block-time [handler-sleep-time]]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "%s: PID is %ld\n", argv[0], (long) getpid());

	handler_sleep_time = (argc > 2) ? get_int(argv[2], GN_NONNEG, "handler_sleep_time") : 1;

	/*
	  establish handler for most signals. During execution of the
	  handler, mask all other signals to prevent handlers
	  recursively interrupting each other (which would make the
	  output hard to read).
	*/

	sa.sa_sigaction = siginfo_handler;
	sa.sa_flags = SA_SIGINFO;
	sigfillset(&sa.sa_mask);

	for (sig = 1; sig < NSIG; sig++) {
		if (sig != SIGTSTP && sig != SIGQUIT) {
			sigaction(sig, &sa, NULL);
		}
	}

	/*
	  optionally block signals and sleep, allowing signals to be
	  sent to us before they are unblocked and handled
	*/

	if (argc > 1) {
		sigfillset(&block_mask);
		sigdelset(&block_mask, SIGINT);
		sigdelset(&block_mask, SIGTERM);

		if (-1 == sigprocmask(SIG_SETMASK, &block_mask, &prev_mask)) {
			perror("sigprocmask() failed");
			exit(EXIT_FAILURE);
		}

		fprintf(stderr, "%s: signals blocked - sleeping %s seconds\n",
			argv[0], argv[1]);

		sleep(get_int(argv[1], GN_GT_0, "block_time"));

		fprintf(stderr, "%s: sleep complete\n", argv[0]);

		if (-1 == sigprocmask(SIG_SETMASK, &prev_mask, NULL)) {
			perror("sigprocmask() failed");
			exit(EXIT_FAILURE);
		}
	}

	while (!done) {
		pause(); // wait for incoming signals
	}

	fprintf(stderr, "caught %d signals\n", sig_cnt);

	fprintf(stderr, "%s() - READY.\n", __func__);
	exit(EXIT_SUCCESS);
}
