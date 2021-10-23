#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>


int main(int argc, char *argv[])
{
	int64_t t = 777;

	/*
	  PRI.64 macros help to get rid of compiler warnings when
	  switching from 32 bit systems to 64 bit, and vice versa
	 */

	printf("printf() as 'd': %" PRId64 "\n", t);

	printf("printf() as 'u': %" PRIu64 "\n", t);

	printf("printf() as 'x': 0x%" PRIx64 "\n", t);

	printf("printf() as 'x': 0x%08" PRIx64 "\n", t);

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
