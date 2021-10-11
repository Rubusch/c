// signaltalk.c
/*
  implements a small proggy to demonstrate communication over signals

  the method using sleep() is NOT a reliable method of synchronization! ;-)
*/

#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

extern int kill(pid_t, int);

void sighup();
void sigint();
void sigquit();

int main(int argc, char **argv)
{
	pid_t pid = 0;

	if (0 > (pid = fork())) {
		perror("fork failed");
		exit(EXIT_FAILURE);

	} else if (pid == 0) {
		// child code
		puts("child");
		signal(SIGHUP, sighup);
		signal(SIGINT, sigint);
		signal(SIGQUIT, sigquit);
		while (1)
			;

	} else {
		// parent code
		puts("parent - SIGHUP");
		kill(pid, SIGHUP);

		sleep(3); // pause for 3 secs

		puts("parent - SIGINT");
		kill(pid, SIGINT);

		sleep(3);

		puts("parent - SIGQUIT");
		kill(pid, SIGQUIT);

		sleep(3);
	}
	exit(EXIT_SUCCESS);
}

void sighup()
{
	signal(SIGHUP, sighup);
	puts("child - I have received a SIGHUP");
}

void sigint()
{
	signal(SIGINT, sigint);
	puts("child - I have received a SIGINT");
}

void sigquit()
{
	puts("child - I have received a SIGQUIT");
	exit(EXIT_SUCCESS);
}
