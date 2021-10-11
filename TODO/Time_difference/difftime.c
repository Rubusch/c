// difftime.c
/*
  calculates the time difference between two time points
//*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUMBER 65535

double using_time()
{
	time_t start, stop;

	// start
	start = time(NULL);
	puts("using time: system time");

	// counting
	unsigned long cnt = 0;
	while (cnt != NUMBER) {
		printf("%ld ", cnt);
		++cnt;
	}

	// stop
	stop = time(NULL);

	// difference
	return difftime(stop, start);
}

double using_clock()
{
	clock_t start, stop;

	// start
	start = clock();
	puts("using time: cpu clock time");

	// counting
	unsigned long cnt = 0;
	while (cnt != NUMBER) {
		printf("%ld ", cnt);
		++cnt;
	}

	// stop
	stop = clock();

	// difference
	return (unsigned long)(stop - start) / CLOCKS_PER_SEC;
}

int main()
{
	puts("time measuring");

	double difA = using_time();
	double difB = using_clock();

	puts("\n\n\n");
	fprintf(stderr, "system time: %.0f seconds to count to %d\n", difA,
		NUMBER);
	fprintf(stderr, "cpu clock time: %.0f seconds to count to %d\n", difB,
		NUMBER);

	exit(EXIT_SUCCESS);
}
