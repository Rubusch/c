/*
  usage:
  create a single timer with an initial timer expiry of 2 seconds and
  an interfval of 5 seconds

    $ ./timer.elf 2:5
      timer ID: 31756704 (2:5)
      [17:20:30] got signal 64
              *sival_ptr      = 31756704
              timer_getoverrun() = 0
      [17:20:35] got signal 64
              *sival_ptr      = 31756704
              timer_getoverrun() = 0
      [17:20:40] got signal 64
              *sival_ptr      = 31756704
              timer_getoverrun() = 0
      [17:20:45] got signal 64
              *sival_ptr      = 31756704
              timer_getoverrun() = 0

  --> SIGTSTP to suspend, signal 20
      [1]+  Stopped                 ./timer.elf 2:5

    $ fg
      ./timer.elf 2:5
      [17:20:52] got signal 64
              *sival_ptr      = 31756704
              timer_getoverrun() = 0
  --> SIGINT to stop
      ^C

  demonstrates the use of signals as the notification mechanism for a
  POSIX timer


  based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 500
 */
#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <signal.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>

#define TIMER_SIG SIGRTMAX              // the timer notification signal

/*
  current_time()

  Return a string containing the current time formatted according to
  the specification in 'format' (see strftime(3) for specifiers). If
  'format' is NULL, we use "%c" as a specifier (which gives the date
  and time as for ctime(3), but without the trailing newline). Returns
  NULL on error.

  based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 194
*/
#ifndef BUFSIZ
# define BUFSIZ 1024
#endif

char*
curr_time(const char *format)
{
	static char buf[BUFSIZ]; // non-reentrant!!!
	time_t t;
	size_t ret;
	struct tm *tm;

	t = time(NULL);
	tm = localtime(&t);
	if (NULL == tm) {
		return NULL;
	}

	ret = strftime(buf, BUFSIZ, (format != NULL) ? format : "%c", tm);

	return (0 == ret) ? NULL : buf;
}
/* --- */

/*
  itimerspec_from_str()

  Convert a string of the following from to an itimerspec structure:

    value.sec[/value.nanosec][:interval.sec[/interrval.nanosec]]

  Optional components that are omitted cause 0 to be assigned to the
  corresponding structure fields.

  based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 502
*/
void
itimerspec_from_str(char *str, struct itimerspec *tsp)
{
	char *dupstr, *cptr, *sptr;

	dupstr = strdup(str);

	cptr = strchr(dupstr, ':'); // locate character ':' in string
	if (NULL != cptr) {
		*cptr = '\0';
	}

	sptr = strchr(dupstr, '/'); // locate character '/' in string
	if (NULL != sptr) {
		*sptr = '\0';
	}

	tsp->it_value.tv_sec = atoi(dupstr);
	tsp->it_value.tv_nsec = (sptr != NULL) ? atoi(sptr + 1) : 0;
	if (NULL == cptr) {
		tsp->it_interval.tv_sec = 0;
		tsp->it_interval.tv_nsec = 0;
	} else {
		sptr = strchr(cptr + 1, '/');
		if (NULL != sptr) {
			*sptr = '\0';
		}
		tsp->it_interval.tv_sec = atoi(cptr + 1);
		tsp->it_interval.tv_nsec = (sptr != NULL) ? atoi(sptr + 1) : 0;
	}
	free(dupstr);
}
/* --- */

static void
signal_handler(int sig, siginfo_t* si, void* uc)
{
         timer_t *tidptr;

	 /*
	   On each timer expiration, the signal specified in
	   sev.sigev_signo will be delivered to the process. The
	   handler for this signal displays the value that was
	   supplied in sev.sigev_value.sival_ptr i.e.the timer ID
	   tidlist[idx] and the overrun value for the timer.
	  */
	 tidptr = si->si_value.sival_ptr;

	 // UNSAFE: using stdio and other non-reentrant functions
	 // inside a signal handler is unsave

	 fprintf(stderr, "[%s] got signal %d\n", curr_time("%T"), sig);
	 fprintf(stderr, "\t*sival_ptr\t= %ld\n", (long) *tidptr);
	 fprintf(stderr, "\ttimer_getoverrun() = %d\n", timer_getoverrun(*tidptr));
}

int
main(int argc, char *argv[])
{
	struct itimerspec ts;
	struct sigaction sa;
	struct sigevent sev;
	timer_t *tidlist;
	int idx;

	if (2 > argc) {
		fprintf(stderr, "usage:\n$ %s <secs>[/nsecs][:int-secs[/int-nsecs]]...\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	tidlist = calloc(argc - 1, sizeof(*tidlist));
	if (NULL == tidlist) {
		perror("calloc() failed");
		exit(EXIT_FAILURE);
	}

	// establish handler for timer notification signals

	sa.sa_flags = SA_SIGINFO;
	/*
	  The signal handler takes three arguments, not one. In this
	  case, sa_sigaction should be set instead of sa_handler.
	  This flag is meaningful only when es‐ tablishing a signal
	  handler.
	*/
	sa.sa_sigaction = signal_handler;
	sigemptyset(&sa.sa_mask);
	if (-1 == sigaction(TIMER_SIG, &sa, NULL)) {
		perror("sigaction() failed");
		exit(EXIT_FAILURE);
	}

	// sigevent - create and start one timer for each command-line argument

	sev.sigev_notify = SIGEV_SIGNAL; // notify via signal
	sev.sigev_signo = TIMER_SIG;     // notify using this signal

	for (idx = 0; idx < argc-1; idx++) {
		// convert the command-line arguments to itimerspec strucutres
		itimerspec_from_str(argv[idx + 1], &ts);

		/*
		  for each command-line argument, create and arm a
		  POSIX timer that uses the SIGEV_SIGNAL notification
		  mechanism
		 */
		sev.sigev_value.sival_ptr = &tidlist[idx];
		// allows handler to get ID of this timer
		if (-1 == timer_create(CLOCK_REALTIME, &sev, &tidlist[idx])) {
			perror("timer_create() failed");
			exit(EXIT_FAILURE);
		}

		fprintf(stderr, "timer ID: %ld (%s)\n",
			(long) tidlist[idx], argv[idx + 1]);

		if (-1 == timer_settime(tidlist[idx], 0, &ts, NULL)) {
			perror("timer_settime() failed");
			exit(EXIT_FAILURE);
		}
	}

	while (true) {
		/*
		  Having created and armed the timers, wait for timer
		  expirations by executing a loop that repeatedly
		  calls pause()
		 */
		pause();
	}

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
