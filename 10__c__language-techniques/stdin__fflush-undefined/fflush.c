// fflush.c
/*
  fflush(stdin) can lead to undefined behaviour!!!

  The fflush function sets the error indicator for the stream and returns EOF if
  a write error occurs, otherwise it returns zero.
//*/

#include <stdio.h> /* fflush() */
#include <stdlib.h>
#include <string.h> /* memset() */
#include <unistd.h> /* sleep() */

#define STRSIZ 20

int main(int argc, char **argv)
{
	puts("stdin: enter some text, end with ENTER");

	char str[STRSIZ];
	memset(str, '\0', sizeof(str));
	for (int idx = 0; idx < 2; ++idx) {
		scanf("%[^\n]s", str); // read input, until \n
		fprintf(stdout, "read: '%s'\n", str);
		//    fflush(stdin); // can have undefined behavior, depends on compiler
		// alternative method:
		int c;
		while ((c = getchar()) != EOF && c != '\n')
			;
		memset(str, '\0', sizeof(str));
	}

	puts("READY.");
	exit(EXIT_SUCCESS);
}
