/*
   display the calender time based on timezone

   based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 191
 */

#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <time.h>
#include <sys/time.h>

#define SECS_IN_TROPICAL_YEAR (365.24219 * 24 * 60 * 60)

int main(int argc, char *argv[])
{
	time_t tme;
	struct tm *gmp, *locp;
	struct tm gm, loc;
	struct timeval tv;

	tme = time(NULL);
	fprintf(stderr, "seconds since the Epoch (1 Jan 1970): %ld", (long) tme);
	fprintf(stderr, " (about %6.3f years)\n\n", tme / SECS_IN_TROPICAL_YEAR);

	if (-1 == gettimeofday(&tv, NULL)) {
		perror("gettimeofday() failed");
		goto err;
	}

	fprintf(stderr, " gettimeofday() returned %ld secs, %ld microsecs\n\n",
			(long) tv.tv_sec, (long) tv.tv_usec);

	gmp = gmtime(&tme);
	if (NULL == gmp) {
		perror("gmtime() failed");
		goto err;
	}

	gm = *gmp; // save local copy, since *gmp may be modified by asctime() or
	           // gmtime()

	fprintf(stderr, "Broken down by gmtime():\n\n");

	fprintf(stderr, " year=%d mon=%d mday=%d hour=%d min=%d sec=%d ",
			gm.tm_year, gm.tm_mon, gm.tm_mday,
			gm.tm_hour, gm.tm_min, gm.tm_sec);

	fprintf(stderr, "wday=%d yday=%d isdst=%d\n\n",
			gm.tm_wday, gm.tm_yday, gm.tm_isdst);

	locp = localtime(&tme);
	if (NULL == locp) {
		perror("localtime() failed");
		goto err;
	}

	loc = *locp; // save local copy

	fprintf(stderr, "Broken down by localtime():\n\n");

	fprintf(stderr, " year=%d, min=%d, mday=%d, hour=%d, min=%d, sec=%d ",
			loc.tm_year, loc.tm_mon, loc.tm_mday,
			loc.tm_hour, loc.tm_min, loc.tm_sec);

	fprintf(stderr, "wday=%d yday=%d isdst=%d\n\n",
			loc.tm_wday, loc.tm_yday, loc.tm_isdst);

	fprintf(stderr, "asctime() formats the gmtime() value as:\t%s",
			asctime(&gm));

	fprintf(stderr, "ctime() formats the time() value as:\t%s",
			ctime(&tme));

	fprintf(stderr, "mktime() of gmtime() value:\t%ld secs\n",
			(long) mktime(&gm));

	fprintf(stderr, "mktime() of localtime() value:\t%ld secs\n\n",
			(long) mktime(&loc));


	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
err:
	exit(EXIT_FAILURE);
	}
