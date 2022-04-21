/*
  usage:
  $ ulimit -s unlimited
  $ ./sigaltstack.elf
      ...many intervening lines of output removed...


  demostrates sigaltstack()


  references:
  based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 436
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

static void
sigsegv_handler(int sig)
{
	int x;

	// UNSAFE: don't use stdio functions inside signalhandler
	fprintf(stderr, "caught signal %d (%s)\n",
		sig, strsignal(sig));
	fprintf(stderr, "top of handler stack near\t%10p\n",
		(void*) &x);
	fflush(NULL);

	_exit(EXIT_FAILURE); // NB: don't use exit(), prefer _exit() in signal handler
}

static void
overflow_stack(int ncalls)
{
	char arr[100000];

	fprintf(stderr, "%s()...\n", __func__);
	fprintf(stderr, "call %4d - top of stack near %10p\n",
		ncalls, &arr[0]);
	overflow_stack(ncalls + 1);
}

int
main(int argc, char* argv[])
{
	stack_t sigstack;
	struct sigaction sa;
	int idx;


	fprintf(stderr, "top of standard stack is near %10p\n",
		(void*) &idx);

	// allocate alternate stack and inform kernel of its existence

	sigstack.ss_sp = malloc(SIGSTKSZ);
	if (NULL == sigstack.ss_sp) {
		fprintf(stderr, "malloc() failed\n");
		exit(EXIT_FAILURE);
	}

	sigstack.ss_size = SIGSTKSZ;
	sigstack.ss_flags = 0;
	if (-1 == sigaltstack(&sigstack, NULL)) {
		fprintf(stderr, "sigaltstack() failed\n");
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "alternate stack is at\t%10p - %p\n",
		sigstack.ss_sp, (char*) sbrk(0) - 1);

	sa.sa_handler = sigsegv_handler; // establish handler for sigsegv
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_ONSTACK;
	if (-1 == sigaction(SIGSEGV, &sa, NULL)) { // handler uses alternate stack
		// NB: SIGSEGV probably can't be caught
		perror("sigaction() failed");
		exit(EXIT_FAILURE);
	}

	overflow_stack(1);

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
