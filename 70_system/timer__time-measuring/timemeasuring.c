// timemeasuring.c
/*
  the example shows how to set two timestamps and how to compute the
  difference in human readable time
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main()
{
	struct timespec ts_resolution, ts_start, ts_end;
	unsigned long int difference = 0;

	// 1. set up the clock resolution
	clock_getres(CLOCK_MONOTONIC, &ts_resolution);

	fprintf(stderr, "clock resolution set up to %lus and %lins\n",
	       ts_resolution.tv_sec, ts_resolution.tv_nsec);

	// 2. measurement start
	clock_gettime(CLOCK_MONOTONIC, &ts_start);

	/*
	  operation
	//*/
	int cnt = 0;
	for (cnt = 0; cnt < 10; ++cnt) {
		puts("foobar");
	}

	// 3. measurement stop
	clock_gettime(CLOCK_MONOTONIC, &ts_end);

	// 4. calculating the time difference
	difference = (ts_end.tv_sec - ts_start.tv_sec) * 1000000000 +
		     ts_end.tv_nsec - ts_start.tv_nsec;

	fprintf(stderr, "Elapsed time: %luns\n", difference);

	puts("READY.");
	exit(EXIT_SUCCESS);
}
