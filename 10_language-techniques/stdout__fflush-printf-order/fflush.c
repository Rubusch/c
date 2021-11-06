// fflush.c
/*
  printf on stdout and stderr can twist order due to buffering, fflush(stdout)
  may help to flush the stream buffer

  ALWAYS use unbuffered streams for debugging!!

  The fflush function sets the error indicator for the stream and returns EOF if
  a write error occurs, otherwise it returns zero.
//*/

#include <stdio.h> /* fflush() */
#include <stdlib.h>
#include <string.h> /* memset() */
#include <unistd.h> /* sleep() */

int main(int argc, char **argv)
{
	puts("stdout: twisted order between buffered and unbuffered stream");
	char buf[BUFSIZ];
	memset(buf, '\0', sizeof(buf));
	fprintf(stdout, "turn buffer on\n");
	setvbuf(stdout, buf, _IOFBF,
		BUFSIZ); // setup buffer for stdout to BUFSIZ

	fprintf(stdout,
		"1. ipsum lorum primeiro (buffered stdout, but fflush-ed)\n");
	fflush(stdout); // flush - comes at once, then the 'big task'

	fprintf(stdout,
		"2. ipsum lorum segundo (buffered stdout)\n"); // this will come even
		// AFTER SLEEP!!!
	//  fflush(stdout); // flush - turn this on, for order: 1-2-3

	fprintf(stderr, "3. ipsum lorum terceiro (unbuffered stderr)\n");
	fprintf(stderr, "\tsleeping...\n");
	sleep(1);

	fprintf(stderr, "\tZzz..\n");
	sleep(1);

	fprintf(stderr, "\tZzz..\n");
	sleep(1);

	fprintf(stderr, "\tZzz..\n");
	sleep(5); // a 'big task'

	// the output order will be: 1-3-2, since 3 is unbuffered and 2 will wait on
	// the 'big task' (i.e. sleep) to finish

	exit(EXIT_SUCCESS);
}
