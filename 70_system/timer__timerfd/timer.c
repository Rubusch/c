/*
  usage:
    $ ./timer.elf 1:1 10
      1.101: expirations read: 1; total=1
      2.101: expirations read: 1; total=2
      3.101: expirations read: 1; total=3
      4.101: expirations read: 1; total=4
      5.101: expirations read: 1; total=5
      6.101: expirations read: 1; total=6
      7.101: expirations read: 1; total=7
      8.101: expirations read: 1; total=8
      9.101: expirations read: 1; total=9
      10.101: expirations read: 1; total=10
      READY.


  Demonstrates the use of the timerfd API

  This program takes two command-line arguments. the first argument is
  mandatory, and specifies the initial time and interval for a
  timer. this argument is interpreted using the itimerspec_from_str()
  function shown in other examples, too. the second argument, which
  isi optional, specifies the maximum number of expirations of the
  timer that the program should wait for before terminating; the
  default for this argument is 1.


  Based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 510
 */
#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/timerfd.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>

#include "tlpi/get_num.h"

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

int
main(int argc, char *argv[])
{
	struct itimerspec ts;
	struct timespec start, now;
	int max_exp, fd, secs, nanosecs;
	uint64_t num_exp, total_exp;
	ssize_t len;

	if (2 > argc) {
		fprintf(stderr, "usage:\n$ %s secs[/nsecs][:int-secs[/int-nsecs]] [max-exp]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	itimerspec_from_str(argv[1], &ts);
	max_exp = (argc > 2) ? get_int(argv[2], GN_GT_0, "max-exp") : 1;
	fd = timerfd_create(CLOCK_REALTIME, 0);
	if (-1 == fd) {
		perror("timerfd_create() failed");
		exit(EXIT_FAILURE);
	}

	if (-1 == timerfd_settime(fd, 0, &ts, NULL)) {
		perror("timerfd_settime() failed");
		exit(EXIT_FAILURE);
	}

	if (-1 == clock_gettime(CLOCK_MONOTONIC, &start)) {
		perror("clock_gettime() failed");
		exit(EXIT_FAILURE);
	}

	for (total_exp = 0; total_exp < max_exp; ) {
		/*
		  read number of expirations on the timer, and then
		  display time elapsed since timer was started,
		  followed by number of expirations read and total
		  expirations so far
		*/

		len = read(fd, &num_exp, sizeof(num_exp));
		if (len != sizeof(uint64_t)) {
			fprintf(stderr, "read() failed\n");
			exit(EXIT_FAILURE);
		}

		total_exp += num_exp;

		if (-1 == clock_gettime(CLOCK_MONOTONIC, &now)) {
			perror("clock_gettime() failed");
			exit(EXIT_FAILURE);
		}

		secs = now.tv_sec - start.tv_sec;
		nanosecs = now.tv_nsec - start.tv_sec;
		if (nanosecs < 0) {
			secs--;
			nanosecs += 1000000000;
		}

		fprintf(stderr, "%d.%03d: expirations read: %llu; total=%llu\n",
			secs, (nanosecs + 500000) / 1000000,
			(unsigned long long) num_exp,
			(unsigned long long) total_exp);
	}

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
