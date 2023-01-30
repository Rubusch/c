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


static int RUNNING = 1;
static pid_t PID_SERVER;

void
exception_msg(const char* message)
{
	fprintf(stderr, "%s(): signaling '%s'\n",
		__func__, message);

	union sigval value;                   // NB: this is sigval, with 'g'
	value.sival_ptr = (void*) message;    // NB: this is sival, w/o 'g' !!!
	sigqueue(PID_SERVER, SIGUSR1, value);
}

void
sighandler(int signum, siginfo_t *info, void *ptr)
{
	// ATTENTION:
	// using stdio in signal handler is not async-signal-safe
	// ref.:
	// Linux Programming Interface, Michael Kerrisk, 2010, p.426
	fprintf(stderr, "\n%s()\n", __func__);
	const char* msg = (const char*) info->si_value.sival_ptr;
	fprintf(stderr, "%s(): received '%s'\n", __func__, msg);

	RUNNING = 0;
	fprintf(stderr, "SET: RUNNING == '%d'\n", RUNNING);
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
	PID_SERVER = getpid();
	switch (pidchld = fork()) {
	case -1: // error case
		perror("fork()");
		_exit(EXIT_FAILURE);
	case 0: // child: trigger signal
		sleep(2);

		/*
		  prepare message and issue signal
		*/
		exception_msg("EXCEPTION: 123 !@#...");
		_exit(EXIT_SUCCESS);
	default:
		// loop, as long as "RUNNING" is not triggered
		while (RUNNING) {
			fprintf(stderr, "LOOP: RUNNING == '%d'\n", RUNNING);
			sleep(1);
		}

		// do clean up here
		do_cleanup();
		break;
	}

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
