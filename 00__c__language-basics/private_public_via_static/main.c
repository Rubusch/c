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

	fprintf(stdout, "%s():\tcall public funcion\n", func);
	if(!public_function())
		fprintf(stderr, "%s():\tpublic function failed\n", func);

	fprintf(stdout, "%s():\tcall public static function\n", func);
	if(!public_inlined_function())
		fprintf(stderr, "%s():\tpublic static function failed\n", func);


	puts("");
	puts("READY.");
	exit(EXIT_SUCCESS);
}
