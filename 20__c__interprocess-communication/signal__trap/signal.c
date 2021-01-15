// signal.c
/*
  implements a proggy that sends a signal and catches it
*/

#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

void handler_int(int);
void handler_quit(int);


int main(int argc, char **argv)
{
	// 1. set up the signal handler to the signal
	signal(SIGINT, handler_int);
	signal(SIGQUIT, handler_quit);

	puts("CTRL-c disbled - use CTRL-\\ to quit");

	// infinite loop
	while (1)
		;
}


// 2. signal handler implementation
void handler_int(int dummy)
{
	// 3. make sure that the next time the signal can be received
	// (system resets the sighandler on some systems)
	signal(SIGINT, handler_int);

	puts("you have pressed CTRL-c");
}


void handler_quit(int dummy)
{
	// here no action to be done due to the reset..
	puts("CTRL-\\ pressed to quit");
	exit(EXIT_SUCCESS);
}
