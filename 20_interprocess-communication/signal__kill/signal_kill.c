// signal_kill.c
/*
  demonstrates sending of a signal, in this case the STOP signal
  to the own process ID

  the prg only suspends, it needs a SIGTERM form e.g. htop
*/

#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

int kill(pid_t, int);

int main(int argc, char **argv)
{
	// get own IP
	pid_t pid = getpid();

	// suicide
	kill(pid, SIGKILL);

	// never executed
	return EXIT_SUCCESS;
}
