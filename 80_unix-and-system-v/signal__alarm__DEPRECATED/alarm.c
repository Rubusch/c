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
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h> /* signal() */
#include <unistd.h> /* alarm() */


void handle_alarm(int sig)
{
	puts("Operation timed out. Exiting..\n");
	exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
	puts("alarm signal");
	// ...

	// set a signal handler for ALARM signals
	signal(SIGALRM, handle_alarm); /* NB: signal() is deprecated, prefer sigaction() */

	// some text
	puts("wait 30 secs - then alarm, prg ends");

	// start a 30 sec alarm
	alarm(30);
	while (1)
		;

	// never will be executed
	puts("ends");
	exit(EXIT_SUCCESS);
}
