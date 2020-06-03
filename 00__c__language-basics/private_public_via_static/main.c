/*
  main.c
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "foo.h"

int main()
{
	char func[] = "main";

	fprintf(stdout, "%s(): call public funcion\n", func);
	if(!public_function())
		fprintf(stderr, "%s(): public function failed\n", func);

	fprintf(stdout, "%s(): call public static function\n", func);
	if(!public_inlined_function())
		fprintf(stderr, "%s(): public static function failed\n", func);


	puts("");
	puts("READY.");
	exit(EXIT_SUCCESS);
}
