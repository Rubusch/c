/*
   retrieve process CPU times

   based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 208
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>

#include "tlpi/get_num.h"

static void
display_process_times(const char *msg)
{
	struct tms tm;
	clock_t clock_time;
	static long ticks = 0;

	if (msg != NULL) {
		fprintf(stderr, "%s", msg);
	}

	if (0 == ticks) { // fetch clock ticks on first call
		ticks = sysconf(_SC_CLK_TCK);
		if (-1 == ticks) {
			perror("sysconf() failed");
			goto err;
		}
	}

	clock_time = clock();
	if (-1 == clock_time) {
		perror("clock() failed");
		goto err;
	}

	fprintf(stderr, "\tclock() returns: %ld clocks-per-sec (%.2f secs)\n",
		(long) clock_time, (double) clock_time / CLOCKS_PER_SEC);

	if (-1 == times(&tm)) {
		perror("times() failed");
		goto err;
	}

	fprintf(stderr, "\ttimes() yields: user CPU=%.2f; system CPU: %.2f\n",
		(double) tm.tms_utime / ticks,
		(double) tm.tms_stime / ticks);

	return;
err:
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	int ncalls, idx;

	fprintf(stderr, "CLOCKS_PER_SEC=%ld  sysconf(_SC_CLK_TCK)=%ld\n\n",
		(long) CLOCKS_PER_SEC, sysconf(_SC_CLK_TCK));

	display_process_times("at program start:\n");

	ncalls = (argc > 1) ? get_int(argv[1], GN_GT_0, "num-calls") : 10000000;
	for (idx = 0; idx < ncalls; idx++) {
		(void) getppid();
	}

	display_process_times("after getppid() loop:\n");

	exit(EXIT_SUCCESS);
}

