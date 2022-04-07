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

#ifndef BUFSIZ
# define BUFSIZ 200
#endif

int main(int argc, char *argv[])
{
	time_t tme;
	struct tm *loc;
	char buf[BUFSIZ];

	if (NULL == setlocale(LC_ALL, "")) {
		perror("setlocale failed");
		goto err;
	}

	tme = time(NULL);

	fprintf(stderr, "ctime() of time() value is: %s", ctime(&tme));

	loc = localtime(&tme);
	if (NULL == loc) {
		fprintf(stderr, "localtime() failed\n");
		goto err;
	}

	fprintf(stderr, "asctime() of local time is: %s", asctime(loc));
	if (0 == strftime(buf, BUFSIZ, "%A, %d %B %Y, %H:%M:%S %Z", loc)) {
		fprintf(stderr, "strftime() failed\n");
		goto err;
	}
	fprintf(stderr, "strftime() of local time is: %s\n", buf);


	exit(EXIT_SUCCESS);
err:
	exit(EXIT_FAILURE);
}
