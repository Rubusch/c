// signalhanlder
/*
  Demonstration of a signal handler w/ message passing

  Message passing is performed by the siginfo_t* struct and its
  sigval_ptr i.e. via function pointer passed.

  Experimental grounds to play with using e.g. htop and sending
  signals to the application, e.g. SIGNINT - depending on the
  implementation/modifications the signal will terminate the program
  or the program will continue.
*/

#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>


static int RUNNING__ = 1;

void
exception_msg()
{
	fprintf(stderr, "Some important EXCEPTION has happened!!!\n");
}

void
sighandler(int signum, siginfo_t *info, void *ptr)
{
	// ATTENTION:
	// using stdio in signal handler is not async-signal-safe
	// ref.:
	// Linux Programming Interface, Michael Kerrisk, 2010, p.426
	fprintf(stderr, "\n%s()\n", __func__);

	// prepare function pointer
	void (*fptr)();

	// obtain function pointer
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
	fptr = (void (*)()) info->si_value.sival_ptr;
#pragma GCC diagnostic pop
	fptr();

	RUNNING__ = 0;
	fprintf(stderr, "SET: RUNNING__ == '%d'\n", RUNNING__);
}

void
do_cleanup()
{
	fprintf(stderr, "...cleaning up...\n");
}

int
main(int argc, char **argv)
{
	// setup sigaction
	struct sigaction sa;

	sa.sa_sigaction = sighandler; // NB: either set sa.sa_handler (simple), or sa.sa_sigaction (extended)
	sigemptyset(&sa.sa_mask); // now need a signal mask, pre-init mask
	sa.sa_flags = SA_SIGINFO; // for message passing, needs SA_SIGINFO!!!
	sigaction(SIGUSR1, &sa, NULL);

	/*
	  SCAFFOLDING: fork out, wait some secs, then signal()
	*/
	pid_t pidchld;
	pid_t pidmain = getpid();
	switch (pidchld = fork()) {
	case -1: // error case
		perror("fork()");
		_exit(EXIT_FAILURE);
	case 0: // child: trigger signal
		sleep(2);

		/*
		  prepare message and issue signal
		*/
		union sigval value;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
		value.sival_ptr = (void *) exception_msg; // set function pointer for the message
#pragma GCC diagnostic pop
		sigqueue(pidmain, SIGUSR1, value); // send the signal to PID of parent!!! NB: child has a exact copy of sighandler

		_exit(EXIT_SUCCESS);
	default:
		// loop, as long as "RUNNING__" is not triggered
		while (RUNNING__) {
			fprintf(stderr, "LOOP: RUNNING__ == '%d'\n", RUNNING__);
			sleep(1);
		}

		// do clean up here
		do_cleanup();
		break;
	}

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
