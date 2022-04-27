/*
  usage:
    $ ./conditiontimed.elf 5:5 10:10
      timer ID: 31736384 (5:5)
      timer ID: 31736448 (10:10)
      [23:14:34] thread notify
              timer ID=31736384
              timer_getoverrun()=0
      main(): expire_cnt = 1
      [23:14:39] thread notify
              timer ID=31736384
              timer_getoverrun()=0
      main(): expire_cnt = 2
      [23:14:39] thread notify
              timer ID=31736448
              timer_getoverrun()=0
      main(): expire_cnt = 3
      [23:14:44] thread notify
              timer ID=31736384
              timer_getoverrun()=0
      main(): expire_cnt = 4
      [23:14:49] thread notify
      [23:14:49] thread notify
              timer ID=31736448
              timer_getoverrun()=0
      main(): expire_cnt = 5
              timer ID=31736384
              timer_getoverrun()=0
      main(): expire_cnt = 6

 --> send SIGTSTP (20), suspend
      [1]+  Stopped                 ./conditiontimed.elf 5:5 10:10

    $ fg
      ./conditiontimed.elf 5:5 10:10
      [23:15:01] thread notify
              timer ID=31736384
      [23:15:01] thread notify
              timer ID=31736448
              timer_getoverrun()=0
              timer_getoverrun()=1 <----- got timer overruns
      main(): expire_cnt = 7
      main(): expire_cnt = 9
      [23:15:04] thread notify
              timer ID=31736384
              timer_getoverrun()=0
      main(): expire_cnt = 10

 --> SIGINT, to stop
      ^C


  demonstrates the use of SIGEV_THREAD, this program performs the
  following:

  - for each command line argument, the program creates and arms a
    POSIX timer that uses the SIGEV_THREAD notification mechanism

  - each time this timer expires, the function specified by
    sev.sigev_notify_function will be invoked in a separate
    thread. when this function is invoked, it receives the value
    specified in

      sev.sigev_value.sival_ptr

    as an argument. we assign the address of the timer ID
    (tidlist[idx]) to this field so that the notification function can
    obtain the ID of the timer that caused its invocation

  - having created and armed all of the timers, the main program
    enters a loop that waits for timer expirations. each time through
    the loop the program uses pthread_cond_wait() to wait for a
    condition variable "cond" to be signaled by the thread that is
    handling a timer notification

  - the thread_func() function is invoked on each timer
    expiration. after printing a message, it increments the value of
    the global variable expire_cnt. to allow also for added to
    expire_cnt.
    the notification function also signals the condition
    variable cond so that the main program knows to check that a timer
    has expired.


  based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 506
 */
#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static int expire_cnt = 0;  // number of expirations of all timers

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

/*
  thread notification function
 */
static void
thread_func(union sigval sv)
{
	timer_t *ptid;
	int ret;

	ptid = sv.sival_ptr;

	fprintf(stderr, "[%s] thread notify\n", curr_time("%T"));
	fprintf(stderr, "\ttimer ID=%ld\n", (long) *ptid);
	fprintf(stderr, "\ttimer_getoverrun()=%d\n", timer_getoverrun(*ptid));

	/*
	  increment counter variable shared with main thread and
	  signal condition variable to notify main thread of the
	  change
	 */

	ret = pthread_mutex_lock(&mtx);
	if (0 != ret) {
		fprintf(stderr, "%s(): pthread_mutex_lock() failed\n", __func__);
		pthread_exit(0);
	}

	expire_cnt += 1 + timer_getoverrun(*ptid);

	ret = pthread_mutex_unlock(&mtx);
	if (0 != ret) {
		fprintf(stderr, "%s(): pthread_mutex_unlock() failed\n", __func__);
		pthread_exit(0);
	}

	ret = pthread_cond_signal(&cond);
	if (0 != ret) {
		fprintf(stderr, "%s(): pthread_cond_lock() failed\n", __func__);
		pthread_exit(0);
	}
}

int
main(int argc, char *argv[])
{
	struct sigevent sev;
	struct itimerspec ts;
	timer_t *tidlist;
	int idx, ret;

	if (2 > argc) {
		fprintf(stderr, "usage:\n$ %s secs[/nsecs][:int-secs[/int-nsecs]]...\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	tidlist = calloc(argc-1, sizeof(*tidlist));
	if (NULL == tidlist) {
		perror("calloc() failed");
		exit(EXIT_FAILURE);
	}

	sev.sigev_notify = SIGEV_THREAD; // notify via thread
	sev.sigev_notify_function = thread_func; // thread start function
	sev.sigev_notify_attributes = NULL;
	// could be pointer to pthread_attr_t structure

	// create and start one timer for each command-line argument
	for (idx = 0; idx < argc -1; idx++) {
		itimerspec_from_str(argv[idx + 1], &ts);

		/*
		  for each command-line argument, create and arm a
		  POSIX timer that uses the SIGEV_SIGNAL notification
		  mechanism
		 */
		sev.sigev_value.sival_ptr = &tidlist[idx];

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

	/*
	  The main thread waits on a condition variable that is
	  signaled on each invocation of the thread notification
	  function. We print a message so that the user can see that
	  this occurred.
	 */

	ret = pthread_mutex_lock(&mtx);
	if (0 != ret) {
		fprintf(stderr, "%s(): pthread_mutex_lock() failed\n", __func__);
		exit(EXIT_FAILURE);
	}

	while (true) {
		ret = pthread_cond_wait(&cond, &mtx);
		if (0 != ret) {
			fprintf(stderr, "%s(): pthread_mutex_unlock() failed\n",
				__func__);
			exit(EXIT_FAILURE);
		}
		fprintf(stderr, "%s(): expire_cnt = %d\n", __func__, expire_cnt);
	}

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
