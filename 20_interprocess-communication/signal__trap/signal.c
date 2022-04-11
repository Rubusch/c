// signal.c
/*
  implements a proggy that sends a signal and catches it
*/

#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

// 3. signal handler implementation
void handler_int(int dummy)
{
	puts("you have pressed CTRL-c");
	raise(SIGKILL);
}

void handler_quit(int dummy)
{
	// here no action to be done due to the reset..
	puts("CTRL-\\ pressed to quit");
	raise(SIGKILL);
}

// 2. handler multiplexer
void handler(int signum)
{
	// ATTENTION:
	// using stdio in signal handler is not async-signal-safe
	// ref.:
	// Linux Programming Interface, Michael Kerrisk, 2010, p.426

	switch (signum) {
	case SIGINT:
		handler_int(signum);
		break;
	case SIGQUIT:
		handler_quit(signum);
		break;
	default:
		fprintf(stderr, "invalid signal\n");
		break;
	}
}


int main(int argc, char **argv)
{
	// 1. set up the signal handler to the signal
	struct sigaction sa;
	sa.sa_handler = handler;
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);

	puts("CTRL-c disbled - use CTRL-\\ to quit");

	// infinite loop
	while (1)
		;

	exit(EXIT_SUCCESS);
}
