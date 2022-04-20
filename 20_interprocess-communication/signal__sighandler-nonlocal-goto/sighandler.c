/*
  usage:
  $ ./sighandler.elf

  e.g. send signals from another terminal, e.g via htop, or kill

  $ ./sighandler.elf
    signal mask at startup:
                    <empty signal set>
    calling sigsetjmp()
    ^Creceived signal 2 (Interrupt), signal mask is:
                    2 (Interrupt)
    after jump from sighandler, signal mask is:
                    <empty signal set>

    ^Creceived signal 2 (Interrupt), signal mask is:
                    2 (Interrupt)
    after jump from sighandler, signal mask is:
                    <empty signal set>

    ^Creceived signal 2 (Interrupt), signal mask is:
                    2 (Interrupt)
    after jump from sighandler, signal mask is:
                    <empty signal set>
    ...


  demostrates a signal handler using a non-local goto
  sigsetjmp()/siglongjmp()


  references:
  based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 432
  https://man7.org/tlpi/index.html

  tpli codes, distribution version e.g. here:
  https://github.com/bradfa/tlpi-dist
 */

#define _GNU_SOURCE /* strsinal() dclaration from <string.h> */
#define _POSIX_C_SOURCE 600 /* sigsetjmp() / siglongjmp() */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <setjmp.h>
#include <signal.h>
#include <unistd.h>

#define USE_SIGSETJMP 1

#ifndef NSIG
# define NSIG 3
#endif

static volatile sig_atomic_t can_jump = 0;
// set to 1 once "env" buffer has been initialized by [sig]setjmp()

#ifdef USE_SIGSETJMP
static sigjmp_buf senv;
#else
static jmp_buf env;
#endif

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

static void
sighandler(int sig)
{
	// NB: unsafe, the sighandler uses stdio functions and strsignal()
	fprintf(stderr, "received signal %d (%s), signal mask is:\n",
		sig, strsignal(sig));

	print_sig_mask(stderr, NULL);

	if (!can_jump) {
		fprintf(stderr, "'env' buffer not yet set, doing a simple return\n");
		return;
	}

#ifdef USE_SIGSETJMP
	siglongjmp(senv, 1);
#else
	longjmp(env, 1);
#endif
}

int
main(int argc, char* argv[])
{
	struct sigaction sa;

	print_sig_mask(stderr, "signal mask at startup:\n");

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = sighandler;
	if (-1 == sigaction(SIGINT, &sa, NULL)) {
		perror("sigaction() failed");
		exit(EXIT_FAILURE);
	}

#ifdef USE_SIGSETJMP
	fprintf(stderr, "calling sigsetjmp()\n");
	if (0 == sigsetjmp(senv, 1)) {
#else
	fprintf(stderr, "calling setjmp()\n");
	if (0 == setjmp(env)) {
#endif
                // executed after [sig]setjmp()
		can_jump = 1;

	} else {
		// executed afater [sig]longjmp()
		print_sig_mask(stderr, "after jump from sighandler, signal mask is:\n");
	}

	for (;;) {
		pause(); // wait for signals until killed
	}

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
