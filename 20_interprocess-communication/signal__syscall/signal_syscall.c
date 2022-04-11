// signal_syscall.c
/*
  demonstrates the reaction on a sent signal
  Send a SIGKILL (e.g. with htop) to end the prg!
*/

#include <stdio.h>
#include <stdlib.h>

#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

void catch_int(int sig)
{
	// ATTENTION:
	// using stdio in signal handler is not async-signal-safe
	// ref.:
	// Linux Programming Interface, Michael Kerrisk, 2010, p.426

	//  puts("Eh eh that tickles...\n");
	printf("Eh eh that tickles...\n");
	fflush(stdout);
}

int main(int argc, char **argv)
{
	struct sigaction sa;
	sa.sa_handler = catch_int;
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);

	// infinite loop
	while (1)
		pause();
}
