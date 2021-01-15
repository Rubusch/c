// signalhanlder
/*
  demonstration of a signal handler

  experimental grounds to play with using e.g. htop and sending
  signals to the application, e.g. SIGNINT - depending on the
  implementation/modifications the signal will terminate the program
  or the program will continue
*/

#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <signal.h>
#include <unistd.h>

int flag = 0;

void sighandler()
{
	signal(SIGINT, sighandler);
	flag = 0;
}


int main(int argc, char **argv)
{
	// ...
	signal(SIGINT, sighandler);
	// ...
	flag = 1;
	while (flag)
		pause();
	// ...
	exit(EXIT_SUCCESS);
}
