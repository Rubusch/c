/*
  classics: copy one array into another, via C pointer
*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
	const char orig_arr[] = "duo com faciunt idem non est idem";
	const int ORIG_ARR_SIZE = sizeof(orig_arr);
	char arr[ORIG_ARR_SIZE];

	// initialization
	memset(arr, '\0', ORIG_ARR_SIZE-1);

	const char *pSrc = orig_arr;
	char *pDst = arr;
	fprintf(stderr, "(before): orig_arr [0x%08x] '%s', arr [0x%08x] '%s'\n",
		(int) &orig_arr, orig_arr, (int) &arr, arr);

	while (pSrc != orig_arr + ORIG_ARR_SIZE)
		*pDst++ = *pSrc++;

	fprintf(stderr, "(after): orig_arr [0x%08x] '%s', arr [0x%08x] '%s'\n",
		(int) &orig_arr, orig_arr, (int) &arr, arr);

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
