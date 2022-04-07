/*
   retrieve and converte calender times

   based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 197
 */

#define _XOPEN_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <time.h>
#include <sys/time.h>

#define SBUF_SIZE 1000

void usage(char* name)
{
	fprintf(stderr, "usage:\n");
	fprintf(stderr, "$ %s \"9:39:46pm 5 Apr 2022\"\n", name);
}

int main(int argc, char *argv[])
{
	struct tm tm;
	char sbuf[SBUF_SIZE];
	char *ofmt;

	if (argc != 2) {
		usage(argv[0]);
		goto err;
	}

	if (NULL == setlocale(LC_ALL, "")) {
		fprintf(stderr, "setlocale() failed\n");
		goto err;
	}

	{
		// first formatation approach
		memset(&tm, 0, sizeof(tm));
		if (NULL == strptime(argv[1], "", &tm)) {
			fprintf(stderr, "strptime() failed\n");
			goto err;
		}
		tm.tm_isdst = -1; // not set by strptime(), tells mktime() to determine if DST is in effect
		fprintf(stderr, "calender time (seconds since Epoch): %ld\n", (long) mktime(&tm));

		ofmt = "%H:%M:%S %A, %d %B %Y %Z";
		fprintf(stderr, "format: \"%%H:%%M:%%S %%A, %%d %%B %%Y %%Z\"\n");
		if (0 == strftime(sbuf, SBUF_SIZE, ofmt, &tm)) {
			fprintf(stderr, "strftime failed\n");
			goto err;
		}
		fprintf(stderr, "strftime() yields: %s\n", sbuf);
		fprintf(stderr, "\n");
	}

	{
		// second formatation
		memset(&tm, 0, sizeof(tm));
		if (NULL == strptime(argv[1], "", &tm)) {
			fprintf(stderr, "strptime() failed\n");
			goto err;
		}
		tm.tm_isdst = -1; // not set by strptime(), tells mktime() to determine if DST is in effect
		fprintf(stderr, "calender time (seconds since Epoch): %ld\n", (long) mktime(&tm));

		fprintf(stderr, "format: \"%%I:%%M:%%S%%p %%d %%b %%Y\"\n");
		ofmt = "%I:%M:%S%p %d %b %Y";
		if (0 == strftime(sbuf, SBUF_SIZE, ofmt, &tm)) {
			fprintf(stderr, "strftime failed\n");
			goto err;
		}
		fprintf(stderr, "strftime() yields: %s\n", sbuf);
		fprintf(stderr, "\n");
	}

	{
		// third formatation approach with explicitly set formatting to strptime
		memset(&tm, 0, sizeof(tm));
		if (NULL == strptime(argv[1], "%I:%M:%S%p %d %b %Y", &tm)) {
			fprintf(stderr, "strptime() failed\n");
			goto err;
		}
		tm.tm_isdst = -1; // not set by strptime(), tells mktime() to determine if DST is in effect
		fprintf(stderr, "calender time (seconds since Epoch): %ld\n", (long) mktime(&tm));

		fprintf(stderr, "format: \"%%I:%%M:%%S%%p %%d %%b %%Y\" \"%%F %%T\"\n");
		ofmt = "%I:%M:%S%p %d %b %Y %F %T";
		if (0 == strftime(sbuf, SBUF_SIZE, ofmt, &tm)) {
			fprintf(stderr, "strftime failed\n");
			goto err;
		}
		fprintf(stderr, "strftime() yields: %s\n", sbuf);
		fprintf(stderr, "\n");
	}

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
err:
	exit(EXIT_FAILURE);
	}

