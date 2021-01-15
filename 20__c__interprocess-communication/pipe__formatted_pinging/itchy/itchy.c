// itchy.c
/*
 */

#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ME "ITCHY:"

int main(int argc, char **argv)
{
	fprintf(stderr, "%s started\n", ME);

	// receives through pipe as argument
	if (argc == 2) {
		fprintf(stderr, "%s received commandline argument \"%s\"\n", ME, argv[1]);
	} else {
		fprintf(stderr, "%s I can't hear you!\n", ME);
	}

	// sends
	fprintf(stderr, "%s sends..\n", ME);
	fprintf(stdout, "Itchyargument");

	fprintf(stderr, "%s done!\n", ME);
	exit(EXIT_SUCCESS);
}
