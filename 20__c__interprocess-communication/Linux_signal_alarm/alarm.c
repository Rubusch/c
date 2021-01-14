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
#include <string.h>

#include <signal.h>
#include <unistd.h>

#define USRNAME_SIZ 40

char user[USRNAME_SIZ];

void handle_alarm(int);


void handle_alarm(int sig)
{
	puts("Operation timed out. Exiting..\n");
	exit(0);
}


int main(int argc, char **argv)
{
	puts("alarm signal");
	// ...

	// set a signal handler for ALARM signals
	signal(SIGALRM, handle_alarm);

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
