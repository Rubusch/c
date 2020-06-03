/*
  main.c
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "foo.h"

int main()
{

	if(!public_function())
		fprintf(stderr, "public function failed\n");

	if(!public_inlined_function())
		fprintf(stderr, "public static function failed\n");


	puts("READY.");
	exit(EXIT_SUCCESS);
}
