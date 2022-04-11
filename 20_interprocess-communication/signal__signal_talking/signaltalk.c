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

void sighup(int signum __attribute__((unused)))
{
	puts("child - I have received a SIGHUP");
}

void sigint(int signum __attribute__((unused)))
{
	puts("child - I have received a SIGINT");
}

void sigquit(int signum __attribute__((unused)))
{
	puts("child - I have received a SIGQUIT");
	// ATTENTION:
	// never use exit() in signal handlers
	raise(SIGKILL);
}

void sig_handler(int signum)
{
	// ATTENTION:
	// using stdio in signal handler is not async-signal-safe
	// ref.:
	// Linux Programming Interface, Michael Kerrisk, 2010, p.426

	switch (signum) {
	case SIGHUP:
		sighup(signum);
		break;
	case SIGINT:
		sigint(signum);
		break;
	case SIGQUIT:
		sigquit(signum);
		break;
	default:
		fprintf(stderr, "invalid signal\n");
		break;
	}
}

int main(int argc, char **argv)
{
	pid_t pid = 0;

	if (0 > (pid = fork())) {
		perror("fork failed");
		exit(EXIT_FAILURE);

	} else if (pid == 0) {
		// child code
		puts("child");

		struct sigaction sa;
		sa.sa_handler = sig_handler;
		sa.sa_flags = 0;
		sigaction(SIGHUP, &sa, NULL);
		sigaction(SIGINT, &sa, NULL);
		sigaction(SIGQUIT, &sa, NULL);

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

