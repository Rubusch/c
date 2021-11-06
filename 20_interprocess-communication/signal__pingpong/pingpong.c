/*
  ping-pong

  the demo shows that a received SIGINT at any time will stop the
  program only after the current "ping-" has received its "pong"

  idea by book example
*/
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// rename "void*" to something meaningful, e.g. "sighandler_t"
typedef void (*sighandler_t)(int);

// flag to exit the main loop
volatile sig_atomic_t running = 1;

static sighandler_t
register_sighandler(int sig_nr, sighandler_t signalhandler)
{
	struct sigaction sa_new, sa_old;

	sa_new.sa_handler = signalhandler; // set the simple handler

	sigemptyset(&sa_new.sa_mask); // reset sa
	sa_new.sa_flags = SA_RESTART; // set a flag

	if (0 > sigaction (sig_nr, &sa_new, &sa_old)) // register
		return SIG_ERR;

	return sa_old.sa_handler; // return the old sa
}

static void sighandler(int sig_nr) {
	running--;
	register_sighandler(sig_nr, sighandler);
}

int main(void) {
	register_sighandler(SIGINT, sighandler);

	// by the loop every "ping" is assured to receive a "pong"
	while (running) {
		sleep(1);
		fprintf(stderr, "Ping-");
		sleep(1);
		fprintf(stderr, "Pong\n");
	}
	exit(EXIT_SUCCESS);
}
