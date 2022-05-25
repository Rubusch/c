/*
  usage:
    $ ./timer.elf 1 800000 1 0
            elapsed value interval
    START:    0.00
            elapsed value interval
    main:     0.00    1.30    1.00
            elapsed value interval
    main:     0.00    0.80    1.00
            elapsed value interval
    main:     0.00    0.30    1.00
            elapsed value interval
    ALARM:    0.00    1.00    1.00
            elapsed value interval
    main:     0.00    0.80    1.00
            elapsed value interval
    main:     0.00    0.30    1.00
            elapsed value interval
    ALARM:    0.00    1.00    1.00
            elapsed value interval
    main:     0.00    0.80    1.00
            elapsed value interval
    main:     0.00    0.30    1.00
            elapsed value interval
    ALARM:    0.00    1.00    1.00
    READY.

  demonstrates using a real-time timer; getitimer() and setitimer()

  references:
  based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 482
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <stdbool.h>

#include "tlpi/get_num.h"

static volatile sig_atomic_t got_alarm = 0;
// set nonzero on receipt of SIGALRM

/*
  retrieve and display the real time, and (if 'include_timer' is TRUE)
  the current value and interval for the ITIMER_REAL timer
*/

static void
display_times(const char* msg, bool include_timer)
{
	/*
	  execute a continuous loop, consuming CPU time and
	  periodically calling the function display_times(), which
	  displays the elapsed real time since the program began, as
	  well as the current state of the ITIMER_REAL timer
	 */
	struct itimerval itv;
	static struct timeval start;
	struct timeval curr;
	static int ncalls;

	ncalls = 0; // number of calls to this function

	if (0 == ncalls) { // initialize elapsed time meter
		if (-1 == gettimeofday(&start, NULL)) {
			perror("gettimeofday() failed");
			exit(EXIT_FAILURE);
		}
	}

	if (0 == ncalls % 20) { // print header every 20 lines
		fprintf(stderr, "\telapsed\tvalue interval\n");
	}

	if (-1 == gettimeofday(&curr, NULL)) {
		perror("gettimeofday() failed");
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "%-7s\t%6.2f", msg,
		curr.tv_sec - start.tv_sec + (curr.tv_usec - start.tv_usec) / 1000000.0);

	if (include_timer) {
		if (-1 == getitimer(ITIMER_REAL, &itv)) {
			perror("getitimer() failed");
			exit(EXIT_FAILURE);
		}
		fprintf(stderr, "\t%6.2f\t%6.2f",
			itv.it_value.tv_sec + itv.it_value.tv_usec / 1000000.0,
			itv.it_interval.tv_sec + itv.it_interval.tv_usec / 1000000.0);
	}
	fprintf(stderr, "\n");
	ncalls++;
}

static void
sigalrm_handler(int sig)
{
	/*
	  each time the timer expires, the SIGALRM handler is invoked,
	  and it sets a global flag, got_alarm

	  whenever this flag is set, the loop in the main program
	  calls display_times() in order to show when the handler was
	  called and the state of the timer
	 */
	got_alarm = 1;
}

int
main(int argc, char *argv[])
{
	struct itimerval itv;
	clock_t prev_clk;
	int max_sigs; // number of signals to catch before exiting
	int sig_cnt;  // number of signals so far caught
	struct sigaction sa;

	if (2 > argc) {
		fprintf(stderr, "%s [secs [usecs [int-secs [int-usecs]]]]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/*
	  establish a handler for the SIGALRM signal
	 */
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = sigalrm_handler;
	if (-1 == sigaction(SIGALRM, &sa, NULL)) {
		perror("sigaction() failed");
		exit(EXIT_FAILURE);
	}

	// set timer from the command-line arguments
	itv.it_value.tv_sec = (argc > 1) ? get_long(argv[1], 0, "secs") : 2;
	itv.it_value.tv_usec = (argc > 2) ? get_long(argv[2], 0, "usecs") : 0;
	itv.it_interval.tv_sec = (argc > 3) ? get_long(argv[3], 0, "int-secs") : 0;
	itv.it_interval.tv_usec = (argc > 4) ? get_long(argv[4], 0, "int-usecs") : 0;

	// exit after 3 signals, or on first signal if interval is 0
	max_sigs = (itv.it_interval.tv_sec == 0 && itv.it_interval.tv_usec == 0) ? 1 : 3; // up to 3

	/*
	  set the value and interval fields for a real (ITIMER_REAL)
	  timer using the values supplied in its command-line
	  arguments. If these arguments are absent, the program sets a
	  timer that expires just once, after 2 seconds
	 */
	display_times("START:", false);
	if (-1 == setitimer(ITIMER_REAL, &itv, NULL)) {
		perror("setitimer() failed");
		exit(EXIT_FAILURE);
	}

	prev_clk = clock();
	sig_cnt = 0;

	for (;;) {
		// inner loop consumes at least 0.5 seconds CPU time
		while (5 > ((clock() - prev_clk) * 10 / CLOCKS_PER_SEC)) {
			/*
			  if the timer has a zero interval, then the
			  program exits on delivery of the first
			  signal
			 */
			if (got_alarm) {
				got_alarm = 0;
				display_times("ALARM:", true);

				/*
				  otherwise, it catches up to three
				  signals before terminating
				 */
				sig_cnt++;
				if (sig_cnt >= max_sigs) {
					fprintf(stderr, "READY.\n");
					exit(EXIT_FAILURE);
				}
			}
		}

		prev_clk = clock();
		display_times("main: ", true);
	}
	exit(EXIT_SUCCESS);
}
