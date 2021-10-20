// itchy.c
/*
  signals
*/

#define _XOPEN_SOURCE 600

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ME "ITCHY:"

// signalhandler
void handle_alarm(int dummy)
{
	// to prevent signal resetting
	signal(SIGALRM, handle_alarm); /* NB: signal() is deprecated, prefer sigaction() */
	fprintf(stderr, "%s SIGALRM received - eh eh, that tickles!\n", ME);
	fflush(stdout);
}

int main(int argc, char **argv)
{
	fprintf(stderr, "%s set up signalhandler\n", ME);
	signal(SIGALRM, handle_alarm); /* NB: signal() is deprecated, prefer sigaction() */

	while (1)
		;

	fprintf(stderr, "%s done!\n", ME);
	exit(EXIT_SUCCESS);
}
