/*
  foo.c
*/

#include "foo.h"

static int private = 777;

int public_function()
{
	puts("a public function is declared in .h and defined in .c file");

	fprintf(stdout, "a 'static variable in .c file' is a private member to this compile unit: '%d'\n", private);

	return 0;
}


