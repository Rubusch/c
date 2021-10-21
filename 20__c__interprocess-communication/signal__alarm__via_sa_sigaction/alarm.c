// alarm.c
/*
  Signals are similar to interrupts but are thrown by the software.
  Interrupts always come from the hardware, btw softinterrupts come
  from the CPU (= hardware!)

  signal handler:
  - make it short/simple
  - proper signal masking, preferably use sigaction()
  - be careful with "fault" signals, e.g. signals that indicate
      program bugs (SIGBUS, SIGSEGV, SIGFPE)
  - be careful with timers, don't abuse the VTALRM signal, if
      you need to have more than one timer at a time, don't use
      signals
  - signals are NOT an event driven framework, this is abuse of signals
      better use a select-based loop inside a network server or an
      event loop of a windowing program directly

  NB: flags
SA_ONSTACK
If set and an alternate signal stack has been declared with sigaltstack(), the signal shall be delivered to the calling process on that stack. Otherwise, the signal shall be delivered on the current stack.
SA_RESETHAND
If set, the disposition of the signal shall be reset to SIG_DFL and the SA_SIGINFO flag shall be cleared on entry to the signal handler.

SA_RESTART
This flag affects the behavior of interruptible functions; that is, those specified to fail with errno set to [EINTR]. If set, and a function specified as interruptible is interrupted by this signal, the function shall restart and shall not fail with [EINTR] unless otherwise specified. If the flag is not set, interruptible functions interrupted by this signal shall fail with errno set to [EINTR].
SA_SIGINFO
If cleared and the signal is caught, the signal-catching function shall be entered as:
void func(int signo);


*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h> /* signal() */
#include <unistd.h> /* alarm() */
#include <string.h> /* memset() */


void handle_alarm(int sig, siginfo_t *siginfo, void *context)
{
	fprintf(stderr, "%s(): Operation timed out. Exiting..\n", __func__);
	exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
	puts("alarm signal");
	// ...

	struct sigaction saction;
	memset(&saction, 0, sizeof(saction));

	/* sa_sigaction: uses function with 3 arguments, in case of
	 * additional parameters, context or message passing */
	saction.sa_sigaction = &handle_alarm;

        /* SA_SIGINFO: tell sigaction() to use the sa_sigaction field,
	 * not sa_handler */
	saction.sa_flags = SA_SIGINFO;

	sigaction(SIGALRM, &saction, NULL);


	puts("wait 30 secs - then alarm, prg ends");

	// issues a SIGALRM int 30 sec
	alarm(30);
	while (1)
		;

	// never will be executed
	puts("ends");
	exit(EXIT_SUCCESS);
}
