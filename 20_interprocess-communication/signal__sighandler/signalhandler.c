// signalhanlder
/*
  demonstration of a signal handler

  experimental grounds to play with using e.g. htop and sending
  signals to the application, e.g. SIGNINT - depending on the
  implementation/modifications the signal will terminate the program
  or the program will continue
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <signal.h>
#include <unistd.h>

int running = 0;

void sighandler()
{
	// ATTENTION:
	// using stdio in signal handler is not async-signal-safe
	// ref.:
	// Linux Programming Interface, Michael Kerrisk, 2010, p.426
	fprintf(stderr, "\n%s()\n", __func__);
	running = 0;
}

int main(int argc, char **argv)
{
	// ...
	struct sigaction sa;
	sa.sa_handler = sighandler;
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	// ...
	running = 1;
	while (running)
		pause();
	// ...
	exit(EXIT_SUCCESS);
}
