/*
  Signalhandler Backtrace

  The idea is taken from an older article of the linux journal and
  re-implemented.

  NB: the demo needs '-rdynamic' to be set as gcc compile flag, to
  show the function names.

---
  stackoverflow says:

  In a CMake project, this had mistakenly been set as
  ADD_COMPILE_OPTIONS(-rdynamic). Instead, it needs to be
  ADD_LINK_OPTIONS(-rdynamic) or equivalent to have the desired
  behaviour. - Stéphane Jul 17 '20 at 19:54

  Better yet for those using cmake: SET (CMAKE_ENABLE_EXPORTS
  TRUE). This sets the -rdynamic linker flag. - Stéphane Jan 17 at
  18:55
*/

#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <execinfo.h>
#include <ucontext.h>

/*
  get the context of the caller, to obtain its backtrace i.e. it's
  instruction pointer (REG_RIP).
*/
void sig_handler_backtrace(int sig, siginfo_t *info, void *secret)
{
	void *trace[16];
	char **messages = (char **)NULL;
	int i, trace_size = 0;
	ucontext_t *uc = (ucontext_t *)secret;

	if (sig == SIGSEGV) {
		fprintf(stdout,
			"%s() - signal %d, faulty address is %p, from %p\n",
			__func__, sig, info->si_addr,
			(void *)uc->uc_mcontext.gregs[REG_RIP]);
	} else {
		fprintf(stdout, "Got signal %d\n", sig);
	}

	// obtain backtrace entries
	trace_size = backtrace(trace, 16);

	// overwrite sigaction with caller's address (approach by linux journal)
	trace[1] = (void *)uc->uc_mcontext.gregs[REG_RIP];
	messages = backtrace_symbols(trace, trace_size);

	// skip first stack frame (points here)
	printf("[bt] Execution path:\n");
	for (i = 1; i < trace_size; ++i) {
		fprintf(stdout, "[bt] %s\n", messages[i]);
	}

	fprintf(stdout, "READY.\n");
	exit(EXIT_SUCCESS);
}

/*
  testee (provoking the SIGSEGV)

  the following provokes a segmentation violation, i.e. SIGSEGV, the
  signal will be caught by the signal handler.
*/
//*
// some further backtrace
int func_another(int a, char b)
{
	char *p = (char *)0xdeadbeef;

	a = a + b;
	*p = 10; // CRASH here!!

	return 2 * a;
}
int func()
{
	int res, a = 5;
	res = 5 + func_another(a, 't');
	return res;
}
/*/
// simpler backtrace
int func() {
	int res, a = 5;
	{
		char b = 't';
		char *p = (char *)0xdeadbeef;

		a = a + b;
		*p = 10;	// CRASH here!!
		res = 2*a;
	}
	return res;
}
// */

int main(int argc, char *argv[])
{
	struct sigaction sa;

	// install signal handler
	sa.sa_sigaction = sig_handler_backtrace;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART | SA_SIGINFO;
	sigaction(SIGSEGV, &sa, NULL);
	sigaction(SIGUSR1, &sa, NULL);
	// further signals go here..

	// provoke a SIGSEGV
	fprintf(stdout, "%d\n", func());

	fprintf(stdout, "READY.\n");
	exit(EXIT_SUCCESS);
}
