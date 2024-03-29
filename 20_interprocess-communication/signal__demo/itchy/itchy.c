// itchy.c
/*
  signals
*/

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ME "ITCHY:"

// signalhandler
void handle_alarm(int dummy)
{
	// ATTENTION:
	// using stdio in signal handler is not async-signal-safe
	// ref.:
	// Linux Programming Interface, Michael Kerrisk, 2010, p.426
	fprintf(stderr, "%s SIGALRM received - eh eh, that tickles!\n", ME);
}

int main(int argc, char **argv)
{
	fprintf(stderr, "%s set up signalhandler\n", ME);

	struct sigaction sa __attribute__((unused));
	sa.sa_handler = handle_alarm;
	sa.sa_flags = 0;
	sigaction(SIGALRM, &sa, NULL);

	while (1)
		;

	fprintf(stderr, "%s done!\n", ME);
	exit(EXIT_SUCCESS);
}
