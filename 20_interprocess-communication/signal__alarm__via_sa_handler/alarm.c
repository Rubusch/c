// alarm.c
/*
  Signals are similar to interrupts but are thrown by the software.
  Interrupts always come from the hardware, btw softinterrupts come
  from the CPU (= hardware!)


  struct sighandler_t

  - sighandler_t->sa_handler() classical signal handler, takes one
    argument the signal number, can also be set to
       * SIG_DFL for the default action

       * SIG_IGN to ignore this signal

       * A pointer to a signal handling function; this function
         receives the signal number as its only argument


  - sighandler_t->sa_sigaction() signal handler, takes three
    arguments, and allows for passing the siginfo_t (parent PID, etc)


  Also check for sa_mask, allows to filter and block signals

  sa_flags, set action to use
       * SA_ONSTACK - If set and an alternate signal stack has been
           declared with sigaltstack(), the signal shall be delivered
           to the calling process on that stack. Otherwise, the signal
           shall be delivered on the current stack.

       * SA_RESETHAND - If set, the disposition of the signal shall be
           reset to SIG_DFL and the SA_SIGINFO flag shall be cleared
           on entry to the signal handler.

       * SA_RESTART - This flag affects the behavior of interruptible
           functions; that is, those specified to fail with errno set
           to [EINTR]. If set, and a function specified as
           interruptible is interrupted by this signal, the function
           shall restart and shall not fail with [EINTR] unless
           otherwise specified. If the flag is not set, interruptible
           functions interrupted by this signal shall fail with errno
           set to [EINTR].

       * SA_SIGINFO - If cleared and the signal is caught, the
           signal-catching function shall be entered as: void func(int
           signo);

       * SA_ONESHOT - The signalhandler executes only once
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h> /* sigaction() */
#include <unistd.h> /* alarm() */
#include <string.h> /* memset() */


void handle_alarm(int sig)
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

	/* sa_handler: only takes the signal number as arg */
	saction.sa_handler = handle_alarm;

        /* 0: sigaction() uses the (simple) sa_handler */
	saction.sa_flags = 0; // alternative SA_SIGINFO;

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
