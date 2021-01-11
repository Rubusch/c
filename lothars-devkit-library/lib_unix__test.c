#include "lib_unix.h"

#include <stdlib.h>
#include <stdio.h>

#define NDEBUG
#include <assert.h>


void test__lothars__calloc()
{
	int* ptr = NULL;
	size_t nmemb = MAXLINE;
	size_t size = sizeof(*ptr);

	fprintf(stdout, "executing %s()...", __func__);
	assert(NULL == ptr); /* pre-condition */
	ptr = lothars__calloc(nmemb, size); // FIXME: ld error      
	assert(NULL != ptr); /* post-condition */
	fprintf(stdout, "ok\n");
}



int main(int argc, char* argv[])
{
	test__lothars__calloc();
	// TODO

	fprintf(stdout, "READY.\n");
	exit(EXIT_FAILURE);
}
